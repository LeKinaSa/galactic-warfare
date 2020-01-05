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


typedef struct {
  Vector2 vertices[3];  /**< @brief Array containing the three vertices of the triangle */
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

double Triangle_area(const Triangle* this);


typedef struct {
  double radius;
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

double Circle_area(const Circle* this);


/** @brief Enumeration of the different shape types. */
enum shape_type {
  NO_SHAPE,
  TRIANGLE,
  CIRCLE
};

/**
 * @brief Detects a collision between a triangle and a circle, based on at least one of three criteria.
 * @param triangle      Pointer to triangle structure to use for collision detection
 * @param triangle_pos  Position of the centre of the triangle (around which the vertices are defined by their relative position)
 * @param circle        Pointer to circle structure to use for collision detection
 * @param circle_pos    Position of the centre of the circle
 * @returns true if the given shapes collide
 */
bool triangle_circle_collision(const Triangle* triangle, Vector2 triangle_pos, const Circle* circle, Vector2 circle_pos);


typedef struct {
  xpm_image_t img;
  enum shape_type collision_shape_type;
  void* collision_shape;
} Sprite;

typedef struct {
  Sprite sprite;
  Vector2 position;
  Vector2 velocity;
  Vector2 offset;
} Entity;

typedef struct {
  Sprite sprite;
  Vector2 position;
  Vector2 offset;
} MouseCursor;

void update_cursor_position(MouseCursor* cursor, Vector2 mouse_pos);


/* --- PLAYER --- */

/** @brief Structure representing the current state of the player */
typedef struct {
  Entity* entity;         /**< @brief Pointer to player entity */
  uint8_t current_health; /**< @brief Player's current health points */
  uint8_t speed;          /**< @brief Player's current speed (can be modified by powerups) */
  float angle;            /**< @brief The player's angle relative to the x-axis (between -PI and PI) */
  bool fire;              /**< @brief True if the player is attempting to fire a bullet */
} Player;

/** @brief Enumeration used for identyfing which player fired a particular bullet. */
enum player_id {
  PLAYER_ONE,
  PLAYER_TWO,
  PLAYER_HOST = PLAYER_ONE,
  PLAYER_REMOTE = PLAYER_TWO
};

/**
 * @brief Updates the position of the bullets and player.
 * @param bullets   linked list containing information about all bullets
 * @param player    pointer to player
 */
void update_entity_positions(LinkedList* bullets, Player* player);

/* --- BULLET --- */

/** @brief Structure representing a bullet */
typedef struct {
  Entity entity;          /**< @brief Bullet entity */
  enum player_id parent;  /**< @brief Player who fired the bullet */
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
 */
void detect_collisions(LinkedList* bullets, Powerup** current_powerup, Player* player);

/**@}*/

#endif /* GAME_LOGIC_H */
