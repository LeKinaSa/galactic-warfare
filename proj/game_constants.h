#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include "i8254.h"

/* File containing various constants game-related constants. */

#define FRAMERATE               30
#define INTERRUPTS_PER_FRAME    TIMER0_INTERRUPTS_PER_SECOND / FRAMERATE    /* Numer of Timer0 interrupts after which to draw the next frame */

#define PLAYER_MAX_HEALTH       200
#define PLAYER_BASE_SPEED       10
#define PLAYER_BASE_DAMAGE      20

#endif
