#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/vminfo.h>


#define do(xxx) printf( "%20s=%lld\n", # xxx, mem.xxx);

int main(int argc, char *argv[]) {
   int retcode;
   struct vminfo mem;
   
   while(1) {
	retcode = vmgetinfo(&mem, VMINFO, sizeof(struct vminfo));
	if(retcode < 0) {
		printf("vmgetinfo failed retcode=%d errno=%d\n",retcode,errno);
		return 42;
	}
	do(minperm);
	do(maxperm);
	do(minfree);
	do(maxfree);
	do(minpgahead);
	do(maxpgahead);
	sleep(1);
  }
}

