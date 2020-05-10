#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dr.h>

int main(int argc, char* argv[]) 
{
   int i, ret, tot;
   lpar_load_t load;
    lpar_info_format1_t f1;
    lpar_info_format2_t f2;

    while(1) {
	ret=getlparload(&load,sizeof(lpar_load_t));
	printf("\nlpar_load returned=%d loadavg=%d utilavg=%d\n",ret,load.loadavg,load.utilavg);

	ret=lpar_get_info(LPAR_INFO_FORMAT1,&f1,sizeof(f1));
	printf("lpar_get_info ret=%d version=%d\n"
		"maxmem   =%lld minmem =%lld\n"
		"memregion=%lld\n"
		"dispatch_wheel_time=%lld\n"
		"lparNo   =%4d\n"
		"lparflags=0x%4x\n"
		"max_pcpu =%4d\n"
		"minvcpu  =%4d maxvcpu =%4d\n"
		"minlcpu  =%4d maxlcpu =%4d\n"
		"mincap   =%4d maxcap  =%4d\n"
		"capincrements=%d\n"
		"smthreads=%d\n"
		"lparname=%s sanity=%d\n",
	ret,
	f1.version,
	f1.max_memory,
	f1.min_memory,
	f1.memory_region,
	f1.dispatch_wheel_time,
	f1.lpar_number,
	f1.lpar_flags,
	f1.max_pcpus_in_sys,
	f1.min_vcpus,
	f1.max_vcpus,
	f1.min_lcpus,
	f1.max_lcpus,
	f1.minimum_capacity,
	f1.maximum_capacity,
	f1.capacity_increment,
	f1.smt_threads,
	f1.lpar_name,1234);
	
	ret=lpar_get_info(LPAR_INFO_FORMAT2,&f2,sizeof(f2));
	printf("lpar_get_info ret=%d version=%d\n"
		"online_mem=%lld\n"
		"tot_dispatch_time=%lld\n"
		"pool_idle_time   =%lld\n"
		"dispatch_latency =%lld\n"
		"lpar_flags       =0x%4x\n"
		"pcpu_in_sys      =%4d\n"
		"online_vcpus     =%4d\n"
		"online_lcpus     =%4d\n"
		"pcpus_in_pool    =%4d\n"
		"unalloc_capacity =%4d\n"
		"entitled_capacity=%4d\n"
		"variable_weight  =%4d\n"
		"unalloc_weight   =%d\n"
		"min_req_vcpu_capacity=%d\n"
		"group_id =%d\n"
		"pool_id  =%d sanity=%d\n",
	ret,
	f2.version,
	f2.online_memory,
	f2.tot_dispatch_time,
	f2.pool_idle_time,
	f2.dispatch_latency,
	f2.lpar_flags,
	f2.pcpus_in_sys,
	f2.online_vcpus,
	f2.online_lcpus,
	f2.pcpus_in_pool,
	f2.unalloc_capacity,
	f2.entitled_capacity,
	f2.variable_weight,
	f2.unalloc_weight,
	f2.min_req_vcpu_capacity,
	f2.group_id,
	f2.pool_id,1234);
	
	sleep(1);
    }
}
