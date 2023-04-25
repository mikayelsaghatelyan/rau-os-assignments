#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>

//code works until Ctrl+C is pressed from the terminal
int main() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 33;
    attr.mq_curmsgs = 0;

    mqd_t descriptor1to2 = mq_open("/queue1to2", O_CREAT | O_WRONLY, 0666, &attr);
    mqd_t descriptor2to1 = mq_open("/queue2to1", O_CREAT | O_RDONLY, 0666, &attr);

    while (true) {
        sleep(2);
        if (descriptor1to2 == (mqd_t) -1) {
            perror("Unable to create a message queue.\n");
            return -1;
        }

        char msg1to2[10] = "PING";

        if (mq_send(descriptor1to2, msg1to2, 10, 0) == -1) {
            perror("Unable to send a message.\n");
            return -1;
        }

        char msg2to1[50];
        int byte_count = mq_receive(descriptor2to1, msg2to1, 50, NULL);
        if (byte_count == -1) {
            perror("Unable to receive the message.\n");
            return 1;
        }
        printf("Message received: %s\n", msg2to1);
    }
}
