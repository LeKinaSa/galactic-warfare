#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

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


typedef struct {
  uint8_t current_health;
  Vector2 position;
  Vector2 velocity;
} Player;

/* Constructs a new Player structure */
Player* Player_new(Vector2 position, Vector2 velocity);

/* Frees memory allocated for a Player structure */
void Player_delete(Player* this);



#endif /* GAME_LOGIC_H */
