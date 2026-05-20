#include "../sched/sched.h"
#include <uapi/linux/sched/types.h>
/*
 * CSS scheduling class.
 * Implements SCHED_CSS
 */
//TODO_LUIS EDF done, next CBS 

void __setparam_css(struct task_struct *p,
                           const struct sched_attr *attr)
{
    struct sched_css_entity *css;

    css = &p->css;

    /*
     * Static parameters only

    css->runtime =
        attr->sched_css_runtime;
    css->period =
        attr->sched_css_period;
     */
    css->deadline =
        attr->sched_css_deadline;
    css->runtime = 0;
    css->period = 0;

    printk(KERN_EMERG
           "[EDF] pid=%d runtime=%llu deadline=%llu period=%llu\n",
           p->pid,
           css->runtime,
           css->deadline,
           css->period);
}

static void insert_css_task_rb(struct css_rq *css_rq,
                               struct task_struct *p)
{
	struct rb_node **link = &css_rq->tasks.rb_node;
	struct rb_node *parent = NULL;

    	while (*link) {

        	struct task_struct *entry;

        	parent = *link;

        	entry = rb_entry(parent,
                	struct task_struct,
                        css.node);

        	if (p->css.deadline < entry->css.deadline)
           		link = &(*link)->rb_left;
	        else
	            	link = &(*link)->rb_right;
    	}

    	rb_link_node(&p->css.node, parent, link);
	rb_insert_color(&p->css.node,
                    &css_rq->tasks);
}

static struct task_struct *pick_earliest_task(struct css_rq *css_rq)
{
    	struct rb_node *left;

    	left = rb_first(&css_rq->tasks);

	if (!left)
		return NULL;

    	return rb_entry(left,
        	struct task_struct,
                css.node);
}

static void enqueue_task_css(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_EMERG
       		"[EDF] enqueue called\n");

	raw_spin_lock(&rq->css.lock);

    	insert_css_task_rb(&rq->css, p);

    	rq->css.curr = pick_earliest_task(&rq->css);

    	rq->css.nr_running++;

    	add_nr_running(rq, 1);

	curr = rq->curr;

    if (curr && p->css.deadline < curr->css.deadline) {
        resched_curr(rq);
    }

    	raw_spin_unlock(&rq->css.lock);

	printk(KERN_EMERG
		"[EDF] enqueue pid=%d deadline=%llu\n",
		p->pid,
		p->css.deadline);
}

static bool dequeue_task_css(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_EMERG
       		"[EDF] dequeue called\n");

	raw_spin_lock(&rq->css.lock);

    	rb_erase(&p->css.node,
        	&rq->css.tasks);

    	rq->css.curr = pick_earliest_task(&rq->css);

    	rq->css.nr_running--;

    	sub_nr_running(rq, 1);

    	raw_spin_unlock(&rq->css.lock);

	printk(KERN_EMERG
		"[EDF] dequeue pid=%d\n",
		p->pid);


    	return true;
}

static void task_woken_css(struct rq *rq,
                           struct task_struct *p)
{
    struct task_struct *curr = rq->curr;

    printk(KERN_EMERG
           "[EDF] task_woken curr=%d new=%d\n",
           curr ? curr->pid : -1,
           p->pid);

    if (!curr)
        return;

    printk(KERN_EMERG
    	"[EDF] task_woken deadline comparison curr=%llu new=%llu\n",
       	curr->css.deadline,
        p->css.deadline);

    if (p->css.deadline <
        curr->css.deadline) {

        printk(KERN_EMERG
               "[EDF] task_woken PREEMPT curr=%d new=%d\n",
               curr->pid,
               p->pid);

        resched_curr(rq);
    }
}

/*
 * Preempt the current task with a newly woken task if needed:
 */
static void wakeup_preempt_css(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_EMERG
       		"[EDF] wakeup called\n");

	struct task_struct *curr = rq->curr;

    	if (!curr)
        	return;

    	if (p->css.deadline < curr->css.deadline) {

		printk(KERN_EMERG
		       "[EDF] PREEMPT curr=%d new=%d\n",
		       curr->pid,
		       p->pid);

		resched_curr(rq);
	}
}
/*
static struct task_struct *pick_task_css(struct rq *rq, struct rq_flags *rf)
{
	struct task_struct *p;

    	raw_spin_lock(&rq->css.lock);

    	p = rq->css.curr;

    	raw_spin_unlock(&rq->css.lock);

	printk(KERN_EMERG
       		"[EDF] PICK pid=%d deadline=%llu\n",
       		p->pid,
       		p->css.deadline);

    	return p;
}*/
static struct task_struct *pick_task_css(struct rq *rq, struct rq_flags *rf)
{
    struct rb_node *left;
    struct task_struct *p;

    printk(KERN_EMERG
           "[EDF] pick_next_task_css called\n");

    left = rb_first(&rq->css.tasks);

    if (!left)
        return NULL;

    p = rb_entry(left,
                 struct task_struct,
                 css.node);

    printk(KERN_EMERG
           "[EDF] PICK pid=%d deadline=%llu\n",
           p->pid,
           p->css.deadline);

    return p;
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
    printk(KERN_EMERG
           "[EDF] switched_to pid=%d\n",
           p->pid);

    if(rq->curr)
    {
	 printk(KERN_EMERG
         	"[EDF] switched_to PREEMPT curr=%llu new=%llu\n",
               	p->css.deadline,
               	rq->curr->css.deadline);


    }

    if (rq->curr &&
        p->css.deadline <
        rq->curr->css.deadline) {

        printk(KERN_EMERG
               "[EDF] switched_to PREEMPT curr=%d new=%d\n",
               rq->curr->pid,
               p->pid);

        resched_curr(rq);
    }
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
	.task_woken = task_woken_css,
};
