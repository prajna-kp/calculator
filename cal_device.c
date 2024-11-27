#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "calc_device"

static int major_number;
//static struct cdev calc_cdev;
static char buffer[100]; // Buffer for input
static int num1, num2, result;
static char operation;

static int calc_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Calculator device opened\n");
    return 0;
}

static ssize_t calc_write(struct file *file, const char *user_buffer, size_t len, loff_t *offset) {
    // Parse the input from the user
    if(len > sizeof(buffer) - 1) {
        printk(KERN_ALERT "Input too long\n");
        return -EINVAL;
    }
    if(copy_from_user(buffer,user_buffer,len)) {
        return -EFAULT;
    }
    buffer[len] = '\0'; // Null terminate the buffer

    // Expecting input in the format: num1 num2 operation (e.g., 5 3 +)
    if (sscanf(buffer, "%d %d %c", &num1, &num2, &operation) != 3) {
        printk(KERN_ALERT "Invalid input format\n");
        return -EINVAL;
    }

    // Perform the operation based on the input
    switch (operation) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
             result = num1 - num2;
              break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            result = num1 / num2;
            break;
        default:
            printk(KERN_ALERT "Unsupported operation\n");
            return -EINVAL;
    }

    printk(KERN_INFO "Received: %d %c %d, Result: %d\n", num1, operation, num2, result);
    return len;
}

static ssize_t calc_read(struct file *file, char *user_buffer, size_t len, loff_t *offset) {
    // Write the result to user-space
    if (copy_to_user(user_buffer, &result, sizeof(result))) {
        return -EFAULT;
    }
    printk(KERN_INFO "Sent result: %d\n", result);
    return sizeof(result);
}

static struct file_operations fops = {
    .open = calc_open,
    .write = calc_write,
    .read = calc_read,
};

static int __init calc_init(void) {
    printk(KERN_INFO "Calculator module loaded\n");

    // Register the device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register character device\n");
        return major_number;
    }
    printk(KERN_INFO "Calculator device registered with major number %d\n", major_number);

    // Create the character device
    return 0;
}
tatic void __exit calc_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Calculator module unloaded\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prajna");
MODULE_DESCRIPTION("A simple calculator kernel module");
MODULE_VERSION("1.0");

