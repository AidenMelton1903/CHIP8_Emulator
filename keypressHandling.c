#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "keypressHandling.h"
#include "customMacros.h"

HANDLE KEYMutex;

//this is an ordered list of windows keys, I changed them to be more in line with the CHIP8's actual keyboard layout
/*int keyMap[16] = {
    0x31, 0x32, 0x33, 0x34, // 1 2 3 4
    0x51, 0x57, 0x45, 0x52, // Q W E R
    0x41, 0x53, 0x44, 0x46, // A S D F
    0x5A, 0x58, 0x43, 0x56  // Z X C V
};*/

//said changed keymap
int keyMap[16] = {
    0x31, 0x32, 0x33, 0x34,
    0x51, 0x57, 0x52, 0x41,
    0x53, 0x44, 0x46, 0x5A,
    0x58, 0x43, 0x56, 0x31
};



//for debug purposes
void printKeyState(){
	char bitArray[17];
	uint16_t mask = 0b1000000000000000;
	for(uint8_t i = 0; i < 16; i++){
		uint16_t value = handleKeypressesLocal() & mask;
		
		if(value==0){
			bitArray[i]='0';
		}
		
		else{
			bitArray[i]='1';
		}
		
		mask>>=1;
	}
	
	bitArray[16]='\0';
	
	printf("key state: %s\n",bitArray);
}

uint16_t handleKeypressesLocal(){
	uint16_t keyStateLocal = 0;
	for (int i = 0; i < 16; i++){
		uint16_t location = 1<<i;
		if (GetAsyncKeyState(keyMap[i]) & 0x8000){
			keyStateLocal |= location;
		}
	}
	ReleaseMutex(KEYMutex);
	return keyStateLocal;
}