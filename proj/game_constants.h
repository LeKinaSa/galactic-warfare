#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include "i8254.h"

/* File containing various constants game-related constants. */

#define FRAMERATE               30                  /* Number of frames drawn per second */
#define FRAME_DELTA             1.0 / FRAMERATE     /* Interval between frames (in seconds) */
#define INTERRUPTS_PER_FRAME    TIMER0_INTERRUPTS_PER_SECOND / FRAMERATE    /* Numer of Timer0 interrupts after which to draw the next frame */

#define PLAYER_MAX_HEALTH       200
#define PLAYER_BASE_SPEED       50
#define PLAYER_BASE_DAMAGE      20

#endif
