#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

class ROSBridge {
  private:
    int sockfd;
    sockaddr_in broadcastAddr{};
    socklen_t addrLen;

  public:
    ROSBridge(const char *broadcast_addr = "192.168.1.255", int port = 8888) {
        init(broadcast_addr, port);
        printf("[ROS2 Bridge] Broadcast socket ready\n");
    }

    ~ROSBridge() {
        close(sockfd);
    }

    int init(const char *broadcast_addr, int port) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            perror("socket creation failed");
            return -1;
        }

        int broadcastEnable = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
            perror("setsockopt (SO_BROADCAST) failed");
            return -1;
        }

        broadcastAddr.sin_family = AF_INET;
        broadcastAddr.sin_port = htons(port);
        inet_pton(AF_INET, broadcast_addr, &broadcastAddr.sin_addr);
        addrLen = sizeof(broadcastAddr);
        return 0;
    }

    template <typename T> ssize_t send_struct(const T &data) {
        ssize_t bytesSent = sendto(sockfd, &data, sizeof(data), 0, (struct sockaddr *)&broadcastAddr, addrLen);
        return bytesSent;
    }
};
