#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/uidgid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jefferson Le");
MODULE_DESCRIPTION("Character Device Assignment 8");
MODULE_VERSION("1.0");


static int saved_uid;

static int device_open(struct inode *inode, struct file *file){
    saved_uid = from_kuid_munged(current_user_ns(), current_uid());
    pr_info("Opened. User ID is %d\n", saved_uid);
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buf, size_t count, loff_t *ppos){
    int len;
    char user_id_buffer[32];
    len = snprintf(user_id_buffer, sizeof(user_id_buffer), "%u", saved_uid);

    if(*ppos >= len){
        return 0;
    }

    if(count > len - *ppos){
        count = len - *ppos;
    }

    if(copy_to_user(buf, user_id_buffer + *ppos, count)){
        return -EFAULT;
    }
    *ppos += count;   
    return count;
}

static struct file_operations fops = {
.owner = THIS_MODULE,
.open = device_open,
.read = device_read,
};

static struct miscdevice my_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "lab8",
    .fops = &fops,
    .mode = S_IRUGO,
};

static int __init my_device_init(void){
    int ret;
    ret = misc_register(&my_device);
    if(ret){
        pr_err("Couldn't register device\n");
        return ret;
    }
    pr_info("Device has been registered\n");
    return 0;
}

static void __exit my_device_exit(void){
    misc_deregister(&my_device);
    pr_info("Device UNREGISTERED\n");
}

module_init(my_device_init);
module_exit(my_device_exit);