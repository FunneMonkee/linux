#ifndef __CSS_TASK_H_
#define __CSS_TASK_H_

struct sched_css_entity{
	struct rb_node node;

	u64 deadline;
	u64 abs_deadline;

	//TODO_LUIS
	u64 runtime;
	u64 period;
	u64 remaining_runtime;
};

#endif 
