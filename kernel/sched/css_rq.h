#ifndef __CSS_RQ_H_
#define __CSS_RQ_H_
#include <linux/sched.h>
#include <linux/rbtree.h>
#include <linux/spinlock.h>

struct css_rq {
	struct rb_root tasks;
	struct task_struct *curr;
	raw_spinlock_t lock;
	unsigned nr_running;
};

void init_css_rq(struct css_rq *rq);

#endif
