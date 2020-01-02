#include <lcom/lcf.h>
#include <stdlib.h>
#include <math.h>

#include "utils.h"


LinkedList* LinkedList_new(size_t elem_size) {
  LinkedList* this = (LinkedList*) malloc(sizeof(LinkedList));
  this->first = NULL;
  this->last = NULL;
  this->size = 0;
  this->elem_size = elem_size;
  return this;
}

void LinkedList_delete(LinkedList* this) {
  static Node* it;

  if (this != NULL) {
    if (this->size > 0) {
      it = this->first->next;
      while(it != NULL) {
        free(it->previous->data);
        free(it->previous);
        it = it->next;
      }
    }

    free(this);
  }
}

void LinkedList_add(LinkedList* this, void* elem) {
  if (this != NULL && elem != NULL) {
    if (this->size > 0) {
      this->last->next = (Node*) malloc(sizeof(Node));
      this->last->next->data = malloc(this->elem_size);
      memcpy(this->last->next->data, elem, this->elem_size);
      this->last->next->next = NULL;
      this->last->next->previous = this->last;
      this->last = this->last->next;
    }
    else {
      this->first = (Node*) malloc(sizeof(Node));
      this->first->data = malloc(this->elem_size);
      memcpy(this->first->data, elem, this->elem_size);
      this->first->next = NULL;
      this->first->previous = NULL;
      this->last = this->first;
    }
    ++this->size;
  }
}

void LinkedList_erase(LinkedList* this, void* elem) {
  static Node* it;

  if (this != NULL && elem != NULL) {
    if (this->size > 0) {
      if (memcmp(this->first->data, elem, this->elem_size) == 0) {
        it = this->first;
        this->first = this->first->next;
        free(it->data);
        free(it);
        --this->size;
      }
      else {
        it = this->first->next;
        while (memcmp(it->data, elem, this->elem_size) != 0 && it != NULL) {
          it = it->next;
        }
        if (it != NULL) {
          if (it->next == NULL) {
            this->last = it->previous;
          }
          else {
            it->previous->next = it->next;
            it->next->previous = it->previous;
          }
          free(it->data);
          free(it);
          --this->size;
        }
      }
    }
  }
}


int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  uint32_t word;

  if (sys_inb(port, &word) == EINVAL) {
    printf("Error when calling sys_inb.\n");
    return 1;
  }

  *value = (uint8_t)word;
  return 0;
}


int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) {
    printf("Error when calling util_get_LSB.\n");
    return 1;
  }

  *lsb = (uint8_t)(val);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) {
    printf("Error when calling util_get_MSB.\n");
    return 1;
  }

  *msb = (uint8_t)(val >> 8);
  return 0;
}

int util_get_parts(uint32_t val, uint8_t* part1, uint8_t* part2, uint8_t* part3, uint8_t* part4) {
  if ((part1 == NULL) || (part2 == NULL) || (part3 == NULL) || (part4 == NULL)) {
    printf("Error when calling util_get_MSB.\n");
    return 1;
  }

  *part1 = (uint8_t)(val >> 24);
  *part2 = (uint8_t)(val >> 16);
  *part3 = (uint8_t)(val >>  8);
  *part4 = (uint8_t)(val);
  return 0;
}

int util_get_val(uint16_t *val, uint8_t msb, uint8_t lsb) {
  if (val == NULL) {
    printf("Error when calling util_get_MSB.\n");
    return 1;
  }

  *val = ((uint16_t)(msb << 8)) | ((uint16_t)(lsb));
  return 0;
}

int util_join_parts(uint32_t *val, uint8_t part1, uint8_t part2, uint8_t part3, uint8_t part4) {
  if (val == NULL) {
    printf("Error when calling util_get_MSB.\n");
    return 1;
  }

  *val = ((uint32_t)(part1 << 24)) | ((uint32_t)(part2 << 16)) | ((uint32_t)(part3 << 8)) | ((uint32_t)(part1));
  return 0;
}

double clamp(double x, double lower, double upper) {
  return fmin(upper, fmax(x, lower));
}


int min(int a, int b) {
  return a <= b ? a : b;
}

int max(int a, int b) {
  return a >= b ? a : b;
}

void util_erase(uint8_t queue[], int *queue_size, int erase_size) {
  for (int index = erase_size; index < *queue_size; ++ index) {
    queue[index - erase_size] = queue[index];
  }
  *queue_size -= erase_size;
}
