#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <libperfstat.h>
#include <sys/systemcfg.h>

/* See /usr/include/sys/iplcb.h to explain the below */
#define XINTFRAC        ((double)(_system_configuration.Xint)/(double)(_system_configuration.Xfrac))

/* hardware ticks per millisecond */
#define HWTICS2MSECS(x)    ((double)x * XINTFRAC)/1000000.0

#ifndef FIRST_DISK
#define FIRST_DISK ""
#endif
char *fix(char *s)
{
	int j;
	for(j=0;j<IDENTIFIER_LENGTH;j++) {
		if( !(isalpha(s[j]) ||  
			isdigit(s[j]) || 
			s[j] == '-' ||
			s[j] == '_' ||
			s[j] == ' '
		     ) ) {
			s[j] = 0;
			break;
		}
	}
	return s;
}

int main(int argc, char* argv[]) 
{
	int i, j, ret, disks;
	perfstat_disk_t *a;
	perfstat_disk_t *b;
	perfstat_id_t name;
	char *substring;
	int elapsed=1;
	int ncpus;

/* This could be used to zap all the counters 
- it is not considered friendly to other tools!! */
/*	perfstat_partial_reset(NULL, FLUSH_DISK|RESET_DISK_MINMAX); */

	/* get the number of CPUs */
   	ncpus=perfstat_cpu(NULL, NULL, sizeof(perfstat_cpu_t), 0);
	printf("Number of CPUs = %d\n",ncpus);

	/* check how many perfstat_disk_t structures are available */
	disks = perfstat_disk(NULL, NULL, sizeof(perfstat_disk_t), 0);
	if(disks==-1) {
		perror("perfstat_disk(NULL)");
		exit(3);
	}

	/* allocate enough memory for all the structures */
	a = malloc( sizeof(perfstat_disk_t) * disks);
	b = malloc( sizeof(perfstat_disk_t) * disks);

	/* ask to get all the structures available in one call */
	/* return code is number of structures returned */
	strcpy(name.name,FIRST_DISK);
	ret = perfstat_disk(&name, a, sizeof(perfstat_disk_t), disks);

	if(disks==-1) {
		perror("perfstat_disk(a)");
		exit(4);
	}
	printf("Found %d disks\n",ret);

	printf("Description\n");
	for (i = 0; i < ret; i++) 
		printf("%10s \"%s\"\n",	fix(a[i].name), fix(a[i].description));
	printf("Details\n");
	for (i = 0; i < ret; i++) 
		printf("%10s VG=\"%16s\" Blocksize=%llu Rating=%8lluKB/sec\n",	
			fix(a[i].name), fix(a[i].vgname),
			a[i].bsize, a[i].xrate);

#ifdef AIX53
	printf("AIX53 Details\n");
	for (i = 0; i < ret; i++) 
		printf("%10s Paths=%u Adapter=\"%s\"\n",
		fix(a[i].name),
		b[i].paths_count, fix(b[i].adapter)); 
#endif
	printf("\n");

	for(;;) {
#define DELTA(member) (b[i].member - a[i].member)
		printf(" - - - - - - - - - - - - - - - - - - \n");
		sleep(elapsed);
		strcpy(name.name,FIRST_DISK);
		ret = perfstat_disk(&name, b, sizeof(perfstat_disk_t), disks);

		if(ret==-1) {
			perror("perfstat_disk(b)");
			exit(4);
		}

		for (i = 0; i < ret; i++) {
			printf("%-16s SizeMB FreeMB   xfers   ReadMB WriteMB   Time%% Qdepth\n",fix(b[i].name));
			printf("                %6llu %6llu   %6.1f   %6.1f  %6.1f   %5.1f %5llu\n",
				b[i].size,
				b[i].free,
				DELTA(xfers)/elapsed,
				(double)(DELTA(rblks)*(double)b[i].bsize/1024.0/(double)elapsed),
				(double)(DELTA(wblks)*(double)b[i].bsize/1024.0/(double)elapsed),
				(double)DELTA(time)/(double)elapsed,
				b[i].qdepth);

			if(  	b[i].name[0] =='c' && 
				b[i].name[1] =='d' &&
				isdigit(b[i].name[2]) )
				continue;
			printf("                Read minrserv avgrserv maxrserv rtimeout rfailed\n");
			printf("                     %8.1f %8.1f %8.1f %7llu %7llu\n",
#define NONZERO(x) ((x)?(x):1)
				(double)(HWTICS2MSECS(b[i].min_rserv)),
				(double)(HWTICS2MSECS(DELTA(rserv))/NONZERO(DELTA(__rxfers))),
				(double)(HWTICS2MSECS(b[i].max_rserv)),
				b[i].rtimeout,
				b[i].rfailed
				);
			printf("               Write minwserv avgwserv maxwserv wtimeout wfailed\n");
			printf("                     %8.1f %8.1f %8.1f %7llu %7llu\n",
				(double)(HWTICS2MSECS(b[i].min_wserv)),
				(double)(HWTICS2MSECS(DELTA(wserv))/NONZERO((DELTA(xfers)-DELTA(__rxfers)))),
				(double)(HWTICS2MSECS(b[i].max_wserv)),
				b[i].wtimeout,
				b[i].wfailed
				);
			printf("               Queue mintime* avgtime* maxtime* avgWQsz avgSQsz SQfull wq_depth\n");
			printf("              *=msec %8.1f %8.1f %8.1f %7.1f %7.1f %3llu %5llu\n",
				(double)(HWTICS2MSECS(b[i].wq_min_time)),
				(double)(HWTICS2MSECS(DELTA(wq_time))/NONZERO(DELTA(xfers))/elapsed),
				(double)(HWTICS2MSECS(b[i].wq_max_time)),
				(double)(DELTA(wq_sampled))/(100.0*(double)elapsed*(double)ncpus),
				(double)(DELTA(q_sampled))/(100.0*(double)elapsed*(double)ncpus),
				DELTA(q_full),
				b[i].wq_depth);
		}
		printf("\n\n");
		memcpy(a,b,sizeof(perfstat_disk_t) * disks );
	}
}
