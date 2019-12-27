#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

typedef struct Node {
   void* data;
   struct Node* previous;
   struct Node* next;
} Node;

typedef struct {
    Node* first;
    Node* last;
    size_t size;
    size_t elem_size;
} LinkedList;

LinkedList* LinkedList_new(size_t elem_size);

void LinkedList_delete(LinkedList* this);

void LinkedList_add(LinkedList* this, void* elem);

void LinkedList_erase(LinkedList* this, void* elem);


int (util_sys_inb)(int port, uint8_t *value);

int (util_get_LSB)(uint16_t val, uint8_t *lsb);

int (util_get_MSB)(uint16_t val, uint8_t *msb);

/* Returns the value of x clamped by a lower and upper bound */
double clamp(double x, double lower, double upper);

int min(int a, int b);
int max(int a, int b);

#endif /* __UTILS_H */
