/* Author: Suki Sahota */
#include "globals.h"
#include "errno.h"

#include "main/interrupt.h"

#include "proc/sched.h"
#include "proc/kthread.h"

#include "util/init.h"
#include "util/debug.h"

void ktqueue_enqueue(ktqueue_t *q, kthread_t *thr);
kthread_t * ktqueue_dequeue(ktqueue_t *q);

/*
 * Updates the thread's state and enqueues it on the given
 * queue. Returns when the thread has been woken up with wakeup_on or
 * broadcast_on.
 *
 * Use the private queue manipulation functions above.
 */
void
sched_sleep_on(ktqueue_t *q)
{
  curthr->kt_state = KT_SLEEP;
  ktqueue_enqueue(q, curthr);
  sched_switch();
}

kthread_t *
sched_wakeup_on(ktqueue_t *q)
{
  kthread_t *thr = ktqueue_dequeue(q);

  if (thr == NULL) {
    return thr;
  }

  /* thr is the thread to be woken up */
  /* Middle KASSERT statements begin */
  KASSERT((thr->kt_state == KT_SLEEP) || (thr->kt_state == KT_SLEEP_CANCELLABLE)); /* thr must be in either one of these two states */
  /* Middle KASSERT statements end */  

  sched_make_runnable(thr); // kt_state is set in sched_make_runnable()
  return thr;
}

void
sched_broadcast_on(ktqueue_t *q)
{
  kthread_t *thr;

  while ((thr = ktqueue_dequeue(q)) != NULL) {
    sched_make_runnable(thr);
  }
}
