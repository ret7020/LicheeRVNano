#ifndef TOPIC_H
#define TOPIC_H

#define TOPIC_NAME "/topic_data"
#define TOPIC_SEM_NAME "/topic_sem"
#define TOPIC_SIZE sizeof(struct TopicMessage)

struct TopicMessage {
    int id;
    float value;
};

#endif
