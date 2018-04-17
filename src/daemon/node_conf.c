/**************************************************************
*	Energy Aware Runtime (EAR)
*	This program is part of the Energy Aware Runtime (EAR).
*
*	EAR provides a dynamic, dynamic and ligth-weigth solution for
*	Energy management.
*
*    	It has been developed in the context of the Barcelona Supercomputing Center (BSC)-Lenovo Collaboration project.
*
*       Copyright (C) 2017  
*	BSC Contact 	mailto:ear-support@bsc.es
*	Lenovo contact 	mailto:hpchelp@lenovo.com
*
*	EAR is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*	
*	EAR is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*	
*	You should have received a copy of the GNU Lesser General Public
*	License along with EAR; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*	The GNU LEsser General Public License is contained in the file COPYING	
*/


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <daemon/node_conf.h>

/*
typedef struct node_conf{
	char nodename[MAX_NODENAME_PATH];
}node_conf_t;
*/

node_conf_t *global_node_list;
static uint whoami;

void longname2short(char *name)
{
	int i=0;
	while((name[i]!='.') && (name[i]!='\0')) i++;
	name[i]='\0';
}

/** This function reads the list of nodes from the file and creates a node list, memory is allocated and must be released
 * with node_list_dispose function
*/
node_conf_t *read_node_list(char *node_conf_file,uint *nodes)
{
	FILE* fd;
	uint node_cnt;
	uint num_nodes;
	char curr_node[MAX_NODENAME_PATH],my_node[MAX_NODENAME_PATH],my_node_short[MAX_NODENAME_PATH];
	gethostname(my_node, sizeof(my_node));
	strcpy(my_node_short,my_node);
	/* We create the short name */
	longname2short(my_node_short);
	fd=fopen(node_conf_file,"r");
	if (fd==NULL){
		fprintf(stderr,"Error opening %s file (%s)\n",node_conf_file,strerror(errno));
		return NULL;
	}
	if (fscanf(fd,"%u\n",&num_nodes)!=1){
		fprintf(stderr,"Error, num_nodes not available in %s\n",node_conf_file);
		fclose(fd);
		return NULL:
	}
	assert(num_nodes>0);
	global_node_list=(node_conf_t *)malloc(num_nodes*sizeof(node_conf_t));
	if (global_node_list==NULL){
		fprintf(stderr,"Error allocating memory for global_node_list (%s)\n",strerror(errno));
		return NULL;
	}
	memset(global_node_list,0,num_nodes*sizeof(node_conf_t));
	while (fscanf(fd,"%s\n",curr_node)){
		fprintf(stderr,"Node %d=%s\n",node_cnt,curr_node);
		assert(node_cnt<num_nodes);
		global_node_list[node_cnt].nodename=malloc(strlen(curr_node)+1);
		if (global_node_list[node_cnt].nodename==NULL){
			fprintf(stderr,"Error allocating memory for global_node_list (%s)\n",strerror(errno));
			return NULL;
		}
		strcpy(global_node_list[node_cnt].nodename,curr_node);			
		if ((strcmp(curr_node,my_node)==0) || (strcmp(curr_node,my_node_short)==0)){
			whoami=node_cnt;
			fprintf(stderr,"My node is at node_cnt %u\n",node_cnt);
		}
		node_cnt++;
	}
	fprintf(stderr,"%d nodes detected\n",node_cnt);
	*nodes=node_cnt;
	return global_node_list;	
}
/** releases the memory previously allocated with read_node_list and/or create_forward_list function
 */
void node_list_dispose(node_conf_t *node_list,uint nodes)
{
	uint i;
	if (node_list!=NULL){ 
		for (i=0;i<nodes;i++){
			free(node_list[i]);
		}
		free(node_list);
	}
}

uint compute_levels(uint nodes, uint tree_with)
{
	uint i=0;
	nodes_cov=1;
	do{
		i++;
		nodes_cov=nodes_cov*tree_with;
	}while(nodes_cov>=nodes);
	return i;
}

/** Based on TREE_WITH, creates a list of nodes where messages must be forwarded (null if this node is a leaf). It allocates memory
 */
node_conf_t *create_forward_list(node_conf_t * system_nodelist,uint nodes,uint *forward_list_size)
{
	char mynode[MAX_NODENAME_PATH];
	uint to_forward;
	uint levels;
	to_forward=TREE_WITH;	
	if (TREE_WITH==0){ 
		*forward_list_size=0;
		return NULL;
	}
	levels=compute_levels(nodes,TREE_WITH);
	fprintf(stderr,"We need %u to cover all the nodes\n",(int)levels);
}


#else
#endif
