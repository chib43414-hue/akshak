#ifndef AURORA_PRIVACY_H
#define AURORA_PRIVACY_H

#include <stdint.h>

typedef struct {
    uint8_t network_enabled;
    uint8_t camera_enabled;
    uint8_t microphone_enabled;
    uint8_t persistent_storage_enabled;
    uint8_t external_ai_enabled;
} PrivacyState;

void privacy_init(void);
const PrivacyState* privacy_state(void);
int privacy_set_network(uint8_t enabled);
int privacy_set_camera(uint8_t enabled);
int privacy_set_microphone(uint8_t enabled);
int privacy_set_persistence(uint8_t enabled);
int privacy_set_external_ai(uint8_t enabled);

#endif
