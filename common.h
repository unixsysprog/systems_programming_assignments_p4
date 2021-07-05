/*
 	common.h by Christopher Fuentes
 	cfuentes@g.harvard.edu

 	This file contains constants, enums, and structs common to the 
 	various pong files. 
*/

#ifndef COMMON_H
#define COMMON_H
	
/* Gameplay Constants */
#define UP_KEY 			'k'	/* key to move paddle up */
#define DOWN_KEY		'm'	/* key to move paddle down */
#define QUIT_KEY 		'Q'	/* key to quit the application */
#define NUM_BALLS 		3	/* number of times player can miss */
#define	TICKS_PER_SEC		60 	
#define PADDING 		3
#define HORIZONTAL_CHAR 	'-'
#define VERTICAL_CHAR 		'|'
#define TIME_STR_MAX_LEN 	16 	/* "TOTAL TIME: MM:SS" */

#define QUEUE_LEN		1024 	/* Length of action queue */

/* Paddle Constants */
#define PADDLE_CHAR 		'#'

/* Ball Constants */
#define	BLANK			' '
#define BALL_CHAR 	 	'O'
#define MAX_BALL_RAND_VAL 	5  	/* max possible val for velocity */

/* Point */
typedef struct {
	int 	x,
		y;
} point;

/* Vertical Directions */
typedef enum { down, up} direction;

/* Types of bounces, depending on surface */
typedef enum { b_horizontal, b_vertical, b_paddle } bounce_type;


#endif