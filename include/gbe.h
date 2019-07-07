#ifndef GBE_H
#define GBE_H

#include <SDL2/SDL.h>

int gbe_run(SDL_Renderer* ren, unsigned char* rom);

union reg {
    unsigned short full;
    struct halfstr {
        unsigned char l;
        unsigned char h;
    } half;
};

#define FLAG_ZERO 0x80
#define FLAG_SUBTRACT 0x40
#define FLAG_HALF_CARRY 0x20
#define FLAG_CARRY 0x10

#define ROM_BANK0_OFFSET 0x0000
#define ROM_BANK_SW_OFFSET 0x4000
#define VRAM_OFFSET 0x8000
#define RAM_BANK_SW_OFFSET 0xA000
#define RAM0_OFFSET 0xC000
#define ECHO_RAM_OFFSET 0xE000
#define OAM_OFFSET 0xFE00
#define IO_OFFSET 0xFF00
#define RAM1_OFFSET 0xFF80
#define INTERRUPT_ENABLE_ADDR 0xFFFF

struct gbe_cpu
{
    unsigned short int pc;
    unsigned short int sp;
    unsigned char screen_dirty;
    struct register_str {
        union reg af;
        union reg bc;
        union reg de;
        union reg hl;
    } registers;
    union au {
        unsigned char contiguous[65536];
        struct ps {
            unsigned char rom_bank0[16384];
            unsigned char switchable_rom_bank[16384];
            unsigned char vram[8196];
            unsigned char switchable_ram_bank[8196];
            unsigned char internal_ram0[8196];
            unsigned char echo_internal_ram[7680];
            unsigned char sprite_attrib_memory[160];
            unsigned char empty_but_unusable_for_io0[96];
            unsigned char io_ports[76];
            unsigned char empty_but_unusable_for_io1[52];
            unsigned char internal_ram1[127];
            unsigned char interrupt_enable_register;
        } paged;
    } addr;
    struct lcd_controller_reg lcd_controller;
};

enum gbe_log_level
{
    GBE_LOG_TRACE = 0,
    GBE_LOG_DEBUG = 1,
    GBE_LOG_INFO = 2,
    GBE_LOG_WARNING = 3,
    GBE_LOG_ERROR = 4
};

extern const char* gbe_log_levelstr[];
extern const unsigned char gbe_bootstrap_rom[256];
extern const unsigned char nintendo_graphic[48];

enum gbe_color_components {
    RED = 0,
    GREEN = 1,
    BLUE = 2
};

struct gbe_options {
    const char* rom_filename;
    unsigned int rom_size;
};

#define gbe_screen_width 160
#define gbe_screen_height 144
extern const int gbe_pixel_count;

extern unsigned int gbe_log_min_level;

void gbe_log(enum gbe_log_level lvl, char* format, ...);
void gbe_step(struct gbe_cpu* cpu, unsigned char* screen_buf, int* stepmode);
void gbe_dump(struct gbe_cpu cpu);
void write_byte(struct gbe_cpu* cpu, unsigned short addr, unsigned char byte);

#endif // GBE_H