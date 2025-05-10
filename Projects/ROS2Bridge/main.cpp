#include "libs/ros2_bridge/transport.h"
#include "libs/ros2_bridge/messages.h"
#include <stdio.h>

int main() {
    ROSBridge bridge = ROSBridge(); // UDP broadcast bridge

    TFMessage message;
    message.x = 1.5f;
    message.y = 2.5f;
    message.z = 3.5f;
    message.roll = .0;
    message.pitch = .0;
    message.yaw = .0;

    while (true){
        message.x += 0.05f;
        bridge.send_struct(message);
        usleep(1000000);
    }


    return 0;
}