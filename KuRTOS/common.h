#ifndef COMMON_H
#define COMMON_H

#define OS_TICKS_PER_SEC        200
#define CPU_CLK_FREQ    20000000L
#define DIV_ROUND(n,d)   (((n)+((d)>>1))/(d))
#define MSEC_TO_TICKS(ms) DIV_ROUND ((INT32U)(ms)*OS_TICKS_PER_SEC , 1000)
#define T_PS 8                
#define T_RELOAD               (CPU_CLK_FREQ/(OS_TICKS_PER_SEC*T_PS))


#define SAVE_ISP(x) {_asm("STC FB,$$[FB]",x);}
#define RESTORE_ISP(x) \
						{ \
						_asm("LDC $$[FB],ISP",x);\
						_asm("POPM FB");\
						_asm("POPM R0,R1,R2,R3,A0,A1");\
						_asm("REIT");}		
#define NULL 0x0
#define true 0x1
#define false 0x0
typedef unsigned char BOOL;
typedef unsigned int INT16U;
typedef unsigned long INT32U;
typedef void (*TASK_HANDLER)();

#define  ENTER_CRITICAL()   _asm ("PUSHC FLG");_asm("FCLR I")   
#define  EXIT_CRITICAL()   _asm ("POPC FLG")                


// Sched.c
void InitSched();
void StartSched();
void sched();
void InitTask(TASK_HANDLER task, INT16U *stbase, int t_id);
void delay(INT16U ticks);

// Sem.c
void initSemaphore();;
BOOL createSemaphore(int id);
void waitSemaphore(int id);
void deleteSemaphore(int id);
void releaseSemaphore(int id);

// Message.c
void* _receiveMessageQueue(void *queue, int *head, int *tail);

#define QUEUE_SIZE 255
#define createMessageQueue(ch, type) type queue_##ch[QUEUE_SIZE];\
											int queue_##ch_head = 0;\
											int queue_##ch_tail = 0;
											
#define sendMessageQueue(ch, value) { \
											if ( (queue_##ch_tail + 1) % QUEUE_SIZE != 0) {\
											queue_##ch[queue_##ch_tail] = value; \
											queue_##ch_tail =  (queue_##ch_tail + 1) % QUEUE_SIZE; \
											}\
											}
#define receiveMessageQueue(ch) _receiveMessageQueue(((void*)queue_##ch), &queue_##ch_head, &queue_##ch_tail)



#endif