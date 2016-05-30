#include "common.h"

typedef enum {NONE, CREATED, DELETED} SEM_STATE;

typedef struct {
	int id;
	int cnt;
	SEM_STATE state;
} SEMAPHORE;

#define SEM_CNT 128
SEMAPHORE semaphore[SEM_CNT];

void initSemaphore() {
	int i = 0;
	for (i = 0 ; i < SEM_CNT ; i++) {
		semaphore[i].id = 0;
		semaphore[i].cnt = 0;
		semaphore[i].state = NONE;	
	}
}

BOOL createSemaphore(int id) {
	ENTER_CRITICAL();
	if (semaphore[id].state == CREATED) return false;
	semaphore[id].state = CREATED;
	EXIT_CRITICAL();
	return true;
}

void waitSemaphore(int id) {
	ENTER_CRITICAL();
	if (semaphore[id].cnt == 0) {
		semaphore[id].cnt = 1;
		//_asm("INT #21");
		//sched();
	} else {
		while (1) {
			if (semaphore[id].cnt == 0) break;
			schedWaitEvent();
			_asm("INT #0");
		}
	}
	EXIT_CRITICAL();
};

void deleteSemaphore(int id) {
	semaphore[id].state = DELETED;
	semaphore[id].cnt = 0;
}

void releaseSemaphore(int id) {
	ENTER_CRITICAL();
	if (semaphore[id].cnt == 1) {
		semaphore[id].cnt = 0;
		schedWakeUp();
		_asm("INT #0");	
	}
	EXIT_CRITICAL();
}