#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/types.h>

MODULE_LICENSE("Dual BSD/GPL");

#define PROCNAME "sample"

static char *data_message[] = {
        "Fedora",
        "Red Hat Enterprise Server",
        "Debian",
        "Vine",
        "Ubuntu",
        0
};

static void *as_start(struct seq_file *m, loff_t *pos)
{
        loff_t n = *pos;
        char **p;
        int i;

        printk(KERN_INFO "%lld (%s)\n", n, __func__);

        if (n == 0) {
                seq_printf(m, "=== seq_file header ===\n");
        }

        p = data_message;
        for (i = 0; p[i]; i++) {
                n--;
                if (n < 0)
                        return (void *)((unsigned long)i + 1);
        }
        return 0;
}

static void *as_next(struct seq_file *m, void *p, loff_t *pos)
{
        unsigned long n = (unsigned long)p;
        char **ptr;

        printk(KERN_INFO "%lu (%s)\n", n, __func__);

        (*pos)++;

        ptr = data_message;
        if (ptr[n]) {
                return (void *)(n + 1);
        }

        return 0;
}


static void as_stop(struct seq_file *m, void *p)
{
       printk(KERN_INFO "%lu (%s)\n", (unsigned long)p, __func__);
}

static int as_show(struct seq_file *m, void *p)
{
       unsigned long n = (unsigned long)p - 1;
        
       printk(KERN_INFO "%lu (%s)\n", (unsigned long)p, __func__);
    
       seq_printf(m, "[%lu] %s\n", n, data_message[n]);

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
static struct proc_ops sample_operations = {
        .proc_open = sample_proc_open,
        .proc_read = seq_read,
        .proc_lseek = seq_lseek,
        .proc_release = seq_release,
};

static int sample_init(void)
{
        struct proc_dir_entry *entry;

       entry = proc_create(PROCNAME, S_IRUGO | S_IWUGO, NULL, &sample_operations);
        if (!entry) {
                printk(KERN_WARNING "Failed to create proc entry %s\n", PROCNAME);
                return -ENOMEM;
        }

        printk(KERN_INFO "driver loaded\n");

        return 0;
}

static void sample_exit(void)
{
        remove_proc_entry(PROCNAME, NULL);
        printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
