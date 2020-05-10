#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libperfstat.h>

int	main(int argc, char*argv[]) 
{
	int	i;
	int	loop;
	int	ret;
	int	adapters;
	perfstat_diskadapter_t * astat;
	perfstat_diskadapter_t * save;

	/* find out the number of adapters */
	adapters = perfstat_diskadapter(NULL, NULL, sizeof(perfstat_diskadapter_t), 0);
	if (adapters == -1) {
		printf("ERROR: adapters=%d errno=%d\n", adapters, errno);
		perror("x");
		exit(43);
	}
	printf("%d adapter(s) found\n",adapters);
	/* just assume these work OK, so no error checking */
	astat = malloc(sizeof(perfstat_diskadapter_t) * adapters);
	save  = malloc(sizeof(perfstat_diskadapter_t) * adapters);

	if(astat <= 0 || save <= 0) {
		perror("malloc failed");
		exit(42);
	}
		
	for (loop = 1; ; loop++) { /* print statistics for each of the adapters */
		ret = perfstat_diskadapter((perfstat_id_t * )FIRST_DISKADAPTER, astat, sizeof(perfstat_diskadapter_t), adapters);
		if (ret == -1) {
			printf("ERROR: adapters=%d retcode=%d errno=%d\n", adapters, ret, errno);
			perror("perfstat_diskadapter");
			exit(42);
		}
		if (loop != 1) {
		    for (i = 0; i < ret; i++) {
			printf("Statistics for adapter    : %s which is adapter number=%d\n", astat[i].name,i+1);
			printf("description               : %s\n", astat[i].description);
			printf("number of devices         : %d\n", astat[i].number);
			printf("Size  and Free (MB)       : %llu   %llu\n", astat[i].size, astat[i].free);
			printf("Capability KB/sec         : %llu\n", astat[i].xrate);
			printf("transfers                 : %-20llu delta=%llu\n", astat[i].xfers, astat[i].xfers - save[i].xfers);
			printf("number of blocks read     : %-20llu delta=%llu\n", astat[i].rblks, astat[i].rblks - save[i].rblks);
			printf("number of blocks written  : %-20llu delta=%llu\n", astat[i].wblks, astat[i].wblks - save[i].wblks);
			printf("time                      : %-20llu delta=%llu\n", astat[i].time,  astat[i].time  - save[i].time);
		    }
		    sleep(1);
		}
		memcpy(save, astat, sizeof(perfstat_diskadapter_t) * adapters);
		printf("\n");
	}
}


