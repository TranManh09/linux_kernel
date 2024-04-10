#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TENTUI99");

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVERNAME "read_write"
#define CLASSNAME  "class of name"

int driver_open(struct inode * instance, struct file * a){
	printk("open kernel");
	return 0;
}

int driver_close(struct inode * instance, struct file *a){
	printk("close kernel");
	return 0;
}

static char buffer[255];
static int buffer_pointer = 0;
static ssize_t driver_read(struct file * File, char * user_buffer, size_t count, loff_t * offs){
	

	printk("\n %s\n", user_buffer);

	return count;
}

static ssize_t driver_write(struct file * File, const char * user_buffer, size_t count, loff_t * offs){
	int to_copy, not_copied, delta;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(buffer));

	/* Copy data to user */
	not_copied = copy_from_user(buffer, user_buffer, to_copy);
	buffer_pointer = to_copy;

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};
static int __init InitModule(void)
{
	printk("hello kernel");
	if(alloc_chrdev_region(&my_device_nr,0,1, DRIVERNAME)>0){
		printk("can't allocated minor and minor\n");
		return -1;
	}

	if((my_class = class_create(THIS_MODULE, CLASSNAME))== NULL){
		printk("can't create class\n");
		goto ALLOCERR;
	}

	if(device_create(my_class,NULL,my_device_nr,NULL,DRIVERNAME) == NULL){
		printk("can't create device\n");
		goto DEVERR;
	}

	cdev_init(&my_device,&fops);

	if(cdev_add(&my_device, my_device_nr,1) == -1){
		printk("can't register device\n");
		goto ADDERR;
	}
	
	return 0;
	ADDERR:
		device_destroy(my_class,my_device_nr);
	DEVERR:
		class_destroy(my_class);
	ALLOCERR:
		unregister_chrdev_region(my_device_nr,1);
	return -1;
}

static void __exit ExitModule(void)
{
	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr,1);

}

module_init(InitModule);
module_exit(ExitModule);
