#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/pid.h>
#include <linux/kthread.h>

#define CLONE_NEGPID 0x00001000
#define NEGPID_IOCTL_SET _IOW('n', 1, long)

extern long kernel_negpid_thread(int (*fn)(void *), void *arg,
				 unsigned long flags, long neg_pid);

static struct task_struct *vtask;

static int vtask_fn(void *data)
{
	pr_info("[test_negpid] vtask pid=%ld\n",
		(long)task_pid_nr(current));
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	}
	return 0;
}

static long negpid_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	long neg_pid;
	long ret;

	if (cmd != NEGPID_IOCTL_SET)
		return -EINVAL;

	if (copy_from_user(&neg_pid, (void __user *)arg, sizeof(neg_pid)))
		return -EFAULT;

	if (neg_pid >= 0)
		return -EINVAL;

	pr_info("[test_negpid] ioctl: neg_pid=%ld\n", neg_pid);

	ret = kernel_negpid_thread(vtask_fn, NULL,
				   CLONE_FS | CLONE_FILES,
				   neg_pid);
	pr_info("[test_negpid] kernel_negpid_thread ret=%ld\n", ret);
	return ret;
}

static const struct file_operations negpid_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = negpid_ioctl,
};

static struct miscdevice negpid_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "negpid",
	.fops = &negpid_fops,
};

static int __init test_negpid_init(void)
{
	return misc_register(&negpid_misc);
}

static void __exit test_negpid_exit(void)
{
	misc_deregister(&negpid_misc);
}

module_init(test_negpid_init);
module_exit(test_negpid_exit);
MODULE_LICENSE("GPL");
