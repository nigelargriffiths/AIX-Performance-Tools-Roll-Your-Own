#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dr.h>

int main(int argc, char* argv[]) 
{
	int seconds=1;
	double  dispatched;
	double  saved_dispatched;
	lpar_info_format2_t f2;

	if(argc>1 && argv[1][0] == '-' && argv[1][1] == '?') {
		printf("%s [seconds]\n"
			"Print the LPAR Entitlement, Virtual Processor number and Physical CPU Used (Consumed)\n"
			"Default seconds=1\n\n",argv[0]);
		exit(1);
	}
	if(argc>1) seconds=atoi(argv[1]);
	if(seconds <1 || seconds >9999999)
		seconds=1;

	if(lpar_get_info(LPAR_INFO_FORMAT2,&f2,sizeof(f2)) == -1) {
		printf("lpar_get_info error\n");
		exit(2);
	}
	saved_dispatched = f2.tot_dispatch_time;
	sleep(seconds);

    while(1) {
	if(lpar_get_info(LPAR_INFO_FORMAT2,&f2,sizeof(f2)) == -1) {
		printf("lpar_get_info error\n");
		exit(3);
	}
	dispatched = (f2.tot_dispatch_time - saved_dispatched) / 1000.0 / 1000.0 / 1000.0 / (double) seconds; 
								/* it was in nano-seconds!! */
	saved_dispatched = f2.tot_dispatch_time;

	
	printf("E=%.2f VP=%d PhysicalCPUused=%.3f\n", ((float)f2.entitled_capacity)/100.0, f2.online_vcpus, dispatched);

	sleep(seconds);
    }
}
