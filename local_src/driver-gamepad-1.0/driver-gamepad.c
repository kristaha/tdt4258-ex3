/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>

#include "efm32gg.h"

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

/* Defines */
#define DRIVER_NAME "gamepad"
#define GPIO_EVEN_IRQ_LINE = 17
#define GPIO_ODD_IRQ_LINE = 18
#define DEV_NR_COUNT = 1


/* Function prototypes */
static int __init gamepad_init(void);
static void __exit gamepad_exit(void);

/*Static variables */
static dev_t devNumber;
struct cdev gamepad_cdev;


/* Structs */
static struct file_operations gamepad_fops = {
	.owner = THIS_MODULE,
	.open = gamepadOpen,
	.relase = gamepadRelase,
	.write = gamepadWrite,
	.read = gamepadRead
	
};

//static int __init template_init(void)
static int __init gamepad_init(void)
{
	printk("Hello World, here is your module speaking\n");
	
	/* Dynamicly allocate device numbers */
	int x = alloc_chrdev_region(&devNumber, 0, DEV_NR_COUNT, DRIVER_NAME);
	if(x < 0){
		printk(KERN_ALERT"Error: could not allocate device number \n");
		return -1;
	}
	/*Adding device to kernel*/
	cdev_init(&gamepad_cdev, &gamepad_fops);
	gamepad_cdev.owner = THIS_MODULE; 
	int addNumber = cdev_add(&gamepad_cdev, devNumber, 1);
	if (addNumber < 0) {
		printk(KERN_ALERT "Error: could not add device driver\n");
		return -1;
	}
	
	/* Ckecks if needed registers are allocated */
	if(request_mem_region(GPIO_PC_MODEL, 4, DRIVER_NAME) == NULL){
		printk(KERN_ALERT "Error: Could not request memory for GPIO_PC_MODEL\n ");
		return -1;
	}
	if(request_mem_region(GPIO_PC_DIN, 2, DRIVER_NAME) == NULL){
		printk(KERN_ALERT "Error: Could not request memory for GPIO_PC_DIN\n");
		return -1;
	}
	if(request_mem_region(GPIO_PC_DOUT, 2, DRIVER_NAME) == NULL){
		printk(KERN_ALERT "Error: Could not request memory for GPIO_PC_DOUT\n");
		return -1;
	}
	
	/* Init gpio the same way as in exercise one and two
	   Since arm is I/O mapped we do not need to use virtual memory */
	iowrite32(0x33333333, GPIO_PC_MODEL);
	iowrite32(0Xff, GPIO_PC_DOUT);
	
	/*Init interrupts*/	
	iowrite32(0x22222222, GPIO_EXTIPSELL);
	request_irq(GPIO_EVEN_IRQ_LINE, (irqreturn_t) gpioInterruptHandler, 0, DRIVER_NAME, &gamepad_cdev );
	request_irq(GPIO_ODD_IRQ_LINE, (irqreturn_t) gpioInterruptHandler, 0, DRIVER_NAME, &gamepad_cdev);
	

	return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

//static void __exit template_cleanup(void)
static void __exit gamepad_exit(void)
{

	printk("Short life for a small module...\n");
	cdev_del(&gampad_cdev);  //Delete the cdev struct
	unregister_chr_dev_region(devNumber, DEV_NR_COUNT); // Unregister device
}

/* Interrupt handler */
irqreturn_t gpioInterruptHandler(int irq, void *dev_id, struct pt_regs *regs){
	printk("Interrupt beeing handled\n");
}

static int gamepadRead(struct file* file, char __user* buff, size_t count, loff_t* offp){
	printk(KERN_ALERT "Gamepad read\n");
	return 0;
}

static int gamepadOpen(struct inode* node, struct file* file ){
	printk(KERN_ALERT "Gamepad opened\n");
	return 0;
}

static int gamepadRelase(struct inode* node, struct file* file){
	printk(KERN_ALERT "Gamepad relased");
	return 0;
}

static int gamepadWrite(struct file* file, const char __user* buff, size_t count, loff_t* offp){
	printk(KERN_ALERT "Gamepad write");
	return 0;
}

static int 
module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Modul to handle the gampad\n");
MODULE_LICENSE("GPL");

