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


Triangle* Triangle_new(Vector2 vertex1, Vector2 vertex2, Vector2 vertex3) {
  double det = vertex1.x * (vertex2.y - vertex3.y) + vertex2.x * (vertex3.y - vertex1.y) + vertex3.x * (vertex1.y - vertex2.y);

  if (det == 0.0) {
    return NULL;
  }
  
  Triangle* this = (Triangle*) malloc(sizeof(Triangle));
  this->vertices[0] = vertex1;
  this->vertices[1] = vertex2;
  this->vertices[2] = vertex3;
  return this;
}

void Triangle_delete(Triangle* this) {
  if (this != NULL) {
    free(this);
  }
}

double Triangle_area(const Triangle* this) {
  if (this == NULL) {
    return 0.0;
  }

  return 0.5 * (this->vertices[0].x * (this->vertices[1].y - this->vertices[2].y) + 
  this->vertices[1].x * (this->vertices[2].y - this->vertices[0].y) + 
  this->vertices[2].x * (this->vertices[0].y - this->vertices[1].y));
}


Circle* Circle_new(double radius) {
  if (radius <= 0.0) {
    return NULL;
  }

  Circle* this = (Circle*) malloc(sizeof(Circle));
  this->radius = radius;
  return this;
}

void Circle_delete(Circle* this) {
  if (this != NULL) {
    free(this);
  }
}

double Circle_area(const Circle* this) {
  return M_PI * pow(this->radius, 2.0);
}

bool triangle_circle_collision(const Triangle* triangle, Vector2 triangle_pos, const Circle* circle, Vector2 circle_pos) {
  /* Convert relative vertex positions to absolute */
  Vector2 real_vertices[3] = { 
    Vector2_add(triangle_pos, triangle->vertices[0]), 
    Vector2_add(triangle_pos, triangle->vertices[1]),
    Vector2_add(triangle_pos, triangle->vertices[2])
  };

  /* Check if any of the triangle's vertices are inside the circle */
  for (size_t i = 0; i < 3; ++i) {
    if (Vector2_distance_to(circle_pos, real_vertices[i]) <= circle->radius) {
      return true;
    }
  }

  /* Check if the circle's centre is inside the triangle */
  Vector2 sides[3][2] = { 
  { real_vertices[0], real_vertices[1] },
  { real_vertices[1], real_vertices[2] },
  { real_vertices[0], real_vertices[2] } };

  bool is_inside = true;
  Vector2 normal;

  for (size_t i = 0; i < 3; ++i) {
    normal.x = sides[i][1].y - sides[i][0].y;
    normal.y = sides[i][0].x - sides[i][1].x;

    if (Vector2_dot_prod(circle_pos, normal) < 0) {
      is_inside = false;
      break;
    }
  }

  if (is_inside) {
    return true;
  }

  /* TODO: Check if circle intersects an edge of the triangle */

  return false;
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
  const double MAX_X = vg_get_x_resolution() - cursor->sprite.img.width;
  const double MAX_Y = vg_get_y_resolution() - cursor->sprite.img.height;
  cursor->position.x = clamp((double) cursor->position.x, 0.0, MAX_X);
  cursor->position.y = clamp((double) cursor->position.y, 0.0, MAX_Y);
}


Powerup* Powerup_new(Entity* entity, enum powerup_type type) {
  if (entity == NULL) {
    return NULL;
  }

  Powerup* this = (Powerup*) malloc(sizeof(Powerup));
  this->entity = entity;
  this->type = type;
  return this;
}

void Powerup_delete(Powerup* this) {
  if (this != NULL) {
    free(this);
  }
}
