#ifndef LCD_CONTROLLER_H
#define LCD_CONTROLLER_H

enum lcd_modes {
    LCD_MODE_HBLANK = 0,
    LCD_MODE_VBLANK = 1,
    LCD_MODE_OAM = 2,
    LCD_MODE_VRAM = 3
};

enum lcd_ctrl_bits {
    LCD_CONTROL_BG_WIND_DISP = 0x01,
    LCD_CONTROL_OBJ_DISP = 0x02,
    LCD_CONTROL_OBJ_SIZE = 0x04,
    LCD_CONTROL_TILEMAP = 0x08,
    LCD_CONTROL_TILE_DATA = 0x10,
    LCD_CONTROL_WIND_DISP = 0x20,
    LCD_CONTROL_WIND_TILEMAP = 0x40,
    LCD_CONTROL_OPERATION = 0x80
};

struct lcd_controller_reg
{
    unsigned char control;
    unsigned char scrollY;
    unsigned char scrollX;
    unsigned char scanline;
    unsigned short tick;

    unsigned char tiles[384][8][8];

    enum lcd_modes mode;
};

struct sprite
{
    unsigned char y;
    unsigned char x;
    unsigned char tile;
    struct options {
            unsigned char priority : 1;
            unsigned char vFlip : 1;
            unsigned char hFlip : 1;
            unsigned char palette : 1;
    } opts;
};

int lcd_step(struct lcd_controller_reg* lcd, unsigned char* vram, unsigned char* sprite_attrib_memory, unsigned char* ioPorts, unsigned char* screen, unsigned short ticks);
void lcd_render_scanline(struct lcd_controller_reg* lcd, unsigned char* vram, unsigned char* screen, unsigned char* sprite_attrib_memory, unsigned char* ioPorts);
void lcd_update_tile(struct lcd_controller_reg* lcd, unsigned char* vram, unsigned short address);

#endif // LCD_CONTROLLER_H