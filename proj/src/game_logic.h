#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <lcom/lcf.h>
#include "utils.h"

/** @defgroup game_logic game_logic
 * @{
 * @brief Functions and data structures related to the game's internal logic
 */

/** @brief Basic two-dimensional vector. */
typedef struct {
  double x, y;
} Vector2;

/** @brief Calculates the norm (length) of the given vector */
double Vector2_norm(Vector2 this);

/** @brief Calculates the sum of two vectors */
Vector2 Vector2_add(Vector2 lhs, Vector2 rhs);

/** @brief Calculates the difference of two vectors */
Vector2 Vector2_subtract(Vector2 lhs, Vector2 rhs);

/** @brief Returns a normalized (length = 1) version of a vector */
Vector2 Vector2_normalized(Vector2 this);

/** @brief Returns the dot product of the given vectors */
double Vector2_dot_prod(Vector2 lhs, Vector2 rhs);

/** @brief Performs a scalar multiplication on the given vector */
Vector2 Vector2_scalar_mult(double scalar, Vector2 this);

/** @brief Returns the distance between the two vectors */
double Vector2_distance_to(Vector2 lhs, Vector2 rhs);

/** @brief Returns the angle between the two vectors in radians */
double Vector2_angle_to(Vector2 lhs, Vector2 rhs);

static const Vector2 UP = {0.0, -1.0};
static const Vector2 DOWN = {0.0, 1.0};
static const Vector2 LEFT = {-1.0, 0.0};
static const Vector2 RIGHT = {1.0, 0.0};

/**
 * @brief Generates a random position, given maximum values for the x and y coordinates (minimum is 0).
 * @param max_x     maximum value of x coordinate
 * @param max_y     maximum value of y coordinate
 * @return          randomly generated position within the defined bounds
 */
Vector2 generate_random_pos(uint16_t max_x, uint16_t max_y);

/**
 * @brief Rotates a point around the origin by a given angle using a rotation matrix.
 * The rotation matrix is suited to a cartesian system with an inverted y-axis (positive down, negative up).
 * @param point point to rotate
 * @param angle angle of rotation in radians (usually -PI to PI, but works with any angle)
 * @return      rotated point
 */
Vector2 rotate_point(Vector2 point, double angle);

/** @brief Structure that represents a triangle. */
typedef struct {
  Vector2 vertices[3];  /**< @brief array containing the three vertices of the triangle */
} Triangle;

/**
 * @brief Creates a new triangle structure. The given points must be valid and defined in clockwise order to
 * facilitate collision detection.
 * @param vertex1  first vertex of the triangle
 * @param vertex2  second vertex of the triangle
 * @param vertex3  third vertex of the triangle
 * @returns pointer to created triangle. NULL if params were invalid.
 */
Triangle* Triangle_new(Vector2 vertex1, Vector2 vertex2, Vector2 vertex3);

/**
 * @brief Frees memory allocated for a triangle structure.
 * @param this  pointer to the triangle structure to delete
 */
void Triangle_delete(Triangle* this);

/** @brief Structure that represents a circle */
typedef struct {
  double radius;  /**< @brief radius of the circle */
} Circle;

/**
 * @brief Creates a new circle structure.
 * @param radius  radius of the circle, must be positive
 * @returns pointer to created circle. NULL if params were invalid.
 */
Circle* Circle_new(double radius);

/**
 * @brief Frees memory allocated for a circle structure.
 * @param this  pointer to the circle structure to delete
 */
void Circle_delete(Circle* this);


/** @brief Enumeration of the different shape types. */
enum shape_type {
  NO_SHAPE,
  TRIANGLE,
  CIRCLE
};

/**
 * @brief Detects a collision between a triangle and a circle, based on at least one of three criteria.
 * @param triangle      pointer to triangle structure to use for collision detection
 * @param triangle_pos  position of the centre of the triangle (around which the vertices are defined by their relative position)
 * @param circle        pointer to circle structure to use for collision detection
 * @param circle_pos    position of the centre of the circle
 * @returns true if the given shapes collide
 */
bool triangle_circle_collision(const Triangle* triangle, Vector2 triangle_pos, const Circle* circle, Vector2 circle_pos);

/**
 * @brief Detects a collision between two circles
 * @param circle1       pointer to first circle structure
 * @param circle1_pos   position of the centre of th first circle
 * @param circle2       pointer to second circle structure
 * @param circle2_pos   position of the centre of the second circle
 * @return true if the given shapes collide
 */
bool circle_circle_collision(const Circle* circle1, Vector2 circle1_pos, const Circle* circle2, Vector2 circle2_pos);

/** @brief Structure representing a sprite, with an optional collision shape */
typedef struct {
  xpm_image_t img;                        /**< @brief xpm_image_t structure containing information about the image */
  enum shape_type collision_shape_type;   /**< @brief type of collision shape */
  void* collision_shape;                  /**< @brief pointer to the collision shape, NULL if sprite has no collision shape */
} Sprite;

/** @brief Structure representing an entity (any object that be placed or moved around the display) */
typedef struct {
  Sprite sprite;    /**< @brief sprite that will be drawn on the display */
  Vector2 position; /**< @brief exact position of the entity */
  Vector2 velocity; /**< @brief velocity of the entity, added to the position every frame */
  Vector2 offset;   /**< @brief position of the centre of the sprite relative to the upper left corner, used for rotation and collision */
} Entity;

/** @brief Structure representing the mouse cursor */
typedef struct {
  Sprite sprite;      /**< @brief sprite that will be drawn on the display */
  Vector2 position;   /**< @brief position of the mouse cursor */
  Vector2 offset;     /**< @brief position of the centre of the cursor (the cursor is a sight, not an arrow) */
} MouseCursor;

/**
 * @brief Updates the position of the mouse cursor.
 * @param cursor      pointer to MouseCursor structure
 * @param mouse_pos   new cursor position
 */
void update_cursor_position(MouseCursor* cursor, Vector2 mouse_pos);


/* --- PLAYER --- */

/** @brief Structure representing the current state of the player */
typedef struct {
  Entity* entity;         /**< @brief Pointer to player entity */
  uint8_t current_health; /**< @brief Player's current health points */
  uint8_t speed;          /**< @brief Player's current speed (can be modified by powerups) */
  uint8_t damage;         /**< @brief Player's current damage (can be modified by powerups) */
  float angle;            /**< @brief The player's angle relative to the x-axis (between -PI and PI) */
  bool fire;              /**< @brief True if the player is attempting to fire a bullet */
} Player;

/**
 * @brief Updates the position of the bullets and player.
 * @param bullets   linked list containing information about all bullets
 * @param player    pointer to player
 */
void update_entity_positions(LinkedList* bullets, Player* player);

/**
 * @brief Resets a player's speed and damage to the base values.
 * @param player  pointer to player to reset
 */
void reset_speed_and_damage(Player* player);

/* --- BULLET --- */

/** @brief Structure representing a bullet */
typedef struct {
  Entity entity;          /**< @brief Bullet entity */
  bool friendly;          /**< @brief True if the bullet was fired by the player, false if it came from the enemy player. */
  uint8_t damage;         /**< @brief Damage the bullet will inflict */
} Bullet;


/* --- POWERUP --- */

/** @brief Enumeration of the different powerup types. */
enum powerup_type {
  SPEED,
  DAMAGE
};

/** @brief Structure representing a powerup (collectable that improves the player's speed or damage
 * for a short amount of time) */
typedef struct {
  Entity* entity;
  enum powerup_type type;
} Powerup;

/**
 * @brief Creates a new powerup, given an entity and type.
 * @param entity      pointer to powerup entity
 * @param type        type of the new powerup
 * @return            pointer to created powerup. NULL if params were invalid
 */
Powerup* Powerup_new(Entity* entity, enum powerup_type type);
/**
 * @brief Frees memory allocated for a powerup structure.
 * @param this  pointer to the powerup structure to delete
 */
void Powerup_delete(Powerup* this);


/**
 * @brief Detects collisions between the player and powerups / bullets, updating entities accordingly.
 * @param bullets           linked list containing information about all bullets
 * @param current_powerup   pointer to current powerup pointer, allows for it to be set to NULL if a powerup collision occurs
 * @param player            pointer to player
 * @param enemy             pointer to enemy
 */
void detect_collisions(LinkedList* bullets, Powerup** current_powerup, Player* player, Player* enemy);

/**@}*/

#endif /* GAME_LOGIC_H */
