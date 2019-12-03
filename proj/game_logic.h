#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <lcom/lcf.h>

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



/* Value that controls the rendering order of the sprites. Sprites with a higher z_layer value will be drawn on top. */
enum z_layer {
  BACKGROUND,
  POWERUP,
  PLAYER,
  BULLET,
  MOUSE_CURSOR
};

typedef struct {
  xpm_image_t img;
  enum z_layer layer;
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

void update_entity_positions(Entity* entities[], uint8_t num_entities);
void update_cursor_position(MouseCursor* cursor, Vector2 mouse_pos);

typedef struct {
  Entity* entity;
  uint8_t current_health;
  double angle;
  bool fire;
} Player;

#endif /* GAME_LOGIC_H */
