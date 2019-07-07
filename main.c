#include <stdio.h>
#include <SDL2/SDL.h>
#include <getopt.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "lcd_controller.h"
#include "gbe.h"

int setup_window(struct gbe_options config);
int create_renderer(SDL_Window* win, struct gbe_options config);

void load_rom(unsigned char** rombuf, const char* filename)
{
    int fd = open(filename, O_RDWR);
    int rom_size = 0;
    struct stat sb;
    fstat(fd, &sb);
    rom_size = sb.st_size;
    gbe_log(GBE_LOG_DEBUG, "Loading %d-byte ROM", rom_size);
    if(fd != -1) {
        *rombuf = mmap(NULL, rom_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        if(*rombuf == NULL) {
            gbe_log(GBE_LOG_ERROR, "Could not load rom file into memory");
        } else if (*rombuf == MAP_FAILED) {
            gbe_log(GBE_LOG_ERROR, "Could not map rom file: %s", strerror(errno));
        }
    } else {
        gbe_log(GBE_LOG_ERROR, "Could not open rom file %s for read/write", filename);
    }
    close(fd);
}

int create_renderer(SDL_Window* win, struct gbe_options config)
{
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    int ret = 0;
    unsigned char* rom = NULL;
    if(ren != NULL) {
        load_rom(&rom, config.rom_filename);
        gbe_run(ren, rom);
    } else {
        printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
        ret = -1;
    }
    return ret;
}

int setup_window(struct gbe_options config)
{
    SDL_Window* win = SDL_CreateWindow("Gameboy Emulator", 100, 100, 320, 288, SDL_WINDOW_SHOWN);
    int ret = 0;
    if(win != NULL) {
        ret = create_renderer(win, config);
        SDL_DestroyWindow(win);
    } else {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        ret = -1;
    }
    return ret;
}

void print_usage(char* progname)
{
    printf("Usage: %s -r romfile\n", progname);
}

int main(int argc, char** argv)
{
    int ret = 0, c = 0, optind = 0;
    char* rom_filename = malloc(256);
    struct gbe_options config;
    int romfile_set = 0;

    config.rom_filename = rom_filename;

    static struct option long_options[] = {
        {"rom", required_argument, 0, 'r'},
        {"log", required_argument, 0, 'l'},
        {0, 0, 0, 0}
    };

    while((c = getopt_long(argc, argv, "r:l:", long_options, &optind)) != -1)
    {
        switch(c)
        {
            case 0:
                if(strncmp(long_options[optind].name, "rom", 3) == 0)
                {
                    strncpy(rom_filename, optarg, 256);
                    rom_filename[255] = 0;
                    romfile_set = 1;
                } else if (strncmp(long_options[optind].name, "log", 3) == 0)
                {
                    gbe_log_min_level = atoi(optarg);
                    gbe_log_min_level %= 5;
                }
                break;
            case 'r':
                strncpy(rom_filename, optarg, 256);
                rom_filename[255] = 0;
                romfile_set = 1;
                break;
            case 'l':
                gbe_log_min_level = atoi(optarg);
                gbe_log_min_level %= 5;
                break;
        }
    }

    if(!romfile_set)
    {
        gbe_log(GBE_LOG_ERROR, "You must specify a rom file to load.");
        print_usage(argv[0]);
    } 
    else if(SDL_Init(SDL_INIT_VIDEO) == 0) 
    {
        ret = setup_window(config);
        SDL_Quit();
    } 
    else 
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        ret = -1;
    }

    free(rom_filename);

    return ret;
}