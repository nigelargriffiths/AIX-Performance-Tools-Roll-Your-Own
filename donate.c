#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libperfstat.h>
#include <sys/systemcfg.h>

#define XINTFRAC	((double)(_system_configuration.Xint)/(double)(_system_configuration.Xfrac))
#define HTIC2SEC(x)	((double)x * XINTFRAC)/(double)1000000000.0

u_longlong_t last_time_base;
u_longlong_t last_pcpu_user;
u_longlong_t last_pcpu_sys;
u_longlong_t last_pcpu_idle;
u_longlong_t last_pcpu_wait;
u_longlong_t last_lcpu_user;
u_longlong_t last_lcpu_sys;
u_longlong_t last_lcpu_idle;
u_longlong_t last_lcpu_wait;
u_longlong_t last_vcsw = 0;
u_longlong_t last_idle_donated_purr = 0;
u_longlong_t last_busy_donated_purr = 0;
u_longlong_t last_busy_stolen_purr = 0;
u_longlong_t last_idle_stolen_purr = 0;


/* Save the current values for the next iteration */
void	save_last_values(perfstat_cpu_total_t *cpustats, perfstat_partition_total_t *lparstats)
{
	last_vcsw      = lparstats->vol_virt_cswitch + lparstats->invol_virt_cswitch;
	last_time_base = lparstats->timebase_last;

	last_pcpu_user = lparstats->puser;
	last_pcpu_sys  = lparstats->psys;
	last_pcpu_idle = lparstats->pidle;
	last_pcpu_wait = lparstats->pwait;

	last_lcpu_user = cpustats->user;
	last_lcpu_sys  = cpustats->sys;
	last_lcpu_idle = cpustats->idle;
	last_lcpu_wait = cpustats->wait;

	last_idle_donated_purr = lparstats->idle_donated_purr;
	last_busy_donated_purr = lparstats->busy_donated_purr;
	last_busy_stolen_purr  = lparstats->busy_stolen_purr;
	last_idle_stolen_purr  = lparstats->idle_stolen_purr;
}


void	display_lpar_util()
{
	u_longlong_t delta_pcpu_user;
	u_longlong_t delta_pcpu_sys;
	u_longlong_t delta_pcpu_idle;
	u_longlong_t delta_pcpu_wait;
	u_longlong_t delta_lcpu_user;
	u_longlong_t delta_lcpu_sys;
	u_longlong_t delta_lcpu_idle;
	u_longlong_t delta_lcpu_wait;
	u_longlong_t delta_busy_stolen_purr;
	u_longlong_t delta_idle_stolen_purr;
	u_longlong_t delta_idle_donated_purr;
	u_longlong_t delta_busy_donated_purr;
	u_longlong_t vcsw, lcputime, pcputime;
	u_longlong_t delta_time_base;
	double	phys_proc_consumed;
	double	delta_sec;
	perfstat_partition_total_t lparstats;
	perfstat_cpu_total_t cpustats;

	static first_time = 1;

	/* retrieve the metrics */
	if (!perfstat_partition_total(NULL, &lparstats, sizeof(perfstat_partition_total_t), 1)) {
		perror("perfstat_partition_total");
		exit(-1);
	}
	if (!lparstats.type.b.donate_enabled)  {
		printf("This is not a Dedicated CPU LPAR with Donating Enabled\nStopping\n");
		exit(1);
	}

	if (!perfstat_cpu_total(NULL, &cpustats, sizeof(perfstat_cpu_total_t), 1)) {
		perror("perfstat_cpu_total");
		exit(-1);
	}
	if (first_time) {
		first_time = 0;
		/* first iteration, we only read the data, print the header and save the data */
		save_last_values(&cpustats, &lparstats);
		return;
	}

	delta_lcpu_user  = cpustats.puser - last_lcpu_user;
	delta_lcpu_sys   = cpustats.psys  - last_lcpu_sys;
	delta_lcpu_idle  = cpustats.pidle - last_lcpu_idle;
	delta_lcpu_wait  = cpustats.pwait - last_lcpu_wait;
	lcputime = delta_lcpu_user + delta_lcpu_sys + delta_lcpu_idle + delta_lcpu_wait;

	/* Logical Processor Utilization */
	printf("Logical CPU percentages\n");
	printf("luser=%5.1f lsys=%5.1f lwait=%6.1f lidle=%6.1f\n", 
	    (double)delta_lcpu_user * 100.0 / (double)lcputime,
	    (double)delta_lcpu_sys  * 100.0 / (double)lcputime,
	    (double)delta_lcpu_wait * 100.0 / (double)lcputime,
	    (double)delta_lcpu_idle * 100.0 / (double)lcputime);

	delta_pcpu_user  = lparstats.puser - last_pcpu_user;
	delta_pcpu_sys   = lparstats.psys  - last_pcpu_sys;
	delta_pcpu_idle  = lparstats.pidle - last_pcpu_idle;
	delta_pcpu_wait  = lparstats.pwait - last_pcpu_wait;
	pcputime = delta_pcpu_user + delta_pcpu_sys + delta_pcpu_idle + delta_pcpu_wait;

	/* Physical Processor Utilization */
	printf("Physical CPU percentages\n");
	printf("puser=%5.1f psys=%5.1f pwait=%6.1f pidle=%6.1f\n", 
	    (double)delta_pcpu_user * 100.0 / (double)pcputime,
	    (double)delta_pcpu_sys  * 100.0 / (double)pcputime,
	    (double)delta_pcpu_wait * 100.0 / (double)pcputime,
	    (double)delta_pcpu_idle * 100.0 / (double)pcputime);

	delta_time_base = lparstats.timebase_last - last_time_base;

	/* Physical Processor Consumed */
	phys_proc_consumed = (double)pcputime / (double)delta_time_base;
	printf("Physical CPU consumed (physc)=%6.2f ", (double)phys_proc_consumed);

	/* Virtual CPU Context Switches per second */
	vcsw = lparstats.vol_virt_cswitch + lparstats.invol_virt_cswitch;
	delta_sec = HTIC2SEC(delta_time_base);
	printf(" and Virtual CPU Context switches(vcsw)=%6.2f\n", (double)(vcsw - last_vcsw) / delta_sec);

	printf("Physical CPU Used            Donate        Stolen\n");
	printf("%6s %6s %6s %6s %6s %6s %6s %6s\n", "User", "Sys", "Wait", "Idle", "Idle", "Busy", "Idle", "Busy");
	printf("%6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f",
		(double)(lparstats.puser - last_pcpu_user) / (double)delta_time_base,
		(double)(lparstats.psys - last_pcpu_sys) / (double)delta_time_base,
		(double)(lparstats.pwait - last_pcpu_wait) / (double)delta_time_base,
		(double)(lparstats.pidle - last_pcpu_idle) / (double)delta_time_base,
		(double)(lparstats.idle_donated_purr - last_idle_donated_purr) / (double)delta_time_base,
		(double)(lparstats.busy_donated_purr - last_busy_donated_purr) / (double)delta_time_base,
		(double)(lparstats.idle_stolen_purr - last_idle_stolen_purr) / (double)delta_time_base,
		(double)(lparstats.busy_stolen_purr - last_busy_stolen_purr) / (double)delta_time_base);

	printf("= %6.3f\n\n", (
	    (double)(lparstats.puser - last_pcpu_user) + 
	    (double)(lparstats.psys - last_pcpu_sys)  + 
	    (double)(lparstats.pwait - last_pcpu_wait) + 
	    (double)(lparstats.pidle - last_pcpu_idle) + 
	    (double)(lparstats.idle_donated_purr - last_idle_donated_purr) + 
	    (double)(lparstats.busy_donated_purr - last_busy_donated_purr) + 
	    (double)(lparstats.idle_stolen_purr - last_idle_stolen_purr) + 
	    (double)(lparstats.busy_stolen_purr - last_busy_stolen_purr) )  / (double)delta_time_base);

	save_last_values(&cpustats, &lparstats);
}


int	main(int argc, char*argv[])
{
	while (1) {
		display_lpar_util();
		sleep(1);
	}
	return(0);
}
