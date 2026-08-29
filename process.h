#ifndef AURORA_PROCESS_H
#define AURORA_PROCESS_H

#include <stdint.h>
#include <stddef.h>

#define AURORA_MAX_PROCESSES 8

typedef enum {
    PROCESS_UNUSED = 0,
    PROCESS_READY = 1,
    PROCESS_RUNNING = 2,
    PROCESS_BLOCKED = 3,
} ProcessState;

typedef struct {
    uint32_t pid;
    ProcessState state;
    uint8_t privileged;
    uint8_t network_capability;
    uint8_t sensor_capability;
    const char* name;
} Process;

void process_init(void);
int process_create(const char* name, uint8_t privileged);
const Process* process_get(size_t index);
size_t process_count(void);

#endif
