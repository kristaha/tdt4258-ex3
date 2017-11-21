#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h> 
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/signal.h>
#include <asm/siginfo.h>
#include <linux/platform_device.h>
#include "efm32gg.h"

/*
 * template_init - function to insert this module into kernel space
 * Returns 0 if successfull, otherwise -1
 */

/* Defines */
#define DRIVER_NAME "gamepad"
#define DEV_NR_COUNT 1
#define PLATFORM_MEM_INDEX 0
#define PLATFORM_IRQ_EVEN_INDEX 0
#define PLATFORM_IRQ_ODD_INDEX 1

/* Function prototypes */
static int __init gamepad_init(void);
static int myProbe(struct platform_device *dev); 
static int myRemove(struct platform_device *dev); 
static void __exit gamepad_exit(void);
static ssize_t gamepadRead(struct file*, char* __user, size_t count);
static int gamepadOpen(struct inode*, struct file*);
static int gamepadRelease(struct inode*, struct file*);
static ssize_t gamepadWrite(struct file*, const char* __user, size_t, loff_t*);
irqreturn_t gpioInterruptHandler(int irq, void *dev_id, struct pt_regs *regs);
static int gamepad_fasync(int fd, struct file *filp, int mode);

/* Static variables */
static dev_t devNumber;
struct cdev gamepad_cdev;
struct class* cl;
struct fasync_struct* async_queue;

/* Struct for file operations related to driver */
static struct file_operations gamepad_fops = {
	.owner = THIS_MODULE,
	.open = gamepadOpen,
	.release = gamepadRelease,
	.write = gamepadWrite,
	.read = gamepadRead,
	.fasync = gamepad_fasync,	
};

/* Variables for platform driver */
static const struct of_device_id my_of_match[] = { 
	{.compatible = "tdt4258", },
	{ },
};
MODULE_DEVICE_TABLE(of, my_of_match);
static struct platform_driver my_driver = {
	.probe = myProbe,
	.remove = myRemove,
	.driver = {
		.name = "my",
		.owner = THIS_MODULE,
		.of_match_table = my_of_match,
	 },
};

struct resource *res;
 
/* Initiate gamepad and add it to kernel */
static int __init gamepad_init(void){
	platform_driver_register(&my_driver);
	return 0;
}

static int myProbe(struct platform_device *dev){

	int x;
	int addNumber;
	printk("Hello World, here is your module speaking\n");
	
	/* Dynamicly allocate device numbers */
	x = alloc_chrdev_region(&devNumber, 0, DEV_NR_COUNT, DRIVER_NAME);
	if(x < 0){
		printk(KERN_ALERT"Error: could not allocate device number \n");
		return -1;
	}
	
	/* Ckecks if needed registers are allocated */		
	res = platform_get_resource(dev, IORESOURCE_MEM,PLATFORM_MEM_INDEX);
	if(request_mem_region(res->start, res->end - res->start, DRIVER_NAME) == NULL){
		printk(KERN_ALERT "Error: Could not request memory for GPIO_PC_DIN\n");
		return -1;
	}

	/* Init gpio the same way as in exercise one and two.
	   Since arm is I/O mapped we do not need to use virtual memory */
	iowrite32(0x33333333,(void*) (res->start + GPIO_PC_MODEL));
	iowrite32(0xff, (void*) (res->start + GPIO_PC_DOUT));
	
	/* Setup  interrupts */	
	iowrite32(0x22222222, (void*) (res->start + GPIO_EXTIPSELL));
	request_irq(platform_get_irq(dev,PLATFORM_IRQ_EVEN_INDEX), (irq_handler_t) gpioInterruptHandler, 
		0, DRIVER_NAME, &gamepad_cdev); // Even
	request_irq(platform_get_irq(dev,PLATFORM_IRQ_ODD_INDEX), (irq_handler_t) gpioInterruptHandler, 
		0, DRIVER_NAME, &gamepad_cdev); // odd

	/* Adding device to kernel */
	cdev_init(&gamepad_cdev, &gamepad_fops);
	gamepad_cdev.owner = THIS_MODULE; 
	addNumber = cdev_add(&gamepad_cdev, devNumber, 1);
	if (addNumber !=  0) {
		printk(KERN_ALERT "Error: could not add device driver\n");
		return -1;
	}
	cl = class_create(THIS_MODULE, DRIVER_NAME); 
	device_create(cl, NULL, devNumber, NULL, DRIVER_NAME); 
	
	/* Enable interrupts */
	iowrite32(0xff, (void*) (res->start + GPIO_EXTIFALL));
	iowrite32(0xff, (void*) (res->start + GPIO_EXTIRISE));
	iowrite32(0xff, (void*) (res->start + GPIO_IEN));
	
	printk(KERN_ALERT "Driver loaded\n");	

	return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 */

static void __exit gamepad_exit(void){
	platform_driver_unregister(&my_driver);
}

static int myRemove(struct platform_device *dev){ 
	printk("Short life for a small module...\n");
	/* Disable interrupts */
	iowrite32(0, (void*) (res->start + GPIO_IEN));
	
	/* Free irq */
	free_irq(platform_get_irq(dev,PLATFORM_IRQ_ODD_INDEX), &gamepad_cdev); // odd
	free_irq(platform_get_irq(dev,PLATFORM_IRQ_EVEN_INDEX), &gamepad_cdev); // even

	/* Free memory */
	release_mem_region(res->start, res->end - res->start);

	cdev_del(&gamepad_cdev);  //Delete the cdev struct
	unregister_chrdev_region(devNumber, DEV_NR_COUNT); // Unregister device
	return 0;
}

/* Interrupt handler */
irqreturn_t gpioInterruptHandler(int irq, void *dev_id, struct pt_regs *regs){
	printk("Interrupt beeing handled\n");
	iowrite32(ioread32((void*) (res->start + GPIO_IF) ), (void*) (res->start + GPIO_IFC));
	if(async_queue){
		kill_fasync(&async_queue, SIGIO, POLL_IN);
	}
	return IRQ_HANDLED; 
}

static ssize_t gamepadRead(struct file* file, char __user* buff, size_t count){
	uint32_t data;
	printk(KERN_ALERT "Gamepad read\n");
	data = ioread32((void*) (res->start + GPIO_PC_DIN));
	copy_to_user(buff, &data, count);

	return 1;
}

static int gamepadOpen(struct inode* node, struct file* file ){
	printk(KERN_ALERT "Gamepad opened\n");
	return 0;
}

static int gamepadRelease(struct inode* node, struct file* file){
	printk(KERN_ALERT "Gamepad released\n");
	return 0;
}

static ssize_t gamepadWrite(struct file* file, const char __user* buff, size_t count, loff_t* offp){
	printk(KERN_ALERT "Gamepad write\n");
	return 1;
}

static int gamepad_fasync(int fd, struct file *filp, int mode){
	printk("Fasync handler\n");
	return fasync_helper(fd, filp, mode, &async_queue);	
}
 
module_init(gamepad_init);
module_exit(gamepad_exit);

MODULE_DESCRIPTION("Modul to handle the gampad\n");
MODULE_LICENSE("GPL");

