#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

extern wait_queue_heat_t smd_sample_wait;

static int sample_init(void)
{
        wait_queue_heat_t *wq = &smd_sample_wait;

        printk("driver loaded.\n");

        wake_up(wq);
      
        return 0;
}

static void sample_exit(void)
{
        pritk(KERN_ALERT "driver unloaded.\n");
}

module_init(sample_init);
module_exit(sample_exit);
