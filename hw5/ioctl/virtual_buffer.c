#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define BUF_SIZE 1024
#define MAJOR_NUMBER 240

#define MY_IOCTL_NUMBER 100
#define MY_IOCTL_READ _IOR(MY_IOCTL_NUMBER, 0, int)
#define MY_IOCTL_WRITE _IOW(MY_IOCTL_NUMBER, 1, int)

#define MSG(string, args...) if(debug) printk(KERN_DEBUG string, ##args)

static char *buffer = NULL;
static int debug = 0;

static int my_open(struct inode *inode, struct file *flip){
	MSG("[VB] opened\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *filp){
	MSG("[VB] released\n");
	return 0;
}

static ssize_t my_write(struct file *flip, const char *buf, size_t count, loff_t *f_pos){
	copy_from_user(buffer, buf, count);
	MSG("[VB] write to buffer: %s\n", buffer);
	return count;
}

static ssize_t my_read(struct file *flip, char *buf, size_t count, loff_t *f_pos){
	MSG("[VB] read to buffer: %s\n", buffer);
	copy_to_user(buf, buffer, count);
	return count;
}

static long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	int size;

	if(_IOC_TYPE(cmd) != MY_IOCTL_NUMBER){
		MSG("[VB] ioctl cmd type error: %d\n", _IOC_TYPE(cmd));
	}

	size = _IOC_SIZE(cmd);

	switch(cmd){
		case MY_IOCTL_WRITE:
			copy_from_user((void*)&debug, (const void*)arg, (unsigned long)size);
			MSG("[VB] debug is set to %d\n", debug);
			break;
		case MY_IOCTL_READ:
			MSG("[VB] current debug value is %d\n", debug);
			copy_to_user((void*)arg, (const void*)&debug, (unsigned long)size);
			break;
		default:
			MSG("[VB] invalid ioctl command: %d\n", cmd);
			break;
	}
	return 0;
}

static struct file_operations vd_fops = {
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release,
	.unlocked_ioctl = my_ioctl
};

int __init my_init(void){
	if(register_chrdev(MAJOR_NUMBER, "virtual_buffer", &vd_fops) < 0){
		MSG("[VB] initial fail\n");
	}
	else
		MSG("[VB] initialized\n");

	buffer = (char*)kmalloc(BUF_SIZE, GFP_KERNEL);

	if(buffer != NULL)
		memset(buffer, 0, BUF_SIZE);

	return 0;
}

void __exit my_exit(void){
	unregister_chrdev(MAJOR_NUMBER, "virtual_buffer");
	MSG("[VB] exited\n");
	kfree(buffer);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
