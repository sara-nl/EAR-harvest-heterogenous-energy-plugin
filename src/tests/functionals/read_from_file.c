

#include <common/types/application.h>


void main(void)
{
    application_t *apps;
    int num_apps = 0;
    num_apps = read_application_text_file("/home/xlalonso/test2.csv", &apps, 0);

    int i;
    for (i = 0; i < num_apps; i++)
        report_application_data(&apps[i]);
}
