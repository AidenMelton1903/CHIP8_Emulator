#ifndef CUSTOM_MACROS_H
#define CUSTOM_MACROS_H

//this file initially started with just macros (hence the name) but it evolved into a way to declare variables that I want multiple files to have access to.
//global variables would probably have been cleaner in hindsight

#include <windows.h>
#define VF 15
#define charMemStart 0x50
#define stackSize 256
#define startingLoc 0x200

extern uint8_t memory[4096];
extern uint8_t display[32*64];
extern uint8_t registers[16];
extern uint16_t I;
extern uint16_t pc;
extern uint16_t stack[stackSize];
extern uint8_t sp;
extern uint8_t executing;


extern uint8_t DT;
extern uint8_t DTTerminateFlag;
extern HANDLE DTMutex;
extern uint8_t ST;
extern uint8_t STTerminateFlag;
extern HANDLE STMutex;
extern HANDLE DISPMutex;
extern uint8_t executing;

void clearArray8(uint8_t* arrPointer, int size);
void clearArray16(uint16_t* arrPointer, int size);

#endif