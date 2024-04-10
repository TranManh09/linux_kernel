#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("A");
MODULE_DESCRIPTION("USE IOCTL TO CONTROL LED");
 
#define DRIVERNAME "gpio_driver"
#define CLASSNAME  "class_driver"
 
 
#define WR_VALUE _IOW('a', 1, uint32_t *);
#define RD_VALUE _IOW('a', 2, uint32_t *);
 
static dev_t my_device_nr;
static struct class * my_class;
static struct cdev my_device;
 
 
static int driver_open(struct inode * File, struct file *insatnce){
	printk("open driver\n");
        return 0;
}
 
static int driver_close(struct inode * File, struct file *insatnce){
	printk("close driver\n");
        return 0;
}
char buf[255];
static ssize_t driver_read(struct file *File, char* buffer, size_t len, loff_t * offp){
	printk("Read Function\n");
	return len;
}
 
static ssize_t driver_write(struct file *File,const char *buffer, size_t len, loff_t * offp){
	int a;
	a = copy_from_user(buf,buffer,len);
	switch(buf[0]){
		case 0:
			gpio_set_value(4,0);
		break;
		case 1:
			gpio_set_value(4,1);
		break;
		default:
			printk("Invalid gpio\n");
		break;
	}
	return len;
}
 
static long driver_ioctl(struct file * File, unsigned int cmd, unsigned long arg){
	int a;
	//a = copy_from_user(buf, arg, sizeof(arg));
	switch(cmd){
		case 0:
			gpio_set_value(4,0);
		break;
		case 1:
			gpio_set_value(4,1);
		break;
		default:
			printk("Invalid gpio\n");
		break;
	}
	return 0;
}
 
static struct file_operations fops = {
	.owner          = THIS_MODULE,
	.open           = driver_open,
	.release        = driver_close,
	.read           = driver_read,
	.write          = driver_write,
	.unlocked_ioctl = driver_ioctl,
};
 
static int __init InitModule(void){
	printk("hello kernel\n");
	if(alloc_chrdev_region(&my_device_nr, 0,1, DRIVERNAME)<0){
		printk("can't allocated major and minor\n");
		return -1;
	}
 
	if((my_class = class_create(THIS_MODULE, CLASSNAME))== NULL){
		printk("can't create class\n");
		goto CLASSERR;
	}
 
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVERNAME) == NULL){
		printk("can't create device filer\n");
		goto DEVERR;
	}
 
	cdev_init(&my_device, &fops);
	if(cdev_add(&my_device, my_device_nr,1)<0){
		printk("can't register device file\n");
		goto ADDERR;
	}
 
	if(gpio_request(4,"pin_4")){
		printk("can't request gpio 4\n");
		goto ADDERR;
	}
	if(gpio_direction_output(4,0)){
		printk("can't set gpio 4 is output\n");
		goto GPIOERR;
	}
 
	return 0;
	GPIOERR:
		gpio_free(4);
	ADDERR:
		device_destroy(my_class,my_device_nr);
	DEVERR:
		class_destroy(my_class);
	CLASSERR:
		unregister_chrdev_region(my_device_nr,1);
		return -1;
}
 
static void __exit ExitModule(void){
	gpio_set_value(4,0);
	gpio_free(4);
	device_destroy(my_class,my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr,1);
	printk("goodbye kernel\n");
}
 
module_init(InitModule);
module_exit(ExitModule);
