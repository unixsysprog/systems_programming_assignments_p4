/*
	Project: pong 
	by Christopher Fuentes
	cfuentes@g.harvard.edu
	HUID 60857326

	ball.h defines the interface for a singleton ball object. 
*/

#ifndef BALL_H
#define BALL_H

#include "common.h"
#include <stdbool.h>


/* Initialize the ball to it's start position. */
void ball_init(int screenwidth, int screenheight);

/* Start the ball moving in a random direction */
void serve_ball();

/* Reverses appropriate trajectory coefficients, option to randomize speed */
void bounce(bounce_type b_type);

/* Get next potential position of ball */
void next_pos(point *pbuff);

/* Moves the ball to point p */
void move_ball(point p);

/* Gets current pos of ball */
point cur_pos();

#endif