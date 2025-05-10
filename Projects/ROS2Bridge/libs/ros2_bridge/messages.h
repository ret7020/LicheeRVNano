#pragma once

#include <cstdint>

enum MessageType : uint8_t {
    MSG_TYPE_NONE = 0,
    MSG_TYPE_TF = 1,
    MSG_TYPE_TOPIC = 2
};

struct TFMessage {
    uint8_t message_type = MSG_TYPE_TF;
    uint8_t message_id = 0;

    float x, y, z;
    float roll, pitch, yaw;

};