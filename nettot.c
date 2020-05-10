#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libperfstat.h>

int main(int argc, char* argv[]) {
   int i, ret, tot;
   perfstat_netinterface_total_t net1;
   perfstat_netinterface_total_t net2;

   ret = perfstat_netinterface_total(NULL, &net1, sizeof(perfstat_netinterface_total_t), 1);
   for(;;) {

	sleep(1);
        ret = perfstat_netinterface_total(NULL, &net2, sizeof(perfstat_netinterface_total_t), 1);
   				 
   if (ret > 0) {
       printf("\nTotal Statistics for %d networks\n", net2.number);

       printf("Recv KB/s: %8llu KB\n", (net2.ibytes - net1.ibytes) / 1024);
       printf("Sent KB/s: %8llu KB\n", (net2.obytes - net1.obytes) / 1024 ); 

	memcpy(&net1, &net2, sizeof(perfstat_netinterface_total_t) );
     }
  }
}
