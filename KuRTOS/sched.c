#include "qsk_bsp.h"
#include "common.h"

typedef enum _STATE {DORMANT, READY, RUNNING, WAITING} STATE;
typedef struct {
	INT16U *isp;
	int t_id;
	STATE state;
	INT32U delay;
} ku_tcb;

static int task_count = 0;

static ku_tcb tasks[128];

void InitTask(TASK_HANDLER task, INT16U *stbase, int t_id) {
	ku_tcb *tcb;
	int flag = 0x0040;
	stbase--;
	*stbase-- =		(flag & 0x00FF)
					| (((INT32U)task >> 8) & 0x00000F00)
					| ((flag << 4) & 0xF000);
	*stbase-- = (((INT32U)task) & 0x0000FFFF);
	*stbase-- = (INT16U)0;
	*stbase-- = (INT16U)0;
	*stbase-- = (INT16U)0;
	*stbase-- = (INT16U)0;
	*stbase-- = (INT16U)0;
	*stbase-- = (INT16U)0;
	*stbase = (INT16U)0;
	
	tcb = &tasks[t_id];
	tcb->isp = stbase;
	tcb->t_id = t_id;
	tcb->state = DORMANT;

	task_count++;
}

#define TIMER_CONFIG 0x40 //0x80
#define CNTR_IPL 0x01

void InitSched() {
	ta0 = T_RELOAD;
	_asm("fclr i");
	ta0ic |= 1;
	ta0mr = TIMER_CONFIG;
	_asm("fset i");
}

void StartSched() {
	ta0s = 1;	
}

#pragma INTERRUPT sched
static int current = -1;
INT32U tick = 0;

void delay(INT16U ticks) {
	if (ticks > 0) {
		ENTER_CRITICAL();
		tasks[current].delay = ticks + tick;
		if (tasks[current].delay < ticks) {
			tasks[current].delay = ticks;
		}
		tasks[current].state = WAITING;
		EXIT_CRITICAL();
		_asm("INT #0");
		//
		while(1) {
			if (tasks[current].delay < tick ) break;
			tasks[current].state = WAITING;
			_asm("INT #0");	
		}
	}	
}

void schedWakeUp() {
	int i = 0;
	for (i = 0 ; i < task_count ; i++) {
		if (tasks[i].state == WAITING) {
			tasks[i].state = READY;	
		}	
	}	
}

void sched(void) {
	INT16U *stk;
	int i = 0;
	_asm("fset i");
	ENTER_CRITICAL();
	tick += 5;
	
	if (current == -1) {
		current = 0;
		tasks[current].state = RUNNING;	
	} else {
		tasks[current].state = READY;
		SAVE_ISP(stk);
		tasks[current].isp = stk;
		current = ++current % task_count;
		tasks[current].state = RUNNING;	
	}
	
	stk = tasks[current].isp;
	EXIT_CRITICAL();
	RESTORE_ISP(stk);
}

void schedWaitEvent() {
	int i = 0;
	if (tasks[i].state == RUNNING) {
		tasks[i].state = WAITING;
	}
}

#pragma INTERRUPT button_handler
extern int isButtonClick;
void button_handler() {
	INT16U *stk;
	int i = 0;
	
	ENTER_CRITICAL();

	tasks[current].state = READY;
	SAVE_ISP(stk);
	tasks[current].isp = stk;
	tasks[current].state = RUNNING;	
	isButtonClick = 1;
	stk = tasks[current].isp;
	EXIT_CRITICAL();
	RESTORE_ISP(stk);
}
