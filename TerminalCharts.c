#include<float.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<ncurses.h>
#include<fcntl.h>
#include<getopt.h>
#include<limits.h>
#define barclose 0x1
#define barhigh 0x2
#define barlow 0x4
#define baropen 0x8
#define colouredchart 0x4 
#define chart_begin_end 0x2 

static chtype bar_arr[0x10];
static size_t tsize, tlen, tptr = -1;

static struct bar{ double o,h,l,c;
	  	 } *data;

static void newtick(double y)
	{
	tptr = (tptr+1)%tsize;
	data[tptr] = (struct bar)
		{.o=y,.h=y, .l=y, .c=y,
		};
	if (tlen < tsize)
	++tlen;
	}

static void updatetick(double y)
	{
	if (y > data[tptr].h)
	data[tptr].h = y;
	else if (y < data[tptr].l)
	data[tptr].l = y;
	data[tptr].c = y;
	}

static struct populate{
	unsigned int bars;
	int (*populatetick)(const struct populate *, WINDOW *, int, int,const struct bar *), maxy, maxx, margin;
	const char *stocksymb;
	size_t begin, end;
	double lowest_tick, highest_tick, chartrange;
	}drw;

extern int y_axis(const struct populate *drw, int screen_range, double y)
	{
	return (drw->highest_tick-y)/drw->chartrange*(screen_range-1)+1;
	}

extern int populatetick_japcandle(const struct populate *drw, WINDOW *win, int screen_range, int x, const struct bar *pt)
	{
	attr_t attrs_orig;
	short colors_orig;
	wattr_get(win, &attrs_orig, &colors_orig, 0);
	if (drw->bars & colouredchart) 
		{
		if (pt->o < pt->c)
		wattron(win, COLOR_PAIR(COLOR_GREEN));
		else if (pt->o > pt->c)
		wattron(win, COLOR_PAIR(COLOR_RED));
		} 
	else if (pt->o > pt->c)
	wattron(win, A_BOLD);
	int o = y_axis(drw, screen_range, pt->o);
	int h = y_axis(drw, screen_range, pt->h);
	int l = y_axis(drw, screen_range, pt->l);
	int c = y_axis(drw, screen_range, pt->c);
	mvwvline(win, h, x, ACS_VLINE, l - h + 1);
	mvwvline(win, o, x, ' '|A_REVERSE, c-o+1);
	mvwvline(win, c, x, ' '|A_REVERSE, o-c+1);
	unsigned int ch = baropen;
	if (pt->l < pt->o)
	ch |= barlow;
	if (pt->h > pt->o)
	ch |= barhigh;
	if (o==c){
		ch |= barclose;
		} 
	else    {
		mvwaddch(win, o, x, bar_arr[ch]);
		ch = barclose;
		if (pt->l < pt->c)
		ch |= barlow;
		if (pt->h > pt->c)
		ch |= barhigh;
		}
	mvwaddch(win, c, x, bar_arr[ch]);
	wattr_set(win, attrs_orig, colors_orig, 0);
	return c;
	}

extern int populatetick_bar(const struct populate *drw, WINDOW *win, int screen_range, int x, const struct bar *pt)
	{
	attr_t attrs_orig;
	short colors_orig;
	wattr_get(win, &attrs_orig, &colors_orig, 0);
	if (drw->bars & colouredchart) 
		{
		if (pt->o < pt->c)
		wattron(win, COLOR_PAIR(COLOR_GREEN));
		else if (pt->o > pt->c)
		wattron(win, COLOR_PAIR(COLOR_RED));
		} 
	else if (pt->o > pt->c)
	wattron(win, A_BOLD);
	int o = y_axis(drw, screen_range, pt->o);
	int h = y_axis(drw, screen_range, pt->h);
	int l = y_axis(drw, screen_range, pt->l);
	int c = y_axis(drw, screen_range, pt->c);
	mvwvline(win, h, x, ACS_VLINE, l-h+1);
	unsigned int ch = baropen;
	if (pt->l < pt->o)
	ch |= barlow;
	if (pt->h > pt->o)
	ch |= barhigh;
	if (o==c){
		ch |= barclose;
		} 
	else    {
		mvwaddch(win, o, x, bar_arr[ch]);
		ch = barclose;
		if (pt->l < pt->c)
		ch |= barlow;
		if (pt->h > pt->c)
		ch |= barhigh;
		}
	mvwaddch(win, c, x, bar_arr[ch]);
	wattr_set(win, attrs_orig, colors_orig, 0);
	return c;
	}

static void populatechart(struct populate *drw)
	{
	size_t i;
	getmaxyx(stdscr, drw->maxy, drw->maxx);
	drw->end = (tptr + 1) % tsize;
	size_t maxx = drw->maxx - 2;  
	drw->begin = (drw->end - (tlen < maxx ? tlen : maxx)) % tsize;
	
	drw->lowest_tick = DBL_MAX;
	drw->highest_tick = -DBL_MAX;
	for (i = drw->begin; i != drw->end; i = (i + 1) % tsize) 
		{
		if (data[i].l < drw->lowest_tick)
		drw->lowest_tick = data[i].l;
		if (data[i].h > drw->highest_tick)
		drw->highest_tick = data[i].h;
		}
	drw->chartrange = drw->highest_tick - drw->lowest_tick;
	drw->margin = snprintf(0, 0, "%lg", drw->highest_tick);
	int n = snprintf(0, 0, "%lg", drw->lowest_tick);
	if (n > drw->margin)
	drw->margin = n;
	double dlast = tlen ? data[tptr].c : 0.0;
	n = snprintf(0, 0, "%lg", dlast);
	if (n > drw->margin)
	drw->margin = n;
	maxx = drw->maxx - drw->margin - 2;
	drw->begin = (drw->end - (tlen < maxx ? tlen : maxx)) % tsize;

	WINDOW *win = newwin(drw->maxy, drw->maxx - drw->margin, 0, 0);
	int y = 0, x = 1, maxy = drw->maxy - 2;  
	for (i = drw->begin; i != drw->end; i = (i + 1) % tsize)
	y = drw->populatetick(drw, win, maxy, x++, data + i);
	x = getmaxx(win) - 1; 
	box(win, 0, 0);
	if (drw->stocksymb) 
		{
		wattron(win, A_BOLD | A_REVERSE);
		mvwprintw(win, 0, 4, " %s ", drw->stocksymb);
		wattroff(win, A_BOLD | A_REVERSE);
		}
	mvwaddch(win, 5, x, ACS_RTEE);
	mvwaddch(win, maxy, x, ACS_RTEE);
	mvwaddch(win, y, x, ACS_RTEE);
	wrefresh(win);
	delwin(win);
	win = newwin(drw->maxy, drw->margin, 0, drw->maxx - drw->margin);
	mvwprintw(win, 1, 0, "%lg", drw->highest_tick);
	mvwprintw(win, maxy, 0, "%lg", drw->lowest_tick);
	mvwprintw(win, y, 0, "%lg", dlast);
	wclrtoeol(win);
	wrefresh(win);
	delwin(win);
	}

static void autoscalewindow(void)
	{
	int width = getmaxx(stdscr);
	size_t newsize = tsize ? tsize : 1;
	while (newsize < (size_t)width) 
	newsize <<= 1;
	if (newsize != tsize)
		{
		struct bar *newdata = malloc(newsize*sizeof*newdata);
		size_t len = tptr+1;
		memcpy(newdata, data, len*sizeof*newdata);
		memcpy(newdata+newsize-tsize+len, data+len, (tlen-len)*sizeof*newdata);
		free(data);
		tsize = newsize;
		data = newdata;
		}
	}

int main(int argc,char **argv)
	{
	initscr();
	start_color();
	#ifdef NCURSES_VERSION
	use_default_colors();
	init_pair(COLOR_RED, COLOR_RED, -1);
	init_pair(COLOR_GREEN, COLOR_GREEN, -1);
	#else
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	#endif
	
	drw.bars |= colouredchart;
	curs_set(0);
	noecho();
	autoscalewindow();
	int input = getopt_long(argc, argv, "s:", 0, 0);
	if (!strcmp(optarg, "japcandle")) 
		{bar_arr[0x0] = '0';
		bar_arr[0x1] = '1';
		bar_arr[0x2] = '2';
		bar_arr[0x3] = ' '|A_REVERSE;
		bar_arr[0x4] = '4';
		bar_arr[0x5] = ' '|A_REVERSE;
		bar_arr[0x6] = ACS_VLINE;
		bar_arr[0x7] = ' '|A_REVERSE;
		bar_arr[0x8] = '8';
		bar_arr[0x9] = ACS_HLINE;
		bar_arr[0xa] = ' '|A_REVERSE;
		bar_arr[0xb] = ' '|A_REVERSE;
		bar_arr[0xc] = ' '|A_REVERSE;
		bar_arr[0xd] = ' '|A_REVERSE;
		bar_arr[0xe] = ' '|A_REVERSE;
		bar_arr[0xf] = ACS_PLUS;
		drw.populatetick = populatetick_japcandle;
		}
		else if (!strcmp(optarg, "bar"))
		{bar_arr[0x0] = '0';
		bar_arr[0x1] = '1';
		bar_arr[0x2] = '2';
		bar_arr[0x3] = ACS_LLCORNER;
		bar_arr[0x4] = '4';
		bar_arr[0x5] = ACS_ULCORNER;
		bar_arr[0x6] = ACS_VLINE;
		bar_arr[0x7] = ACS_LTEE;
		bar_arr[0x8] = '8';
		bar_arr[0x9] = ACS_HLINE;
		bar_arr[0xa] = ACS_LRCORNER;
		bar_arr[0xb] = ACS_BTEE;
		bar_arr[0xc] = ACS_URCORNER;
		bar_arr[0xd] = ACS_TTEE;
		bar_arr[0xe] = ACS_RTEE;
		bar_arr[0xf] = ACS_PLUS;
		drw.populatetick = populatetick_bar;
		}
	
	int fd_stdin = STDIN_FILENO;
	if (optind < argc && strcmp(argv[optind], "-")) 
		{
		const char *fn = argv[optind];
		if (!drw.stocksymb)
		drw.stocksymb = fn;
		int fd = open(fn, O_RDONLY);
		fd_stdin = dup(STDIN_FILENO);
		dup2(fd, STDIN_FILENO);
		close(fd);
		}
	int status;
	char buf[BUFSIZ], *ptr = buf;
	size_t n = sizeof buf;
	while(1){
		for (status; status = getnstr(ptr, n), status != ERR;)
			{
			if (status == KEY_RESIZE)
				{endwin();
				autoscalewindow();
				drw.bars |= chart_begin_end;
				break;  
				}
			if (*buf != ',')
				{double y = strtod(buf, &ptr);
				if (ptr == buf)
				continue;
				newtick(y);
				}
			else
				{ptr = buf;
				}
			while (*ptr == ',')
				{char *end;
				double y = strtod(++ptr, &end);
				if (end == ptr)
				break;
				updatetick(y);
				ptr = end;
				}
			ptr = buf;
			n = sizeof buf;
			timeout(20);
			drw.bars |= chart_begin_end;
			

			}
		if (!(drw.bars & chart_begin_end))
		break;
		populatechart(&drw);
		refresh();
		size_t len = strlen(ptr);
		ptr += len;
		n -= len;
		timeout(-1);
		drw.bars &= ~chart_begin_end;
		
		}
	if (fd_stdin != STDIN_FILENO) 
		{
		dup2(fd_stdin, STDIN_FILENO);
		char *stocksymb = malloc(snprintf(0, 0, "%s (Offline)", drw.stocksymb));
		sprintf(stocksymb, "%s (Offline)", drw.stocksymb);
		drw.stocksymb = stocksymb;
		while(1){
			populatechart(&drw);
			refresh();
			if (getch() != KEY_RESIZE)
			break;
			endwin();
			}
		}
	endwin();
	return 0;
	}


