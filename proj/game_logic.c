#include <stdlib.h>
#include <math.h>

#include "game_logic.h"
#include "game_constants.h"
#include "video.h"
#include "utils.h"

Vector2* Vector2_new(double x, double y) {
  Vector2* this = (Vector2*) malloc(sizeof(Vector2));

  this->x = x;
  this->y = y;
  
  return this;
}

void Vector2_delete(Vector2* this) {
  free(this);
}

double Vector2_norm(const Vector2* this) {
  return sqrt(pow(this->x, 2.0) + pow(this->y, 2.0));
}

Vector2* Vector2_add(const Vector2* lhs, const Vector2* rhs) {
  double x = lhs->x + rhs->x;
  double y = lhs->y + rhs->y;

  return Vector2_new(x, y);
}

Vector2* Vector2_subtract(const Vector2* lhs, const Vector2* rhs) {
  double x = lhs->x - rhs->x;
  double y = lhs->y - rhs->y;

  return Vector2_new(x, y);
}

Vector2* Vector2_normalized(const Vector2* this) {
  double norm = Vector2_norm(this);

  if (norm == 0.0) {
    return Vector2_new(0.0, 0.0);
  }

  return Vector2_new(this->x / norm, this-> y / norm);
}

double Vector2_dot_prod(const Vector2* lhs, const Vector2* rhs) {
  return (lhs->x * rhs->x) + (lhs->y * rhs->y);
}

Vector2* Vector2_scalar_mult(double scalar, const Vector2* this) {
  return Vector2_new(scalar * this->x, scalar * this->y);
}

double Vector2_distance_to(const Vector2* lhs, const Vector2* rhs) {
  return Vector2_norm(Vector2_subtract(lhs, rhs));
}

double Vector2_angle_to(const Vector2* lhs, const Vector2* rhs) {
  double delta_x = rhs->x - lhs->x;
  double delta_y = lhs->y - rhs->y;
  return atan2(delta_y, delta_x);
}


int compare_entity_ptr(const void* lhs, const void* rhs) {
  Entity* ptr1 = *((Entity**) lhs);
  Entity* ptr2 = *((Entity**) rhs);

  if (ptr1->sprite.layer == ptr2->sprite.layer) {
    return 0;
  }
  else if (ptr1->sprite.layer < ptr2->sprite.layer) {
    return -1;
  }
  else {
    return 1;
  }
}

void update_entity_positions(Entity* entities[], uint8_t num_entities) {
  uint16_t x_res = vg_get_x_resolution(), y_res = vg_get_y_resolution();
  Entity* current_entity;

  for (uint8_t i = 0; i < num_entities; i++) {
    current_entity = entities[i];

    current_entity->position.x = clamp((current_entity->position.x + current_entity->velocity.x), 0.0, (double)(x_res - current_entity->sprite.img.width));
    current_entity->position.y = clamp((current_entity->position.y + current_entity->velocity.y), 0.0, (double)(y_res - current_entity->sprite.img.height));
  }
}
