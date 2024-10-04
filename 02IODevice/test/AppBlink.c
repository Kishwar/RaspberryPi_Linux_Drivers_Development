#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define DEVICE_PATH "/dev/pio"
#define SLEEP_DURATION 1000000 // Sleep duration in microseconds (1000,000 µs = 1 seconds)

int main() {
    int fd;
    char write_buffer[2];

    // Open the /dev/pio device file
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Failed to open /dev/pio");
        return EXIT_FAILURE;
    }

    // Loop 10 times to write 1, sleep, then write 0, sleep
    for (int i = 0; i < 10; i++) {
        // Write '1' to the device
        write_buffer[0] = '1';
        if (write(fd, write_buffer, 1) == -1) {
            perror("Failed to write '1' to /dev/pio");
            close(fd);
            return EXIT_FAILURE;
        }
        usleep(SLEEP_DURATION); // Sleep for 0.5 seconds

        // Write '0' to the device
        write_buffer[0] = '0';
        if (write(fd, write_buffer, 1) == -1) {
            perror("Failed to write '0' to /dev/pio");
            close(fd);
            return EXIT_FAILURE;
        }
        usleep(SLEEP_DURATION); // Sleep for 0.5 seconds
    }

    // Close the device file
    close(fd);
    printf("Toggled /dev/pio 10 times.\n");
    return EXIT_SUCCESS;
}

