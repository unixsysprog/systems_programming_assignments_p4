/*
	Project: pong 
	by Christopher Fuentes
	cfuentes@g.harvard.edu
	HUID 60857326

	ball.c contains the logic for the singleton ball
	object, including a declaration of its struct 
	(which specifies speed/angle/velocity information)
	as well as functions for moving the ball. 

	There are also helper functions for randomizing
	the trajectory using rand().
*/

#include "ball.h"
#include <stdlib.h>
#include <curses.h>

typedef struct {
	int	x_pos, x_dir,
		y_pos, y_dir,
		y_delay, y_count,
		x_delay, x_count;
	char	symbol ;
} ball;

/* center of the screen */
static point center;
/* The persistant ball for the game */
static ball _ball = {0,0,0,0,0,0,0,0,BALL_CHAR};

/* returns -1 or 1 with random probability */
static int randdir(); 
/* returns random number within 0 and MAX_NUM -1 */
static int randrange();
/* randomly resets velocity factors */
static void reset_velocity();
/* places ball in the center of the screen */
static void center_ball();
/*
	Initialize ball (centered) and render it onscreen.

	@param screenwidth 		width of screen
	@param screenheight 	height of screen
*/
void ball_init(int screenwidth, int screenheight) {
	center.x 	= screenwidth 	/ 2.0;	/* store horiz. center */
	center.y 	= screenheight 	/ 2.0; 	/* store vert center */
	
	center_ball();
}

/*
 	Changes ball's position to the center of the screen,
 	clearing the original spot and refreshing the display
 	when finished. 
*/
void center_ball(){
	mvaddch(_ball.y_pos, _ball.x_pos, BLANK);

	_ball.x_pos 	= center.x;	/* Center horizontally 	*/
	_ball.y_pos 	= center.y;	/* Center vertically 	*/

	mvaddch(_ball.y_pos, _ball.x_pos, _ball.symbol);
	refresh();
}

/* 
	Randomly resets the angle and magnitude of velocity while
	keeping the coefficients the same. 
 */
void reset_velocity(){
	int x, y; 	/* the x and y count/delay */

	do {
		x = randrange();
		y = randrange();
	} while (x == 0);   	/* make sure x is not zero */
	_ball.y_count 	= _ball.y_delay 	= y;
	_ball.x_count	= _ball.x_delay 	= x;
}

/*
	Centers the ball and randomizes the ball's speed/angle/velocity
*/
void serve_ball(){
	reset_velocity();		/* Randomize angle/speed 	*/

	_ball.y_dir 	= randdir();	/* Randomize coefficients 	*/
	_ball.x_dir 	= randdir();

	center_ball();			/* Center and render 		*/
}

/* 
	Changes one of the ball's velocity coefficients (x or y)
	and potentially angle/speed, depending on the type of bounce. 

	@param 	the type of bounce, representing a wall bounce,
			ceiling/floor bounce, or paddle bounce. 
*/
void bounce(bounce_type b_type) {
	switch (b_type) {
		case b_horizontal: 			/* Hit the left wall 	*/
			_ball.y_dir *= -1;
			break;

		case b_vertical:			/* Hit ceiling/floor 	*/
			_ball.x_dir *= -1;
			break;

		case b_paddle:				/* Hit the paddle 		*/
			_ball.x_dir *= -1;
			reset_velocity();
	}	
}

/*
	Fills pbuff with the next position in the ball's trajectory,
	given it's current speed/angle/velocity.

	@param 	a point buffer to hold the x/y of the next point. 
*/
void next_pos(point *pbuff) {
	pbuff->x = _ball.x_pos;	/* initialize to ball's current pos */
	pbuff->y = _ball.y_pos;

	/* If there will be vert movement, increment pbuff's y counter */
	if (_ball.y_delay > 0 && _ball.y_count - 1 == 0) 
		pbuff->y = _ball.y_pos + _ball.y_dir;

	/* If there will be horiz movement, increment pbuff's x counter */
	if (_ball.x_delay > 0 && _ball.x_count - 1 == 0) 
		pbuff->x = _ball.x_pos + _ball.x_dir;
}

/*
	Actually updates the ball's position to point p. 
	Calculates the delta between the current position 
	and the new position p, and only updates the position
	variables if there is movement in a given direction. 

	@param 	new position for the ball

*/
void move_ball(point p) {
	bool 	x_move 	= (p.x != _ball.x_pos),	/* ball position changes */
		y_move 	= (p.y != _ball.y_pos);

	if (_ball.y_delay) --_ball.y_count;
	if (_ball.x_delay) --_ball.x_count;

	if (x_move || y_move) {		 /* ball moved */
		mvaddch(_ball.y_pos, _ball.x_pos, BLANK);
		if (x_move) {
			_ball.x_pos 	+= _ball.x_dir;	
			_ball.x_count 	= _ball.x_delay;
		} 
		if (y_move) {
			_ball.y_pos 	+= _ball.y_dir ;
			_ball.y_count 	= _ball.y_delay; 
		}
		mvaddch(_ball.y_pos, _ball.x_pos, _ball.symbol);
		refresh();
	}
}

/* Returns a point containing current position of the ball */
point cur_pos() { 
	point p = { _ball.x_pos, _ball.y_pos }; 
	return p;
}

/* returns -1 or 1 with random probability 			*/
int randdir() { return (rand() % 2) == 1 ? 1 : -1; }

/* returns random number within 0 and MAX_BALL_RAND_VAL -1 	*/
int randrange() { return rand() % MAX_BALL_RAND_VAL; }
