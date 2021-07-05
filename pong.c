/*
	Project: pong 
	by Christopher Fuentes
	cfuentes@g.harvard.edu
	HUID 60857326

	pong.c contains the main logic for pong. 

	This includes initialization of a timer, 
	a queue for holding actions, and orchestrating 
	the interactions between the user, the terminal
	display, and the game objects (i.e, it is the main
	ViewController in MVC terms).
*/

#include	<stdio.h>
#include	<curses.h>
#include	<signal.h>
#include	<unistd.h>
#include	<stdlib.h>
#include 	<stdbool.h>
#include	"alarmlib.h"
#include 	"common.h"
#include 	"paddle.h"
#include 	"ball.h"

static int 	balls_left 	= NUM_BALLS,
		_width, 	/* playable screen width */
		_height; 	/* playable screen height */

/* Counter for elapsed seconds */
static float 	elapsed_seconds = 0;

/* Action queue counter for ball updates */
static int ball_queue = 0;

/* Result of ball moving */
typedef enum { 	m_none, 
		m_bounce_x, 
		m_bounce_y, 
		m_paddle_bounce, 
		m_lose } move_result;

/* set up initial board state & timers */
void set_up();
/* tear down curses */
void wrap_up();
/* checks potential ball move for collisions, lose conditions */
void check_ball_move();
/* update balls position */
void update_ball();
/* specifies how a potential movement would result */
move_result bounce_or_lose(point p);
/* draw the walls */
void court_init();
/* render the header */
void draw_header();
/* decrement balls_left and start the next ball (if any) */
bool next_ball();
/* Sets up sigaction for SIGALRM*/
void setup_signals();
/* Move the paddle up or down */
void handle_paddle_move(direction d);
/* Handle one timer tick */
void timer_ticked();
/* Helper function for min of two ints */
int min(int a, int b);

int main(int ac, char **av){
	if (ac > 1) {
		fprintf(stderr, "usage: ./pong\n");
		exit(1);
	}

	int c; 		/* user's character 	*/
	set_up(); 	/* init all objects 	*/
	serve_ball(); 	/* start up ball 	*/

	while ( balls_left > 0 && ( c = getch() ) != QUIT_KEY ) {
		if ( c == UP_KEY ) 		paddle_up(cur_pos());
		else if ( c == DOWN_KEY )	paddle_down(cur_pos());

		if (ball_queue > 0 ) {
			update_ball(); 
			ball_queue--;
		}
	}
	wrap_up();	/* tear down 		*/
	return 0;
}

/*
	When a timer ticks, increase the queue count
	to indicate that another ball movement update 
	is required.
*/
void timer_ticked() { ball_queue++; }

/* 	
	Sets a signal handler for SIGALRM to call
	the "check_ball_move" function, and indicates
	that any interrupted function should be restarted 
	(e.g., getchar()).

	Sets SIGINT to be ignored. 
*/
void setup_signals() {
	struct sigaction 	new_act,	/* new and old sig actions */
				old_act;

	new_act.sa_handler	 = timer_ticked; /* set handler */

	sigemptyset( &new_act.sa_mask ); /* don't block any other signals */
	sigaction( SIGALRM, &new_act, &old_act );

	signal(SIGINT, SIG_IGN); 	/* Ignore interrupts */
	signal(SIGWINCH, SIG_IGN);	/* Ignore window resize */	
}

/*	
	Initializes curses, game objects, headers, and ticker signal.
*/
void set_up() {
	srand(getpid()); 			/* seed rand with pid 		*/
	initscr();				/* turn on curses	 		*/
	getmaxyx(stdscr, _height, _width); 	/* get screen dims 	*/

	noecho();				/* turn off echo	 		*/
	cbreak();				/* turn off buffering	 	*/
	curs_set(false); 			/* don't display cursor 	*/

	paddle_init(_width, _height);		/* init objects 		*/
	ball_init(_width, _height);
	court_init();

	draw_header();				

	setup_signals(); 			/* configure singal handling 	*/

	refresh();
	
	set_ticker( 1000 / TICKS_PER_SEC );	/* send millisecs per tick */
}

/*
	Advances the ball by one tick of the timer 
	(which may not cause any visual change on the screen,
	but will tell the ball to update it's underlying model
	to reflect such).
*/
void update_ball() {
	float 	dt 		=  1.0 / TICKS_PER_SEC, /* delta time */
		cur_seconds 	=  elapsed_seconds;
		elapsed_seconds += dt; 	/* Increment time counter*/

	if ((int)elapsed_seconds > (int)cur_seconds) 
		draw_header();	/* Only redraw header if there's a change */

	point 	ball_pos;
	next_pos(&ball_pos);

	check_ball_move(ball_pos);
}


/*
	Simulates what the next move will be and handles
	the potential result. 

	If the next move is out of bounds, the game is lost. 

	If the next move is a collision, determines the bounce
	type and tells the ball to bounce itself appropriately. 

	Else, the next move is just normal movement and the
	ball should update itself accordingly. 

	@param 	next ball position
*/
void check_ball_move(point ball_pos) {
	move_result r 	= bounce_or_lose(ball_pos);

	if (r == m_bounce_x) 		{ bounce(b_horizontal); }
	else if (r == m_bounce_y) 	{ bounce(b_vertical); 	}
	else if (r == m_paddle_bounce) 	{ bounce(b_paddle); 	}
	else if (r == m_lose ) 		{ next_ball(); 	}
	else 				{ move_ball(ball_pos); }
}

/*
	Moves the paddle and checks for/handles contact.
	Note: If the player attempts to move the paddle 
	such that they would intersect, the paddle 
	instead doesn't move (resulting in an out-of-bounds). 

	@param 	Direction the paddle should move

*/
void handle_paddle_move(direction d) {
	point p 	= cur_pos();	/* get ball's current position */

	if (d == up) 	paddle_up(p);
	else 		paddle_down(p);

	check_ball_move(p);
}

/*
	Given a potential (x,y), determines whether
	the move results in a bounce (wall collision),
	paddle bound (paddle collision), a loss (out of bounds),
	or nothing special.

	@param 		position of the ball
	@return 	m_lose if out of bounds, 
			m_none if no contact,
			m_bounce_x/y for wall contact,
			m_paddle_bounce for paddle contact
*/
move_result bounce_or_lose(point p) {
	/* first check contact with walls */
	if (p.x == PADDING) 		{ return m_bounce_y; } /* hit left wall */
	if (p.y == PADDING) 		{ return m_bounce_x; } /* hit bottom    */
	if (p.y == _height - PADDING)	{ return m_bounce_x; } /* hit top       */
	if (paddle_contact(p)) 	{ return m_paddle_bounce; } /* hit paddle    */

	/* if ball is going out of bounds, lose */
	if (p.x < PADDING || p.x > _width - PADDING || 
		p.y > _height - PADDING || p.y < PADDING)
		return m_lose;

	/* If we haven't bounced or lost, nothing happens */
	return m_none;  
}

/* 
	Draw the ceiling, floor, and left wall of the court
*/
void court_init() {
	int 	x, y, 	/* the cartesian coordinates to draw the court */
		gamebot 	= _height - PADDING, /* bottom edge y coord */
		gameright 	= _width - PADDING;  /* right edge x coord  */

	for (x = PADDING, y = PADDING; 		x < gameright; 	x++ )
		mvaddch(y, x, HORIZONTAL_CHAR);

	for (x = PADDING, y = gamebot; 		x < gameright; 	x++ )
		mvaddch(y, x, HORIZONTAL_CHAR);

	for (x = PADDING, y = PADDING + 1; 	y < gamebot ; 	y++ )
		mvaddch(y, x, VERTICAL_CHAR);
}

/*
 	Draws the header, consisting of BALLS LEFT:#, and
 	TOTAL TIME: MM:SS, padded 1 char in from either side
 	and sitting just above the ceiling.
*/
void draw_header() {
	int 	row = PADDING + 1, 	/* header sits just above ceiling */
		col = PADDING - 1, 	/* header starts 1 char in */
		minutes,		/* minutes to display */
		seconds;		/* seconds to display */
	mvprintw(col, row, "BALLS LEFT: %d", balls_left - 1); /* 1 in play */

	/* Limit min to 59. More than an hour is excessive gameplay 	*/
	minutes 	= min((int)elapsed_seconds / 60, 59); 	
	seconds 	= (int)elapsed_seconds % 60;

	row 		=  _width - PADDING - TIME_STR_MAX_LEN;
	mvprintw(col, row, "TOTAL TIME: %02d:%02d", minutes, seconds);
}

/*
	Decrements balls_left and re-renders the header. 

	@returns 	false if no balls left, else true. 
*/
bool next_ball(){
	if (balls_left-- == 1) return false;
	serve_ball();
	draw_header();
	return true;
}


/*
*	Disables the ticker and cleans up the curses window
*/
void wrap_up() {
	set_ticker(0);
	endwin();
}

/* 
	Helper function for min of two ints 

	@param 		an int
	@param 		another int
	@return 	whichever is smaller, or the first if equal.
*/
int min(int a, int b){ return (a < b ? a : (b < a ? b : a)); }
