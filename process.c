#include "process.h"

static Process table[AURORA_MAX_PROCESSES];
static uint32_t next_pid = 1;

void process_init(void) {
    for (size_t i = 0; i < AURORA_MAX_PROCESSES; i++) {
        table[i].pid = 0;
        table[i].state = PROCESS_UNUSED;
        table[i].privileged = 0;
        table[i].network_capability = 0;
        table[i].sensor_capability = 0;
        table[i].name = (const char*)0;
    }
    next_pid = 1;
}

int process_create(const char* name, uint8_t privileged) {
    for (size_t i = 0; i < AURORA_MAX_PROCESSES; i++) {
        if (table[i].state == PROCESS_UNUSED) {
            table[i].pid = next_pid++;
            table[i].state = PROCESS_READY;
            table[i].privileged = privileged ? 1 : 0;
            table[i].network_capability = 0;
            table[i].sensor_capability = 0;
            table[i].name = name;
            return (int)table[i].pid;
        }
    }
    return -1;
}

const Process* process_get(size_t index) {
    if (index >= AURORA_MAX_PROCESSES) return (const Process*)0;
    return &table[index];
}

size_t process_count(void) {
    size_t count = 0;
    for (size_t i = 0; i < AURORA_MAX_PROCESSES; i++) {
        if (table[i].state != PROCESS_UNUSED) count++;
    }
    return count;
}
