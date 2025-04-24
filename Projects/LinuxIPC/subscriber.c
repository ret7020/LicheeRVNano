#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include "messages.h"

int main() {
    int shm_fd = shm_open(TOPIC_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    struct TopicMessage *msg = mmap(0, TOPIC_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    sem_t *sem = sem_open(TOPIC_SEM_NAME, 0);

    while (1) {
        sem_wait(sem);
        // printf("[Subscriber] Received: id=%d, value=%.2f\n", msg->id, msg->value);
    }

    munmap(msg, TOPIC_SIZE);
    close(shm_fd);
    sem_close(sem);
    return 0;
}
