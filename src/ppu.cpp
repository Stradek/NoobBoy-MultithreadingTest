#include <iostream>

#include "ppu.h"

PPU::PPU(Registers *registers, Interrupts* interrupts, MMU* mmu){
    this->registers = registers;
    this->mmu = mmu;
    this->interrupts = interrupts; 
    // TODO: Rewrite this to use mmu->read_byte(0xff40)
    control = (Control*)&mmu->memory[0xff40];
    scrollY = &mmu->memory[0xff42];
    scrollX = &mmu->memory[0xff43];
    scanline = &mmu->memory[0xff44];
}

void PPU::step(){
    modeclock += mmu->clock.t_instr;
    if(!this->control){
        return;
    }
    if (!control->lcdEnable) {return;}
    switch (mode) {
        case 0: // HBLANK
            if(modeclock >= 204){
                modeclock -= 204;
                mode = 2;
                *this->scanline+=1;

                mmu->write_byte(0xff41, mmu->read_byte(0xff41) & ~(1));
                compare_ly_lyc();
                // if(*this->scanline == mmu->read_byte(0xff45)){// && *this->scanline > 0){
                //     mmu->write_byte(0xff41, mmu->read_byte(0xff41) | (1 << 2));
                // }else{
                //     mmu->write_byte(0xff41, mmu->read_byte(0xff41) & ~(1 << 2));
                // }
                if(*this->scanline == 144){
                    // if(this->interrupts->is_interrupt_enabled(INTERRUPT_VBLANK)){
                        this->interrupts->set_interrupt_flag(INTERRUPT_VBLANK);
                        can_render = true;
                        // exit(1);
                    // }
                    can_render = true;
                    mode = 1;
                }
                
                // render_scan_lines();

                // std::cout << "SCANLINE: "<< +(*this->scanline) << std::endl;
                // if(*this->scanline == mmu->read_byte(0xff45) && *this->scanline > 0){
                //     mmu->write_byte(0xFF41, mmu->read_byte(0xFF41) | (1 << 2));
                // }else{
                //     mmu->write_byte(0xFF41, mmu->read_byte(0xFF41) & ~(1 << 2));
                // }
            }
            break;
        case 1: // VBLANK
            if(modeclock >= 456) {
                // modeclock = 0;
                *this->scanline+=1;

                if(*this->scanline > 153){
                    //Restart scanning modes
                    *this->scanline = 0;
                    mode = 2;
                    mmu->renderFlag.viewport = true;
                }
                mmu->write_byte(0xff41, mmu->read_byte(0xff41) | 1);
                if(*this->scanline == mmu->read_byte(0xff45) && *this->scanline > 0){
                    mmu->write_byte(0xff41, mmu->read_byte(0xff41) | (1 << 2));
                }else{
                    mmu->write_byte(0xff41, mmu->read_byte(0xff41) & ~(1 << 2));
                }
                modeclock -=456;
            }
            break;
        case 2: // OAM
            if(modeclock >= 80){
                modeclock -=80;
                mode = 3;
            }
            break;
        case 3: // VRAM
            if(modeclock >= 172){
                mode = 0;
                render_scan_lines();

                modeclock -= 172;
            }
            break;
            
    }
}

void PPU::compare_ly_lyc(){
    mmu->write_byte(0xff41, mmu->read_byte(0xff41) & ~(1 << 2));

    uint8_t lyc = mmu->read_byte(0xFF45);
    uint8_t stat = mmu->read_byte(0xFF41);

    if (lyc == *scanline) {
        stat |= 1 << 2;
        if (stat & (1 << 6))
            this->interrupts->set_interrupt_flag(INTERRUPT_LCD);
    }
}

void PPU::render_scan_lines(){
    if(!this->control->lcdEnable)
        return;

    bool row_pixels[160] = {0};
    this->render_scan_line_background(row_pixels);
    this->render_scan_line_window();
    this->render_scan_line_sprites(row_pixels);
}

void PPU::render_scan_line_background(bool* row_pixels){
    uint16_t address = 0x9800;

    if(this->control->bgDisplaySelect)
        address += 0x400;

    address += ((*this->scrollY + *this->scanline) / 8 * 32) % (32*32);

    uint16_t start_row_address = address;
    uint16_t end_row_address = address + 32;
    address += (*this->scrollX >> 3);

    int x = *this->scrollX & 7;
    int y = (*this->scanline + *this->scrollY) & 7;
    int pixelOffset = *this->scanline * 160;
    
    int pixel = 0;
    for(int i = 0; i < 21; i++){
        uint16_t tile_address = address + i;
        if(tile_address >= end_row_address)
            tile_address  = (start_row_address + tile_address % end_row_address);

        int tile = this->mmu->read_byte(tile_address);
        if(!this->control->bgWindowDataSelect && tile < 128)
            tile += 256;

        for(; x < 8; x++){
            if(pixel >= 160) break;

            int colour = mmu->tiles[tile][y][x];
            framebuffer[pixelOffset++] = mmu->palette_BGP[colour];
            if(colour > 0)
                row_pixels[pixel] = true;
            pixel++;
        }
        x=0;
    }
}

void PPU::render_scan_line_window(){
    if(!this->control->windowEnable){
        return;
    }

    if(mmu->read_byte(0xFF4A) > *this->scanline){
        return; 
    }

    uint16_t address = 0x9800;
    if(this->control->windowDisplaySelect)
        address += 0x400;
    
    address += ((*this->scanline - mmu->read_byte(0xFF4A)) / 8) * 32;
    int y = (*this->scanline - mmu->read_byte(0xFF4A)) & 7;
    int x = 0;

    int pixelOffset = *this->scanline * 160;
    pixelOffset += mmu->read_byte(0xFF4B) - 7;

    for(uint16_t tile_address = address; tile_address < address + 20; tile_address++){
        int tile = this->mmu->read_byte(tile_address);

        if(!this->control->bgWindowDataSelect && tile < 128)
            tile += 256;

        for(; x < 8; x++){
            int colour = mmu->tiles[tile][y][x];
            framebuffer[pixelOffset++] = mmu->palette_BGP[colour];
        }
        x=0;
    }

}

void PPU::render_scan_line_sprites(bool* row_pixels){
    if(!this->control->spriteDisplayEnable) 
        return;

    for(auto sprite : mmu->sprites){
        for(int tile_num = 0; tile_num < 1 + int(control->spriteSize); tile_num++){
            int tile_id = sprite.tile + tile_num;
            int sprite_pos_y = sprite.y + 8 * tile_num;
            if(sprite_pos_y <= *scanline && (sprite_pos_y + 8) > *scanline) {
                // Iterate over both tiles
                uint8_t y = *scanline;
                if(sprite.y > 0)
                    y %= sprite_pos_y;

                // Flip vertically
                if(sprite.options.yFlip) y = 7 - y;

                for(int x = 0; x < 8; x++){
                    int x_wrap = (sprite.x + x) % 256;
                    int pixelOffset = *this->scanline * 160 + x_wrap;
                    if(x_wrap >= 0 && x_wrap < 160) {
                        // Flip horizontally
                        uint8_t xF = sprite.options.xFlip ? 7 - x : x;
                        uint8_t colour_n = mmu->tiles[tile_id][y][xF];
                        
                        if(colour_n){
                            COLOUR colour = mmu->palette_OBP0[colour_n];
                            if(sprite.options.palleteNumber)
                                colour = mmu->palette_OBP0[colour_n];
                            if(!row_pixels[x_wrap] || !sprite.options.renderPriority){
                                framebuffer[pixelOffset] = colour;
                            }
                        }
                        pixelOffset++;
                    }
                }
            }
        }
    }
}
