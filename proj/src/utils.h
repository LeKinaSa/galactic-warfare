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

/**
 * @brief Reads a byte from the specified port.
 * @param port  port to read
 * @param value pointer to where the byte will be stored
 * @return      zero if no errors occurred, non-zero otherwise
 */
int (util_sys_inb)(int port, uint8_t *value);

/**
 * @brief Gets least significant byte of a 16 bit number.
 * @param val   16 bit number
 * @param lsb   pointer to where the LSB will be stored
 * @return zero if no errors occurred, non-zero otherwise
 */
int (util_get_LSB)(uint16_t val, uint8_t *lsb);
/**
 * @brief Gets most significant byte of a 16 bit number.
 * @param val   16 bit number
 * @param lsb   pointer to where the MSB will be stored
 * @return zero if no errors occurred, non-zero otherwise
 */
int (util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief Gets the four bytes that make up a given 32 bit number, from most to least significant.
 * @param val   32 bit number
 * @param part1 pointer to where the first byte will be stored
 * @param part2 pointer to where the second byte will be stored
 * @param part3 pointer to where the third byte will be stored
 * @param part4 pointer to where the fourth byte will be stored
 * @return      zero if no errors occurred, non-zero otherwise
 */
int util_get_parts(uint32_t val, uint8_t* part1, uint8_t* part2, uint8_t* part3, uint8_t* part4);

/**
 * @brief Appends two bytes to make a 16 bit number.
 * @param val   pointer to where the 16 bit number will be stored
 * @param msb   most significant byte
 * @param lsb   least significant byte
 * @return      zero if no errors occurred, non-zero otherwise
 */
int util_get_val(uint16_t* val, uint8_t msb, uint8_t lsb);

/**
 * @brief Appends four bytes to make a 32 bit number, from most to least significant.
 * @param val   pointer to where the 32 bit number will be stored
 * @param part1 first byte
 * @param part2 second byte
 * @param part3 third byte
 * @param part4 fourth byte
 * @return      zero if no errors occurred, non-zero otherwise
 */
int util_join_parts(uint32_t* val, uint8_t part1, uint8_t part2, uint8_t part3, uint8_t part4);

/** 
 * @brief Returns the value of x limited by a lower and upper bound.
 * @param x     value to clamp
 * @param lower lower bound
 * @param upper upper bound
 * @return      value limited by the bounds
 */
double clamp(double x, double lower, double upper);

/** @brief Returns the smallest of two integers. */
int min(int a, int b);
/** @brief Returns the largest of two integers. */
int max(int a, int b);

/**
 * @brief Erases a given number of bytes from the start of a queue, shifting the next bytes to the
 * start.
 * @param queue         queue to perform operation on
 * @param queue_size    pointer to the size of the queue (so it can be changed later)
 * @param erase_size    number of bytes to erase
 */
void util_erase(uint8_t queue[], int *queue_size, int erase_size);

/**@}*/

#endif /* __UTILS_H */
