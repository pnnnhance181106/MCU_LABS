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
	}
}

