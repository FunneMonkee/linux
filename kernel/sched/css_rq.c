#include "css_rq.h"

void init_css_rq(struct css_rq *rq){
	rq->tasks = RB_ROOT;
	raw_spin_lock_init(&rq->lock);
	rq->curr= NULL;
	rq->nr_running = 0;
}
