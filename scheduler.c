#include "scheduler.h"

#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include <assert.h>
#include <curses.h>
#include <ucontext.h>

// This is an upper limit on the number of tasks we can create.
#define MAX_TASKS 12

// This is the size of each task's stack memory
#define STACK_SIZE 65536


enum Status { EXITED, SLEEPING, WAITING, READY };


// This struct will hold the all the necessary information for each task

typedef struct task_info {

  // This field stores all the state required to switch back to this task
  ucontext_t context;

  // This field stores another context. This one is only used when the task is exiting.
  ucontext_t exit_context;

  int status;
  size_t wake_time;
  int waiting_for;

} task_info_t;


int current_task = 0;          //< The handle of the currently-executing task

int num_tasks = 1;             //< The number of tasks created so far

task_info_t tasks[MAX_TASKS];  //< Information for every task


/**
 * Initialize the scheduler. Programs should call this before calling any other
 * functions in this file.
 */
void scheduler_init() {

  // TODO: Initialize the state of the scheduler

}


/**
 * Run task
 */
void task_run(task_t handle) {
  int old_task = current_task;
  current_task = handle;
  swapcontext(&tasks[old_task].context, &tasks[handle].context);
}


/*

 * Loops through all tasks and runs the next task that is ready

 */

void run_next_ready_task() {
  int new_task = current_task;
  while (true) {

    // get to next index
    new_task = (new_task + 1) % num_tasks;

    // check if next task is waiting, if yes, check if the task it's waiting for is done and set it to ready

    if (tasks[new_task].status == WAITING) {
      int waiting = tasks[new_task].waiting_for;
      if (tasks[waiting].status == EXITED) {
        tasks[new_task].status = READY;
      }
    }

    // check if next task is sleeping, if yes, check if it is supposed to wake up and set it to ready
    if (tasks[new_task].status == SLEEPING && tasks[new_task].wake_time < time_ms() ){
      tasks[new_task].status = READY;
    }

    // break if next task is ready
    if (tasks[new_task].status == READY) {
      break;
    }

  }
  task_run(new_task);
}


/**

 * This function will execute when a task's function returns. This allows you
 * to update scheduler states and start another task. This function is run
 * because of how the contexts are set up in the task_create function.
 */

void task_exit() {
  tasks[current_task].status = EXITED;
  run_next_ready_task();
}


/**
 * Create a new task and add it to the scheduler.
 *
 * \param handle  The handle for this task will be written to this location.
 * \param fn      The new task will run this function.
 */

void task_create(task_t* handle, task_fn_t fn) {

  // Claim an index for the new task
  int index = num_tasks;
  num_tasks++;

  // Set the task handle to this index, since task_t is just an int
  *handle = index;

  // We're going to make two contexts: one to run the task, and one that runs at the end of the task
  // so we can clean up. Start with the second
  // First, duplicate the current context as a starting point
  getcontext(&tasks[index].exit_context);

  // Set up a stack for the exit context
  tasks[index].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].exit_context.uc_stack.ss_size = STACK_SIZE;

  // Set up a context to run when the task function returns. This should call task_exit.
  makecontext(&tasks[index].exit_context, task_exit, 0);

  // Now we start with the task's actual running context
  getcontext(&tasks[index].context);

  // Allocate a stack for the new task and add it to the context
  tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].context.uc_stack.ss_size = STACK_SIZE;

  // Now set the uc_link field, which sets things up so our task will go to the exit context when
  // the task function finishes
  tasks[index].context.uc_link = &tasks[index].exit_context;

  // And finally, set up the context to execute the task function
  makecontext(&tasks[index].context, fn, 0);

  // Initialize other fields
  tasks[index].status = READY;
  tasks[index].wake_time = 0;
  tasks[index].waiting_for = -1;
}


/**
 * Wait for a task to finish. If the task has not yet finished, the scheduler should
 * suspend this task and wake it up later when the task specified by handle has exited.
 *
 * \param handle  This is the handle produced by task_create
 */

void task_wait(task_t handle) {

  // set status of current task to waiting and waiting for to be the handle
  tasks[current_task].status = WAITING;
  tasks[current_task].waiting_for = handle;

  // Find the next task that needs to be run, and run it
  run_next_ready_task();
}


/**

 * The currently-executing task should sleep for a specified time. If that time is larger
 * than zero, the scheduler should suspend this task and run a different task until at least
 * ms milliseconds have elapsed.
 *
 * \param ms  The number of milliseconds the task should sleep.
 */

void task_sleep(size_t ms) {

  // set status to sleeping
  tasks[current_task].status = SLEEPING;

  // set wake up time
  tasks[current_task].wake_time = time_ms() + ms;

  // run next task while current task sleeps
  run_next_ready_task();

}


/**
 * Read a character from user input. If no input is available, the task should
 * block until input becomes available. The scheduler should run a different
 * task while this task is blocked.
 *
 * \returns The read character code
 */

int task_readchar() {

  int input = getch();
  while (input == ERR) {
    run_next_ready_task();
    input = getch();
  }
  return input;
}
