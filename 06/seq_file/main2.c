#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/sched/signal.h>
#include <linux/rcupdate.h>
#include <linux/sched/task.h>

MODULE_LICENSE("Dual BSD/GPL");

#define PROCNAME "sample2"

static void *as_start(struct seq_file *m, loff_t *pos)
{
        loff_t n = *pos;
        struct task_struct *p = NULL;

        printk("%lld (%s)\n", n, __func__);

        if (n == 0) {
                seq_printf(m, "=== seq_file header ===\n");
        }

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 18))
        read_lock(&tasklist_lock);
#else
        rcu_read_lock();
#endif
        if (n == 0)
                return (&init_task);

        for_each_process(p){
                n--;
                if (n <= 0)
                        return (p);
        }
        return 0;
}

static void *as_next(struct seq_file *m, void *p, loff_t *pos)
{
        struct task_struct *tp = (struct task_struct *)p;

        printk("%lld (%s)\n", *pos, __func__);

        (*pos)++;

        tp = next_task(tp);
        if (tp == &init_task)
                return NULL;
        return (tp);
}

static void as_stop(struct seq_file *m, void *p)
{
        printk("%p (%s)\n", p, __func__);

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 18))
        read_unlock(&tasklist_lock);
#else
        rcu_read_unlock();
#endif
}

static int as_show(struct seq_file *m, void *p)
{
        struct task_struct *tp = (struct task_struct *)p;

        printk("%p (%s)\n", tp, __func__);

        seq_printf(m, "[%s] pid=%d\n", tp->comm, tp->pid);
        seq_printf(m, "     tgid=%d\n",tp->tgid);
        seq_printf(m, "     state=%ld\n", task_state(tp));
        seq_printf(m, "     mm=0x%p\n", tp->mm);
        seq_printf(m, "     utime=%llu\n", tp->utime);
        seq_printf(m, "     stime=%llu\n", tp->stime);
        seq_printf(m, "     oom_score_adj=%d\n", tp->signal->oom_score_adj);
        seq_puts(m, "\n");

        return 0;
}

static struct seq_operations sample_seq_op = {
        .start = as_start,
        .next = as_next,
        .stop = as_stop,
        .show = as_show,
};

static int sample_proc_open(struct inode *inode, struct file *file)
{
        return seq_open(file, &sample_seq_op);
}

static struct proc_ops sample_seq_ops = {
        .proc_open = sample_proc_open,
};

static int sample_init(void)
{
        struct proc_dir_entry *entry;

        entry = proc_create(PROCNAME, S_IRUGO | S_IWUGO, NULL, &sample_seq_ops);
        if (!entry) {
                printk(KERN_ERR "failed to create /proc/%s\n", PROCNAME);
                return -ENOMEM;
        }
        printk("driver loaded\n");

        return 0;
}

static void sample_exit(void)
{
        remove_proc_entry(PROCNAME, NULL);

        printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
