#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>

int	main(int argc, char*argv[]) 
{
	int	i;
	int	paths;
	int	disks;
	perfstat_diskpath_t * statp;
	perfstat_disk_t dstat;
	char	*substring;

	/* check how many perfstat_diskpath_t structures are available */
	paths = perfstat_diskpath(NULL, NULL, sizeof(perfstat_diskpath_t), 0);

	/* allocate enough memory for all the structures */
	statp = calloc(paths, sizeof(perfstat_diskpath_t));
	perfstat_id_t first;
#ifndef FIRST_DISKPATH
#define FIRST_DISKPATH ""   
#endif

	while (1) {
		/* ask to get all the structures available in one call */
		/* return code is number of structures returned */
		paths = perfstat_diskpath((perfstat_id_t * )FIRST_DISKPATH, statp, sizeof(perfstat_diskpath_t), paths);
		printf("Found %d disk paths\n", paths);

		/* print statistics for each of the disk paths */
		for (i = 0; i < paths; i++) {
			printf("\nStatistics for disk path : %s\n", statp[i].name);
			printf("----------------------\n");
			printf("xrate         : %llu KB/sec\n", statp[i].rblks);
			printf("xfers         : %llu transfers\n", statp[i].rblks);
			printf("blocks read   : %llu 512 byte blocks\n", statp[i].rblks);
			printf("blocks written: %llu 512 byte blocks\n", statp[i].wblks);
			printf("time (active) : %llu\n", statp[i].wblks);
			printf("adapter name  : %s\n", statp[i].adapter);
		}

		/* retrieve paths for last disk if any */
		if (paths > 0) {
			/* extract the disk name from the last disk path name */
			substring = strstr(statp[paths - 1].name, "_Path");
			if (substring == NULL) {
				return (-1);
			}
			substring[0] = '\0';

			/* set name to the disk name */
			strcpy(first.name, statp[paths-1].name);

			/* retrieve info about disk */
			disks = perfstat_disk(&first, &dstat, sizeof(perfstat_disk_t), 1);
			printf("----------------------\n");


			/* print statistics for each of the paths */
			for (i = 0; i < disks; i++) {
				printf("\nStatistics for disk path : %s\n", statp[i].name);
				printf("----------------------\n");
				printf("number of blocks read     : %llu\n", statp[i].rblks);
				printf("number of blocks written  : %llu\n", statp[i].wblks);
				printf("adapter name              : %s\n", statp[i].adapter);
			}
		}
		sleep(1);
	}
}


