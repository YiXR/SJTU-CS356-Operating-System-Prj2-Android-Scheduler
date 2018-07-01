/*About wrr.c
*This program mainly to realize the wrr scheduling.
*In wrr.c, we realize the wrr method by finding out whether the task is in foreground and back ground,
*and giving different time slice for them.
*I add some printk in enqueu_task_wrr and task_tick_wrr to show the information about the task.
*/


#include "sched.h"
#include <linux/slab.h>
#include <linux/kernel.h>
//#define DEBUG
#ifdef CONFIG_CGROUP_SCHED
static char group_path[4096];

/*We can find the code in debug.c.*/
static char *task_group_path(struct task_group *tg)
{
	if (autogroup_path(tg, group_path, 4096))
		return group_path;
	/*
	 * May be NULL if the underlying cgroup isn't fully-created yet
	 */
	if (!tg->css.cgroup) {
		group_path[0] = '\0';
		return group_path;
	}
	cgroup_path(tg->css.cgroup, group_path, 4096);
	return group_path;
}
#endif

/*wrr init*/
void init_wrr_rq(struct wrr_rq *wrr_rq, struct rq *rq)
{
	printk(KERN_ERR "init_wrr_rq\n");
    wrr_rq->wrr_nr_running = 0;
    INIT_LIST_HEAD(&wrr_rq->list);
}

/*When a task is runable, it will use this function. This function will be given task and rq, and it will put in task into rq again.*/
static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	char * gp;
#ifdef DEBUG
	printk(KERN_INFO "enqueue task weighted rr BEGIN %d\n", rq->wrr.wrr_nr_running);
#endif
	rcu_read_lock();
	gp = task_group_path(task_group(p));
	if(strcmp(gp,"/bg_non_interactive") == 0)
		printk(KERN_INFO "I am in %s; pid:%d; proc:%s",gp,p->pid,p->comm); //If switch to background, then printk
	rcu_read_unlock();
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct list_head *list = &rq->wrr.list;

    if (flags & ENQUEUE_WAKEUP){
    	wrr_se->timeout = 0;
        list_add(&wrr_se->run_list, list); 
    }
    else
        list_add_tail(&wrr_se->run_list, list);

    rq->wrr.wrr_nr_running++;
    inc_nr_running(rq);
#ifdef DEBUG
	printk(KERN_INFO "enqueue task weighted rr END %d\n", rq->wrr.wrr_nr_running);
	printk(KERN_INFO "enqueue task weighted rr time_slice %d\n", p->wrr.time_slice);
#endif

}

/*It's the function that update the current task's runtime statistics.*/
static void update_curr_wrr(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	u64 delta_exec;

	if (curr->sched_class != &wrr_sched_class)
		return;

	delta_exec = rq->clock_task - curr->se.exec_start;
	if (unlikely((s64)delta_exec < 0))
		delta_exec = 0;

	schedstat_set(curr->se.statistics.exec_max,
		      max(curr->se.statistics.exec_max, delta_exec));

	 curr->se.sum_exec_runtime += delta_exec;
	 curr->se.exec_start = rq->clock_task;
	 cpuacct_charge(curr, delta_exec);
}

/*This function is used to push the task out of the rq.*/
static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{

#ifdef DEBUG
	printk("dequeue_task_wrr BEGIN\n");
	printk("dequeue_task_wrr call update_curr_wrr");
#endif
	struct sched_wrr_entity *wrr_se = &(p->wrr);
	update_curr_wrr(rq);
	list_del(&wrr_se->run_list);
	rq->wrr.wrr_nr_running--;
#ifdef DEBUG
	printk("dequeue_task_weight_rr END\n");
#endif

}


static void requeue_task_wrr(struct rq *rq, struct task_struct *p, int head)
{
#ifdef DEBUG
	printk("requeue_task_wrr begin\n");
#endif
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct list_head *list = &rq->wrr.list;
	if (head)
        list_move(&wrr_se->run_list, list);
    else
        list_move_tail(&wrr_se->run_list, list);
#ifdef DEBUG
    printk("requeue_task_wrr end\n");
#endif
}

/*This funcion will be execute when the task has been yielded.*/
static void yield_task_wrr(struct rq *rq)
{

#ifdef DEBUG
	printk("yield_task_wrr BEGIN\n");
#endif
	requeue_task_wrr(rq, rq->curr, 0);
#ifdef DEBUG
	printk("yield_task_wrr END\n");
#endif
}

/*This function is used to choose the next task. When the uppermost layer is allocated a period of time for the scheduler to run, the function is called and pick the task which will be executed.*/
static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
	struct sched_wrr_entity *first;
	struct task_struct *p;
	struct wrr_rq *wrr_rq;
	
	wrr_rq = &rq->wrr;
	
	if (!wrr_rq->wrr_nr_running)
		return NULL;
	
	if(list_empty(&rq->wrr.list)){
		return NULL;
	}

#ifdef DEBUG
	printk("pick_next_task_wrr BEGIN\n");
#endif
	first = list_first_entry(&rq->wrr.list, struct sched_wrr_entity, run_list);
	p = container_of(first, struct task_struct, wrr);
	if (p == NULL)
		return NULL;

	p->se.exec_start = rq->clock_task;
	
#ifdef DEBUG
	printk("pick_next_task_wrr END\n");
#endif

	return p;
}

/*This function is used to put the prev finished task to the right place in the rq.*/
static void put_prev_task_wrr(struct rq *rq, struct task_struct *p)
{	
#ifdef DEBUG	
	printk("put_prev_task_wrr begin\n");
#endif
	update_curr_wrr(rq);
	p->se.exec_start = 0;
#ifdef DEBUG	
	printk("put_prev_task_wrr begin\n");
#endif
}

/*This function is used to set the running task's exec_start to be rq->clock.*/
static void set_curr_task_wrr(struct rq *rq)
{
#ifdef DEBUG
	printk("set_curr_task_wrr begin\n");
#endif
	struct task_struct *p = rq->curr;
	p->se.exec_start = rq->clock_task;
#ifdef DEBUG
	printk("set_curr_task_wrr end\n");
#endif
}

static void watchdog(struct rq *rq, struct task_struct *p)
{
    unsigned long soft, hard;
    soft = task_rlimit(p, RLIMIT_RTTIME);
    hard = task_rlimit_max(p, RLIMIT_RTTIME); 

    if (soft != RLIM_INFINITY)
    {
        unsigned long next;
        p->wrr.timeout++;
        next = DIV_ROUND_UP(min(soft, hard), USEC_PER_SEC / HZ);
        if (p->wrr.timeout > next)
            p->cputime_expires.sched_exp = p->se.sum_exec_runtime;
    }
}

/*This function is used to decide which task need to be executed and which task need to set time slice and reschedule.*/
static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
	char *gp;
	update_curr_wrr(rq);
	watchdog(rq, p);
	if (p->wrr.time_slice && --p->wrr.time_slice)
		return;

#ifdef DEBUG
	printk("task_tick_weight_rr BEGIN\n");
#endif

	rcu_read_lock();
	gp = task_group_path(task_group(p));
	if(strcmp(gp,"/") == 0){							//Time slice for foreground
		p->wrr.time_slice = WRR_TIMESLICE_FORE;
		printk(KERN_INFO "I am in foreground; pid:%d; time_slice:%d\n",p->pid,p->wrr.time_slice);
		
	}else if(strcmp(gp,"/bg_non_interactive") == 0){	//Time slice for background
		p->wrr.time_slice = WRR_TIMESLICE_BACK;
	}else{
		p->wrr.time_slice = WRR_TIMESLICE_BACK;
	}
	rcu_read_unlock();

	if(rq->wrr.list.prev != rq->wrr.list.next){
		requeue_task_wrr(rq,p,0);
		set_tsk_need_resched(p);
	}

#ifdef DEBUG
	printk("task_tick_weight_rr END\n");
#endif
}

static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
	if (p->on_rq && rq->curr != p)
		if (rq == task_rq(p))
			resched_task(rq->curr);
}

/*This function is to init the time slice, which is called in core.c.*/
static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *p)
{
	char * gp;
	
	rcu_read_lock();
	gp = task_group_path(task_group(p));
	if(strcmp(gp,"/") == 0){							//Time slcie for foreground
		return WRR_TIMESLICE_FORE;
		
	}else if(strcmp(gp,"/bg_non_interactive") == 0){	//Time slcie for background
		return WRR_TIMESLICE_BACK;
	}else{
		return WRR_TIMESLICE_BACK;
	}
	rcu_read_unlock();
}


const struct sched_class wrr_sched_class = {

	.next = &fair_sched_class,               /*Required*/
	.enqueue_task 	= enqueue_task_wrr,		/*Required*/
	.dequeue_task 	= dequeue_task_wrr, 	/*Required*/
	.yield_task 	= yield_task_wrr, 		/*Required*/

	.pick_next_task 	= pick_next_task_wrr, 		/*Required*/
	.put_prev_task 		= put_prev_task_wrr, 		/*Required*/

#ifdef CONFIG_SMP
	//.set_cpus_allowed 		= set_cpus_allowed_wrr, 
#endif

	.set_curr_task 		= set_curr_task_wrr, 		/*Required*/
	.task_tick 		= task_tick_wrr, 				/*Required*/

	.get_rr_interval 	= get_rr_interval_wrr,

	.switched_to 		= switched_to_wrr, 			/*Required*/
};