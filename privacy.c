#include "privacy.h"

static PrivacyState state;

void privacy_init(void) {
    state.network_enabled = 0;
    state.camera_enabled = 0;
    state.microphone_enabled = 0;
    state.persistent_storage_enabled = 0;
    state.external_ai_enabled = 0;
}

const PrivacyState* privacy_state(void) {
    return &state;
}

// These setters only change policy state. Hardware drivers must separately
// prove that the requested capability exists before connecting to a device.
int privacy_set_network(uint8_t enabled) {
    state.network_enabled = enabled ? 1 : 0;
    if (!state.network_enabled) state.external_ai_enabled = 0;
    return 1;
}

int privacy_set_camera(uint8_t enabled) {
    state.camera_enabled = enabled ? 1 : 0;
    return 1;
}

int privacy_set_microphone(uint8_t enabled) {
    state.microphone_enabled = enabled ? 1 : 0;
    return 1;
}

int privacy_set_persistence(uint8_t enabled) {
    // Persistence remains locked until an encrypted storage driver is present.
    if (enabled) return 0;
    state.persistent_storage_enabled = 0;
    return 1;
}

int privacy_set_external_ai(uint8_t enabled) {
    // External AI is never allowed while networking is disabled.
    if (enabled && !state.network_enabled) return 0;
    state.external_ai_enabled = enabled ? 1 : 0;
    return 1;
}
