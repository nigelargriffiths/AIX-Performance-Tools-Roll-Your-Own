#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>

#define LENGTH 10
#define WORM_MAX 50

int debug = 0;
int spin = 1024;
int worm_count;

void a(),b();
int c();
void a() { int i; for(i=0;i<spin;i++) b(); }
void b() { int i; for(i=0;i<1024;i++) c(); }
int c() { return 42; }

struct coord {
	int c;
	int r;
	struct coord *next;
	};

struct worm {
	char symbol;
	struct coord *start;
	};

struct worm worms[WORM_MAX +1];

void display_worm(struct worm *p)
{
	struct coord *q;
	for(q=p->start;q;q=q->next) 
		mvaddch(q->r,q->c,p->symbol);
}

void init_worm(struct worm *wp, int row, int col, char ch)
{
	int i;
	struct coord **p;

	wp->symbol = ch;
	p = &wp->start;
	for(i=0;i<LENGTH;i++) {
		*p = malloc(sizeof(struct coord));
		if(col < COLS -2)
			(*p)->c = col;
		else
			(*p)->c = COLS/2;
		if(row < LINES -2)
			(*p)->r = row;
		else
			(*p)->r = LINES/2;
		(*p)->next = NULL;
		p = &((*p)->next);
	}
}

int timer1,timer2;

void timer(int interrupt)
{
	signal(SIGALRM,timer);
	alarm(1);
	timer2 = timer1;
	timer1=0;
}
	
void die(int interrupt)
{
	clear();
	refresh();
	endwin();
	exit(0);
}

int found(int row, int col)
{
	int i;
	struct coord *p;

	for(i=0;i<worm_count;i++) 
		for(p=worms[i].start; p!=NULL; p=p->next)
			if( p->r == row && p->c == col)
				return 1;
	return 0;
}

void move_worm(struct worm *p)
{
	struct coord *moved;
	struct coord *end;
	struct coord *old;
	int retry = 0;

retry_again:
	/* save tail */
	moved = p->start;

	/* remove it */
	p->start = p->start->next;
	if(! found(moved->r,moved->c) )
		mvaddch(moved->r,moved->c,' ');

	/* find the end */
	for( end=p->start; end->next != NULL; old=end,end=end->next)
		;
	/* add it back on as the head */
	moved->next=NULL;
	end->next = moved;

	/* move the new head on one space */
	switch( rand() % 16 )
	{
	case 0:
		moved->r=end->r -1;
		moved->c=end->c -1;
		break;
	case 1:
		moved->r=end->r -1;
		moved->c=end->c;
		break;
	case 2:
		moved->r=end->r -1;
		moved->c=end->c +1;
		break;
	case 3:
		moved->r=end->r;
		moved->c=end->c +1;
		break;
	case 4:
		moved->r=end->r +1;
		moved->c=end->c +1;
		break;
	case 5:
		moved->r=end->r +1;
		moved->c=end->c;
		break;
	case 6:
		moved->r=end->r +1;
		moved->c=end->c -1;
		break;
	case 7:
		moved->r=end->r;
		moved->c=end->c -1;
		break;
	default:
		moved->r=end->r*2 - old->r;
		moved->c=end->c*2 - old->c;
		break;
	}
	if( moved->r == old->r && moved->c == old->c && retry++<10 )
		goto retry_again;
	if(moved->r <= 0) moved->r++;
	if(moved->c <= 0) moved->c++;
	if(moved->r >= LINES -1) moved->r--;
	if(moved->c >= COLS -1 ) moved->c--;
	mvaddch(moved->r,moved->c,p->symbol);
}

int main(int argc, char **argv)
{
	int i;
	int j;

	if(argc == 2)
		worm_count = atoi(argv[1]);
	else
		worm_count = 2;
	if(worm_count>WORM_MAX)
		worm_count= WORM_MAX;

	if(getenv("SPIN") != 0)
		spin = atoi(getenv("SPIN"));
	signal(SIGINT,die);
	timer(0);
	srand(getpid());
	initscr();
	noecho();
	leaveok( stdscr, TRUE);
	scrollok( stdscr, FALSE);
	box(stdscr, 0, 0);

	for(i=0;i<worm_count;i++) 
		init_worm(&worms[i], 10+i, 10+i, '@'+i);

	for(i=0;;i++) {
		a();
		mvprintw(1,1,"    ");
		for(j=0;j<worm_count;j++) 
			move_worm(&worms[j]);
		timer1++;
		mvprintw(1,1,"%d",timer2);
		refresh();
	}
	clear();
	refresh();
	endwin();
	return 1;
}
