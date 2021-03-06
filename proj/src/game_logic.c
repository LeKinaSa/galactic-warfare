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


Vector2 generate_random_pos(uint16_t max_x, uint16_t max_y) {
  double x = (double) (rand() % max_x);
  double y = (double) (rand() % max_y);

  return (Vector2) { x, y };
}

Vector2 rotate_point(Vector2 point, double angle) {
  return (Vector2) { cos(angle) * point.x + sin(angle) * point.y, - sin(angle) * point.x + cos(angle) * point.y };
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

bool circle_circle_collision(const Circle* circle1, Vector2 circle1_pos, const Circle* circle2, Vector2 circle2_pos) {
  return (Vector2_distance_to(circle1_pos, circle2_pos) < (circle1->radius + circle2->radius));
}



void update_cursor_position(MouseCursor* cursor, Vector2 mouse_pos) {
  cursor->position = Vector2_add(mouse_pos, cursor->offset);
  const double MAX_X = vg_get_x_resolution() - cursor->sprite.img.width;
  const double MAX_Y = vg_get_y_resolution() - cursor->sprite.img.height;
  cursor->position.x = clamp((double) cursor->position.x, 0.0, MAX_X);
  cursor->position.y = clamp((double) cursor->position.y, 0.0, MAX_Y);
}


void update_entity_position(Entity* entity) {
  const uint16_t x_res = vg_get_x_resolution(), y_res = vg_get_y_resolution();

  if (entity != NULL) {
    entity->position.x = clamp((entity->position.x + entity->velocity.x), 0.0, (double)(x_res - entity->sprite.img.width));
    entity->position.y = clamp((entity->position.y + entity->velocity.y), 0.0, (double)(y_res - entity->sprite.img.height));
  }
}

void update_entity_positions(LinkedList* bullets, Player* player) {
  const uint16_t x_res = vg_get_x_resolution(), y_res = vg_get_y_resolution();

  update_entity_position(player->entity);

  static Node* current_node;
  static Node* node_to_erase = NULL;
  static Entity* current_entity;

  static uint16_t bullet_x, bullet_y;

  if (bullets->size > 0) {
    current_node = bullets->first;

    while (current_node != NULL) {
      node_to_erase = NULL;

      current_entity = &((Bullet*)(current_node->data))->entity;
      update_entity_position(current_entity);

      bullet_x = (uint16_t) round(current_entity->position.x);
      bullet_y = (uint16_t) round(current_entity->position.y);

      if (bullet_x == 0 || bullet_x == (x_res - current_entity->sprite.img.width) || 
      bullet_y == 0 || bullet_y == (y_res - current_entity->sprite.img.height)) {
        node_to_erase = current_node;
      }

      current_node = current_node->next;

      if (node_to_erase != NULL) {
        LinkedList_erase(bullets, node_to_erase->data);
      }
    }
  }
}


void reset_speed_and_damage(Player* player) {
  player->speed = PLAYER_BASE_SPEED;
  player->damage = PLAYER_BASE_DAMAGE;
}


void detect_collisions(LinkedList* bullets, Powerup** current_powerup, Player* player, Player* enemy) {
  if (current_powerup == NULL || player == NULL || enemy == NULL) {
    return;
  }

  Entity* player_entity = player->entity;
  Entity* enemy_entity = enemy->entity;
  Circle* player_collision_shape = (Circle*)(player_entity->sprite.collision_shape);
  Circle* enemy_collision_shape = (Circle*)(enemy_entity->sprite.collision_shape);

  if (*current_powerup != NULL) {
    Entity* powerup_entity = (*current_powerup)->entity;
    Circle* powerup_collision_shape = (Circle*)((*current_powerup)->entity->sprite.collision_shape);
    
    if (circle_circle_collision(player_collision_shape, Vector2_subtract(player_entity->position, player_entity->offset), 
    powerup_collision_shape, Vector2_add(powerup_entity->position, powerup_entity->offset))) {
      if ((*current_powerup)->type == SPEED) {
        player->speed = PLAYER_BASE_SPEED * 2;
      }
      else {
        player->damage = PLAYER_BASE_DAMAGE * 2;
      }
      (*current_powerup) = NULL;  
    }
    else if (circle_circle_collision(enemy_collision_shape, Vector2_subtract(enemy_entity->position, enemy_entity->offset),
    powerup_collision_shape, Vector2_add(powerup_entity->position, powerup_entity->offset))) {
      if ((*current_powerup)->type == DAMAGE) {
        enemy->damage = PLAYER_BASE_DAMAGE * 2;
      }
      (*current_powerup) = NULL;
    }
  }

  if (bullets->size > 0) {
    Node* current_node = bullets->first;
    Node* node_to_erase;
    Bullet* current_bullet;
    Circle* bullet_collision_shape = NULL;

    while (current_node != NULL) {
      node_to_erase = NULL;

      current_bullet = (Bullet*)(current_node->data);
      bullet_collision_shape = (Circle*)(current_bullet->entity.sprite.collision_shape);

      if (!current_bullet->friendly) {
        if (circle_circle_collision(player_collision_shape, Vector2_subtract(player_entity->position, player_entity->offset), 
        bullet_collision_shape, current_bullet->entity.position)) {
          node_to_erase = current_node;
          if (player->current_health > current_bullet->damage) {
            player->current_health -= current_bullet->damage;
          }
          else {
            player->current_health = 0;
          }
        }
      }
      else {
        if (circle_circle_collision(enemy_collision_shape, Vector2_subtract(enemy_entity->position, enemy_entity->offset),
        bullet_collision_shape, Vector2_add(current_bullet->entity.position, current_bullet->entity.offset))) {
          node_to_erase = current_node;
          if (enemy->current_health > current_bullet->damage) {
            enemy->current_health -= current_bullet->damage;
          }
          else {
            enemy->current_health = 0;
          }
        }
      }

      current_node = current_node->next;

      if (node_to_erase != NULL) {
        LinkedList_erase(bullets, node_to_erase->data); 
      }
    }
  }
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
