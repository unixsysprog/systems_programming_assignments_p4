/*
	Project: pong 
	by Christopher Fuentes
	cfuentes@g.harvard.edu
	HUID 60857326

	paddle.c contains the logic for the singleton paddle,
	including the declaration of the paddle struct and
	functions for moving it/determining collisions. 
*/

#include <stdlib.h>
#include <curses.h>
#include "paddle.h"

/* A struct to hold info about a paddle */
typedef struct { 
	int 	pad_top,   	/* max y of paddle */
		pad_bot, 	/* min y of paddle */
		pad_col;	/* x val of paddle */
	char 	pad_char;	/* char to draw paddle */
} paddle;

/* Reference to the paddle that will be used during gameplay */
static paddle _paddle = { 0,0,0,'\0'};

/* Variables to store screen dimensions  */
static int 	_screenheight;

/* Attempts to move the paddle in direction d, and redraws */
static void move_paddle(direction d, point ball_pos);
/* Render the paddle on the screen */
static void draw_paddle(bool draw, bool ref);


/*
	Moves the paddle in direction d unless d falls
	outside of the playzone, in which case it doesn't
	move. 

	Checks for collision with the ball, 
*/
void move_paddle(direction d, point ball_pos) {
	int delta = 0;	/* y-delta for movement */

	if (d == up && _paddle.pad_top - 1 > PADDING) 
		delta = -1;
	else if (d == down && _paddle.pad_bot + 1 < _screenheight - PADDING ) 
		delta = 1;

	if (delta != 0) {
		/* clear the existing paddle */
		draw_paddle(false, false);
		/* update bounds */
		_paddle.pad_top += delta;
		_paddle.pad_bot += delta;

		if (paddle_contact(ball_pos)) {
			/* If contact results, undo bounds */
			_paddle.pad_top -= delta;
			_paddle.pad_bot -= delta;
		} 
		/* draw the new paddle and refresh */
		draw_paddle(true, true);
	}
}

/*
* 	Renders or clears the entire paddle on screen 
*	
*	@param 	flags whether to draw (true) or clear (false) the paddle.
*	@param 	flags whether or not to call refresh()
*/
void draw_paddle(bool draw, bool ref) {
	int 	start_y = _paddle.pad_top, 	/* get start and stop points */
		stop_y	= _paddle.pad_bot;
	char 	c 	= draw ? _paddle.pad_char : BLANK; /* char to draw */

	/* Draw the paddle on the screen */
	while (start_y <= stop_y) mvaddch(start_y++, _paddle.pad_col, c);

	/* Refresh if necessary */
	if (ref) refresh();
}

/* 
	Set up paddle with default position. 
	Paddle is 1/3rd of the playfield, 
	so we'll start it 1/3rd from the bottom
	and extend it another 3rd upwards.

	@param screenwidth 	width of screen
	@param screenheight 	height of screen
*/
void paddle_init(int screenwidth, int screenheight) {
	_screenheight		= screenheight; /* Store screen height */
	
	/* gameheight is the height of the in-bounds playable region */
	int 	gameheight 	= _screenheight - (2 * PADDING), 
		third 		= gameheight / 3;  /* start y for paddle */

	_paddle.pad_top 	= third + PADDING;	
	_paddle.pad_bot 	= third + third + PADDING;
	_paddle.pad_col 	= screenwidth - PADDING;
	_paddle.pad_char 	= PADDLE_CHAR;

	/* draw and render paddle */
	draw_paddle(true, true);
}

/* Attempt to move paddle up. Fails if at top of playfield */
void paddle_up(point ball_pos) {    move_paddle(up, ball_pos); 	}

/* Attempt to move paddle down. Fails if at bottom of playfield */
void paddle_down(point ball_pos) {  move_paddle(down, ball_pos); }

/* 
	Returns true if the paddle intersects a point 

	@param 	Position of potential contact
*/
bool paddle_contact(point ball_pos) {
	int 	row;	 	/* incr variable for checking rows */

	/* If x is in the wrong column, there's no contact */
	if (ball_pos.x == _paddle.pad_col) {
		/* Check all of the rows where the paddle is */
		for (row = _paddle.pad_top; row <= _paddle.pad_bot; row++)
			if (row == ball_pos.y) return true;
	}	
	return false;
}
