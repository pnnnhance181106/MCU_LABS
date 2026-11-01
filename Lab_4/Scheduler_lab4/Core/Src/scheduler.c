#include "scheduler.h"
#include <stddef.h>

#define SCH_NULL_INDEX (-1)

static int16_t SCH_head = SCH_NULL_INDEX; //task dau tien
static volatile uint32_t SCH_pendingTicks = 0; //so tick chua duoc dispatcher xu ly
static sTask SCH_tasks_G[SCH_MAX_TASKS];


void SCH_Update(void) {
	if (SCH_pendingTicks < UINT32_MAX)
	SCH_pendingTicks++;
}

static void SCH_Queue_Insert(int16_t index, uint32_t delay) {
	int16_t prev = SCH_NULL_INDEX;
	int16_t cur = SCH_head;

	while (cur != SCH_NULL_INDEX && delay >= SCH_tasks_G[cur].Delay) {
		//di chuyen toi vi tri co delay be hon delay cua task phia sau
		delay -= SCH_tasks_G[cur].Delay;
		prev = cur;
		cur = SCH_tasks_G[cur].Next;
	}

	SCH_tasks_G[index].Delay = delay; //luu gia tri index tuong doi moi
	SCH_tasks_G[index].Next = cur;

	if (cur != SCH_NULL_INDEX) {
		SCH_tasks_G[cur].Delay -= delay; //dieu chinh delay cua task phia sau
	}
	if (prev == SCH_NULL_INDEX) {
		//task moi co deadline som nhat, tro thanh head
		SCH_head = index;
	}
	else {
		//task moi duoc noi sau prev
		SCH_tasks_G[prev].Next = index;
	}
}
static void SCH_Queue_Remove(int16_t index) {
	//ham xoa 1 task, phai cong delay cua no cho task ke tiep
	int16_t prev = SCH_NULL_INDEX;
	int16_t cur = SCH_head;

	while (cur != SCH_NULL_INDEX && cur != index) {
		prev = cur;
		cur = SCH_tasks_G[cur].Next;
	}
	if (cur == SCH_NULL_INDEX) return;

	int16_t next_idx = SCH_tasks_G[cur].Next;
	if (next_idx != SCH_NULL_INDEX) {
		SCH_tasks_G[next_idx].Delay += SCH_tasks_G[cur].Delay;
	}
	if (prev == SCH_NULL_INDEX) {
		SCH_head = next_idx;
	}
	else {
		SCH_tasks_G[prev].Next = next_idx;
	}
	SCH_tasks_G[cur].Next = SCH_NULL_INDEX;
}

void SCH_Init(void) {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	SCH_head = SCH_NULL_INDEX;
	SCH_pendingTicks = 0;

	for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
		SCH_tasks_G[i].pTask = NULL;
		SCH_tasks_G[i].Delay = 0;
		SCH_tasks_G[i].Period = 0;
		SCH_tasks_G[i].TaskID = SCH_INVALID_TASK_ID;
		SCH_tasks_G[i].Next = SCH_NULL_INDEX;
	}
	__set_PRIMASK(primask);
}

uint32_t SCH_Add_Task(
    void (*pFunction)(void),
    uint32_t DELAY,
    uint32_t PERIOD)
{
	if (pFunction == NULL) return SCH_INVALID_TASK_ID;

	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	uint32_t index = 0;
	while (index < SCH_MAX_TASKS && SCH_tasks_G[index].pTask != NULL) {
		index++;
	}
	if (index == SCH_MAX_TASKS) {
		__set_PRIMASK(primask);
		return SCH_INVALID_TASK_ID;
	}

    // Tinh tu thoi diem hien tai,
    // bao gom cac tick chua duoc xu ly
    if (DELAY > UINT32_MAX - SCH_pendingTicks) {
        __set_PRIMASK(primask);
        return SCH_INVALID_TASK_ID;
    }

    uint32_t queueDelay = DELAY + SCH_pendingTicks;

    SCH_tasks_G[index].Period = PERIOD;
    SCH_tasks_G[index].TaskID = index;
    SCH_tasks_G[index].Next = SCH_NULL_INDEX;
    SCH_tasks_G[index].pTask = pFunction;

    SCH_Queue_Insert((int16_t)index, queueDelay);

    __set_PRIMASK(primask);

    return index;
}


void SCH_Dispatch_Tasks(void)
{
    while (1)
    {
        void (*taskFunction)(void) = NULL;

        uint32_t primask = __get_PRIMASK();
        __disable_irq();

        // Khong co task trong queue
        if (SCH_head == SCH_NULL_INDEX)
        {
            SCH_pendingTicks = 0;
            __set_PRIMASK(primask);
            return;
        }

        int16_t index = SCH_head;

        // Xu ly cac tick chua duoc tieu thu
        if (SCH_tasks_G[index].Delay > 0)
        {
            if (SCH_pendingTicks < SCH_tasks_G[index].Delay)
            {
                SCH_tasks_G[index].Delay -= SCH_pendingTicks;
                SCH_pendingTicks = 0;

                __set_PRIMASK(primask);
                return;
            }

            SCH_pendingTicks -= SCH_tasks_G[index].Delay;
            SCH_tasks_G[index].Delay = 0;
        }

        // Task dau queue da den han
        taskFunction = SCH_tasks_G[index].pTask;
        uint32_t period = SCH_tasks_G[index].Period;

        // Lay task ra khoi queue
        SCH_head = SCH_tasks_G[index].Next;
        SCH_tasks_G[index].Next = SCH_NULL_INDEX;

        if (period > 0)
        {
            // Tai lap lich theo deadline ban dau
            SCH_Queue_Insert(index, period);
        }
        else
        {
            // One-shot: giai phong task
            SCH_tasks_G[index].pTask = NULL;
            SCH_tasks_G[index].Delay = 0;
            SCH_tasks_G[index].Period = 0;
            SCH_tasks_G[index].TaskID = SCH_INVALID_TASK_ID;
        }

        // Mo ngat truoc khi thuc thi callback
        __set_PRIMASK(primask);

        if (taskFunction != NULL)
        {
            taskFunction();
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
    SCH_Queue_Remove((int16_t)taskID);

    SCH_tasks_G[taskID].pTask = NULL;
    SCH_tasks_G[taskID].Delay = 0;
    SCH_tasks_G[taskID].Period = 0;
    SCH_tasks_G[taskID].TaskID = SCH_INVALID_TASK_ID;
    SCH_tasks_G[taskID].Next = SCH_NULL_INDEX;
    // Khoi phuc trang thai ngat ban dau
    __set_PRIMASK(primask);

    return 1;
}

