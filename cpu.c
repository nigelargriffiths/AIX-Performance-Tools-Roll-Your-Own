#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libperfstat.h>

int	main(int argc, char *argv[]) 
{
	perfstat_partition_total_t part;
	perfstat_cpu_t * statp;
	perfstat_cpu_t * statq;
	int	i, retcode, cputotal;
	unsigned long long timebase, timebase_saved, hardware_ticks;

	/* check how many perfstat_cpu_t structures are available */
	cputotal =  perfstat_cpu(NULL, NULL, sizeof(perfstat_cpu_t), 0);
	if(cputotal == -1) printf("ret=%d errono=%d perfstat_cpu(NULL)\n",retcode, errno);
	/* return code is number of structures returned */
	/*printf("number of perfstat_cpu_t available : %d\n", cputotal);*/

	/* allocate enough memory for all the structures */
	statp = malloc(cputotal * sizeof(perfstat_cpu_t));
	statq = malloc(cputotal * sizeof(perfstat_cpu_t));

	/* ask to get all the structures available in one call */
	retcode = perfstat_cpu((perfstat_id_t *)FIRST_CPU, statq, sizeof(perfstat_cpu_t), cputotal);
	if(retcode == -1) printf("ret=%d errono=%d perfstat_cpu(cputotal=%d)\n",retcode, errno, cputotal);

	for (; ; ) {
	    retcode = perfstat_partition_total(NULL, &part, sizeof(perfstat_partition_total_t), 1);
			hardware_ticks=part.timebase_last - timebase_saved;
			timebase_saved = part.timebase_last;
	    if(retcode == -1) printf("ret=%d errono=%d timebase=%llu\n",retcode, errno, hardware_ticks);
		sleep(1);
		retcode = perfstat_cpu((perfstat_id_t *)FIRST_CPU, statp, sizeof(perfstat_cpu_t), cputotal);
		if(retcode == -1) printf("ret=%d errono=%d perfstat_cpu(cputotal=%d)\n",retcode, errno, cputotal);

		printf("    Logical          Physical         Stats\n");
		printf("    usr:sys:idl:wait usr:sys:idl:wait syscall: read:write: fork: exec: charIO\n");
		for (i = 0; i < retcode; i++) {
			printf("%3d%3ld:%3ld:%3ld:%3ld", 
				i,
				(long)(statp[i].user  - statq[i].user),
				(long)(statp[i].sys   - statq[i].sys),
				(long)(statp[i].idle  - statq[i].idle),
				(long)(statp[i].wait  - statq[i].wait));
			
			printf("  %3ld:%3ld:%3ld:%3ld",    
				(long)((statp[i].puser - statq[i].puser)*100/hardware_ticks),
				(long)((statp[i].psys  - statq[i].psys )*100/hardware_ticks),
				(long)((statp[i].pidle - statq[i].pidle)*100/hardware_ticks),
				(long)((statp[i].pwait - statq[i].pwait)*100/hardware_ticks));

      			printf("  %7llu:%5llu:%5llu:%5llu:%5llu:%7llu\n", 
					statp[i].syscall  - statq[i].syscall,
					statp[i].sysread  - statq[i].sysread,
					statp[i].syswrite - statq[i].syswrite,
					statp[i].sysfork  - statq[i].sysfork,
					statp[i].sysexec  - statq[i].sysexec,
					statp[i].readch   - statq[i].readch +
					statp[i].writech  - statq[i].writech );
		}
		memcpy(statq, statp, sizeof(perfstat_cpu_t) * cputotal);
	}
}
