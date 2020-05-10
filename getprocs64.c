#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <procinfo.h>
#include <sys/time.h>
#include <sys/procfs.h>

/* Macro to hide time maths */
#define TIMED(member) ((double)(procs[i].member.tv_sec+ procs[i].member.tv_usec/1000000.0))

/*
int getprocs64 ( struct procentry64 *ProcessBuffer, int ProcessSize, struct fdsinfo64 *FileBuffer, int FileSize, pid_t *IndexPointer, int Count);
*/

main(int argc, char **argv)
{
	pid_t firstproc = (pid_t)0;
	struct procentry64 *procs;
	struct psinfo ps;
	int actual_processes;
	int initial_processes;
	int i;
	char filename[64];
	int fd;
	int count;

	firstproc = 0;
	initial_processes = getprocs64(NULL, 0, NULL, 0, &firstproc, 999999);
	procs = malloc( initial_processes * sizeof(struct procentry64) );

	firstproc = 0; /* you have to reset this every time */
	actual_processes = getprocs64(procs, sizeof(struct procentry64), NULL, 0, &firstproc, initial_processes);

	printf("PID,CPU-Time,Memory,Threads,Status,Start,UID,Command,CMD-Arguments\n");

	for(i=0;i<actual_processes;i++) {

		sprintf(ps.pr_psargs, "data missing", 12);
		sprintf(filename, "/proc/%d/psinfo", procs[i].pi_pid);
		/*printf("opening file %s\n",filename);*/
		fd=open(filename, O_RDONLY);
		if(fd == -1) {
			perror("open failure");
		} else {
			count = read(fd,&ps,sizeof(struct psinfo));
			if(count != sizeof(struct psinfo)) {
				perror("read failure");
				printf("count was %d expected %d\n", count, sizeof(struct psinfo));
			}
			close(fd);
		}

		/* patch up weird commands */
		if(procs[i].pi_pid == 0) strcpy(procs[0].pi_comm, "Swapper");
		if(procs[i].pi_state == SZOMB)
			strcpy(procs[i].pi_comm, "><defunct Zombie>");
		if(!strcmp("gil", procs[i].pi_comm))
			strcpy(procs[i].pi_comm, "gil = TCP/IP");

		printf("%d,%0.0f,%d,%d,%d,%d,%d,%s,%s\n",
		    (int)procs[i].pi_pid,
		    (double) (TIMED(pi_ru.ru_utime) +TIMED(pi_ru.ru_stime)),
		    (int)procs[i].pi_size*4, /* convert pages to KBytes */
		    (int)procs[i].pi_thcount,
		    (int)procs[i].pi_state,
		    (int)procs[i].pi_start,
		    (int)procs[i].pi_uid,
		    procs[i].pi_comm,
		    ps.pr_psargs
		    );
	}
}
