/*
	Project: pong 
	by Christopher Fuentes
	cfuentes@g.harvard.edu
	HUID 60857326

	ball.h defines the interface for a singleton paddle object. 
*/

#ifndef PADDLE_H
#define PADDLE_H

#include <stdbool.h>
#include "common.h"

/* Set up paddle with default position within screen dimensions */
void paddle_init(int screenwidth, int screenheight);

/* Moves paddle up, returns true on ball collision */
void paddle_up(point ball_pos); 

/* Moves paddle down, returns true on ball collision */
void paddle_down(point ball_pos);

/* Returns true if the paddle intersects p */
bool paddle_contact(point ball_pos);

#endif
