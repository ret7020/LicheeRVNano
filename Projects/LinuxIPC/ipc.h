#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include <cstring>

template <typename T>
class Publisher {
public:
    Publisher(const std::string& topic_name)
        : shm_name("/" + topic_name + "_shm"), sem_name("/" + topic_name + "_sem") {
        
        shm_fd = shm_open(shm_name.c_str(), O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) throw std::runtime_error("Failed to open shared memory");

        ftruncate(shm_fd, sizeof(T));

        data = static_cast<T*>(mmap(nullptr, sizeof(T), PROT_WRITE, MAP_SHARED, shm_fd, 0));
        if (data == MAP_FAILED) throw std::runtime_error("Failed to mmap");

        sem = sem_open(sem_name.c_str(), O_CREAT, 0666, 0);
        if (sem == SEM_FAILED) throw std::runtime_error("Failed to open semaphore");
    }

    void publish(const T& msg) {
        std::memcpy(data, &msg, sizeof(T));
        sem_post(sem);
    }

    ~Publisher() {
        munmap(data, sizeof(T));
        close(shm_fd);
        sem_close(sem);
        // Full memory clean
        shm_unlink(shm_name.c_str());
        sem_unlink(sem_name.c_str());
    }

private:
    std::string shm_name;
    std::string sem_name;
    int shm_fd;
    T* data;
    sem_t* sem;
};
