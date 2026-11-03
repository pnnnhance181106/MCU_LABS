
#include "scheduler.h"
#include "main.h"
#include <stddef.h>
#include <stdint.h>

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint32_t current_task_count = 0;
static uint32_t next_task_id = 1;

/**
 * @brief khoi tao scheduler
 */
void SCH_Init(void) {
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = NULL;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = SCH_INVALID_TASK_ID;
    }
    current_task_count = 0;
    next_task_id = 1;
}

/**
 * @brief thuc thi add task moi va add task dinh ki
 *
 */
static uint32_t SCH_Add_Task_With_ID(
    void (*pFunction)(void),
    uint32_t DELAY,
    uint32_t PERIOD,
    uint32_t taskID
) {
    if (current_task_count >= SCH_MAX_TASKS || pFunction == NULL) {
        return SCH_INVALID_TASK_ID;
    }

    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    uint32_t insert_idx = 0;
    uint32_t new_delay = DELAY;

    // duyet tim vi tri chen trong delta queue
    while (insert_idx < current_task_count &&
           new_delay >= SCH_tasks_G[insert_idx].Delay) {
        new_delay -= SCH_tasks_G[insert_idx].Delay;
        insert_idx++;
    }


    // giảm Delta Delay của task ngay sau nó
    if (insert_idx < current_task_count) {
        SCH_tasks_G[insert_idx].Delay -= new_delay;
    }

    // Dịch các phần tử phía sau sang phải 1 vị trí
    for (int32_t i = (int32_t)current_task_count;
         i > (int32_t)insert_idx; i--) {
        SCH_tasks_G[i] = SCH_tasks_G[i - 1];
    }

    // Lưu thông tin task mới vào vị trí chèn
    SCH_tasks_G[insert_idx].pTask = pFunction;
    SCH_tasks_G[insert_idx].Delay = new_delay;
    SCH_tasks_G[insert_idx].Period = PERIOD;
    SCH_tasks_G[insert_idx].RunMe =
        (insert_idx == 0 && new_delay == 0) ? 1 : 0;
    SCH_tasks_G[insert_idx].TaskID = taskID;

    current_task_count++;

    __set_PRIMASK(primask);
    return taskID;
}

/**
 * Dang ky task moi vao
 */
uint32_t SCH_Add_Task(
    void (*pFunction)(void),
    uint32_t DELAY,
    uint32_t PERIOD
) {
    uint32_t assigned_id = next_task_id++;

    if (next_task_id == SCH_INVALID_TASK_ID) {
        next_task_id = 1;
    }

    return SCH_Add_Task_With_ID(
        pFunction, DELAY, PERIOD, assigned_id
    );
}

/**
 * @brief Cập nhật thời gian trong ngắt Timer (ISR)
 */
void SCH_Update(void) {
    if (current_task_count > 0 &&
        SCH_tasks_G[0].pTask != NULL) {

        if (SCH_tasks_G[0].Delay > 0) {
            SCH_tasks_G[0].Delay--;
        }

        // Khi task ở đầu hàng đợi hết thời gian chờ
        if (SCH_tasks_G[0].Delay == 0) {
            SCH_tasks_G[0].RunMe = 1;
        }
    }
}

/**
 * @brief Xóa tác vụ khỏi hàng đợi theo ID
 */
uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (current_task_count == 0 ||
        taskID == SCH_INVALID_TASK_ID) {
        return 0;
    }

    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    int32_t target_idx = -1;

    for (uint32_t i = 0; i < current_task_count; i++) {
        if (SCH_tasks_G[i].TaskID == taskID) {
            target_idx = (int32_t)i;
            break;
        }
    }

    if (target_idx == -1) {
        __set_PRIMASK(primask);
        return 0;
    }

    // Nếu không phải task cuối, cộng dồn Delta Delay
    // của nó cho task kế tiếp
    if (target_idx < (int32_t)(current_task_count - 1)) {
        SCH_tasks_G[target_idx + 1].Delay +=
            SCH_tasks_G[target_idx].Delay;
    }


    for (uint32_t i = (uint32_t)target_idx;
         i < current_task_count - 1; i++) {
        SCH_tasks_G[i] = SCH_tasks_G[i + 1];
    }

    // giai phong phan tu cuoi cung
    uint32_t last_idx = current_task_count - 1;

    SCH_tasks_G[last_idx].pTask = NULL;
    SCH_tasks_G[last_idx].Delay = 0;
    SCH_tasks_G[last_idx].Period = 0;
    SCH_tasks_G[last_idx].RunMe = 0;
    SCH_tasks_G[last_idx].TaskID = SCH_INVALID_TASK_ID;

    current_task_count--;

   // task moi dua len hang doi co delay = 0, gan RunMe
    if (current_task_count > 0 &&
        SCH_tasks_G[0].Delay == 0) {
        SCH_tasks_G[0].RunMe = 1;
    }

    __set_PRIMASK(primask);
    return 1;
}

/**
 * @brief dieu phoi va thuc thi ca task vu
 */
void SCH_Dispatch_Tasks(void) {
    while (current_task_count > 0 &&
           SCH_tasks_G[0].RunMe > 0) {

        uint32_t primask = __get_PRIMASK();
        __disable_irq();

        if (current_task_count == 0 ||
            SCH_tasks_G[0].RunMe == 0) {
            __set_PRIMASK(primask);
            break;
        }

        void (*taskFunction)(void) = SCH_tasks_G[0].pTask;
        uint32_t period = SCH_tasks_G[0].Period;
        uint32_t taskID = SCH_tasks_G[0].TaskID;

        // Xóa task vừa tới hạn khỏi đầu hàng đợi
        SCH_Delete_Task(taskID);

     // tac vu tuan hoan, luu thong tin de add lai
        if (period > 0 && taskFunction != NULL) {
            SCH_Add_Task_With_ID( taskFunction, period, period, taskID );
        }

        __set_PRIMASK(primask);

        // thuc thi callback
        if (taskFunction != NULL) {
            (*taskFunction)();
        }
    }
}
