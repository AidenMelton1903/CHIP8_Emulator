#include <stdint.h>
#include <string.h>

#include "characterSetInitialization.h"
#include "customMacros.h"

/*my thought process was that this would be less error prone than modifying the system's memory directly
an external file might have been more appropriate for this application, however
or at least some way to parse the data from a more human readable format
in any case, it doesn't seem like many programs use these characters anyways and it's too late to turn back now
*/

void initCharSet(){
	uint8_t charSet[16][5];
	
	uint8_t arr0[5];
	arr0[0]=0b11110000;
	arr0[1]=0b10010000;
	arr0[2]=0b10010000;
	arr0[3]=0b10010000;
	arr0[4]=0b11110000;
	memcpy(charSet[0], arr0, sizeof(arr0));
	
	arr0[0]=0b00100000;
	arr0[1]=0b01100000;
	arr0[2]=0b00100000;
	arr0[3]=0b00100000;
	arr0[4]=0b01110000;
	memcpy(charSet[1], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b00010000;
	arr0[2]=0b11110000;
	arr0[3]=0b10000000;
	arr0[4]=0b11110000;
	memcpy(charSet[2], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b00010000;
	arr0[2]=0b11110000;
	arr0[3]=0b00010000;
	arr0[4]=0b11110000;
	memcpy(charSet[3], arr0, sizeof(arr0));
	
	arr0[0]=0b10010000;
	arr0[1]=0b10010000;
	arr0[2]=0b11110000;
	arr0[3]=0b00010000;
	arr0[4]=0b00010000;
	memcpy(charSet[4], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b10000000;
	arr0[2]=0b11110000;
	arr0[3]=0b00010000;
	arr0[4]=0b11110000;
	memcpy(charSet[5], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b10000000;
	arr0[2]=0b11110000;
	arr0[3]=0b10010000;
	arr0[4]=0b11110000;
	memcpy(charSet[6], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b00010000;
	arr0[2]=0b00100000;
	arr0[3]=0b01000000;
	arr0[4]=0b01000000;
	memcpy(charSet[7], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b10010000;
	arr0[2]=0b11110000;
	arr0[3]=0b10010000;
	arr0[4]=0b11110000;
	memcpy(charSet[8], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b10010000;
	arr0[2]=0b11110000;
	arr0[3]=0b00010000;
	arr0[4]=0b11110000;
	memcpy(charSet[9], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b10010000;
	arr0[2]=0b11110000;
	arr0[3]=0b10010000;
	arr0[4]=0b10010000;
	memcpy(charSet[0xA], arr0, sizeof(arr0));
	
	arr0[0]=0b11100000;
	arr0[1]=0b10010000;
	arr0[2]=0b11100000;
	arr0[3]=0b10010000;
	arr0[4]=0b11100000;
	memcpy(charSet[0xB], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b10000000;
	arr0[2]=0b10000000;
	arr0[3]=0b10000000;
	arr0[4]=0b11110000;
	memcpy(charSet[0xC], arr0, sizeof(arr0));
	
	arr0[0]=0b11100000;
	arr0[1]=0b10010000;
	arr0[2]=0b10010000;
	arr0[3]=0b10010000;
	arr0[4]=0b11100000;
	memcpy(charSet[0xD], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b10000000;
	arr0[2]=0b11110000;
	arr0[3]=0b10000000;
	arr0[4]=0b11110000;
	memcpy(charSet[0xE], arr0, sizeof(arr0));
	
	arr0[0]=0b11110000;
	arr0[1]=0b10000000;
	arr0[2]=0b11110000;
	arr0[3]=0b10000000;
	arr0[4]=0b10000000;
	memcpy(charSet[0xF], arr0, sizeof(arr0));
	
	for(int i  = 0; i < 16; i++){
		for(int j = 0; j < 5; j++){
			memory[charMemStart+((i*5)+j)]=charSet[i][j];
		}
	}
}
