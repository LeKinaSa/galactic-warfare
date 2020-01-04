#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

/** @defgroup utils utils
 * @{
 * @brief General purpose functions and data structures.
 */

/**
 * @brief Structure used to implement a generic doubly-linked list.
 */
typedef struct Node {
   void* data;              /**< @brief Memory location of the node's data */
   struct Node* previous;   /**< @brief Pointer to the previous node. NULL if it doesn't exist */
   struct Node* next;       /**< @brief Pointer to the next node. NULL if it doesn't exist. */
} Node;

/**
 * @brief Implementation of a generic doubly-linked list using void* and dynamic memory allocation.
 */
typedef struct {
    Node* first;        /**< @brief Pointer to the first node of the linked list. NULL if the list is empty */
    Node* last;         /**< @brief Pointer to the last node of the linked list. NULL if the list is empty */
    size_t size;        /**< @brief Number of nodes (elements) of the linked list */
    size_t elem_size;   /**< @brief Size of the each element in the linked list */
} LinkedList;

/**
 * @brief Creates a new linked list with elements of the given size.
 * Memory for the linked list is dynamically allocated.
 * 
 * @param elem_size     size of the elements of the linked list
 * @return LinkedList*  pointer to newly created linked list
 */
LinkedList* LinkedList_new(size_t elem_size);

/**
 * @brief Safely deletes a linked list. Any dynamically allocated memory is freed.
 * 
 * @param this  pointer to linked list to delete
 */
void LinkedList_delete(LinkedList* this);

/**
 * @brief Adds a new element to the linked list.
 * 
 * @param this  pointer to linked list 
 * @param elem  pointer to element to add
 */
void LinkedList_add(LinkedList* this, void* elem);

/**
 * @brief Deletes the first occurrence of a given element from the linked list.
 * 
 * @param this  pointer to linked list
 * @param elem  pointer to element to delete
 */
void LinkedList_erase(LinkedList* this, void* elem);


int (util_sys_inb)(int port, uint8_t *value);


int (util_get_LSB)(uint16_t val, uint8_t *lsb);
int (util_get_MSB)(uint16_t val, uint8_t *msb);


int util_get_parts(uint32_t val, uint8_t* part1, uint8_t* part2, uint8_t* part3, uint8_t* part4);

int util_get_val(uint16_t* val, uint8_t msb, uint8_t lsb);

int util_join_parts(uint32_t* val, uint8_t part1, uint8_t part2, uint8_t part3, uint8_t part4);

/** @brief Returns the value of x limited by a lower and upper bound */
double clamp(double x, double lower, double upper);

/** @brief Returns the smallest of two integers. */
int min(int a, int b);
/** @brief Returns the largest of two integers. */
int max(int a, int b);

void util_erase(uint8_t queue[], int *queue_size, int erase_size);

/**@}*/

#endif /* __UTILS_H */
