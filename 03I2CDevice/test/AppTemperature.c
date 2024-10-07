#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define DEVICE_PATH "/dev/pdev"
#define SLEEP_DURATION 10 // Sleep duration in seconds
#define BUFFER_SIZE 128

int write_timestamp(char *buffer, size_t buffer_size) {
    time_t rawtime;
    struct tm *timeinfo;

    // Get current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);  // Convert to local time format

    // Write formatted time into buffer
    return strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

int main() {
    int fd;
    char buffer[BUFFER_SIZE];

    // Open the /dev/pdev device file
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Failed to open /dev/pdev");
        return EXIT_FAILURE;
    }

    // Loop 10 times to write 1, sleep, then write 0, sleep
    for (int i = 0; i < 10; i++) {
        // read from the device
        int read_buffer;
        if (read(fd, &read_buffer, sizeof(int32_t)) == -1) {
            perror("Failed to read /dev/pdev");
            close(fd);
            return EXIT_FAILURE;
        }

        memset(buffer, 0, BUFFER_SIZE);
        write_timestamp(buffer, BUFFER_SIZE);

        printf("%s: %.2fC\n", buffer, read_buffer/100.0);
        sleep(SLEEP_DURATION); // Sleep for 1 second
    }

    close(fd);

    // Close the device file
    close(fd);
    printf("read /dev/pdev 10 times.\n");
    return EXIT_SUCCESS;
}
