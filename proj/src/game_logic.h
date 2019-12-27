#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <lcom/lcf.h>
#include "utils.h"

/* Basic two-dimensional vector. */
typedef struct {
  double x, y;
} Vector2;

/* Calculates the norm (length) of the given vector */
double Vector2_norm(Vector2 this);

/* Adds two vectors */
Vector2 Vector2_add(Vector2 lhs, Vector2 rhs);

/* Subtracts two vectors */
Vector2 Vector2_subtract(Vector2 lhs, Vector2 rhs);

/* Returns a normalized (length = 1) version of a vector */
Vector2 Vector2_normalized(Vector2 this);

/* Returns the dot product of the given vectors */
double Vector2_dot_prod(Vector2 lhs, Vector2 rhs);

/* Performs a scalar multiplication on the given vector */
Vector2 Vector2_scalar_mult(double scalar, Vector2 this);

/* Returns the distance between the two vector */
double Vector2_distance_to(Vector2 lhs, Vector2 rhs);

/* Returns the angle between the two vectors */
double Vector2_angle_to(Vector2 lhs, Vector2 rhs);

static const Vector2 UP = {0.0, -1.0};
static const Vector2 DOWN = {0.0, 1.0};
static const Vector2 LEFT = {-1.0, 0.0};
static const Vector2 RIGHT = {1.0, 0.0};


Vector2 generate_random_pos(uint16_t max_x, uint16_t max_y);

Vector2 rotate_point(Vector2 point, double angle);


typedef struct {
  Vector2 vertices[3];
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


/* Value that controls the rendering order of the sprites. Sprites with a higher z_layer value will be drawn on top. */
enum z_layer {
  BACKGROUND,
  POWERUP,
  PLAYER,
  BULLET,
  MOUSE_CURSOR
};

#define NUM_Z_LAYERS  5


typedef struct {
  xpm_image_t img;
  enum z_layer layer;
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


/* Comparison function for sorting an array of entity pointers with qsort. Used in the rendering pipeline to sort entities by their z layer. */
int compare_entity_ptr(const void* lhs, const void* rhs);

void update_entity_positions(LinkedList* entities[]);
void update_cursor_position(MouseCursor* cursor, Vector2 mouse_pos);


typedef struct {
  Entity* entity;
  uint8_t current_health;
  double angle;
  bool fire;
} Player;

typedef struct {
  Entity* entity;
  bool friendly;
} Bullet;


enum powerup_type {
  SPEED,
  DAMAGE
};

typedef struct {
  Entity* entity;
  enum powerup_type type;
} Powerup;

Powerup* Powerup_new(Entity* entity, enum powerup_type type);
void Powerup_delete(Powerup* this);

#endif /* GAME_LOGIC_H */
