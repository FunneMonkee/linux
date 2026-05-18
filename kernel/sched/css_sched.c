#include "../sched/sched.h"
/*
 * CSS scheduling class.
 * Implements SCHED_CSS
 */
//TODO_LUIS implement the scheduler
static void enqueue_task_css(struct rq *rq, struct task_struct *p, int flags)
{
}
static bool dequeue_task_css(struct rq *rq, struct task_struct *p, int flags)
{
	return true;
}
/*
 * Preempt the current task with a newly woken task if needed:
 */
static void wakeup_preempt_css(struct rq *rq, struct task_struct *p, int flags)
{
}
static struct task_struct *pick_task_css(struct rq *rq, struct rq_flags *rf)
{
	return NULL;
}
static void put_prev_task_css(struct rq *rq, struct task_struct *p, struct task_struct *next)
{
}
static void set_next_task_css(struct rq *rq, struct task_struct *p, bool first)
{
}
static int select_task_rq_css(struct task_struct *p, int cpu, int flags)
{
	return cpu;
}
static void task_tick_css(struct rq *rq, struct task_struct *p, int queued)
{
}
static void prio_changed_css(struct rq *rq, struct task_struct *p, u64 oldprio)
{
}
static void switched_to_css(struct rq *rq, struct task_struct *p)
{
}
static void update_curr_css(struct rq *rq)
{
}
DEFINE_SCHED_CLASS(css) = {
//	.queue_mask = 8,
	.enqueue_task = enqueue_task_css,
	.dequeue_task = dequeue_task_css,
	.wakeup_preempt = wakeup_preempt_css,
	.pick_task = pick_task_css,
	.put_prev_task = put_prev_task_css,
	.set_next_task = set_next_task_css,
	.select_task_rq = select_task_rq_css,
	.set_cpus_allowed = set_cpus_allowed_common,
	.task_tick = task_tick_css,
	.prio_changed = prio_changed_css,
	.switched_to = switched_to_css,
	.update_curr = update_curr_css,
};
