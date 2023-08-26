/*
 * utils.h
 *
 *  Created on: 10 ago. 2023
 *      Original Author: Emiliano Augusto Gonzalez (egonzalez . hiperion @ gmail . com)
 *
 * Further modified by Kayne Ruse, and added to the Toy Programming Language tool repository.
 */

#ifndef UTILS_H_
#define UTILS_H_

typedef struct queue_node_s {
	void *data;
	struct queue_node_s *next;
} queue_node_t;

void dis_enqueue(void *x, queue_node_t *queue_front, queue_node_t *queue_rear);
void dis_dequeue(queue_node_t *queue_front, queue_node_t *queue_rear);

#endif /* UTILS_H_ */
