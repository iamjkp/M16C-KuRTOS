#include "common.h"

void* _receiveMessageQueue(void *queue, int *head, int *tail) {
	int tmp = 0;
	if (*head == *tail) return NULL;
	tmp = ((int*)queue)[*head];
	*head = (*head + 1) % QUEUE_SIZE;
	return (void*)tmp;
}