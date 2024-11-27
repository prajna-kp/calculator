#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define D_FILE "/dev/calc_device"

int main() {
    int fd;
    char input[100];
    int result;

    // Open the device file
    fd = open(D_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device file");
        return 1;
    }

    // Get user input for the operation
    printf("Enter operation (e.g., 5 3 +): ");
    fgets(input, sizeof(input), stdin);

    // Write input to the kernel
    if (write(fd, input, strlen(input)) < 0) {
        perror("Failed to write to device");
        close(fd);
        return 1;
    }

    // Read the result from the kernel
    if (read(fd, &result, sizeof(result)) < 0) {
        perror("Failed to read from device");
        close(fd);
        return 1;
    }

    printf("Result: %d\n", result);
    close(fd);
    return 0;
}
