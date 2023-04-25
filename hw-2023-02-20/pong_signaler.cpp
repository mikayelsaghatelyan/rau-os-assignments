#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>

//code works until Ctrl+C is pressed from the terminal
int main() {
    mqd_t descriptor1to2 = mq_open("/queue1to2", O_RDONLY);
    mqd_t descriptor2to1 = mq_open("/queue2to1", O_WRONLY);

    if (descriptor1to2 == (mqd_t) -1) {
        perror("Unable to open the message queue.\n");
        return -1;
    }
    while (true) {
        sleep(2);
        char msg1to2[50];
        int byte_count = mq_receive(descriptor1to2, msg1to2, 50, NULL);
        if (byte_count == -1) {
            perror("Unable to receive the message.\n");
            return 1;
        }

        printf("Message received: %s\n", msg1to2);

        char msg2to1[10] = "PONG";

        if (mq_send(descriptor2to1, msg2to1, 10, 0) == -1) {
            perror("Unable to send a message.\n");
            return -1;
        }
    }
}
