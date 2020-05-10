/* Example of getting logical and Physical CPU data for POWER5 and AIX 5.3 machines*/
/* The data may not be valid on other hardware or AIX versions */
/* Compile with:  cc -lperfstat cpus.c */

#include <stdio.h>
#include <sys/time.h>
#include <libperfstat.h>
#include <sys/dr.h>


/* macro to calculate the difference between previous and current values */
#define DELTA(member) (p->member - q->member) 

/* the two copies of the cpu data */
perfstat_cpu_total_t cpu_total[2];

perfstat_cpu_total_t *q; /* current snapshot */
perfstat_cpu_total_t *p; /* previous snapsot */
perfstat_cpu_total_t *t; /* temporary */

lpar_info_format2_t f2;

int
main(int argc, char *argv[]) 
{
	double	total;
	double	ptotal;
	double	dispatched;
	double	saved_dispatched;
	double	elapsed;
	int count =0;
	int max = 9999999;
	int seconds=1;

	if(argc>1) seconds=atoi(argv[1]);
	if(argc>2) max=atoi(argv[2]);
	if(seconds!=1 || max != 9999999)
		printf("%s interval=%d count=%d\n\n",argv[0],seconds,max);
	/* setup the pointers */
	q = &cpu_total[0];
	p = &cpu_total[1];
	/* get initial set of data */
	perfstat_cpu_total(NULL, q, sizeof(perfstat_cpu_total_t), 1);

	/* print general processor information */
	printf("Processors active: %d configured: %d type: %s running at %llu MHz\n",
	    q->ncpus, q->ncpus_cfg, q->description, q->processorHZ / 1000000);

	lpar_get_info(LPAR_INFO_FORMAT2, &f2, sizeof(f2));
	saved_dispatched = f2.tot_dispatch_time;

	printf(" L o g i c a l                       P h y s i c a l            Physical\n");
	printf("User    Sys   Idle   Wait |RunQueue |User    Sys   Idle   Wait  [CPU Time] Ticks\n");

	while (count++ < max) {
		sleep(seconds);

		/* get new values after one second */
		perfstat_cpu_total(NULL, p, sizeof(perfstat_cpu_total_t), 1);
		lpar_get_info(LPAR_INFO_FORMAT2, &f2, sizeof(f2));
		dispatched = (f2.tot_dispatch_time - saved_dispatched) / 1000.0 / 1000.0 / 1000.0; /* it was in nano-seconds!! */
		saved_dispatched = f2.tot_dispatch_time;

		/* calculate current total number of ticks */
		/* This is used in case this program is not exaclt in returning to the CPU 
		and more than one seconds worth of clock times occur */
		total  = DELTA(user) + DELTA(sys) + DELTA(idle) + DELTA(wait);
		ptotal  = DELTA(puser) + DELTA(psys) + DELTA(pidle) + DELTA(pwait);

		/* print percentages, total delta ticks and tick rate per cpu per sec */
		printf("%#5.1f  %#5.1f  %#5.1f  %#5.1f |%8llu|%#5.1f  %#5.1f  %#5.1f  %#5.1f [%#8.2f] %#8.0f\n",
		    100.0 * (double) DELTA(user) /  total,
		    100.0 * (double) DELTA(sys)  / total,
		    100.0 * (double) DELTA(idle) / total,
		    100.0 * (double) DELTA(wait) / total,
		    DELTA(runque),
		    100.0 * (double) DELTA(puser) / ptotal,
		    100.0 * (double) DELTA(psys)  / ptotal,
		    100.0 * (double) DELTA(pidle) / ptotal,
		    100.0 * (double) DELTA(pwait) / ptotal, 
			dispatched, ptotal);

		/* Swap the pointer around ready for next time */
		t = p;
		p = q;
		q = t;
	}
}


