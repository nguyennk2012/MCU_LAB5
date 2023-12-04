#include "scheduler.h"
#include "stdlib.h"

static queue_t ReadyQueue;
static queue_t RunQueue;

task_t* createTask(uint32_t TID, uint32_t Period, uint32_t Delay, void(*pTask)())
{
    task_t* newTask = (task_t*)malloc(sizeof(task_t));
    newTask->TID = TID;
    newTask->Delay = Delay;
    newTask->Period = Period;
    newTask->pTask = pTask;
    newTask->RunMe = 0;

    return newTask;
}

void SCH_INIT()
{
    ReadyQueue.size = 0;
    RunQueue.size = 0;
}

uint32_t SCH_Add_Task(void (*func)(), uint32_t Delay, uint32_t Period)
{
    if (ReadyQueue.size == MAX_TASK_SIZE) return 0;   // HOW ABOUT ERROR CODE?
    task_t* newTask = createTask(1, Period, Delay, func);

    // ADD TASK
    int32_t idx = ReadyQueue.size - 1;
    while (idx >= 0 && (int32_t)(newTask->Delay - ReadyQueue.task[idx]->Delay) >= 0)
    {
        newTask->Delay -= ReadyQueue.task[idx]->Delay;
        ReadyQueue.task[idx + 1] = ReadyQueue.task[idx];
        idx--;
    }
    idx += 1;
    if (idx > 0)
    {
        ReadyQueue.task[idx - 1]->Delay -= newTask->Delay;
    }
    ReadyQueue.task[idx] = newTask;
    ++ReadyQueue.size;
    return newTask->TID;
}

void SCH_Update_Task()
{
    if (ReadyQueue.size != 0)
    {
        if (ReadyQueue.task[ReadyQueue.size - 1]->Delay == 0)
        {
            ReadyQueue.size --;
            ReadyQueue.task[ReadyQueue.size]->RunMe += 1;
            ReadyQueue.task[ReadyQueue.size]->Delay = ReadyQueue.task[ReadyQueue.size]->Period;
            RunQueue.task[RunQueue.size] = ReadyQueue.task[ReadyQueue.size];
            RunQueue.size++;
        }
        else
        {
            ReadyQueue.task[ReadyQueue.size - 1]->Delay--;
        }
    }

}

void SCH_Dispatch_Task()
{
    while (RunQueue.size != 0)
    {
        (*(RunQueue.task[RunQueue.size-1]->pTask))();
        RunQueue.task[RunQueue.size - 1]->RunMe -= 1;
        if (RunQueue.task[RunQueue.size - 1]->Period != 0)
        {
            SCH_Add_Task(RunQueue.task[RunQueue.size - 1]->pTask, RunQueue.task[RunQueue.size - 1]->Delay, RunQueue.task[RunQueue.size - 1]->Period);
        }
        free(RunQueue.task[RunQueue.size - 1]);
        RunQueue.task[RunQueue.size - 1] = NULL;
        RunQueue.size--;
    }
}
