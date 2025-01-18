#include "work.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#define WORK_STACK_SIZE 512
#define WORK_PRIORITY 5

K_THREAD_STACK_DEFINE(work_stack_area, WORK_STACK_SIZE);

struct k_work_q work_q;


namespace work {
bool submit(Work_item work_item, int delay) {
  return true;
}

void initialize() {
  k_work_queue_init(&work_q);

  k_work_queue_start(
      &work_q, work_stack_area, K_THREAD_STACK_SIZEOF(work_stack_area), WORK_PRIORITY, NULL);
}

} // namespace work
