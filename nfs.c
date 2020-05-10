#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libperfstat.h>

#define ASSERT(arg1) \
	if( !(arg1) ) { \
		printf("Assert Failure in file=\"%s\" at line=%d asserting=[[%s]]\n", __FILE__,__LINE__,"arg1"); \
		exit(2); \
	}

#define ASSERT_SYSCALL(arg1) \
	if( (arg1) == -1 ) { \
		printf("Syscall Assert Failure in file=\"%s\" at line=%d errno=%d asserting=[[%s]]\n", __FILE__,__LINE__,errno,"arg1"); \
		if(errno != 0) perror("errno means "); \
		exit(2); \
	}

#define ASSERT_NONZERO(arg1) \
	if( (char *)(arg1) == (char *)0) { \
		printf("Pointer Assert Failure in file=\"%s\" at line=%d asserting-non-zero=[[%s]]\n", __FILE__,__LINE__,"arg1"); \
		exit(3); \
	}

int main(int argc, char** argv) {
	int i;
	int protocols;
	perfstat_protocol_t *stat;

	 printf("sizeof perfstat_protocol_t =  %d\n", sizeof(perfstat_protocol_t));


	/*protocols = perfstat_protocol(NULL, NULL, sizeof(perfstat_protocol_t), 0);*/
	ASSERT_SYSCALL(protocols = perfstat_protocol(NULL, NULL, sizeof(perfstat_protocol_t), 0));

	ASSERT_NONZERO(stat = malloc(sizeof(perfstat_protocol_t)*protocols) );

	ASSERT_SYSCALL(protocols = perfstat_protocol((perfstat_id_t *)FIRST_PROTOCOL, stat, sizeof(perfstat_protocol_t), protocols) );

    while(1) {
	for (i = 0; i < protocols; i++) {
		printf("protocol=%d name=%s\n", i, stat[i].name);

		if(!strncmp("ip",stat[i].name,3)) {
                        printf("\tIP: input packets            =%16llu\n", stat[i].u.ip.ipackets);
                        printf("\tIP: input errors             =%16llu\n", stat[i].u.ip.ipackets);
                        printf("\tIP: input queue over flows   =%16llu\n", stat[i].u.ip.iqueueoverflow);
                        printf("\tIP: output packets           =%16llu\n", stat[i].u.ip.opackets); 
                        printf("\tIP: output errors            =%16llu\n", stat[i].u.ip.oerrors); 
                }
		if(!strncmp("ipv6",stat[i].name,5)) {
                        printf("\tIPv6: input packets          =%16llu\n", stat[i].u.ipv6.ipackets);
                        printf("\tIPv6: input errors           =%16llu\n", stat[i].u.ipv6.ipackets);
                        printf("\tIPv6: input queue over flows =%16llu\n", stat[i].u.ipv6.iqueueoverflow);
                        printf("\tIPv6: output packets         =%16llu\n", stat[i].u.ipv6.opackets); 
                        printf("\tIPv6: output errors          =%16llu\n", stat[i].u.ipv6.oerrors); 
                }
		if(!strncmp("icmp",stat[i].name,5)) {
                        printf("\tIMCP: packets received       =%16llu\n", stat[i].u.icmp.received);
                        printf("\tIMCP: packets sent           =%16llu\n", stat[i].u.icmp.sent);
                        printf("\tIMCP: errors                 =%16llu\n", stat[i].u.icmp.errors);
                }
		if(!strncmp("icmpv6",stat[i].name,7)) {
                        printf("\tIMCPv6: packets received     =%16llu\n", stat[i].u.icmpv6.received);
                        printf("\tIMCPv6: packets sent         =%16llu\n", stat[i].u.icmpv6.sent);
                        printf("\tIMCPv6: errors               =%16llu\n", stat[i].u.icmpv6.errors);
                }
		if(!strncmp("udp",stat[i].name,4)) {
                        printf("\tIPv6: input packets          =%16llu\n", stat[i].u.udp.ipackets);
                        printf("\tIPv6: input errors           =%16llu\n", stat[i].u.udp.ipackets);
                        printf("\tIPv6: output packets         =%16llu\n", stat[i].u.udp.opackets); 
                        printf("\tIPv6: output packets dropped =%16llu (no socket)\n", stat[i].u.udp.no_socket); 
                }
		if(!strncmp("tcp",stat[i].name,4)) {
                        printf("\tTCP: input packets           =%16llu\n", stat[i].u.tcp.ipackets);
                        printf("\tTCP: input errors            =%16llu\n", stat[i].u.tcp.ipackets);
                        printf("\tTCP: output packets          =%16llu\n", stat[i].u.tcp.opackets); 
                        printf("\tTCP: connections initiated   =%16llu\n", stat[i].u.tcp.initiated); 
                        printf("\tTCP: connections accepted    =%16llu\n", stat[i].u.tcp.accepted); 
                        printf("\tTCP: connections established =%16llu\n", stat[i].u.tcp.established); 
                        printf("\tTCP: connections dropped     =%16llu\n", stat[i].u.tcp.dropped); 
                }
		if(!strncmp("rpc",stat[i].name,4)) {
			printf("\tRPC: client calls            =%16llu\n", stat[i].u.rpc.client.stream.calls);
			printf("\tRPC: server calls            =%16llu\n", stat[i].u.rpc.client.dgram.calls);
		}

		if(!strncmp("nfs",stat[i].name,4)) {
			printf("\tNFS: client calls            =%16llu\n", stat[i].u.nfs.client.calls);
			printf("\tNFS: server calls            =%16llu\n", stat[i].u.nfs.server.calls);
		}
		if(!strncmp("nfsv2",stat[i].name,6)) {
			printf("\tNFSv2: client calls          =%16llu\n", stat[i].u.nfsv2.client.calls);
			printf("\tNFSv2: server calls          =%16llu\n", stat[i].u.nfsv2.server.calls);
		}
		if(!strncmp("nfsv3",stat[i].name,6)) {
			printf("\tNFSv3: client calls          =%16llu\n", stat[i].u.nfsv3.client.calls);
			printf("\tNFSv3: server calls          =%16llu\n", stat[i].u.nfsv3.server.calls);
		}
	}
	sleep(1);
    }
}
