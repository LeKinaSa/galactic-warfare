#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include "i8254.h"

/* File containing various constants game-related constants. */

#define FRAMERATE               30                  /** @brief Number of frames drawn per second */
#define FRAME_DELTA             1.0 / FRAMERATE     /** @brief Interval between frames (in seconds) */
#define INTERRUPTS_PER_FRAME    TIMER0_INTERRUPTS_PER_SECOND / FRAMERATE    /** @brief Numer of Timer0 interrupts after which to draw the next frame */
#define FRAMES_PER_SHOT         5       /** @brief Number of frames between shots */

#define PLAYER_MAX_HEALTH       200     /** @brief Player health */
#define PLAYER_BASE_SPEED       75      /** @brief Player base speed */
#define PLAYER_BASE_DAMAGE      20      /** @brief Player damage */

#define POWERUP_INTERVAL        1       /** @brief Interval, in minutes, between the generation of powerups */

#define BULLET_SPEED            250

#endif /* GAME_CONSTANTS_H */
