/*    This program is part of the Energy Aware Runtime (EAR).
    It has been developed in the context of the BSC-Lenovo Collaboration project.
    
    Copyright (C) 2017  
	BSC Contact Julita Corbalan (julita.corbalan@bsc.es) 
    	Lenovo Contact Luigi Brochard (lbrochard@lenovo.com)

*/

#include <stdio.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_multifit.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <cpufreq.h>
#define _GNU_SOURCE
#define __USE_GNU

#include <ear_db_type.h>
#include <ear_models/ear_models.h>

application_t *apps;
application_t **apps_ordered;
unsigned int num_app,current_app=0;
unsigned int *samples_f,i,*current,*samples_app;
char *nodename;
char **APPS_NAMES;
unsigned int Diff_apps=0;
unsigned int num_f;
struct Coefficients_info **Coefs;
struct DataApp *CSV;
unsigned int min_f;
unsigned int nom_f;

#define CREATE_FLAGS S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

char text[4096];
int fd_csv;
char text_csv[1024];
#define HEADER_CSV "NAME;TYPE(R/P);NOMINAL;FREQ;POWER;CPI;TIME\n"
#define TEMPLATE_CSV "%s;%c;%u;%u;%12.6f;%12.6f;%12.6f\n"

#define TEMPLATE_POWER "newgraph \n \
title : %s Average Power(fn)\n\
xaxis\n\
label : Frequency\n\
yaxis min 0.0\n\
label : Average Power(Watts)\n\
newcurve\n\
linetype solid\n\
label : Real Power\n\
pts\n\
include ./REAL_Power_%s.txt\n"

#define TEMPLATE_CURVE_POWER "newcurve\n\
linetype dotted\n\
label : Projected Power(%u)\n\
pts\n\
include ./PROJECTED_Power_%s_%u.txt\n "

#define TEMPLATE_CPI "newpage\n\
newgraph\n\
   title :  %s CPI(fn)\n\
   xaxis \n\
      label : Frequency\n\
   yaxis min 0.0\n\
      label : CPI\n\
   newcurve\n\
      linetype solid\n\
      label : Real CPI\n\
      pts\n\
         include ./REAL_CPI_%s.txt\n"
#define TEMPLATE_CURVE_CPI "newcurve\n\
      linetype dotted\n\
      label : Projected CPI(%u)\n\
      pts\n\
         include ./PROJECTED_CPI_%s_%u.txt\n "

#define TEMPLATE_TIME "newpage\n\
newgraph\n\
   title :  %s ExecutionTime(fn)\n\
   xaxis\n\
      label : Frequency\n\
   yaxis min 0.0\n\
      label : Execution time (secs)\n\
   newcurve\n\
      linetype solid\n\
      label : Real Time\n\
      pts\n\
         include ./REAL_Time_%s.txt\n"
#define TEMPLATE_CURVE_TIME "newcurve\n\
      linetype dotted\n\
      label : Projected Time(%u)\n\
      pts\n\
         include ./PROJECTED_Time_%s_%u.txt\n "

void Nominal_for_power(unsigned int ref,char *app_name,double *power,double *tpi);
void Nominal_for_cpi(unsigned int ref,char *app_name,double *cpi,double *tpi);
void Nominal_for_time(unsigned int ref,char *app_name,double *seconds);
void Nominal(unsigned int ref,char *app_name,double *power,double *tpi,double *cpi,double *seconds);


unsigned int *listf;
unsigned int I2F(int i)
{
    return listf[i];
}

unsigned int F2I(unsigned int f)
{
    int i=0;
    while ((i<num_f) && (listf[i]!=f)) i++;
    return i;
}


int CreateGraph(char *appname)
{
    int fd;
    char filename[128];
    sprintf(filename,"EAR_INFO_%s/%s_validation.jgr",nodename,appname);
    fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
    if (fd<0) perror("Error creating graph file\n");
    fprintf(stdout,"File %s created\n",filename);
    return fd;
}
// This function generates real and projected power , by the moment only for nominal=2.6
void AddCurvesPower(int fd,char *appname)
{
    unsigned int freq;
    unsigned int f,ref;
    char filename[1024];
    char value[128];

    fprintf(stdout,"Adding power for %s\n",appname);
    sprintf(text,TEMPLATE_POWER,appname,appname);
    write(fd,text,strlen(text));
    for (ref=0;ref<num_f;ref++){
        if (samples_f[ref]>0){
            sprintf(text,TEMPLATE_CURVE_POWER,I2F(ref),appname,I2F(ref));
            write(fd,text,strlen(text));
        }
    }
}
// This function will generate files for cpi and time since one depends on the other
void AddCurvesCPI(int fd,char *appname)
{
    int fd_cpi_real,fd_cpi_proj;
    int fd_time_real,fd_time_proj;
    int fd_power_real,fd_power_proj;
    unsigned int freq;
    unsigned int f,ref;
    double cpi,tpi,cpi_f0,tpi_f0;
    double cpi_fn,power_fn,seconds_fn;
    double power,power_f0;
    double seconds, seconds_f0;
    double A,B,C;
    double D,E,F;
    char filename[1024];
    char value[128];

    fprintf(stdout,"Adding CPI for %s\n",appname);
    sprintf(text,TEMPLATE_CPI,appname,appname);
    write(fd,text,strlen(text));

    //Generating REAL_CPI_%s.txt and PROJECTED_CPI_%s.txt files
    //Generating REAL_Time_%s.txt and PROJECTED_Time_%s.txt files
    //Generating REAL_Power_%s.txt and PROJECTED_Power_%s.txt files

    sprintf(filename,"EAR_INFO_%s/REAL_Power_%s.txt",nodename,appname);
    fd_power_real=open(filename,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
    if (fd_power_real<0){
        perror("Error opening REAL power file");
        exit(1);
    }
    sprintf(filename,"EAR_INFO_%s/REAL_CPI_%s.txt",nodename,appname);
    fd_cpi_real=open(filename,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
    if (fd_cpi_real<0){
        perror("Error opening REAL CPI file");
        exit(1);
    }
    sprintf(filename,"EAR_INFO_%s/REAL_Time_%s.txt",nodename,appname);
    fd_time_real=open(filename,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
    if (fd_time_real<0){
        perror("Error opening REAL Time file");
        exit(1);
    }
    for (f=0;f<num_f;f++){
        if (samples_f[f]>0){
            freq=I2F(f);
            Nominal(f,appname,&power,&tpi,&cpi,&seconds);
            sprintf(value,"%u %12.6f\n",freq,power);
            write(fd_power_real,value,strlen(value));
            sprintf(value,"%u %12.6f\n",freq,cpi);
            write(fd_cpi_real,value,strlen(value));
            sprintf(value,"%u %12.6f\n",freq,seconds);
            write(fd_time_real,value,strlen(value));
            sprintf(value,TEMPLATE_CSV,appname,'R',freq,freq,power,cpi,seconds);
            write(fd_csv,value,strlen(value));
        }
    }
    close(fd_cpi_real);
    close(fd_time_real);
    close(fd_power_real);
    for (ref=0;ref<num_f;ref++){
        if (samples_f[ref]>0){
            sprintf(filename,"EAR_INFO_%s/PROJECTED_CPI_%s_%u.txt",nodename,appname,I2F(ref));
            fd_cpi_proj=open(filename,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
            if (fd_cpi_proj<0){
                perror("Error opening PROJECTED CPI file");
                exit(1);
            }
            sprintf(filename,"EAR_INFO_%s/PROJECTED_Time_%s_%u.txt",nodename,appname,I2F(ref));
            fd_time_proj=open(filename,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
            if (fd_time_proj<0){
                perror("Error opening PROJECTED Time file");
                exit(1);
            }
            sprintf(filename,"EAR_INFO_%s/PROJECTED_Power_%s_%u.txt",nodename,appname,I2F(ref));
            fd_power_proj=open(filename,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
            if (fd_power_proj<0){
                perror("Error opening PROJECTED power file");
                exit(1);
            }

            sprintf(text,TEMPLATE_CURVE_CPI,I2F(ref),appname,I2F(ref));
            write(fd,text,strlen(text));
            Nominal(ref,appname,&power_f0,&tpi_f0,&cpi_f0,&seconds_f0);
            for(f=0;f<num_f;f++){
                if (samples_f[f]>0){
                    freq=I2F(f);
                    A=Coefs[ref][f].A;
                    B=Coefs[ref][f].B;
                    C=Coefs[ref][f].C;
                    D=Coefs[ref][f].D;
                    E=Coefs[ref][f].E;
                    F=Coefs[ref][f].F;
                    // Power(fn) = A(fn)*Power(f0) + B(fn)*TPI(f0) + C(fn) 
                    power_fn=A*power_f0+B*tpi_f0+C;
                    sprintf(value,"%u %12.6f\n",freq,power_fn);
                    write(fd_power_proj,value,strlen(value));
                    // CPI(fn) = D(fn)*CPI(f0) + E(fn)*TPI(f0) + F(fn) 
                    cpi_fn=D*cpi_f0+E*tpi_f0+F;
                    sprintf(value,"%u %12.6f\n",freq,cpi_fn);
                    write(fd_cpi_proj,value,strlen(value));
                    // TIME(fn) = TIME(f0) * CPI(fn)/CPI(f0) * (f0/fn) 
                    seconds_fn=seconds_f0*cpi_fn/cpi_f0*((double)I2F(ref)/(double)I2F(f));
                    sprintf(value,"%u %12.6f\n",freq,seconds_fn);
                    write(fd_time_proj,value,strlen(value));
                    sprintf(value,TEMPLATE_CSV,appname,'P',I2F(ref),freq,power_fn,cpi_fn,seconds_fn);
                    write(fd_csv,value,strlen(value));
                }
            }
            close(fd_cpi_proj);
            close(fd_time_proj);
            close(fd_power_proj);
        }//samples
    }//ref

}
void AddCurvesTime(int fd,char *appname)
{
    unsigned int ref;
    fprintf(stdout,"Adding Time for %s\n",appname);
    sprintf(text,TEMPLATE_TIME,appname,appname);
    write(fd,text,strlen(text));
    for (ref=0;ref<num_f;ref++){
        if (samples_f[ref]>0){
            sprintf(text,TEMPLATE_CURVE_TIME,I2F(ref),appname,I2F(ref));
            write(fd,text,strlen(text));
        }
    }
    close(fd);
}

unsigned int p_states()
{
    struct cpufreq_available_frequencies *list_f,*first;
    int num_pstates=0;
    list_f=cpufreq_get_available_frequencies(0);
    first=list_f;
    while(list_f!=NULL){
        list_f=list_f->next;
        num_pstates++;
    }
    listf=(unsigned int *)malloc(sizeof(unsigned int)*num_pstates);
    list_f=first;
    for (i=0;i<num_pstates;i++)
    {
        if (i == 1) nom_f = list_f->frequency;
        listf[i]=list_f->frequency;
        list_f=list_f->next;
    }
    cpufreq_put_available_frequencies(first);
    return num_pstates;
}
int TestAlreadyExists(application_t *Applist,unsigned int total_apps, application_t *newApp)
{
    unsigned int pos=0,found=0;
    while((pos<total_apps) && (found==0)){
        if ((strcmp(Applist[pos].app_id,newApp->app_id)==0) && (Applist[pos].nominal==newApp->nominal)) found=1;
        else pos++;
    }
#ifdef DEBUG
    fprintf(stdout,"Checking %s with f=%lu found=%d\n",newApp->app_id,newApp->nominal,found);
#endif
    if (found==0) return -1;
    else return pos;
}
void AverageApplication(application_t * current,unsigned int samples)
{
#ifdef DEBUG
    fprintf(stdout,"Application %s with f=%lu has %d samples\n",current->app_id,current->nominal,samples);
#endif
    double foravg=(double)samples;
    current->seconds=current->seconds/foravg;
    current->GIPS_f0=current->GIPS_f0/foravg;
    current->GBS_f0=current->GBS_f0/foravg;
    current->POWER_f0=current->POWER_f0/foravg;
    current->CPI=current->CPI/foravg;
    current->TPI_f0=current->TPI_f0/foravg;
}
void AcumApp(application_t *A,application_t *B)// A=A+B metrics
{
    A->seconds+=B->seconds;
    A->GIPS_f0+=B->GIPS_f0;
    A->GBS_f0+=B->GBS_f0;
    A->POWER_f0+=B->POWER_f0;
    A->TPI_f0+=B->TPI_f0;
    A->CPI+=B->CPI;
}
void CopyApp(application_t *A,application_t *B)
{
    strcpy(A->app_id,B->app_id);
    A->nominal=B->nominal;
    A->procs=B->procs;
    A->seconds=B->seconds;
    A->GIPS_f0=B->GIPS_f0;
    A->GBS_f0=B->GBS_f0;
    A->POWER_f0=B->POWER_f0;
    A->TPI_f0=B->TPI_f0;
    A->CPI=B->CPI;
}

void Nominal(unsigned int ref,char *app_name,double *power,double *tpi,double *cpi,double *seconds)
{
    int i=0,found=0;
    *power=0;
    *tpi=0;
    *cpi=0;
    *seconds=0;
    while ((i<samples_f[ref])&&(found==0)){
        if (strcmp(apps_ordered[ref][i].app_id,app_name)==0){
            *power=apps_ordered[ref][i].POWER_f0;
            *tpi=apps_ordered[ref][i].TPI_f0;
            *cpi=apps_ordered[ref][i].CPI;
            *seconds=apps_ordered[ref][i].seconds;
            found=1;
        }else i++;
    }

}

void Nominal_for_power(unsigned int ref,char *app_name,double *power,double *tpi)
{
    int i=0,found=0;
    *power=0;
    *tpi=0;
    while ((i<samples_f[ref])&&(found==0)){
        if (strcmp(apps_ordered[ref][i].app_id,app_name)==0){
            *power=apps_ordered[ref][i].POWER_f0;
            *tpi=apps_ordered[ref][i].TPI_f0;
            found=1;
        }else i++;
    }
}
void Nominal_for_cpi(unsigned int ref,char *app_name,double *cpi,double *tpi)
{
    int i=0,found=0;
    *cpi=0;
    *tpi=0;
    while ((i<samples_f[ref])&&(found==0)){
        if (strcmp(apps_ordered[ref][i].app_id,app_name)==0){
            *cpi=apps_ordered[ref][i].CPI;
            *tpi=apps_ordered[ref][i].TPI_f0;
            found=1;
        }else i++;
    }
}
void Nominal_for_time(unsigned int ref,char *app_name,double *seconds)
{
    int i=0,found=0;
    while ((i<samples_f[ref])&&(found==0)){
        if (strcmp(apps_ordered[ref][i].app_id,app_name)==0){
            *seconds=apps_ordered[ref][i].seconds;
            found=1;
        }else i++;
    }
}


void InitCoefs(unsigned int ref,unsigned int i,unsigned int f,double A,double B,double C,double D,double E,double F)
{
    Coefs[ref][i].pstate=f;
    Coefs[ref][i].available=1;
    Coefs[ref][i].A=A;
    Coefs[ref][i].B=B;
    Coefs[ref][i].C=C;
    Coefs[ref][i].D=D;
    Coefs[ref][i].E=E;
    Coefs[ref][i].F=F;
}

void InitApplicationNames(unsigned int numApps)
{
    int i;
    fprintf(stdout,"Initializing names\n");
    APPS_NAMES=(char **)malloc(sizeof(char *)*numApps);
    if (APPS_NAMES==NULL){
        fprintf(stderr,"Error allocating memory for application Names\n");
        exit(1);
    }
    for (i=0;i<numApps;i++) APPS_NAMES[i]=NULL;
    Diff_apps=0;
}

unsigned int ApplicationExist(char *appname){
    unsigned int i;
    if (Diff_apps==0) return 0;
    i=0;
    while (i<Diff_apps){
        if (strncmp(APPS_NAMES[i],appname,MAX_APP_NAME)==0) return 1;
        i++;
    }
    if (i==Diff_apps) return 0;
}

void AddApplication(char *appname)
{
    fprintf(stdout,"New app detected %s, total %d\n",appname,Diff_apps);
    APPS_NAMES[Diff_apps]=(char *)malloc(sizeof(char)*MAX_APP_NAME);
    if (APPS_NAMES[Diff_apps]==NULL){
        fprintf(stderr,"Error allocating memory for application Names\n");
        exit(1);
    }
    strncpy(APPS_NAMES[Diff_apps],appname,MAX_APP_NAME);
    Diff_apps++;
}

unsigned int DifferentApps(){
    return Diff_apps;
}
void WriteInText(int fd,int ref,struct Coefficients_info *Coefs,int num_f)
{
    int i;
    char line[1024];
    for (i=0;i<num_f;i++){
        sprintf(line,"%d;%d;%g;%g;%g;%g;%g;%g\n",I2F(ref),I2F(i),Coefs[i].A,Coefs[i].B,Coefs[i].C,Coefs[i].D,Coefs[i].E,Coefs[i].F);
        write(fd,line,strlen(line));
    }
}

void Usage(char *app)
{
    fprintf(stdout,"usage:%s db_name coefficients_db min_f nodename\n",app);
    exit(1);
}

int main(int argc,char *argv[])
{
    int fd,fd_graphs,fd_csv_coeffs,posApp;
    unsigned int f,pos,ref,i;
    unsigned int filtered_apps=0;
    double power,cpi,tpi;
    char coef_file[256];
    char men[128];
    application_t test;
    int instances_fd;
    char instances_filename[256];
    char *header_instances="NAME;FREQ;TIME;CPI;TPI;GBS;POWER\n";
    char EAR_metric[1024];

    if (argc<5) Usage(argv[0]);
    min_f=(unsigned int)atoi(argv[3]);
    nodename=argv[4];
    fprintf(stdout,"Filtering frequencies lower than %u\n",min_f);
    // results are saved in a EAR_INFO_ folder
    sprintf(coef_file,"EAR_INFO_%s",nodename);
    if (mkdir(coef_file,0777)<0){
        if (errno!=EEXIST){
            perror("Creating EAR_INFO_ folder");
            fprintf(stderr,"File %s\n",coef_file);
            exit(1);
        }
    }

    // We get how many samples per frequency we have
    num_f=p_states();
    fprintf(stdout,"There is %u frequencies in this system (nominal: %u)\n",num_f,nom_f);

    // samples_f is to compute how many samples per frequency we have
    samples_f=(unsigned int *)malloc(sizeof(unsigned int)*num_f);
    if (samples_f==NULL){
        fprintf(stderr,"Error allocating memory for samples_f\n");
        exit(1);
    }
    current=(unsigned int *)malloc(sizeof(unsigned int)*num_f);
    if (current==NULL){
        fprintf(stderr,"Error allocating memory for current\n");
        exit(1);
    }

    for (i=0;i<num_f;i++) samples_f[i]=0;
    for (i=0;i<num_f;i++) current[i]=0;

    // We read data from data file
    fd=open(argv[1],O_RDONLY);
    if (fd<0){
        perror("Error opening DB file");
        fprintf(stderr,"File %s\n",argv[1]);
        exit(1);
    }
    // Number of app (total instances, not different apps)
    num_app=(lseek(fd,0,SEEK_END)/sizeof(application_t));
    if (num_app<0){
        perror("Error calculating num apps");
        exit(1);
    }
    apps=(application_t *)malloc(sizeof(application_t)*num_app);
    if (apps==NULL){
        fprintf(stderr,"Error allocating memory for apps\n");
        exit(1);
    }
    samples_app=(unsigned int *)malloc(sizeof(unsigned int)*num_app);
    if (samples_app==NULL){
        fprintf(stderr,"Error allocating memory for samples_app\n");
        exit(1);
    }
    for(i=0;i<num_app;i++) samples_app[i]=0;
    sprintf(instances_filename,"EAR_INFO_%s/EAR_metrics.csv",nodename);
    instances_fd=open(instances_filename,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
    if (instances_fd<0){
        perror("Creating EAR_metrics.csv");
        exit(1);
    }
    write(instances_fd,header_instances,strlen(header_instances));
    fprintf(stdout,"Reading %u applications from DB....\n",num_app);
    if (lseek(fd,0,SEEK_SET)<0){
        perror("Error while executing lseek to offset=0");
        exit(1);
    }
    for (i=0;i<num_app;i++){
        if (read(fd,&test,sizeof(application_t))!=sizeof(application_t)){
            perror("Error reading app info");
            exit(1);
        }
        if (test.nominal>=min_f){
#ifdef DEBUG
            fprintf(stdout,"New instance %s(%u) : Time %lf CPI %lf TPI %lf Power %lf\n",test.app_id,test.nominal,test.seconds,test.CPI,test.TPI_f0,test.POWER_f0);
#endif
            //NAME;FREQ;TIME;CPI;TPI;GBS;POWER
            sprintf(EAR_metric,"%s;%u;%lf;%lf;%lf;%lf;%lf\n",test.app_id,test.nominal,test.seconds,test.CPI,test.TPI_f0,test.GBS_f0,test.POWER_f0);
            write(instances_fd,EAR_metric,strlen(EAR_metric));
            if ((posApp=TestAlreadyExists(apps,filtered_apps,&test))>=0){
                AcumApp(&apps[posApp],&test);
                samples_app[posApp]++;
            }else{
                CopyApp(&apps[filtered_apps],&test);
                samples_app[filtered_apps]=1;
                filtered_apps++;
            }
        } //min_f
    }
    close(instances_fd);
    // We will consider only applictions with f>=min_f
    num_app=filtered_apps;
    // We must compute the average per (app,f)
    for (i=0;i<num_app;i++){
        AverageApplication(&apps[i],samples_app[i]);
    }
    fprintf(stdout,"There are %d applications with f>=%u\n",num_app,min_f);
    close(fd);
    // we maintain the name's of applications to generate graphs
    InitApplicationNames(num_app);
    for (current_app=0;current_app<num_app;current_app++){
        if (apps[current_app].nominal>=min_f){
            samples_f[F2I(apps[current_app].nominal)]++;
            // We maintain here the list of application names
            if (ApplicationExist(apps[current_app].app_id)==0)	AddApplication(apps[current_app].app_id);
        }
    }
    fprintf(stdout,"Applications grouped by names\n");
    // We group applications with frequencies
    apps_ordered=(application_t**)malloc(sizeof(application_t*)*num_f);
    if (apps_ordered==NULL)	{
        fprintf(stderr,"Error allocating memory for apps_ordered\n");
        exit(1);
    }
    for (i=0;i<num_f;i++){
        apps_ordered[i]=(application_t *)malloc(sizeof(application_t)*samples_f[i]);
        if (apps_ordered[i]==NULL){
            fprintf(stderr,"Error allocating memory for apps_ordered[%u]\n",i);
            exit(1);
        }
    }
    fprintf(stdout,"Sorting applications by frequency......\n");
    unsigned int i_f;
    for (current_app=0;current_app<num_app;current_app++){
        f=apps[current_app].nominal;
        if (f>=min_f){
            i_f=F2I(f);
            pos=current[i_f];
            CopyApp(&apps_ordered[i_f][pos],&apps[current_app]);
            current[i_f]++;
        }
    }
    fprintf(stdout,"Let's compute the coefficients.....\n");
    Coefs=(struct Coefficients_info **) malloc(sizeof(struct Coefficients_info*)*num_f);
    if (Coefs==NULL){
        fprintf(stderr,"Error allocating memory for coefficients_info vector\n");
        exit(1);
    }
    for(f=0;f<num_f;f++){
        Coefs[f]=(struct Coefficients_info *)malloc(sizeof(struct Coefficients_info)*num_f);
        if (Coefs[f]==NULL){
            fprintf(stderr,"Error allocating memory for coefficients_info f=%u\n",f);
            exit(1);
        }
        else{
            for (i=0;i<num_f;i++) Coefs[f][i].available=0;
        }
    }
    InitCoefs(0,0,nom_f,1,0,0,1,0,0);
    double A,B,C,D,E,F;
    // This file is to load in an excel file
    sprintf(coef_file,"EAR_INFO_%s/coefficients_txt.csv",nodename);
    fprintf(stdout,"Creating %s\n",coef_file);
    fd_csv_coeffs=open(coef_file,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
    if (fd_csv_coeffs<0){
        perror("Error creating coefficients text file\n");
        exit(1);
    }
    // HEADER
    sprintf(men,"REF;F;A;B;C;D;E;F\n");
    write(fd_csv_coeffs,men,strlen(men));
    // We compute regression
    for (ref=0;ref<num_f;ref++){
        if (samples_f[ref]){
            fprintf(stdout,"......REFERENCE %u................\n",I2F(ref));
            sprintf(coef_file,"%s.%u",argv[2],I2F(ref));
            fd=open(coef_file,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
            if (fd<0){
                perror("Error creating coefficients file\n");
                exit(1);
            }
            for (f=0;f<num_f;f++){ // Coefficients per frequency
                fprintf(stdout,"----------------------------------------------------------\n");
                if (ref==f){
                    InitCoefs(ref,f,I2F(f),1,0,0,1,0,0);
                }else{
                    int n = samples_f[f];
                    if (n>0){
                        fprintf(stdout,"Computing POWER regression for freq %u with %u samples (REF=%u)\n",I2F(f),samples_f[f],I2F(ref));
                        gsl_matrix *SIGNATURE_POWER = gsl_matrix_calloc(n, 3);
                        gsl_vector *POWER = gsl_vector_alloc(n);
                        gsl_vector *COEFFS = gsl_vector_alloc(3);

                        for (i = 0; i < n; i++) {
                            // POWER
                            gsl_vector_set(POWER, i, apps_ordered[f][i].POWER_f0);

                            Nominal_for_power(ref,apps_ordered[f][i].app_id,&power,&tpi);

                            // SIGNATURE VALUES
                            gsl_matrix_set(SIGNATURE_POWER, i, 0, 1);
                            gsl_matrix_set(SIGNATURE_POWER, i, 1, power);
                            gsl_matrix_set(SIGNATURE_POWER, i, 2, tpi);
                        }

                        double chisq;
                        gsl_matrix *cov = gsl_matrix_alloc(3, 3);
                        gsl_multifit_linear_workspace * wspc = gsl_multifit_linear_alloc(n, 3);
                        gsl_multifit_linear(SIGNATURE_POWER, POWER, COEFFS, cov, &chisq, wspc);

                        fprintf(stdout,"Coefficient for power: %g*POWER_f0 + %g*TPI_f0 + %g\n",
                                gsl_vector_get(COEFFS,1),gsl_vector_get(COEFFS,2),gsl_vector_get(COEFFS,0));
                        A=gsl_vector_get(COEFFS,1);
                        B=gsl_vector_get(COEFFS,2);
                        C=gsl_vector_get(COEFFS,0);
                        gsl_matrix_free(SIGNATURE_POWER);
                        gsl_matrix_free(cov);
                        gsl_vector_free(POWER);
                        gsl_vector_free(COEFFS);
                        gsl_multifit_linear_free(wspc);
                    }
                    if (n>0){
                        fprintf(stdout,"Computing CPI regression for freq %u with %u samples (REF=%u)\n",I2F(f),samples_f[f],I2F(ref));
                        gsl_matrix *SIGNATURE_CPI = gsl_matrix_calloc(n, 3);
                        gsl_vector *CPI = gsl_vector_alloc(n);
                        gsl_vector *COEFFS = gsl_vector_alloc(3);

                        for (i = 0; i < n; i++) {
                            // CPI
                            gsl_vector_set(CPI, i, apps_ordered[f][i].CPI);

                            Nominal_for_cpi(ref,apps_ordered[f][i].app_id,&cpi,&tpi);

                            // SIGNATURE VALUES
                            gsl_matrix_set(SIGNATURE_CPI, i, 0, 1);
                            gsl_matrix_set(SIGNATURE_CPI, i, 1, cpi);
                            gsl_matrix_set(SIGNATURE_CPI, i, 2, tpi);
                        }

                        double chisq;
                        gsl_matrix *cov = gsl_matrix_alloc(3, 3);
                        gsl_multifit_linear_workspace * wspc = gsl_multifit_linear_alloc(n, 3);
                        gsl_multifit_linear(SIGNATURE_CPI, CPI, COEFFS, cov, &chisq, wspc);

                        fprintf(stdout,"Coefficient for cpi: %g*CPI_f0 + %g*TPI_f0 + %g\n",
                                gsl_vector_get(COEFFS,1),gsl_vector_get(COEFFS,2),gsl_vector_get(COEFFS,0));
                        D=gsl_vector_get(COEFFS,1);
                        E=gsl_vector_get(COEFFS,2);
                        F=gsl_vector_get(COEFFS,0);

                        gsl_matrix_free(SIGNATURE_CPI);
                        gsl_matrix_free(cov);
                        gsl_vector_free(CPI);
                        gsl_vector_free(COEFFS);
                        gsl_multifit_linear_free(wspc);

                        InitCoefs(ref,f,I2F(f),A,B,C,D,E,F);
                    }
                }
            } //F
            if (write(fd,Coefs[ref],sizeof(struct Coefficients_info)*num_f)!=(sizeof(struct Coefficients_info)*num_f)){
                perror("Error writting coefficients file");
                exit(1);
            }
            WriteInText(fd_csv_coeffs,ref,Coefs[ref],num_f);
            close(fd);
        }
    } //Reference
    close(fd_csv_coeffs);
    sprintf(coef_file,"EAR_INFO_%s/coeffs_validation.csv",nodename);
    fd_csv=open(coef_file,O_WRONLY|O_CREAT|O_TRUNC,CREATE_FLAGS);
    if (fd_csv<0){
        perror("Error creating validation file");
        exit(1);
    }
    write(fd_csv,HEADER_CSV,strlen(HEADER_CSV));
    // Creating graph files, it is pending to generate data files
    for (i=0;i<Diff_apps;i++){
        if (fd_graphs=CreateGraph(APPS_NAMES[i])){
            AddCurvesPower(fd_graphs,APPS_NAMES[i]);
            AddCurvesCPI(fd_graphs,APPS_NAMES[i]);
            AddCurvesTime(fd_graphs,APPS_NAMES[i]);
        }
    }

}