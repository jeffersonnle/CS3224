#include <linux/init.h>
#include <linux/module.h>

#include <linux/param.h>

MODULE_LICENSE("Dual BSD/GPL");

static unsigned long start_jiffies; 
static ktime_t start_time;

static int hello_init(void) {
    start_jiffies = jiffies; 
	start_time = ktime_get_boottime();
    unsigned int _ttms = 1000 / HZ;
    printk(KERN_INFO "Hello, world %u \n", ttms);
    return 0;
}

static void hello_exit(void)
{
    unsigned long end_jiffies, jiffies_ms;
    ktime_t end_time, delta_time;
    s64 delta_time_ms;

    end_jiffies = jiffies;
    end_time = ktime_get_boottime();
    jiffies_ms = ((end_jiffies - start_jiffies)* 1000) / HZ;
    delta_time_ms = ktime_to_ms((end_time - start_time));

    printk(KERN_INFO "Goodbye, cruel world\nJiffies: %lu\nTime: %lld\n", jiffies_ms, delta_time_ms);
}

module_init(hello_init);
module_exit(hello_exit);

