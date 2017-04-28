
#include "SCHED_H"
#include "lib.h"
#include "rtc.h"
#include "idt.c"


//initialize PIT
void pit_init(){

    idt[PIC_VECTOR_NUM].present = 1;
  /* Set frequency to 33HZ (interrupt every 30 milliseconds). */
    SET_IDT_ENTRY(idt[PIC_VECTOR_NUM], pit_handler);

    outb(PIT_MODE3, PIT_CMDREG);
    outb(DIVISOR_33HZ & 0xFF, PIT_CHANNEL0);
    outb(DIVISOR_33HZ >> 8, PIT_CHANNEL0);

	/* Output from PIT channel 0 is connected to the PIC chip, so that it
	 * generates an "IRQ 0" */
	enable_irq(PIT_IRQ);
}

// call scheduler_tick
void pit_handler(){

  cli();
  send_eoi(PIT_IRQ); // idk if this is right

  scheduler_tick();

  stl();
}


/* scheduler_tick
 *
 * update the cpu clock based the current scheduler clock
 * if the task in not idle call task_running_tick
 */
void scheduler_tick (void){

    unsigned long long now = sched_clock();
    task_t* task->cur_process = process_pcb;
    int idle_at_tick = idle_cpu(task);
    runqueue_t* rq;

    update_cpu_clock(task, rq, now);

    if(!idle_at_tick) task_running_tick(rq,task); //handle real time tasks

}

/* task_running_tick
 *
 * reschedule expired tasks
 * handle real-time tasks ( round robin )
 * handle task time-slice expiration

 *  Arguments: rq - pointer to the current runqueue_t
               p  - pointer to the current task within our runqueue
 */
static void task_running_tick( runqueue_t* rq, task_t* task){

    //reschedule expired tasks
    if (task->pr_array != rq->is_active) {
      set_task_need_resched (p);
      return;
    }

    //handle real-time tasks ( round robin )
    spin_lock (&rq_lock->lock);

      if (!--(p->time_slice) ) {  // if time_slice == 0

        p->time_slice = task_timeslice(p);   //reset time slice
        p->first_time_slice = 0;
        set_task_need_resched(p);   //task now inactive
        requeue_task(p, rq->active);  //place on back of queue
      }
      goto out_unlock;

    //handle tasks
      if(!--(p->time_slice) ) {
      dequeue_task (p, rq->active);  // take task off from front of queue
      set_task_need_resched(p);  //task now inactive
      p->time_slice = task_timeslice(p);
      p->first_time_slice = 0;


    //time-slice expiration
      if (!rw->expired_timestamp) rq->expired_timestamp = jiffies;
      if(!TASK_INTERACTIVE(p) || expired_starving (rq)) {
        enqueue_task (p, rq->expired);
        if(p->static_prio < rq->best_expired_prio) rq->best_expired_prio = p->static_prio;
      } else enqueue_task(p, rq->active);
      }
      else {
        if(TASK_INTERACTIVE (p) &&
           !((task_timeslice(p) - p->time_slice) % TIMESLICE_GRANULARITY(p)) &&
           p->time_slice >= TIMESLICE_GRANULARITY(p) 0 && (p->array == rq->active))
           {
             requeue_task(p, rq->active);
             set_task_need_resched (p);
           }
      }

    out_unlock: spin_unlock(&irq->lock);
}

// places task on back of queue, makes task inactive
void requeue_task(task_t * p, int* active){

  spin_lock(&rq->lock);

  active = 0;
  task_t* s = p->next_task;
  while( !(s->next_task) ){
    s = s->next_task;
  }
  s->next_task = p;
  p->prev_task = s;
  p->next_task = NULL;

  //is this wrong?
  p->first_time_slice = p->time_slice;

  spin_unlock(&rq->lock);
}

// sets TIF_NEED_RESCHED flag for current task to 1
void set_task_need_resched( task_t* p){
    p->TIF_NEED_RESCHED = 1;
}

// // we dont work with any other scheduling principle other than RR so do we need this?
// int rt_task( task_t* p){
//   if(p->policy == SCHED_RR) return 1;
//   return 0;
// }

// gets the current tasks first_time_slice
uint8_t task_timeslice(task_t *p){
  return p->first_time_slice;  // how does first_time_slice work with time_slice variable?
}

// call schedule before and after you need to schedule the tasks needed to be completed or are expired
asmlinkage void schedule (void){

    task_t* prev = process_pcb;
    task_t* next;
    runqueue_t* rq = RunQueue;
    prio_array_t* array;
    list_t* queue:
    int idx;

    if( in_interrupt () )
      BUG();
    release_kernel_lock (prev, smp_process_id() );
    prev->sleep_timestamp = jiffies;
    spin_lock_irp ( &rq_lock);

    switch (prev->stats) {
        case TASK_INTERRUPTIBLE:
            if (signal_pending(prev)) {
              prev->state = TASK_RUNNING:
              break;
            }
        default:
          deactivate_task (prev, rq);
        case TASK_RUNNING: ;
    }
    if (!rq->nr_running) {
      next = rq->idle;
      rq->expired_timestamp = 0;
      goto switch_tasks;  //
    }

    array = rq->active;
    if (!array->nr_active) {

      rq->active = rq->expired;
      rq->expired = array;
      array = rq->active;
      rq->expired_timestamp = 0;
    }

    idx = sched_find_first_bit (array->bitmap);
    queue = array->queue + idx;
    next = list_entry (queue->next, task_t, run_list);

    //
    switch_tasks:
      prefetch(next);
      prev->need_resched = 0;

      if (prev != next) {
          rq->nr_switches++;
          rq->curr = next;
          context_switch(prev, next);
          barrier ():
          rq = RunQueue;
      }

      swpin_unlock_irq (&rq->lock);

      reacquire_kernel_lock (current);
      return;
}

//gets the current clock time
unsigned long long sched_clock(){
  //
}

// gets process id for the current process
uint8_t smp_process_id(task_t* p){
  return p->cur_process->process_id;
}

//removes task from task list, denotes as active
dequeue_task(task_t* p, int* active){
  p->next_task->prev_task = NULL;

  p->prev_task = NULL;
  p->next_task = NULL;

  active = 1;
}

// return 1 if CPU running idle task
int idle_cpu(task_t* p){
  if( !(p->is_active) ) return 1;
  return 0;
}

//get the current runqueue from our cpu
runqueu_t cpu_rq(void){
  return RunQueue;
}

// tracks nanoseconds since last tick/context switch
//right return val?
void update_cpu_clock(task_t* p, runqueue_t* rq, unsigned long long current_sched_time) {

}

// breaks long timeslices into smaller timeslices
//mhow do you code these tasks
TASK_INTERACTIVE(){

}

// return 1 if expired array is empty
int expired_starving(runqueue_t * rq){
  if(rq->expired_array = 0) return 1;
  return 0;
}

//gets current runqueue_t
runqueue_t * this_runqueue(){
  //
}

//DO WE HAVE OUR OWN FN FOR THIS??
in_interrupt(){
  //
}


//make task not active
void deactivate_task(task_t* p){
  p->is_active = 0;
}
