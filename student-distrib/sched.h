
#ifndef _SCHED_H
#define _SCHED_H

//these are functions
#define TASK_RUNNING
#define TASK_INTERRUPTIBLE
#define TASK_UNINTERRUPTIBLE
#define TASK_STOPPED
#define TASK_ZOMBIE

// #define SCHED_NORMAL
// #define SCHED_FIFO  //real time
#define SCHED_RR  //real time

#include "sched.h"
#include "syscall.h"

/* PIT Chip's Command Register Port */
#define PIT_CMDREG        0x43

/* PIT Channel 0's Data Register Port */
#define PIT_CHANNEL0      0x40

/* Divisors for PIT Frequency setting
 * DIVISOR_???HZ = 1193180 / HZ;
 */
#define DIVISOR_100HZ	11932
#define DIVISOR_33HZ	36157
#define DIVISOR_20HZ	59659

/* Pit Mode 3 */
#define PIT_MODE3		0x36

/* IRQ Constant. */
#define PIT_IRQ			0

extern void pit_handler();

/* Initializes the PIT for usage. */
void pit_init(void);

static runqueue_t RunQueue;

struct runqueue_struct{

  spinlock_t rq = SPIN_LOCK_UNLOCKED; // is this right/
  int nr;  // number of runnables
  int nr_active; // number of runnables active
  int nr_switches; //number of context switches
  uint8_t timestamp;  // for interactive jobs //whiat is a timestamp
  uint8_t expired_timestamp;
  int is_active;  // 1 active, 0 idle

  task_t current_task;
  task_t idle_task;


  prio_array_t* active_array = prio_a_1;  // how does double buffering help implement epochs
  prio_array_t* expired_array = prio_a_2;
  prio_array_t prio_a_1;
  prio_array_t prio_a_2;
  SMP_load_balance // what is this?

}runqueue_t;



struct task_struct{

  // doubly linked list node
  task_t* prev_task;
  task_t* next_task;
  uint8_t state_field; // use above defined TASK_ constants
  unsigned long TIF_NEED_RESCHED;  //flag -determines if expired or not
  int policy; // RR
  int is_active // 1 active, 0  idle
  int time_slice = 30ms; // is this right for ms?
  int first_time_slice = 30ms; // is this the right way to write ms?
  prio_array_t* pr_array  // current prio_array in which task is scheduled
  unsigned long long sched_time;
  pcb* cur_process;
  sleep_avg
  run_list

}task_t;


struct prio_array{

  int number_active; // number of active task lists
  uint32_t non_empty_bitmap;   //size 18 Hex values  (140 bits indicating non empty task lists)
  task_t* task_list_array[140]; // 140 task lists

}prio_array_t;



void scheduler_tick(void);
sched_clock();
smp_processor_id();
idle_cpu(cpu);
cpu_rq(cpu);
update_cpu_clock(task_t* p, runqueue_t* rp, unsigned long long current_sched_time);

static void task_running_tick(struct runqueue_t* rq, task_t p);
set_task_need_resched( task_t *) // take care of tasks which have already expired
rt_task(task_t * p)  // returns whether its a real time task or not
task_timeslice(task_t *p)  // gets the currents tasks timeslice
TASK_INTERACTIVE ( task_t * p)  // breaks long timeslices into smaller timeslices
expired_starving ( runqueue_t* rq) //
TIMESLICE_GRANULARITY (task_t* p) //


asmlinkage void schedule (void);
  this_runqueue()  //gets current runqueue
  in_interrupt()  //
  BUG()  // our error call function
  deactivate_task() // make task not active
  signal_pending()
  sched_find_first_bit(uint32_t bitmap) // finds first nonnegative task list from priority queue
  prefetch(next_task)  // get next task and stores into argument
  barrier() //
  this_runqueue()



#endif
