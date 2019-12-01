#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <lcom/lcf.h>

/* Basic two-dimensional vector. */
typedef struct {
  double x, y;
} Vector2;

/* Constructs a new Vector2 structure */
Vector2* Vector2_new(double x, double y);

/* Frees memory allocated for a Vector2 structure */
void Vector2_delete(Vector2* this);

/* Calculates the norm (length) of the given vector */
double Vector2_norm(const Vector2* this);

/* Adds two vectors */
Vector2* Vector2_add(const Vector2* lhs, const Vector2* rhs);

/* Subtracts two vectors */
Vector2* Vector2_subtract(const Vector2* lhs, const Vector2* rhs);

/* Returns a normalized (length = 1) version of a vector */
Vector2* Vector2_normalized(const Vector2* this);

/* Returns the dot product of the given vectors */
double Vector2_dot_prod(const Vector2* lhs, const Vector2* rhs);

/* Performs a scalar multiplication on the given vector */
Vector2* Vector2_scalar_mult(double scalar, const Vector2* this);

/* Returns the distance between the two vector */
double Vector2_distance_to(const Vector2* lhs, const Vector2* rhs);

/* Returns the angle between the two vectors */
double Vector2_angle_to(const Vector2* lhs, const Vector2* rhs);

static const Vector2 UP = {0.0, -1.0};
static const Vector2 DOWN = {0.0, 1.0};
static const Vector2 LEFT = {-1.0, 0.0};
static const Vector2 RIGHT = {1.0, 0.0};



/* Value that controls the rendering order of the sprites. Sprites with a higher z_layer value will be drawn on top. */
enum z_layer {
  BACKGROUND,
  POWERUP,
  PLAYER,
  BULLET
};

typedef struct {
  xpm_image_t img;
  enum z_layer layer;
} Sprite;

typedef struct {
  Sprite sprite;
  Vector2 position;
  Vector2 velocity;
} Entity;

/* Comparison function for sorting an array of entity pointers with qsort. Used in the rendering pipeline to sort entities by their z layer. */
int compare_entity_ptr(const void* lhs, const void* rhs);

void update_entity_positions(Entity* entities[], uint8_t num_entities);

typedef struct {
  Entity* entity;
  uint8_t current_health;
} Player;

#endif /* GAME_LOGIC_H */
