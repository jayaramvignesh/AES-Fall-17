#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/fs.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/gpio.h>

#define DEVICE_NAME "chardriver"
#define CLASS_NAME "designateddriver"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VIGNESH JAYARAM, ARUNDHATHI SWAMI");
MODULE_DESCRIPTION("Simple Character Driver on BBG to control LED driver for Designated Driver Project");
MODULE_VERSION("1.0");

static unsigned int gpioLED1=53;
static unsigned int gpioLED2=54;
static unsigned int gpioLED3=55;
static unsigned int gpioLED4=56;
static bool ledState=0;

/*stoes the device number*/
static int major_number;

/*memory for string passed from userspace*/
static char message[256] = {0};

/*variable to store size of string*/
static uint32_t size_of_message = 0;

/*variable to count number of times device has been opened*/
static uint32_t no_open = 0;

/*device driver class struct pointer*/
static struct class* chardriverclass = NULL;

/*device dtriver device struct pointer*/
static struct device* designateddriverdevice = NULL;

/*function prototype for open*/
static int dev_open(struct inode *inodep, struct file *filep);

/*function prototype for release*/
static int dev_release(struct inode *inodep, struct file *filep);

/*function prototype for read*/
static ssize_t dev_read(struct file *filep,char *buffer,size_t length, loff_t* offset);

/*function prototype for write*/
static ssize_t dev_write(struct file *filep, const char *buffer, size_t length, loff_t* offset);

/*fops structure for linking the prototypes with the callback functions*/
static struct file_operations fops = 
{
  .open = dev_open,
  .read = dev_read,
  .write = dev_write,
  .release = dev_release,
};


/*module init function*/
static int __init ddchardriver_init(void)
{
  printk(KERN_ALERT "PROGRESS: DD Char Driver Init Function Started\n");

  /*Allocate Major Number*/
  major_number = register_chrdev(0,DEVICE_NAME, &fops);

  /*check for success*/
  if(major_number < 0)
  {
    printk(KERN_ALERT "ERROR: Failed to register DD Char Driver\n");
    return major_number;
  }

  printk(KERN_ALERT "PROGRESS: DD Char Driver registered successfully with Major Number:%d\n",major_number);

  /*Register the device class*/
   chardriverclass = class_create(THIS_MODULE,CLASS_NAME);

  if(IS_ERR(chardriverclass))
  {
    unregister_chrdev(major_number,DEVICE_NAME);
    printk(KERN_ALERT "ERROR: Failed to register DD Char Driver Device Class \n");
    return PTR_ERR(chardriverclass);
  }

  printk(KERN_ALERT "PROGRESS: DD Char Driver Device Class registered successfully\n");
  
  /*Register DEvice Driver*/
  designateddriverdevice = device_create(chardriverclass, NULL , MKDEV(major_number, 0), NULL, DEVICE_NAME);

  if(IS_ERR(designateddriverdevice))
  {
    class_destroy(chardriverclass);
    unregister_chrdev(major_number,DEVICE_NAME);
    printk(KERN_ALERT "ERROR: Failed to create DD Char Driver Device\n");
    return PTR_ERR(designateddriverdevice);
  }

 /*LED INIT*/
 //check if gpio is valid
  if(!gpio_is_valid(gpioLED1)){
    printk(KERN_ALERT "INVALID GPIO\n");
    return -ENODEV;
  }
 if(!gpio_is_valid(gpioLED2)){
    printk(KERN_ALERT "INVALID GPIO\n");
    return -ENODEV;
  }
 if(!gpio_is_valid(gpioLED3)){
    printk(KERN_ALERT "INVALID GPIO\n");
    return -ENODEV;
  }
 if(!gpio_is_valid(gpioLED4)){
    printk(KERN_ALERT "INVALID GPIO\n");
    return -ENODEV;
  }



  ledState = false;
  /*initialising led1 as 0 or off*/
  gpio_request(gpioLED1,"sysfs");
  gpio_direction_output(gpioLED1,ledState);
  gpio_export(gpioLED1, false);
 /*initialising led2 as 0 or off*/
  gpio_request(gpioLED2,"sysfs");
  gpio_direction_output(gpioLED2,ledState);
  gpio_export(gpioLED2, false);
 /*initialising led3 as 0 or off*/
  gpio_request(gpioLED3,"sysfs");
  gpio_direction_output(gpioLED3,ledState);
  gpio_export(gpioLED3, false);
 /*initialising led4 as 0 or off*/
  gpio_request(gpioLED4,"sysfs");
  gpio_direction_output(gpioLED4,ledState);
  gpio_export(gpioLED4, false);


  printk(KERN_ALERT "PROGRESS: LED Drivers Initialized\n");
  printk(KERN_ALERT "PROGRESS: DD Char Driver Created SUccessfully. Exiting Init Function\n");
  return 0;
}

/*module clean up function*/
static void __exit ddchardriver_exit(void)
{
 printk(KERN_ALERT "PROGRESS: DD Char Driver Exit and Clean UP Initialized\n");

 /*deinitializing led 1*/
  gpio_set_value(gpioLED1,0);
  gpio_unexport(gpioLED1);
  gpio_free(gpioLED1);

  /*deinitializing led 2*/
  gpio_set_value(gpioLED2,0);
  gpio_unexport(gpioLED2);
  gpio_free(gpioLED2);

  /*deinitializing led 3*/
  gpio_set_value(gpioLED3,0);
  gpio_unexport(gpioLED3);
  gpio_free(gpioLED3);

   /*deinitializing led 4*/
  gpio_set_value(gpioLED4,0);
  gpio_unexport(gpioLED4);
  gpio_free(gpioLED4);


  /*remove the device*/
  device_destroy(chardriverclass, MKDEV(major_number,0));

  /*unregister the device*/
  class_unregister(chardriverclass);
    
  /*Remove the device class*/
  class_destroy(chardriverclass);

  /*unregister the major number*/
  unregister_chrdev(major_number,DEVICE_NAME);

  printk(KERN_ALERT "PROGRESS: DD Char Driver Completed Clean up and exited\n");
}

/************************************************ 
 * Device Open Function
 * increment the counter whenever open is called
 * filep = pointer to file object
 * inodep = pointer to inode object
 ************************************************/

static int dev_open(struct inode *inodep, struct file* filep)
{
  no_open++;
  printk(KERN_INFO "PROGRESS: DD Char Driver has been opened %d times\n",no_open);
  return 0;
}


/************************************************ 
 * Device Read Function
 * filep = pointer to file object
 * length = length of buffer to read
 * buffer = array where contents to read are stored
 * offset = offset if required
 ************************************************/

static ssize_t dev_read(struct file *filep,char *buffer,size_t length, loff_t* offset)
{
  int error_count = 0;

  /*copy to user is used to send data from kernel side to user space*/
  error_count = copy_to_user(buffer,message,size_of_message);

  if(error_count == 0)
  {
    printk(KERN_INFO "PROGRESS: %d characters read from userspace in DD Char Driver\n",size_of_message);
    size_of_message = 0;
    return 0;
  }
  else
  {
    printk(KERN_ALERT "ERROR: DD Char Driver Device failed in read\n");
    return -EFAULT;
  }
}

/************************************************ 
 * Device Write Function
 * filep = pointer to file object
 * length = length of buffer to read
 * buffer = array where contents to write are stored
 * offset = offset if required
 ************************************************/

static ssize_t dev_write(struct file *filep,const char *buffer,size_t length, loff_t* offset)
{
  // sprintf(message , "%z(%zu letters)" , buffer, length);
  int error_count = copy_from_user(message,buffer,length);

  if(error_count == 0)
  {
    printk(KERN_INFO "PROGRESS: DDChar Driver has %zu characters written\n",length);
    size_of_message = 0;

  }
  else
  {
    printk(KERN_ALERT "\nERROR: DD Char Driver Error in Write\n");

    return -EFAULT;
  }
  
  if(strncmp(message, "fail", 4) == 0)
  {
    ledState = true; 
    gpio_set_value(gpioLED1, ledState); 
  
    gpio_set_value(gpioLED2, ledState); 

    gpio_set_value(gpioLED3, ledState); 

    gpio_set_value(gpioLED4, ledState); 
  }
  
  size_of_message = strlen(message);
  printk(KERN_INFO "PROGRESS: DD Char Driver received %zu chars from user\n",length);
  printk(KERN_INFO "PROGRESS: DD Char Driver Received %s message from user\n",message);
  return length;
}

/************************************************ 
 * Device Release Function
 * filep = pointer to file object
 * inodep = pointer to inode object
 ************************************************/

static int dev_release(struct inode *inodep, struct file *filep)
{
  printk(KERN_INFO "PROGRESS: DD Char Driver Successfully closed by User\n");
  return 0;
}


module_init(ddchardriver_init);
module_exit(ddchardriver_exit);
