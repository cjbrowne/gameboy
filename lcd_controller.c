#include "lcd_controller.h"
#include "gbe.h"

int lcd_step(struct lcd_controller_reg* lcd, unsigned char* vram, unsigned char* sprite_attrib_memory, unsigned char* ioPorts, unsigned char* screen, unsigned short ticks)
{
    lcd->control = ioPorts[0x40];
    lcd->scrollY = ioPorts[0x42];
    lcd->scrollX = ioPorts[0x43];
    lcd->scanline = ioPorts[0x44];

    static int last_ticks = 0;

    lcd->tick = ticks - last_ticks;

    last_ticks = ticks;

    switch(lcd->mode)
    {
        case LCD_MODE_HBLANK:
            if(lcd->tick >= 204) {
                lcd->scanline++;
                if(lcd->scanline == 143) {
                    lcd->mode = LCD_MODE_VBLANK;
                } else {
                    lcd->mode = LCD_MODE_OAM;
                }
                lcd->tick -= 204;
            }
            break;
        case LCD_MODE_VBLANK:
            if(lcd->tick >= 456) {
                lcd->scanline++;
                if(lcd->scanline > 153) {
                    lcd->scanline = 0;
                    lcd->mode = LCD_MODE_OAM;
                }
                lcd->tick -= 456;
            }
            break;
        case LCD_MODE_OAM:
            if(lcd->tick >= 80) {
                lcd->mode = LCD_MODE_VRAM;

                lcd->tick -= 80;
            }
            break;
        case LCD_MODE_VRAM:
            if(lcd->tick >= 172) {
                lcd->mode = LCD_MODE_HBLANK;
                lcd_render_scanline(lcd, vram, screen, sprite_attrib_memory, ioPorts);
                lcd->tick -= 172;
            }
            break;
    }

    ioPorts[0x44] = lcd->scanline;

    gbe_log(GBE_LOG_TRACE, "lcd status:");
    gbe_log(GBE_LOG_TRACE, "\t[FF40] (LCDC) %.2x", ioPorts[0x40]);
    gbe_log(GBE_LOG_TRACE, "\t[FF42] (SCRY) %.2x", ioPorts[0x42]);
    gbe_log(GBE_LOG_TRACE, "\t[FF43] (SCRX) %.2x", ioPorts[0x43]);
    gbe_log(GBE_LOG_TRACE, "\t[FF44] (LY  ) %.2x", ioPorts[0x44]);
    gbe_log(GBE_LOG_TRACE, "\tmode: %.1d", lcd->mode);

    // gbe_log(GBE_LOG_TRACE, "tiles:");
    // for(int i = 0; i < 384; i++) {
    //     for(int y = 0; y < 8; y++) {
    //         char* tile = malloc(8);
    //         for(int x = 0; x < 8; x++) {
    //             sprintf(tile, "%d", lcd->tiles[i][y][x]);
    //         }
    //         if(strncmp(tile, "0", 8)!=0)
    //             gbe_log(GBE_LOG_TRACE, "%.4x: %s", i, tile);
    //     }
    // }

    return 1;
}

void lcd_update_tile(struct lcd_controller_reg* lcd, unsigned char* vram, unsigned short address)
{
    address &= 0x1ffe;
	
	unsigned short tile = (address >> 4) & 511;
	unsigned short y = (address >> 1) & 7;
	
	unsigned char x, bitIndex;
	for(x = 0; x < 8; x++) {
		bitIndex = 1 << (7 - x);
		
		//((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
		lcd->tiles[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
	}
}

// got a lot of this routine from https://github.com/CTurt/Cinoop/blob/bccf14a/source/display.c
void lcd_render_scanline(struct lcd_controller_reg* lcd, unsigned char* vram, unsigned char* screen, unsigned char* sprite_attrib_memory, unsigned char* ioPorts)
{
    // find the scan line in the tile map
    int map_offset = (lcd->control & LCD_CONTROL_TILEMAP) ? 0x1c00 : 0x1800;
    map_offset += (((lcd->scanline + lcd->scrollY) & 255) >> 3) << 5;
    
    int line_offset = (lcd->scrollX >> 3);
    int x = lcd->scrollX & 7;
    int y = (lcd->scrollY + lcd->scanline) & 7;
    int pixel_offset = lcd->scanline * 160;

    unsigned short tile = (unsigned short)vram[map_offset + line_offset];
    if((lcd->control & LCD_CONTROL_TILE_DATA) && tile < 128) tile += 256;
    
    unsigned char scanline_row[160] = {0};

    for(int i = 0; i < 160; i++)
    {
        unsigned char colour = lcd->tiles[tile][y][x];
        unsigned char bgPal = ioPorts[0x47];

        scanline_row[i] = colour;

        // gbe_log(GBE_LOG_TRACE, "Setting pixel %d to %.2x", pixel_offset, colour);
        screen[pixel_offset] = ((bgPal >> (colour * 2))&0xF) | 0x80; // bit 7 means this pixel is "dirty" and needs rendering
        
        pixel_offset++;
        if(x == 7)
        {
            x = 0;
            line_offset = (line_offset + 1) & 31;
            tile = vram[map_offset + line_offset];
            if((lcd->control & LCD_CONTROL_TILE_DATA) && tile < 128) tile += 256;
        }
    }

    for(int i = 0; i < 40; i++) {
		struct sprite sprite = ((struct sprite *)sprite_attrib_memory)[i];
		
		int sx = sprite.x - 8;
		int sy = sprite.y - 16;
		
		if(sy <= lcd->scanline && (sy + 8) > lcd->scanline) {
			unsigned char pal[3];

            memcpy(pal, ioPorts + (sprite.opts.palette ? 0x48 : 0x49), 3);

			int pixel_offset = lcd->scanline * 160 + sx;
			
			
			unsigned char tileRow;
			if(sprite.opts.vFlip) tileRow = 7 - (lcd->scanline - sy);
			else tileRow = lcd->scanline - sy;
			
			int x;
			for(x = 0; x < 8; x++) {
				if(sx + x >= 0 && sx + x < 160 && (~sprite.opts.priority || !scanline_row[sx + x])) {
					unsigned char colour;
					
					if(sprite.opts.hFlip) colour = lcd->tiles[sprite.tile][tileRow][7 - x];
					else colour = lcd->tiles[sprite.tile][tileRow][x];
					
					if(colour) {
                        screen[pixel_offset] = pal[colour] | 0x80;
					}
                    pixel_offset++;
				}
			}
		}
    }
}