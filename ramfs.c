#include "ramfs.h"

struct ram_file { const char* path; const char* content; };

static const struct ram_file files[] = {
    {"/manual/privacy", "PRIVACY BASICS\n\nNetwork starts OFF. Session storage is RAM-only.\nNever enter credentials until you intentionally enable networking.\n"},
    {"/manual/safety", "CYBERSECURITY SAFETY\n\nTest only systems you own or are authorized to assess.\nUse isolated labs and training targets. Stop when scope ends.\n"},
    {"/manual/commands", "COMMANDS\n\nhelp status manual privacy about clear\n"},
    {"/system/release", "AURORA VAULT OS 0.2\nIndependent kernel foundation.\n"},
};

void ramfs_init(void) {}

const char* ramfs_read(const char* path) {
    for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
        const char* a = files[i].path;
        size_t j = 0;
        while (a[j] && path[j] && a[j] == path[j]) j++;
        if (a[j] == 0 && path[j] == 0) return files[i].content;
    }
    return (const char*)0;
}

size_t ramfs_count(void) { return sizeof(files) / sizeof(files[0]); }
const char* ramfs_path(size_t index) {
    if (index >= ramfs_count()) return (const char*)0;
    return files[index].path;
}
