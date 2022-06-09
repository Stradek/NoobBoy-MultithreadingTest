#include "joypad.h"


Joypad::Joypad(Interrupts* interrupts, MMU* mmu){
    this->interrupts = interrupts;
    this->mmu = mmu;
}

void Joypad::key_press(ButtonFlags key){
    mmu->joypad &= ~(0xFF & key);
    interrupts->set_interrupt_flag(INTERRUPT_JOYPAD);
}

void Joypad::key_release(ButtonFlags key){
    mmu->joypad |= 0xFF & key;
    interrupts->set_interrupt_flag(INTERRUPT_JOYPAD);
}