#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

struct sample_data {
	spinlock_t lock;
	struct file *file;
	struct list_head list;
	wait_queue_head_t wait;
	int no;
};

struct sample_data head;

void free_struct(void)
{
	struct list_head *listptr;
	struct sample_data *entry;

	list_for_each(listptr, &head.list) {
		entry = list_entry(listptr, struct sample_data, list);
		printk("Free: no=%d (list %p, next %p, prev %p)\n",
			entry->no, &entry->list, entry->list.next, entry->list.prev);
		
		kfree(entry);
	}
}

void show_struct(void)
{
	struct list_head *listptr;
	struct sample_data *entry;

	printk("==================================\n");
	printk("no=%d (list %p, next %p, prev %p)\n",
		head.no, &head.list, head.list.next, head.list.prev);

	list_for_each(listptr, &head.list) {
		entry = list_entry(listptr, struct sample_data, list);
		printk("no=%d (list %p, next %p, prev %p)",
			entry->no, &entry->list, &entry->list.next, entry->list.prev);
	}
}

void remove_struct(struct sample_data *entry)
{
	printk("Free: no=%d (list %p, next %p, prev %p)\n",
		entry->no, &entry->list, entry->list.next, entry->list.prev);

	list_del(&entry->list);
	kfree(entry);
}

struct sample_data *add_struct(int no)
{
	struct sample_data *ptr;

	ptr = kmalloc(sizeof(struct sample_data), GFP_KERNEL);
	if (ptr) {
		ptr->no = no;
		list_add_tail(&ptr->list, &head.list);
	}

	return (ptr);
}

static int devone_init(void)
{
        dev_t dev = MKDEV(devone_major, 0);
        int alloc_ret = 0;
        int major;
        int cdev_err = 0;

        alloc_ret = alloc_chrdev_region(&dev, 0, devone_devs, DRIVER_NAME);
        if (alloc_ret)
                goto error;
        devone_major = major = MAJOR(dev);
        
        cdev_init(&devone_cdev, &devone_fops);
        devone_cdev.owner = THIS_MODULE;

        cdev_err = cdev_Add(&devone_cdev, MKDEV(devone_major, 0), devone_devs);
        if (cdev_err)
                goto error;
        
        printk(KERN_ALERT "%s driver(major %d) installed \n", DRIVER_NAME, major);

        return 0;

error:
        if (cdev_err == 0)
                cdev_del(&devone_cdev);
        
        if (alloc_ret == 0)
                unregister_chrdev_region(dev, devone_devs);
        
        return -1;
}

static int sample_init(void)
{
	struct sample_data *p;

	memset(&head, 0, sizeof(head));
	INIT_LIST_HEAD(&head.list);
	show_struct();

	add_struct(1);
	show_struct();

	p = add_struct(2);
	show_struct();

	add_struct(3);
	show_struct();

	if (p) {
		remove_struct(p);
		show_struct();
	}

	free_struct();

	printk(KERN_ALERT "sample driver installed.\n");

	return 0;
}

static void sample_exit(void)
{
	printk(KERN_ALERT "sample driver removed.\n");
}

module_init(sample_init);
module_exit(sample_exit);

























