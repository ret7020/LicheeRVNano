#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include "messages.h"

int main() {
    int shm_fd = shm_open(TOPIC_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, TOPIC_SIZE);
    struct TopicMessage *msg = mmap(0, TOPIC_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem = sem_open(TOPIC_SEM_NAME, O_CREAT, 0666, 0);

    int i = 0;
    while (1) {
        msg->id = i;
        msg->value = i * 1.5f;
        sem_post(sem);
        // printf("[Publisher] Published: id=%d, value=%.2f\n", msg->id, msg->value);
        usleep(100);
        i++;
        if (i > 200) i = 0;
    }

    munmap(msg, TOPIC_SIZE);
    close(shm_fd);
    sem_close(sem);
    return 0;
}
