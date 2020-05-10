#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <libperfstat.h>

#define do(xxx) printf( "%20s=%lld\n", # xxx, mem.xxx);

int main(int argc, char *argv[]) {
   int retcode;
   perfstat_memory_total_t mem;
   
   while(1) {
	/* ask to get all the structures available in one call */
	retcode = perfstat_memory_total(NULL, 
					&mem, 
					sizeof(perfstat_memory_total_t), 
					1);
	/* return code is number of structures returned */
	if(retcode != 1)
	   printf("perfstat_memort_total returned=%d errono=%d sizeof(perfstat_memory_total_t)=%d\n", 
					retcode,
					errno,
					sizeof(perfstat_memory_total_t));
      printf("\nStatistics for Memory\n");
	do(virt_total);
	do(real_total);
	do(real_free);
	do(real_pinned);
	do(real_inuse);
	do(pgbad);
	do(pgexct);
	do(pgins);
	do(pgouts);
	do(pgspins);
	do(pgspouts);
	do(scans);
	do(cycles);
	do(pgsteals);
	do(numperm);
	do(pgsp_total);
	do(pgsp_free);
	do(pgsp_rsvd);
#ifdef _AIX53
	do(real_system);
	do(real_user);
	do(real_process);
	do(virt_active);
#endif
	sleep(1);
  }
}

