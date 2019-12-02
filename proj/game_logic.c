#include <stdlib.h>
#include <math.h>

#include "game_logic.h"
#include "game_constants.h"
#include "video.h"
#include "utils.h"


double Vector2_norm(Vector2 this) {
  return sqrt(pow(this.x, 2.0) + pow(this.y, 2.0));
}

Vector2 Vector2_add(Vector2 lhs, Vector2 rhs) {
  double x = lhs.x + rhs.x;
  double y = lhs.y + rhs.y;

  return (Vector2) {x, y};
}

Vector2 Vector2_subtract(Vector2 lhs, Vector2 rhs) {
  double x = lhs.x - rhs.x;
  double y = lhs.y - rhs.y;

  return (Vector2) {x, y};
}

Vector2 Vector2_normalized(Vector2 this) {
  double norm = Vector2_norm(this);

  if (norm == 0.0) {
    return (Vector2) {0.0, 0.0};
  }

  return (Vector2) {this.x / norm, this.y / norm};
}

double Vector2_dot_prod(Vector2 lhs, Vector2 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

Vector2 Vector2_scalar_mult(double scalar, Vector2 this) {
  return (Vector2) {scalar * this.x, scalar * this.y};
}

double Vector2_distance_to(Vector2 lhs, Vector2 rhs) {
  return Vector2_norm(Vector2_subtract(lhs, rhs));
}

double Vector2_angle_to(Vector2 lhs, Vector2 rhs) {
  double delta_x = rhs.x - lhs.x;
  double delta_y = lhs.y - rhs.y;
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

void update_cursor_position(MouseCursor* cursor, Vector2 mouse_pos) {
  cursor->position = Vector2_add(mouse_pos, cursor->offset);
}
