#include "scheduler.h"
#include <stddef.h>

static sTask SCH_tasks_G[SCH_MAX_TASKS];

void SCH_Init(void) {
	for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
		SCH_tasks_G[i].pTask = NULL;
		SCH_tasks_G[i].Delay = 0;
		SCH_tasks_G[i].Period = 0;
		SCH_tasks_G[i].RunMe = 0;
		SCH_tasks_G[i].TaskID = SCH_INVALID_TASK_ID;
		SCH_tasks_G[i].Armed = 0;
	}
}

uint32_t SCH_Add_Task(
    void (*pFunction)(void),
    uint32_t DELAY,
    uint32_t PERIOD)
{

    if (pFunction == NULL) {
        return SCH_INVALID_TASK_ID;
    }
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    uint32_t index = 0;

    // Kiem tra con tro ham hop le

    // Tim vi tri trong trong mang task
    while (index < SCH_MAX_TASKS &&
           SCH_tasks_G[index].pTask != NULL) {
        index++;
    }

    // Mang da day
    if (index == SCH_MAX_TASKS) {
    	__set_PRIMASK(primask);
        return SCH_INVALID_TASK_ID;
    }

    // Luu thong tin task
    SCH_tasks_G[index].pTask = pFunction;
    SCH_tasks_G[index].Delay = DELAY;
    SCH_tasks_G[index].Period = PERIOD;
    SCH_tasks_G[index].RunMe = 0;
    SCH_tasks_G[index].TaskID = index;
    SCH_tasks_G[index].Armed = 1;

    __set_PRIMASK(primask);
    // Tra ve ID cua task
    return index;
}

void SCH_Update(void) {
	for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
		if (SCH_tasks_G[i].pTask == NULL || SCH_tasks_G[i].Armed == 0) {
			continue;
		}

		if (SCH_tasks_G[i].Delay > 0) {
			SCH_tasks_G[i].Delay--;
		}

		if (SCH_tasks_G[i].Delay == 0) {
		    if (SCH_tasks_G[i].RunMe < UINT8_MAX) {
		        SCH_tasks_G[i].RunMe++;
		    }

		    if (SCH_tasks_G[i].Period > 0) {
		        SCH_tasks_G[i].Delay = SCH_tasks_G[i].Period;
		    } else {
		        // One-shot chi duoc len lich mot lan
		        SCH_tasks_G[i].Armed = 0;
		    }
		}
	}
}

void SCH_Dispatch_Tasks(void)
{
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {

        uint32_t primask = __get_PRIMASK();
        __disable_irq();

        if (SCH_tasks_G[i].pTask == NULL ||
            SCH_tasks_G[i].RunMe == 0) {
            __set_PRIMASK(primask);
            continue;
        }

        // Lay mot luot thuc thi
        SCH_tasks_G[i].RunMe--;

        void (*taskFunction)(void) =
            SCH_tasks_G[i].pTask;

        uint32_t period =
            SCH_tasks_G[i].Period;

        uint32_t taskID =
            SCH_tasks_G[i].TaskID;

        __set_PRIMASK(primask);

        // Chay callback ngoai vung khoa ngat
        taskFunction();

        if (period == 0) {
            SCH_Delete_Task(taskID);
        }
    }
}


uint8_t SCH_Delete_Task(uint32_t taskID)
{
    // Kiem tra Task ID co hop le khong
    if (taskID >= SCH_MAX_TASKS) {
        return 0;
    }

    // Kiem tra task co ton tai khong
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (SCH_tasks_G[taskID].pTask == NULL) {
    	__set_PRIMASK(primask);
        return 0;
    }

    // Tam khoa ngat de tranh ISR thay doi task

    // Xoa thong tin task
    SCH_tasks_G[taskID].pTask = NULL;
    SCH_tasks_G[taskID].Delay = 0;
    SCH_tasks_G[taskID].Period = 0;
    SCH_tasks_G[taskID].RunMe = 0;
    SCH_tasks_G[taskID].TaskID = SCH_INVALID_TASK_ID;
    SCH_tasks_G[taskID].Armed = 0;

    // Khoi phuc trang thai ngat ban dau
    __set_PRIMASK(primask);

    return 1;
}

