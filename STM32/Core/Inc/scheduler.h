#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_
#include "main.h"
#define MAX_TASK_SIZE 10

typedef struct {
	uint32_t TID;
	uint32_t Delay;
	uint32_t Period;
	uint8_t RunMe;
	void (*pTask)();
} task_t;

typedef struct queue {
	task_t* task[MAX_TASK_SIZE];
	uint32_t size;
} queue_t;


task_t* createTask(uint32_t TID, uint32_t Period, uint32_t Delay, void(*pTask)());
void SCH_INIT();
uint32_t SCH_Add_Task(void (*func)(), uint32_t Delay, uint32_t Period);
void SCH_Update_Task();
void SCH_Dispatch_Task();

#endif /* INC_SCHEDULER_H_ */
