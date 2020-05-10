#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>

int main(int argc, char* argv[]) {
   int i, ret, tot;
   perfstat_disk_total_t data1;
   perfstat_disk_total_t data2;

   ret = perfstat_disk_total(NULL, &data1, sizeof(perfstat_disk_total_t), 1);
   for(;;) {

	sleep(1);
        ret = perfstat_disk_total(NULL, &data2, sizeof(perfstat_disk_total_t), 1);
   				 
   if (ret > 0) {
       printf("\nStatistics for disk total\n");
       printf("Disks   : %8d\n", data2.number);
       printf("Size MB : %8llu\n", data2.size);
       printf("Free MB : %8llu\n", data2.free);

       printf("xfers   : %8llu\n", data2.xfers - data1.xfers);
/*       printf("xrate   : %8llu\n", data2.xrate - data1.xrate); not supported on all disks */
       printf("Read    : %8llu KB\n", (data2.rblks - data1.rblks) /2 ); /* 512 bytes -> 1KB */
       printf("Write   : %8llu KB\n", (data2.wblks - data1.wblks) /2 ); /* 512 bytes -> 1KB */
       printf("Busy%%   : %8llu total for all disks (possible 100%% for each disk)\n", data2.time  - data1.time );

	memcpy(&data1, &data2, sizeof(perfstat_disk_total_t) );
     }
  }
}
