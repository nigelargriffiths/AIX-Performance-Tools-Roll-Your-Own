#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libperfstat.h>

#ifndef FIRST_NETINTERFACE 
#define FIRST_NETINTERFACE ""
#endif
int main(int argc, char* argv[]) {
	int i, ret, tot;
	perfstat_netinterface_t *stat;

	tot = perfstat_netinterface(NULL, NULL, 
	    sizeof(perfstat_netinterface_t), 0);

	stat = malloc(sizeof(perfstat_netinterface_t)*tot);

	ret = perfstat_netinterface((perfstat_id_t *)FIRST_NETINTERFACE, stat, sizeof(perfstat_netinterface_t),tot);
    while(1) {
	ret = perfstat_netinterface((perfstat_id_t *)"", stat, sizeof(perfstat_netinterface_t),tot);
	if(ret <= 0) {
		printf("tot=%d ret=%d errno=%d\n",tot,ret,errno);
		perror("x");
		exit(42);
	}

	for (i = 0; i < ret; i++) {
		printf("\nStatistics for networks : %s\n", stat[i].name);
		printf("----------------------\n");
		printf("mtu             : %llu\n", stat[i].mtu);
		printf("description     : %s\n",   stat[i].description);
		printf("ipackets        : %d\n",   stat[i].ipackets);
		printf("opackets        : %llu\n", stat[i].opackets);
		printf("ibytes          : %d\n",   stat[i].ibytes);
		printf("obytes          : %llu\n", stat[i].obytes);
		printf("ierrors         : %d\n",   stat[i].ierrors);
		printf("oerrors         : %llu\n", stat[i].oerrors);
		printf("collisions      : %d\n",   stat[i].collisions);
#if (_AIX52 | _AIX53)
		printf("bitrate   bit/sec: %llu\n", stat[i].bitrate); 
		printf("bitrate mbits/sec: %llu\n", stat[i].bitrate/1024/1024); 
#endif
	}
	sleep(1);
    }
}
