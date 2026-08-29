#include <stdint.h>
#include <stddef.h>
#include "kernel_core.h"
#include "memory.h"
#include "privacy.h"
#include "paging.h"
#include "graphics.h"
#include "ramfs.h"
#include "process.h"
#include "pci.h"

#define VGA_MEMORY ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define COLOR_BG 0x10
#define COLOR_PANEL 0x18
#define COLOR_CYAN 0x0B
#define COLOR_GREEN 0x0A
#define COLOR_AMBER 0x0E
#define COLOR_TEXT 0x0F
#define COLOR_MUTED 0x08

static size_t cursor_x = 2;
static size_t cursor_y = 22;
static char command[64];
static size_t command_len = 0;
static uint8_t shift_down = 0;
static uint8_t caps_lock = 0;

static char notes_buffer[256];
static size_t notes_len = 0;


static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void halt_cpu(void) {
    __asm__ volatile ("hlt");
}

static void put_cell(size_t x, size_t y, char value, uint8_t color) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    VGA_MEMORY[y * VGA_WIDTH + x] = ((uint16_t)color << 8) | (uint8_t)value;
}

static void clear_screen(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) put_cell(x, y, ' ', COLOR_BG);
    }
    cursor_x = 2;
    cursor_y = 22;
}

static void print_at(size_t x, size_t y, const char* text, uint8_t color) {
    while (*text && x < VGA_WIDTH) put_cell(x++, y, *text++, color);
}

static void horizontal(size_t y, uint8_t color) {
    for (size_t x = 0; x < VGA_WIDTH; x++) put_cell(x, y, 196, color);
}

static void draw_frame(void) {
    clear_screen();
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        put_cell(x, 0, 205, COLOR_CYAN);
        put_cell(x, 3, 205, COLOR_CYAN);
    }
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        put_cell(0, y, 186, COLOR_CYAN);
        put_cell(VGA_WIDTH - 1, y, 186, COLOR_CYAN);
    }
    put_cell(0, 0, 201, COLOR_CYAN); put_cell(VGA_WIDTH - 1, 0, 187, COLOR_CYAN);
    put_cell(0, 3, 204, COLOR_CYAN); put_cell(VGA_WIDTH - 1, 3, 185, COLOR_CYAN);
    print_at(2, 1, "AURORA VAULT OS  //  EPHEMERAL SECURITY WORKSPACE", COLOR_CYAN);
    print_at(57, 1, "LIVE 0.1", COLOR_GREEN);
    print_at(2, 2, "NO LINUX KERNEL  |  NO TELEMETRY  |  SESSION IN RAM", COLOR_MUTED);

    for (size_t y = 5; y < VGA_HEIGHT - 2; y++) put_cell(57, y, 179, COLOR_PANEL);
    print_at(59, 5, "PRIVACY STATUS", COLOR_CYAN);
    horizontal(6, COLOR_PANEL);
    print_at(59, 8, "SESSION", COLOR_MUTED); print_at(59, 9, "EPHEMERAL", COLOR_GREEN);
    print_at(59, 11, "NETWORK", COLOR_MUTED); print_at(59, 12, "LOCKED", COLOR_GREEN);
    print_at(59, 14, "DISK", COLOR_MUTED); print_at(59, 15, "NOT MOUNTED", COLOR_GREEN);
    print_at(59, 17, "CAMERA", COLOR_MUTED); print_at(59, 18, "OFF", COLOR_GREEN);
    print_at(59, 20, "AI", COLOR_MUTED); print_at(59, 21, "LOCAL ONLY", COLOR_AMBER);
    print_at(59, 23, "TICKS", COLOR_MUTED);
    print_at(69, 23, "HEAP", COLOR_MUTED);
    print_at(2, 24, "KERNEL HEAP 64K  /  SESSION MEMORY ONLY", COLOR_MUTED);

    print_at(2, 5, "VAULT TERMINAL", COLOR_CYAN);
    print_at(2, 6, "Type help for available commands.", COLOR_MUTED);
    print_at(2, 8, "[BOOT] Firmware handoff complete.", COLOR_GREEN);
    print_at(2, 9, "[BOOT] Memory-safe boundary pending in next milestone.", COLOR_MUTED);
    print_at(2, 10, "[PRIV] Temporary session policy active.", COLOR_GREEN);
    print_at(2, 11, "[PRIV] Network is disabled by default.", COLOR_GREEN);
    print_at(2, 12, "[INFO] This is an early kernel prototype.", COLOR_AMBER);
    cursor_x = 2;
    cursor_y = 22;
    print_at(2, 21, "----------------------------------------------------------------------------", COLOR_PANEL);
    print_at(2, 22, "vault@aurora:~$ ", COLOR_CYAN);
    cursor_x = 18;
}

static int same_command(const char* a, const char* b) {
    size_t i = 0;
    while (a[i] && b[i] && a[i] == b[i]) i++;
    return a[i] == 0 && b[i] == 0;
}

static int starts_with(const char* str, const char* prefix) {
    size_t i = 0;
    while (prefix[i] && str[i] && str[i] == prefix[i]) i++;
    return prefix[i] == 0;
}

static void clear_output_area(void) {
    for (size_t y = 14; y <= 21; y++) {
        for (size_t x = 2; x < 57; x++) {
            put_cell(x, y, ' ', COLOR_BG);
        }
    }
}

static void show_command(void) {
    command[command_len] = 0;
    clear_output_area();
    if (same_command(command, "help")) {
        print_at(2, 14, "help     show safe commands", COLOR_TEXT);
        print_at(2, 15, "status   show privacy state", COLOR_TEXT);
        print_at(2, 16, "manual   show learning areas", COLOR_TEXT);
        print_at(2, 17, "clear    redraw the workspace", COLOR_TEXT);
        print_at(2, 18, "privacy  inspect capability permissions", COLOR_TEXT);
        print_at(2, 19, "about    show release boundaries", COLOR_TEXT);
        print_at(2, 20, "manuals  list RAM-only offline documents", COLOR_TEXT);
        print_at(2, 21, "ps       show kernel service registry", COLOR_TEXT);
        print_at(2, 22, "calc     calculator app", COLOR_TEXT);
        print_at(2, 23, "guess <n> guess game (1-10)", COLOR_TEXT);
        print_at(35, 14, "agent <1-10>  ping agent", COLOR_TEXT);
        print_at(35, 15, "note <txt>    save to RAM notepad", COLOR_TEXT);
        print_at(35, 16, "notes         read notepad", COLOR_TEXT);
        print_at(35, 17, "sysmon        system monitor", COLOR_TEXT);
    } else if (same_command(command, "status")) {
        print_at(2, 14, "SESSION=EPHEMERAL  NETWORK=OFF  DISK=UNMOUNTED", COLOR_GREEN);
        print_at(2, 15, "CAMERA=OFF  MICROPHONE=OFF  TELEMETRY=DISABLED", COLOR_GREEN);
    } else if (same_command(command, "manual")) {
        print_at(2, 14, "MANUALS: privacy | digital-safety | ethical-security", COLOR_AMBER);
        print_at(2, 15, "All security practice must be authorized and defensive.", COLOR_MUTED);
    } else if (same_command(command, "privacy")) {
        const PrivacyState* p = privacy_state();
        print_at(2, 14, p->network_enabled ? "NETWORK=ON" : "NETWORK=OFF", p->network_enabled ? COLOR_AMBER : COLOR_GREEN);
        print_at(2, 15, p->camera_enabled ? "CAMERA=ON" : "CAMERA=OFF", p->camera_enabled ? COLOR_AMBER : COLOR_GREEN);
        print_at(2, 16, p->microphone_enabled ? "MIC=ON" : "MIC=OFF", p->microphone_enabled ? COLOR_AMBER : COLOR_GREEN);
        print_at(2, 17, p->persistent_storage_enabled ? "STORAGE=PERSISTENT" : "STORAGE=RAM ONLY", p->persistent_storage_enabled ? COLOR_AMBER : COLOR_GREEN);
        print_at(2, 18, p->external_ai_enabled ? "AI=EXTERNAL" : "AI=LOCAL/OFFLINE", p->external_ai_enabled ? COLOR_AMBER : COLOR_GREEN);
    } else if (same_command(command, "about")) {
        print_at(2, 14, "AURORA VAULT OS 0.2 KERNEL RELEASE", COLOR_CYAN);
        print_at(2, 15, "INDEPENDENT KERNEL  /  LIVE USB  /  NO LINUX KERNEL", COLOR_TEXT);
        print_at(2, 16, "NEXT: PROCESSES, MEMORY PROTECTION, GRAPHICS, DRIVERS", COLOR_AMBER);
        print_at(2, 17, "BROWSERS AND NETWORKING ARE NOT ENABLED YET.", COLOR_MUTED);
    } else if (same_command(command, "manuals")) {
        print_at(2, 14, "RAM-ONLY DOCUMENTS", COLOR_CYAN);
        for (size_t i = 0; i < ramfs_count() && i < 4; i++) print_at(2, 15 + i, ramfs_path(i), COLOR_TEXT);
    } else if (same_command(command, "ps")) {
        print_at(2, 14, "PID  STATE    PRIV  NAME", COLOR_CYAN);
        size_t row = 15;
        for (size_t i = 0; i < AURORA_MAX_PROCESSES && row < 22; i++) {
            const Process* p = process_get(i);
            if (!p || p->state == PROCESS_UNUSED) continue;
            print_at(2, row++, p->name, p->privileged ? COLOR_AMBER : COLOR_TEXT);
        }
    } else if (same_command(command, "devices")) {
        print_at(2, 14, "PCI HARDWARE DISCOVERY", COLOR_CYAN);
        print_at(2, 15, "DEVICE DRIVERS REMAIN DEFAULT-DENY UNTIL ENABLED.", COLOR_AMBER);
        print_at(2, 16, "DEVICES FOUND:", COLOR_TEXT);
        char count_text[4];
        size_t count = pci_count();
        count_text[0] = '0' + (char)((count / 10) % 10);
        count_text[1] = '0' + (char)(count % 10);
        count_text[2] = 0;
        print_at(16, 16, count_text, COLOR_TEXT);
    } else if (same_command(command, "clear")) {
        command_len = 0;
        draw_frame();
        return;
    } else if (starts_with(command, "calc ")) {
        const char* expr = command + 5;
        int a = 0, b = 0;
        char op = 0;
        while (*expr == ' ') expr++;
        while (*expr >= '0' && *expr <= '9') { a = a * 10 + (*expr - '0'); expr++; }
        while (*expr == ' ') expr++;
        if (*expr == '+' || *expr == '-' || *expr == '*' || *expr == '/') { op = *expr; expr++; }
        while (*expr == ' ') expr++;
        while (*expr >= '0' && *expr <= '9') { b = b * 10 + (*expr - '0'); expr++; }

        int res = 0;
        if (op == '+') res = a + b;
        else if (op == '-') res = a - b;
        else if (op == '*') res = a * b;
        else if (op == '/') res = b != 0 ? a / b : 0;

        char res_str[16];
        int is_neg = res < 0;
        if (is_neg) res = -res;
        int idx = 0;
        if (res == 0) res_str[idx++] = '0';
        while (res > 0) { res_str[idx++] = '0' + (res % 10); res /= 10; }
        if (is_neg) res_str[idx++] = '-';

        print_at(2, 14, "CALCULATOR APP", COLOR_CYAN);
        print_at(2, 15, "Result:", COLOR_MUTED);
        size_t print_x = 10;
        for (int j = idx - 1; j >= 0; j--) { put_cell(print_x++, 15, res_str[j], COLOR_GREEN); }
    } else if (starts_with(command, "guess ")) {
        const char* num_str = command + 6;
        int guess = 0;
        while (*num_str >= '0' && *num_str <= '9') {
            guess = guess * 10 + (*num_str - '0');
            num_str++;
        }

        uint64_t ticks = kernel_ticks();
        int target = ((uint32_t)ticks % 10) + 1; // Pseudo-random 1 to 10

        print_at(2, 14, "NUMBER GUESSING GAME", COLOR_CYAN);
        if (guess == target) {
            print_at(2, 15, "YOU WON! Correct guess.", COLOR_GREEN);
        } else {
            print_at(2, 15, "Wrong! The number was:", COLOR_AMBER);
            char target_str[3];
            target_str[0] = target == 10 ? '1' : '0' + target;
            target_str[1] = target == 10 ? '0' : 0;
            target_str[2] = 0;
            print_at(25, 15, target_str, COLOR_TEXT);
        }
    } else if (starts_with(command, "agent ")) {
        const char* id_str = command + 6;
        int id = 0;
        while (*id_str >= '0' && *id_str <= '9') { id = id * 10 + (*id_str - '0'); id_str++; }
        if (id >= 1 && id <= 10) {
            print_at(2, 14, "AGENT COMMUNICATION LINK", COLOR_CYAN);
            char resp[64] = "Agent 00 reports: Awaiting instructions.";
            resp[6] = '0' + (id / 10);
            resp[7] = '0' + (id % 10);
            print_at(2, 15, resp, COLOR_GREEN);
        } else {
            print_at(2, 14, "Agent ID must be between 1 and 10.", COLOR_AMBER);
        }
    } else if (same_command(command, "notes")) {
        print_at(2, 14, "NOTEPAD (RAM ONLY)", COLOR_CYAN);
        if (notes_len == 0) {
            print_at(2, 15, "No notes saved in this session.", COLOR_MUTED);
        } else {
            notes_buffer[notes_len] = 0;
            size_t y = 15;
            size_t x = 2;
            const char* content = notes_buffer;
            while (*content && y <= 21) {
                if (*content == '\n') { y++; x = 2; }
                else { put_cell(x++, y, *content, COLOR_TEXT); if (x >= 57) { y++; x = 2; } }
                content++;
            }
        }
    } else if (starts_with(command, "note ")) {
        const char* note_text = command + 5;
        while (*note_text == ' ') note_text++;

        while (*note_text && notes_len < 254) {
            notes_buffer[notes_len++] = *note_text++;
        }
        notes_buffer[notes_len++] = '\n'; // auto newline

        print_at(2, 14, "Note saved to RAM.", COLOR_GREEN);
    } else if (same_command(command, "sysmon")) {
        print_at(2, 14, "SYSTEM MONITOR", COLOR_CYAN);

        size_t mem_used = memory_used() / 1024;
        size_t mem_avail = memory_available() / 1024;
        size_t procs = process_count();
        uint64_t ticks = kernel_ticks();

        char mem_str[32] = "RAM: 000K USED / 000K AVAIL";
        mem_str[5] = '0' + ((mem_used / 100) % 10);
        mem_str[6] = '0' + ((mem_used / 10) % 10);
        mem_str[7] = '0' + (mem_used % 10);

        mem_str[17] = '0' + ((mem_avail / 100) % 10);
        mem_str[18] = '0' + ((mem_avail / 10) % 10);
        mem_str[19] = '0' + (mem_avail % 10);
        print_at(2, 16, mem_str, COLOR_TEXT);

        char proc_str[32] = "PROCESSES: 00 / 32 ACTIVE";
        proc_str[11] = '0' + ((procs / 10) % 10);
        proc_str[12] = '0' + (procs % 10);
        print_at(2, 17, proc_str, COLOR_TEXT);

        char tick_str[32] = "UPTIME TICKS: 00000";
        uint32_t t32 = (uint32_t)ticks;
        tick_str[14] = '0' + ((t32 / 10000) % 10);
        tick_str[15] = '0' + ((t32 / 1000) % 10);
        tick_str[16] = '0' + ((t32 / 100) % 10);
        tick_str[17] = '0' + ((t32 / 10) % 10);
        tick_str[18] = '0' + (t32 % 10);
        print_at(2, 18, tick_str, COLOR_TEXT);

    } else if (command_len != 0) {
        print_at(2, 14, "Unknown command. Type help.", COLOR_AMBER);
    }
    cursor_y = 22;
    cursor_x = 18;
    for (size_t i = 0; i < command_len; i++) put_cell(cursor_x++, cursor_y, command[i], COLOR_TEXT);
    command_len = 0;
}

static char key_to_ascii(uint8_t scancode) {
    static const char map[128] = {
        0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
        'a','s','d','f','g','h','j','k','l',';','\'', '`', 0, '\\',
        'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
    };
    if (scancode < 128) return map[scancode];
    return 0;
}

void kmain(uint32_t magic, uint32_t multiboot_info_address) {
    graphics_init(multiboot_info_address);
    paging_init();
    kernel_core_init();
    memory_init();
    privacy_init();
    ramfs_init();
    process_init();
    process_create("vault-terminal", 1);
    process_create("privacy-guard", 1);
    process_create("manual-service", 0);
    pci_scan();
    draw_frame();
    if (magic != 0x36d76289) print_at(2, 13, "[WARN] Unexpected boot handoff signature.", COLOR_AMBER);

    while (1) {
        if ((inb(0x64) & 1) == 0) {
            halt_cpu();
            continue;
        }
        uint8_t scancode = inb(0x60);
        uint64_t t = kernel_ticks();
        char ts[16];
        for (int i = 0; i < 15; i++) { ts[14-i] = '0' + (t % 10); t /= 10; }
        ts[15] = 0;
        print_at(65, 23, ts, COLOR_TEXT);
        char hs[8];
        size_t free_kib = memory_available() / 1024;
        hs[0] = '0' + (char)((free_kib / 10) % 10);
        hs[1] = '0' + (char)(free_kib % 10);
        hs[2] = 'K'; hs[3] = 0;
        print_at(70, 23, hs, COLOR_TEXT);

        if (scancode == 0x2A || scancode == 0x36) { shift_down = 1; continue; }
        if (scancode == 0x3A) { caps_lock ^= 1; continue; }
        if (scancode & 0x80) {
            if (scancode == 0xAA || scancode == 0xB6) shift_down = 0;
            continue;
        }
        char key = key_to_ascii(scancode);
        if (key >= 'a' && key <= 'z' && (shift_down ^ caps_lock)) key = (char)(key - 'a' + 'A');
        if (shift_down) {
            if (key == '1') key = '!'; else if (key == '2') key = '@'; else if (key == '3') key = '#';
            else if (key == '4') key = '$'; else if (key == '5') key = '%'; else if (key == '6') key = '^';
            else if (key == '7') key = '&'; else if (key == '8') key = '*'; else if (key == '9') key = '(';
            else if (key == '0') key = ')'; else if (key == '-') key = '_'; else if (key == '=') key = '+';
        }
        if (key == '\n') {
            show_command();
        } else if (key == '\b') {
            if (command_len > 0) {
                command_len--;
                cursor_x--;
                put_cell(cursor_x, cursor_y, ' ', COLOR_BG);
            }
        } else if (key >= 32 && key <= 126 && command_len < sizeof(command) - 1) {
            command[command_len++] = key;
            put_cell(cursor_x++, cursor_y, key, COLOR_TEXT);
        }
    }
}
