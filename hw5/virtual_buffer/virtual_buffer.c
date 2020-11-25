#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define BUF_SIZE 1024
#define MAJOR_NUMBER 240

static char *buffer = NULL;

static int my_open(struct inode *inode, struct file *flip){
	printk("[VB] opened\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *filp){
	printk("[VB] released\n");
	return 0;
}

static ssize_t my_write(struct file *flip, const char *buf, size_t count, loff_t *f_pos){
	copy_from_user(buffer, buf, count);
	printk("[VB] write to buffer: %s\n", buffer);
	return count;
}

static ssize_t my_read(struct file *flip, char *buf, size_t count, loff_t *f_pos){
	printk("[VB] read to buffer: %s\n", buffer);
	copy_to_user(buf, buffer, count);
	return count;
}

static struct file_operations vd_fops = {
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release
};

int __init my_init(void){
	if(register_chrdev(MAJOR_NUMBER, "virtual_buffer", &vd_fops) < 0){
		printk("[VB] initial fail\n");
	}
	else
		printk("[VB] initialized\n");

	buffer = (char*)kmalloc(BUF_SIZE, GFP_KERNEL);

	if(buffer != NULL)
		memset(buffer, 0, BUF_SIZE);

	return 0;
}

void __exit my_exit(void){
	unregister_chrdev(MAJOR_NUMBER, "virtual_buffer");
	printk("[VB] exited\n");
	kfree(buffer);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
