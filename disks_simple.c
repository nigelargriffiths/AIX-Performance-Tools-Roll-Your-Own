#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <libperfstat.h>

#ifndef FIRST_DISK
#define FIRST_DISK ""
#endif

/* This strips out junk characters found in the names 
	to work around bugs in some versions */
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
	int seconds = 1;

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
	name.name[0]=0;
	ret = perfstat_disk(&name, a, sizeof(perfstat_disk_t), disks);
	if(disks==-1) {
		perror("perfstat_disk(a)");
		exit(4);
	}
	printf("found %d disks\n",ret);

	for (i = 0; i < ret; i++) {
		printf("%10s \"%s\" VG=\"%s\" Blocksize=%llu\n",	
			fix(a[i].name),
			fix(a[i].description),
			fix(a[i].vgname),
			a[i].bsize);

#ifdef AIX53
		printf("Paths=%u Adapter=\"%s\"\n",b[i].paths_count, fix(b[i].adapter));
#endif
	}
	printf("\n\n");

	for(;;) {
#define DELTA(member) (b[i].member - a[i].member)
		sleep(seconds);
		name.name[0]=0;
		ret = perfstat_disk(&name, b, sizeof(perfstat_disk_t), disks);
		if(ret==-1) {
			perror("perfstat_disk(b)");
			exit(4);
		}

		printf("Disk Name         Size    Free    Transfers   Reads  Writes    Time  Qdepth\n");
		printf("Disk Name         MB      MB                  Blocks Blocks    percent\n" );

		for (i = 0; i < ret; i++) {
			printf("%16s %7llu %7llu %10llu %7llu %7llu %7llu %7llu\n",
				fix(b[i].name),
				b[i].size,
				b[i].free,
				DELTA(xfers)/seconds,
				DELTA(rblks)/seconds,
				DELTA(wblks)/seconds,
				DELTA(time)/seconds,
				b[i].qdepth);
		}
		printf("\n\n");
		memcpy(a,b,sizeof(perfstat_disk_t) * disks );
	}
}
