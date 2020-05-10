#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <libperfstat.h>
#include <signal.h>
#include <curses.h>

#define COLOUR
#ifndef FIRST_CPU
#define FIRST_CPU ""
#endif

void	die(int i)
{
	signal(SIGINT, SIG_IGN);
	mvcur(0, COLS - 1, LINES - 1, 0);
	endwin();
	exit(0);
}


save_cpu(WINDOW *win, int cpuno, double user, double sys, double wait, double idle)
{
	int	i;
	if (cpuno < 1) {
		mvwprintw(win, 1, 1, "    User  Sys Idle Wait  0-----------25-----------50----------75---------100");
	}
	if (cpuno < 0)
		mvwprintw(win, 2, 1, "  %5.1f %5.1f %5.1f %5.1f|", user, sys , wait, idle);
	else {
		mvwprintw(win, 2 + cpuno, 1, "%-2d%5.1f %5.1f %5.1f %5.1f|", cpuno, user, sys , wait, idle );
	}
	if (cpuno < 0)
		cpuno = 0;
	wmove(win, 2 + cpuno, 27);
	for (i = 0; i < (int)(user / 2 ); i++) {
		/*COLOUR attrset(COLOR_PAIR(2)); */
		wprintw(win, "U");
		/*COLOUR attrset(COLOR_PAIR(0)); */
	}
	for (i = 0; i < (int)(sys / 2 ); i++) {
		/*COLOUR attrset(COLOR_PAIR(3)); */
		wprintw(win, "s");
		/*COLOUR attrset(COLOR_PAIR(0)); */
	}
	for (i = 0; i < (int)(wait / 2 ); i++) {
		/*COLOUR attrset(COLOR_PAIR(5)); */
		wprintw(win, "W");
		/*COLOUR attrset(COLOR_PAIR(0)); */
	}
	for (i = 0; i < (int)(idle / 2 ); i++)
		wprintw(win, "-");
}


int
main(int argc, char *argv[])
{

	struct data {
		perfstat_cpu_total_t cpu_tot;
		perfstat_cpu_t * cpus;
		double	time;
	} database[2], *p, *q, *pqtmp;

	int	retcode;
	int	cpus_max;
	int	i;
	double	user, sys, idle, wait;
	perfstat_id_t firstcpu;
	unsigned long long	delta_tot;
	unsigned long long	last_tot;
	unsigned long long	cur_tot;
	WINDOW * wintitle;
	WINDOW * winspec;
	WINDOW * wincpu;
	WINDOW * wincpus;

	p = &database[0];
	q = &database[1];
	/* check how many perfstat_cpu_t structures are available */
	cpus_max =  perfstat_cpu(NULL, NULL, sizeof(perfstat_cpu_t), 0);
	/* allocate enough memory for all the structures */
	p->cpus = malloc(sizeof(perfstat_cpu_t) * cpus_max);
	q->cpus = malloc(sizeof(perfstat_cpu_t) * cpus_max);
	/* get initial set of data */
	perfstat_cpu_total(NULL, &p->cpu_tot, sizeof(perfstat_cpu_total_t), 1);
	perfstat_cpu_total(NULL, &q->cpu_tot, sizeof(perfstat_cpu_total_t), 1);
	perfstat_cpu(&firstcpu, p->cpus, sizeof(perfstat_cpu_t), cpus_max);
	perfstat_cpu(&firstcpu, q->cpus, sizeof(perfstat_cpu_t), cpus_max);

	last_tot = p->cpu_tot.user + p->cpu_tot.sys  + p->cpu_tot.idle + p->cpu_tot.wait;
	initscr();
	signal(SIGINT, die);
	box(stdscr, 0, 0);
	refresh();

	wintitle = newwin(3, COLS - 2, 1, 1);
	box(wintitle, 0, 0);
	mvwprintw(wintitle, 1, 1, "nmon prototype");

	winspec = newwin(4, COLS - 2, 4, 1);
	box(winspec, 0, 0);
	mvwprintw(winspec, 0, 2, "Specifcation");

	wincpu = newwin(4, COLS - 2, 8, 1);
	box(wincpu, 0, 0);
	mvwprintw(wincpu, 0, 2, "CPU-Totals");

	wincpus = newwin(7, COLS - 2, 12, 1);
	box(wincpus, 0, 0);
	mvwprintw(wincpus, 0, 2, "CPUs-Individually");

	wrefresh(wintitle);
	wrefresh(winspec);
	wrefresh(wincpu);
	wrefresh(wincpus);
	refresh();

	/* print general processor information */
	mvwprintw(winspec, 1, 1, "Processors: %s running at %llu MHz",
	    p->cpu_tot.description, p->cpu_tot.processorHZ / 1000000);

	mvwprintw(winspec, 2, 1, "CPUs current %d  max-since-boot=%d absolute-max=%d",
	    p->cpu_tot.ncpus, 
	    p->cpu_tot.ncpus_cfg,
#ifdef _AIX52
	    p->cpu_tot.ncpus_high);
#else
	-1);
#endif
	/* set name to first cpu */
	strcpy(firstcpu.name, FIRST_CPU);

	while (1 == 1) {
		wrefresh(wintitle);
		wrefresh(winspec);
		wrefresh(wincpu);
		wrefresh(wincpus);
		refresh();
		sleep(1);

		pqtmp = p;
		p = q;
		q = pqtmp;

		perfstat_cpu_total(NULL, &p->cpu_tot, sizeof(perfstat_cpu_total_t), 1);
		/* calculate current total number of ticks */
		cur_tot = p->cpu_tot.user + p->cpu_tot.sys  + 
		    p->cpu_tot.idle + p->cpu_tot.wait;
		delta_tot  = cur_tot - last_tot;
		user = 100.0 * (p->cpu_tot.user - q->cpu_tot.user) / delta_tot;
		sys  = 100.0 * (p->cpu_tot.sys  - q->cpu_tot.sys) / delta_tot;
		wait = 100.0 * (p->cpu_tot.wait - q->cpu_tot.wait) / delta_tot;
		idle = 100.0 * (p->cpu_tot.idle - q->cpu_tot.idle) / delta_tot;
		last_tot = cur_tot;

		save_cpu(wincpu, -1, user, sys, wait, idle);
		/* ask to get all the structures available in one call */
		retcode = perfstat_cpu(&firstcpu, p->cpus, sizeof(perfstat_cpu_t), cpus_max);
		mvwprintw(wincpus, 1, 1, "retcode=%d", retcode);
		for (i = 0; i < retcode; i++) {
			user = p->cpus[i].user - q->cpus[i].user;
			sys  = p->cpus[i].sys  - q->cpus[i].sys;
			wait = p->cpus[i].wait - q->cpus[i].wait;
			idle = p->cpus[i].idle - q->cpus[i].idle;
			save_cpu(wincpus, i, user, sys, wait, idle);
		}
	}
}


