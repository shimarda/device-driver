#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("Dual BSD/GPL");

#define PROCNAME "driver/sample"

#define DUMMY_BUFSIZ 4096
static struct proc_dir_entry *entry;

static int sample_show_proc(struct seq_file *m, void *v)
{
        seq_printf(m, "Hello procfs\n");
        return 0;
}

static int sample_init(void)
{

       entry = proc_create_single(PROCNAME, S_IRUGO, NULL, sample_show_proc);

        if (!entry) {
                printk(KERN_WARNING "sample: unable to create /proc entry\n");
                return -ENOMEM;
        }
        printk("driver loaded\n");

        return 0;
}


static void sample_exit(void)
{
        if (entry) {
                proc_remove(entry);
                printk(KERN_ALERT "driver unloaded.\n");
        }
}

module_init(sample_init);
module_exit(sample_exit);
