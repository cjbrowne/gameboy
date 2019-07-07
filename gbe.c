#include <time.h>
#include "lcd_controller.h"
#include "gbe.h"
#include "termcolors.h"

const unsigned char gbe_bootstrap_rom[256] = {
  0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb,
  0x21, 0x26, 0xff, 0x0e, 0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3,
  0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0, 0x47, 0x11, 0x04, 0x01,
  0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b,
  0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22,
  0x23, 0x05, 0x20, 0xf9, 0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99,
  0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20, 0xf9, 0x2e, 0x0f, 0x18,
  0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04,
  0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20,
  0xf7, 0x1d, 0x20, 0xf2, 0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62,
  0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06, 0x7b, 0xe2, 0x0c, 0x3e,
  0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20,
  0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17,
  0xc1, 0xcb, 0x11, 0x17, 0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9,
  0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83,
  0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
  0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63,
  0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e,
  0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c, 0x21, 0x04, 0x01, 0x11,
  0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
  0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe,
  0x3e, 0x01, 0xe0, 0x50
};

const unsigned char ioReset[0x100] = {
	0x0F, 0x00, 0x7C, 0xFF, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
	0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00, 0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF,
	0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xC1, 0x00, 0xFE, 0xFF, 0xFF, 0xFF,
	0xF8, 0xFF, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
	0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
	0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
	0x45, 0xEC, 0x52, 0xFA, 0x08, 0xB7, 0x07, 0x5D, 0x01, 0xFD, 0xC0, 0xFF, 0x08, 0xFC, 0x00, 0xE5,
	0x0B, 0xF8, 0xC2, 0xCE, 0xF4, 0xF9, 0x0F, 0x7F, 0x45, 0x6D, 0x3D, 0xFE, 0x46, 0x97, 0x33, 0x5E,
	0x08, 0xEF, 0xF1, 0xFF, 0x86, 0x83, 0x24, 0x74, 0x12, 0xFC, 0x00, 0x9F, 0xB4, 0xB7, 0x06, 0xD5,
	0xD0, 0x7A, 0x00, 0x9E, 0x04, 0x5F, 0x41, 0x2F, 0x1D, 0x77, 0x36, 0x75, 0x81, 0xAA, 0x70, 0x3A,
	0x98, 0xD1, 0x71, 0x02, 0x4D, 0x01, 0xC1, 0xFF, 0x0D, 0x00, 0xD3, 0x05, 0xF9, 0x00, 0x0B, 0x00
};

const unsigned char nintendo_graphic[48] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
};

const char* gbe_log_levelstr[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"
};

const int gbe_pixel_count = gbe_screen_height * gbe_screen_width;

unsigned int gbe_log_min_level = 0;

#define test_bit(i, b) (i & (1 << b))

void handle_event(SDL_Event e, int* running, int* stepmode, struct gbe_cpu* cpu, unsigned char* screen_buf)
{
    if(e.type == SDL_QUIT) {
        *running = 0;
    } else if (e.type == SDL_KEYUP) {
        switch(e.key.keysym.sym)
        {
            case SDLK_ESCAPE:
            {
                *running = 0;
                break;
            }
            case SDLK_s:
            {
                *stepmode = !*stepmode;
                gbe_log(GBE_LOG_INFO, "Stepmode %s", *stepmode ? "enabled" : "disabled");
                break;
            }
            case SDLK_q:
            {
                gbe_log(GBE_LOG_TRACE, "Stack trace:");
                unsigned short sp = cpu->sp;
                while(sp < 0xFFFF) {
                    gbe_log(GBE_LOG_TRACE, "%.4x: %.2x", sp, cpu->addr.contiguous[sp]);
                    sp++;
                }
                gbe_log(GBE_LOG_TRACE, "%.4x: %.2x", sp, cpu->addr.contiguous[sp]);
                break;
            }
            case SDLK_SPACE:
            {
                if(stepmode)
                {
                    gbe_step(cpu, screen_buf, stepmode);
                }
                break;
            }
            case SDLK_d:
            {
                gbe_dump(*cpu);
                break;
            }
            case SDLK_l:
            {
                gbe_log(GBE_LOG_INFO, "Press any number from 0-4 to set log level:");
                SDL_Event e2;
                SDL_WaitEvent(&e2);
                while(e2.type != SDL_KEYUP) SDL_WaitEvent(&e2);
                switch(e2.key.keysym.sym) {
                    case SDLK_0:
                        gbe_log_min_level = 0;
                        break;
                    case SDLK_1:
                        gbe_log_min_level = 1;
                        break;
                    case SDLK_2:
                        gbe_log_min_level = 2;
                        break;
                    case SDLK_3:
                        gbe_log_min_level = 3;
                        break;
                    case SDLK_4:
                        gbe_log_min_level = 4;
                        break;
                }
                gbe_log(GBE_LOG_INFO, "Set log level to %d", gbe_log_min_level);
                break;
            }
            default:
                break;
        }
    }
}

void gbe_dump(struct gbe_cpu cpu)
{
    FILE* f = fopen("core.dmp", "w+");
    gbe_log(GBE_LOG_INFO, "Dumping core into core.dmp");
    fwrite(cpu.addr.contiguous, 1, 65536, f);
    gbe_log(GBE_LOG_INFO, "Core dump complete");
    fclose(f);
}

void handle_input(int* running, int* stepmode, struct gbe_cpu* cpu, unsigned char* screen_buf)
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        handle_event(e, running, stepmode, cpu, screen_buf);
    }
}

#define TIMESTAMP_LEN 20

void get_timestamp(char* result)
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime(result, TIMESTAMP_LEN, "%F %T", timeinfo);
}

void gbe_log(enum gbe_log_level lvl, char* format, ...)
{
    if(lvl >= gbe_log_min_level) {
        va_list argp;
        // trim messages > 80 chars
        const int msglen = 80;
        char* logmsg = malloc(msglen);
        char* timestamp = malloc(TIMESTAMP_LEN);
        const char* col;
        switch (lvl)
        {
            case GBE_LOG_ERROR:
                col = KRED;
                break;
            case GBE_LOG_WARNING:
                col = KYEL;
                break;
            case GBE_LOG_INFO:
                col = KGRN;
                break;
            case GBE_LOG_DEBUG:
                col = KCYN;
                break;
            case GBE_LOG_TRACE:
                col = KMAG;
                break;
        }

        va_start(argp, format);
        
        vsnprintf(logmsg, msglen, format, argp);

        get_timestamp(timestamp);
    
        printf("[%s] %s%s"KNRM" %s\n", timestamp, col, gbe_log_levelstr[lvl], logmsg);
        va_end(argp);
        free(logmsg);
        free(timestamp);
    }
}

void set_pixel(unsigned char* screen_buf, unsigned int x, unsigned int y, unsigned char val)
{
    if(x < gbe_screen_width && y < gbe_screen_height)
    {
        // only set the lowest 2 bits, because otherwise we will have junk in the screen buffer that won't be displayed
        screen_buf[(y*gbe_screen_width)+x] = (val ^ 0x3) | 0x80;
    }
    else
    {
        gbe_log(GBE_LOG_WARNING, "pixel out of bounds: (%u,%u)", x, y);
    }
    
}

// 0f380f
// 306230
// 8bac0f
// 9bbc0f
static unsigned char get_color_component(enum gbe_color_components col, unsigned char pixel)
{
    static unsigned char cols[][3] = {
        {0x0f, 0x38, 0x0f},
        {0x30, 0x62, 0x30},
        {0x8b, 0xac, 0x0f},
        {0x9b, 0xbc, 0x0f}
    };

    return cols[pixel & 0x3][col];
}


void gbe_update_screen(SDL_Renderer* ren, unsigned char* screen_buf)
{
    int i = 0;
    SDL_Rect r;
    while(i < gbe_pixel_count)
    {
        // set bit 7 to 1 to indicate that this pixel is "dirty" and needs
        // to be redrawn
        if((screen_buf[i] & 0x80) == 0) {
            i++;
            continue;
        }
        r.h = 2;
        r.w = 2;
        r.x = (i % gbe_screen_width) * 2;
        r.y = (i / gbe_screen_width) * 2;
        SDL_SetRenderDrawColor(ren, 
            get_color_component(RED, screen_buf[i]),
            get_color_component(GREEN, screen_buf[i]),
            get_color_component(BLUE, screen_buf[i]),
            0xFF
            );
        SDL_RenderFillRect(ren, &r);
        screen_buf[i] &= ~0x80;
        i++;
    }
}

long diff_in_ns(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) * (long)1e9 + (end.tv_nsec - start.tv_nsec);
}

#define imm16(x) (x->addr.contiguous[x->pc + 1] | (x->addr.contiguous[x->pc + 2] << 8))
#define imm8(x) (x->addr.contiguous[x->pc + 1])

#define SET_FLAGS(x) (cpu->registers.af.half.l |= (x))
#define CLEAR_FLAGS(x) (cpu->registers.af.half.l &= ~(x))
#define FLAGS_ISSET(x) (cpu->registers.af.half.l & (x))

void gbe_step(struct gbe_cpu* cpu, unsigned char* screen_buf, int* stepmode)
{
    unsigned char instr = cpu->addr.contiguous[cpu->pc];
    unsigned short ticks = 0;

    gbe_log(GBE_LOG_TRACE, "executing instruction %.2x", instr);

    switch(instr)
    {
        case 0x00:
            cpu->pc++;
            ticks = 4;
            break;
        case 0x04:
            if((cpu->registers.bc.half.h & 0x0f) == 0x0f) SET_FLAGS(FLAG_HALF_CARRY);
            cpu->registers.bc.half.h++;
            CLEAR_FLAGS(FLAG_SUBTRACT);

            if(cpu->registers.bc.half.h == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0x05:
            if(cpu->registers.bc.half.h & 0xF) CLEAR_FLAGS(FLAG_HALF_CARRY);
            else SET_FLAGS(FLAG_HALF_CARRY);
            cpu->registers.bc.half.h--;
            SET_FLAGS(FLAG_SUBTRACT);
            if(cpu->registers.bc.half.h == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0x06:
            cpu->registers.bc.half.h = imm8(cpu);
            cpu->pc += 2;
            ticks = 8;
            break;
        case 0x0c:
            if((cpu->registers.bc.half.l & 0x0f) == 0x0f) SET_FLAGS(FLAG_HALF_CARRY);
            cpu->registers.bc.half.l++;
            CLEAR_FLAGS(FLAG_SUBTRACT);

            if(cpu->registers.bc.half.l == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0x0d:
            SET_FLAGS(FLAG_SUBTRACT);
            if(cpu->registers.bc.half.l & 0xF) {
                CLEAR_FLAGS(FLAG_HALF_CARRY);
            } else {
                SET_FLAGS(FLAG_HALF_CARRY);
            }
            cpu->registers.bc.half.l--;
            if(cpu->registers.bc.half.l == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0x0e:
            cpu->registers.bc.half.l = imm8(cpu);
            cpu->pc += 2;
            ticks = 8;
            break;
        case 0x11:
            cpu->registers.de.full = imm16(cpu);
            cpu->pc += 3;
            ticks = 12;
            break;
        case 0x13:
            cpu->registers.de.full++;
            cpu->pc++;
            ticks = 4;
            break;
        case 0x15:
            if(cpu->registers.de.half.h & 0xF) CLEAR_FLAGS(FLAG_HALF_CARRY);
            else SET_FLAGS(FLAG_HALF_CARRY);
            cpu->registers.de.half.h--;
            SET_FLAGS(FLAG_SUBTRACT);
            if(cpu->registers.de.half.h == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0x17:
        {
            unsigned char a = cpu->registers.af.half.h;
            unsigned char c = (cpu->registers.af.half.l & FLAG_CARRY) ? 1 : 0;

            if(a & 0x80) cpu->registers.af.half.l |= FLAG_CARRY;
            else cpu->registers.af.half.l &= ~FLAG_CARRY;

            a <<= 1;
            a += c;

            cpu->registers.af.half.h = a;

            if(cpu->registers.af.half.h == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }

            CLEAR_FLAGS(FLAG_SUBTRACT | FLAG_HALF_CARRY);

            cpu->pc++;
            ticks = 4;
            break;
        }
        case 0x18:
            cpu->pc += (signed char)imm8(cpu) + 2;
            ticks = 12;
            break;
        case 0x1A:
            cpu->registers.af.half.h = cpu->addr.contiguous[cpu->registers.de.full];
            cpu->pc++;
            ticks = 8;
            break;
        case 0x1D:
            SET_FLAGS(FLAG_SUBTRACT);
            if(cpu->registers.de.half.l & 0xF) {
                CLEAR_FLAGS(FLAG_HALF_CARRY);
            } else {
                SET_FLAGS(FLAG_HALF_CARRY);
            }
            cpu->registers.de.half.l--;
            if(cpu->registers.de.half.l == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0x1E:
            cpu->registers.de.half.l = imm8(cpu);
            cpu->pc += 2;
            ticks = 8;
            break;
        case 0x20:
            if(cpu->registers.af.half.l & FLAG_ZERO) {
                ticks = 8;
            } else {
                cpu->pc += (signed char)imm8(cpu);
                ticks = 12;
            }
            cpu->pc += 2;
            break;
        case 0x21:
            cpu->registers.hl.full = imm16(cpu);
            cpu->pc += 3;
            ticks = 12;
            break;
        case 0x22:
            write_byte(cpu, cpu->registers.hl.full, cpu->registers.af.half.h);
            cpu->registers.hl.full++;
            cpu->pc++;
            ticks = 8;
            break;
        case 0x23:
            cpu->registers.hl.full++;
            ticks = 8;
            cpu->pc++;
            break;
        case 0x24:
            if((cpu->registers.hl.half.h & 0x0f) == 0x0f) SET_FLAGS(FLAG_HALF_CARRY);
            cpu->registers.hl.half.h++;
            CLEAR_FLAGS(FLAG_SUBTRACT);

            if(cpu->registers.hl.half.h == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0x28:
        {
            unsigned char operand = imm8(cpu);
            if(cpu->registers.af.half.l & FLAG_ZERO) {
                cpu->pc += (signed char) operand;
            }
            cpu->pc += 2;
            ticks = 8;
            break;
        }
        case 0x2E:
            cpu->registers.de.half.l = imm8(cpu);
            cpu->pc += 2;
            ticks = 8;
            break;
        case 0x31:
            cpu->sp = imm16(cpu);
            cpu->pc += 3;
            ticks = 12;
            break;
        case 0x32:
            write_byte(cpu, cpu->registers.hl.full, cpu->registers.af.half.h);
            cpu->registers.hl.full--;
            cpu->pc++;
            ticks = 8;
            break;
        case 0x3d:
            SET_FLAGS(FLAG_SUBTRACT);
            if(cpu->registers.af.half.h & 0xF) {
                CLEAR_FLAGS(FLAG_HALF_CARRY);
            } else {
                SET_FLAGS(FLAG_HALF_CARRY);
            }
            cpu->registers.af.half.h--;
            if(cpu->registers.af.half.h == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0x3e:
            cpu->registers.af.half.h = imm8(cpu);
            cpu->pc += 2;
            ticks = 8;
            break;
        case 0x4F:
            cpu->registers.bc.half.l = cpu->registers.af.half.h;
            cpu->pc++;
            ticks = 4;
            break;
        case 0x57:
            cpu->registers.de.half.h = cpu->registers.af.half.h;
            cpu->pc++;
            ticks = 4;
            break;
        case 0x67:
            cpu->registers.hl.half.h = cpu->registers.af.half.h;
            cpu->pc++;
            ticks = 4;
            break;
        case 0x77:
            write_byte(cpu, cpu->registers.hl.full, cpu->registers.af.half.h);
            cpu->pc++;
            ticks = 8;
            break;
        case 0x7B:
            cpu->registers.af.half.h = cpu->registers.de.half.l;
            cpu->pc++;
            ticks = 4;
            break;
        case 0x7C: // LD A, H
            cpu->registers.af.half.h = cpu->registers.hl.half.h;
            cpu->pc++;
            ticks = 4;
            break;
        case 0x90:
            SET_FLAGS(FLAG_SUBTRACT);
            if(cpu->registers.af.half.h & 0xF) {
                SET_FLAGS(FLAG_HALF_CARRY);
            } else {
                CLEAR_FLAGS(FLAG_HALF_CARRY);
            }
            cpu->registers.af.half.h -= cpu->registers.bc.half.h;
            if(cpu->registers.af.half.h == 0) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            cpu->pc++;
            ticks = 4;
            break;
        case 0xAF:
            cpu->registers.af.full = 0x0000;
            SET_FLAGS(FLAG_ZERO);
            CLEAR_FLAGS(FLAG_CARRY | FLAG_HALF_CARRY | FLAG_SUBTRACT);
            cpu->pc++;
            ticks = 4;
            break;
        case 0xC1:
            cpu->sp++;
            cpu->registers.bc.half.l = cpu->addr.contiguous[cpu->sp];
            cpu->sp++;
            cpu->registers.bc.half.h = cpu->addr.contiguous[cpu->sp];
            cpu->pc++;
            ticks = 12;
            break;
        case 0xC3:
            cpu->pc = imm16(cpu);
            ticks = 16;
            break;
        case 0xC5:
            cpu->addr.contiguous[cpu->sp] = cpu->registers.bc.half.h;
            cpu->sp--;
            cpu->addr.contiguous[cpu->sp] = cpu->registers.bc.half.l;
            cpu->sp--;
            cpu->pc++;
            ticks = 16;
            break;
        case 0xC9:
            ticks = 16;
            cpu->pc++;
            cpu->sp++;
            unsigned short return_addr = cpu->addr.contiguous[cpu->sp];
            cpu->sp++;
            return_addr += (cpu->addr.contiguous[cpu->sp] << 8);
            cpu->pc = return_addr;
            break;
        case 0xCB:
            gbe_log(GBE_LOG_TRACE, "Extended instruction: %.2x", cpu->addr.contiguous[cpu->pc + 1]);
            switch(cpu->addr.contiguous[cpu->pc + 1])
            {
                case 0x11:
                {
                    unsigned char value = cpu->registers.bc.half.l;
                    int carry = FLAGS_ISSET(FLAG_CARRY) ? 1 : 0;
	
                    if(value & 0x80) SET_FLAGS(FLAG_CARRY);
                    else CLEAR_FLAGS(FLAG_CARRY);
                    
                    value <<= 1;
                    value += carry;
                    
                    if(value) CLEAR_FLAGS(FLAG_ZERO);
                    else SET_FLAGS(FLAG_ZERO);
                    
                    CLEAR_FLAGS(FLAG_SUBTRACT | FLAG_HALF_CARRY);
                    
                    cpu->registers.bc.half.l = value;
                    break;
                }
                case 0x7C:
                    SET_FLAGS(FLAG_HALF_CARRY);
                    CLEAR_FLAGS(FLAG_SUBTRACT);
                    if(test_bit(cpu->registers.hl.half.h, 6)) {
                        SET_FLAGS(FLAG_ZERO);
                    } else {
                        CLEAR_FLAGS(FLAG_ZERO);
                    }
                    break;
                default:
                    gbe_log(GBE_LOG_WARNING, "Unknown extended instruction: %.2x", cpu->addr.contiguous[cpu->pc + 1]);
                    break;
            }
            cpu->pc += 2;
            ticks = 4;
            if((cpu->addr.contiguous[cpu->pc + 1] & 0x0F) == 0x6 ||
                (cpu->addr.contiguous[cpu->pc + 1] & 0x0F) == 0xE) {
                    ticks += 16;
                } else {
                    ticks += 8;
                }
            break;
        case 0xCD:
            cpu->addr.contiguous[cpu->sp] = (unsigned char)((cpu->pc + 3) >> 8);
            cpu->sp--;
            cpu->addr.contiguous[cpu->sp] = (unsigned char)(cpu->pc + 3);
            cpu->sp--;
            cpu->pc = imm16(cpu);
            ticks = 24;
            break;
        case 0xE0:
            write_byte(cpu, 0xFF00 + imm8(cpu), cpu->registers.af.half.h);
            cpu->pc += 2;
            ticks = 12;
            break;
        case 0xE2:
            write_byte(cpu, 0xFF00 + cpu->registers.bc.half.l, cpu->registers.af.half.h);
            cpu->pc++;
            ticks = 8;
            break;
        case 0xEA:
            write_byte(cpu, imm16(cpu), cpu->registers.af.half.h);
            cpu->pc += 3;
            ticks = 16;
            break;
        case 0xF0:
            cpu->registers.af.half.h = cpu->addr.contiguous[0xFF00 + imm8(cpu)];
            cpu->pc += 2;
            ticks = 12;
            break;
        case 0xFE:
        {
            SET_FLAGS(FLAG_SUBTRACT);
            if(cpu->registers.af.half.h == imm8(cpu)) {
                SET_FLAGS(FLAG_ZERO);
            } else {
                CLEAR_FLAGS(FLAG_ZERO);
            }
            if(imm8(cpu) > cpu->registers.af.half.h) {
                SET_FLAGS(FLAG_CARRY);
            } else {
                CLEAR_FLAGS(FLAG_CARRY);
            }
            if((imm8(cpu) & 0x0f) > cpu->registers.af.half.h) {
                SET_FLAGS(FLAG_HALF_CARRY);
            } else {
                CLEAR_FLAGS(FLAG_HALF_CARRY);
            }
            cpu->pc += 2;
            ticks = 8;
            break;
        }
        default:
            *stepmode = 1;
            gbe_log(GBE_LOG_WARNING, "Unknown instruction: %.2x", instr);
            break;
    }

    cpu->screen_dirty = lcd_step(&cpu->lcd_controller, cpu->addr.contiguous+VRAM_OFFSET, cpu->addr.contiguous+OAM_OFFSET, cpu->addr.contiguous+IO_OFFSET, screen_buf, ticks);

    gbe_log(GBE_LOG_TRACE, "new cpu state:");
    gbe_log(GBE_LOG_TRACE, "\taf: %.2x %.2x", cpu->registers.af.half.h, cpu->registers.af.half.l);
    gbe_log(GBE_LOG_TRACE, "\tbc: %.2x %.2x", cpu->registers.bc.half.h, cpu->registers.bc.half.l);
    gbe_log(GBE_LOG_TRACE, "\tde: %.2x %.2x", cpu->registers.de.half.h, cpu->registers.de.half.l);
    gbe_log(GBE_LOG_TRACE, "\thl: %.2x %.2x", cpu->registers.hl.half.h, cpu->registers.hl.half.l);
    gbe_log(GBE_LOG_TRACE, "\tpc: %.4x", cpu->pc);
    gbe_log(GBE_LOG_TRACE, "\tsp: %.4x", cpu->sp);
    gbe_log(GBE_LOG_TRACE, "\tflags: Z=%d, N=%d, H=%d, C=%d",
        (cpu->registers.af.half.l & FLAG_ZERO) >> 7,
        (cpu->registers.af.half.l & FLAG_SUBTRACT) >> 6,
        (cpu->registers.af.half.l & FLAG_HALF_CARRY) >> 5,
        (cpu->registers.af.half.l & FLAG_CARRY) >> 4
    );
    gbe_log(GBE_LOG_TRACE, "\tnext instruction: %.2x", cpu->addr.contiguous[cpu->pc]);
    
}

void write_byte(struct gbe_cpu* cpu, unsigned short addr, unsigned char byte)
{
    cpu->addr.contiguous[addr] = byte;
    if(addr >= 0x8000 && addr <= 0x97ff) {
        lcd_update_tile(&cpu->lcd_controller, cpu->addr.contiguous+VRAM_OFFSET, addr);
    }
}

void reset_cpu(struct gbe_cpu* cpu)
{
    cpu->screen_dirty = 1;
    
    cpu->registers.af.full = 0x0000;
    cpu->registers.bc.full = 0x0000;
    cpu->registers.de.full = 0x0000;
    cpu->registers.hl.full = 0x0000;
    cpu->sp = 0x0000;
    cpu->pc = 0x0000;

    memset(cpu->addr.contiguous, 0, 0x10000);
    //memcpy(cpu->addr.contiguous + IO_OFFSET, ioReset, 256);

    memset(cpu->lcd_controller.tiles, 0, sizeof(cpu->lcd_controller.tiles));

    cpu->lcd_controller.mode = 0;
    cpu->lcd_controller.tick = 0;
    cpu->lcd_controller.scrollX = 0;
    cpu->lcd_controller.scrollY = 0;
    cpu->lcd_controller.control = 0;

}

int valid_rom(unsigned char* rom)
{
    gbe_log(GBE_LOG_INFO, "Validating ROM file...");
    if(memcmp(rom + 0x0104, &nintendo_graphic, 48) == 0) {
        gbe_log(GBE_LOG_INFO, "ROM Valid");
        return 1;
    } else {
        gbe_log(GBE_LOG_ERROR, "Invalid ROM file");
        return 0;
    }
}

void insert_rom(struct gbe_cpu* cpu, unsigned char* rom)
{
    memcpy(cpu->addr.contiguous+ROM_BANK0_OFFSET, rom, 8196);
    memcpy(cpu->addr.contiguous+ROM_BANK_SW_OFFSET, rom+8196, 8196);
}

int gbe_run(SDL_Renderer* ren, unsigned char* rom)
{
    unsigned short breakpoint = 0x0000;
    int running = 1;
    unsigned char* screen_buf = malloc(gbe_pixel_count);
    struct gbe_cpu cpu;
    int stepmode = 1;
    long last_frame = 0;

    struct timespec start_time, end_time;

    reset_cpu(&cpu);
    insert_rom(&cpu, rom);
    memcpy(cpu.addr.contiguous, gbe_bootstrap_rom, 256);

    memset(screen_buf, 0x80, gbe_pixel_count);

    gbe_log(GBE_LOG_INFO, "Started GBE");

    if(!valid_rom(rom))
    {
        running = 0;
    }

    while(running)
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
        

        if(!stepmode)
        {
            gbe_step(&cpu, screen_buf, &stepmode);
        }

        if(cpu.pc == breakpoint) stepmode = 1;

        if(cpu.lcd_controller.mode == 0 && cpu.screen_dirty && (((start_time.tv_sec * 1e9) + start_time.tv_nsec) - last_frame) > 1.6e7)
        {
            SDL_RenderClear(ren);
            gbe_update_screen(ren, screen_buf);
            cpu.screen_dirty = 0;
            SDL_RenderPresent(ren);
            last_frame = (start_time.tv_sec * 1e9) + start_time.tv_nsec;
        }

        handle_input(&running, &stepmode, &cpu, screen_buf);

        
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);

        // gbe_log(GBE_LOG_DEBUG, "Loop run took %ld nanoseconds to complete", diff_in_ns(start_time, end_time));

    }

    gbe_log(GBE_LOG_INFO, "Exiting GBE");
    free(screen_buf);

    return 0;
}