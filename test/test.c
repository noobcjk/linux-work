#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define NEG_PID  (-200)

static struct task_struct *vtask;

static int vtask_fn(void *data)
{
	pr_info("[vt_demo] vtask_fn entered, pid=%d\n",
		task_pid_nr(current));
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	}
	return 0;
}

static int __init vt_demo_init(void)
{
	pid_t pid;
	struct pid *pidp;
	struct task_struct *t;

	pr_info("[vt_demo] init start\n");

	current->want_neg_pid = NEG_PID;
	pid = kernel_thread(vtask_fn, NULL,
		CLONE_FS | CLONE_FILES | SIGCHLD);
	current->want_neg_pid = 0;

	pr_info("[vt_demo] kernel_thread returned %d\n", pid);

	pidp = find_get_pid(NEG_PID);
	if (!pidp) {
		pr_err("[vt_demo] find_get_pid failed\n");
		return -ENOENT;
	}

	rcu_read_lock();
	t = pid_task(pidp, PIDTYPE_PID);
	if (t) {
		get_task_struct(t);
		wake_up_process(t);
	}
	rcu_read_unlock();
	put_pid(pidp);

	if (!t) {
		pr_err("[vt_demo] pid_task NULL\n");
		return -ENOENT;
	}

	vtask = t;
	pr_info("[vt_demo] SUCCESS! vtask->pid=%d, tgid=%d\n",
		task_pid_nr(vtask), task_tgid_nr(vtask));
	put_task_struct(vtask);
	return 0;
}

static void __exit vt_demo_exit(void)
{
	pr_info("[vt_demo] exit\n");
}

module_init(vt_demo_init);
module_exit(vt_demo_exit);
MODULE_LICENSE("GPL");
