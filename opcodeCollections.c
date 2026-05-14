#include <stdint.h>
#include <stdio.h>

#include "opcodeCollections.h"
#include "customMacros.h"
#include "keypressHandling.h"

/*contains either opcodes that are particularly large/complex or ones that have multiple secondary opcodes in addition to the primary one,
since often opcodes in the same family follow the same general format (I do not have enough experience reviewing ISAs to tell if this is a regular thing)*/

void executePrimaryOp0(uint16_t opcode){
	uint8_t secondaryBits = opcode & 0x00FF;
	
	//RET
	if(secondaryBits == 0xEE){
		//printf("RET\n");
		if(sp==0){
			return;
		}
		
		sp--;
		pc = stack[sp];
	}
	
	//CLS
	else if(secondaryBits==0xE0){
		//printf("CLS\n");
		memset(display, 0, 64 * 32);
	}
	return;
}

void executePrimaryOp8(uint16_t opcode){
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	uint8_t secondaryBit = opcode & 0x000F;
	uint8_t flag;
	
	switch (secondaryBit){
	//LD Vx Vy
	case 0:
		registers[Vx]=registers[Vy];
		break;
	
	//OR Vx Vy
	case 1:
		registers[Vx] |= registers[Vy];
		break;
	
	//AND Vx Vy
	case 2:
		registers[Vx] &= registers[Vy];
		break;
	
	//XOR Vx Vy
	case 3:
		registers[Vx] ^= registers[Vy];
		break;
	
	//ADD Vx Vy
	case 4:
		uint16_t sum = registers[Vx]+registers[Vy];
		flag = sum > 255;
		registers[Vx] = sum & 0xFF;
		registers[VF] = flag;
		break;
	
	//SUB Vx Vy
	case 5:
		flag = registers[Vx] > registers[Vy];
		registers[Vx]-=registers[Vy];
		registers[VF] = flag;
		break;
	
	//SHR (shift right) Vx Vy
	case 6:
		flag = registers[Vx] & 0x1;
		registers[Vx] >>= 1;
		registers[VF] = flag;
		break;
		
	//SUBN Vx Vy (reversed SUB)
	case 7:
		//printf("SUBN, Vy is %d, Vx is %d\n",registers[Vy],registers[Vx]);
		flag = registers[Vy] >= registers[Vx];
		registers[Vx] = registers[Vy]-registers[Vx];
		registers[VF] = flag;
		break;
	
	//SHL (shift left) Vx Vy
	case 0xE:
		flag = registers[Vx] >> 7;
		registers[Vx] <<= 1;
		registers[VF] = flag;
		break;
	}
	return;
}

void executePrimaryOpDWrap(uint16_t opcode){
	uint8_t X = registers[(opcode & 0x0F00)>>8];
	uint8_t Y = registers[(opcode & 0x00F0)>>4];
	uint8_t n = opcode & 0x000F;
	uint8_t flag = 0;
	
	//printf("DISPLAYING AT %d,%d, size %d, VALUE AT I (%x) IS %d\n",X,Y,n,I,memory[I]);
	
	for(int i = 0; i < n; i++){
		uint8_t inputSprite = memory[I+i];
		
		for(int j = 0; j < 8; j++){
			uint8_t currentBit = (inputSprite >> (7-j)) & 1;
			uint8_t Xk = (X+j);
			uint8_t Yk = (Y+i);
			uint16_t startIndex = Yk*64+Xk;
			
			if(currentBit != 0){
				if(display[startIndex] == 1 && currentBit == 1){
					flag=1;
				}
				if(currentBit > ){
					
				}
				
				display[startIndex] ^= currentBit;
				//printf("PIXEL SET at %d,%d\n", Xk, Yk);
			}
		}
	}
	registers[VF]=flag;
	return;
}

void executePrimaryOpDClip(uint16_t opcode){
	uint8_t X = registers[(opcode & 0x0F00)>>8];
	uint8_t Y = registers[(opcode & 0x00F0)>>4];
	uint8_t n = opcode & 0x000F;
	uint8_t flag = 0;
	
	/*if(I==0x202){
		printf("DISPLAYING AT %d,%d, size %d, VALUE AT I (%x) IS %d\n",X,Y,n,I,memory[I]);
	}*/
	
	WaitForSingleObject(DISPMutex, INFINITE);
	
	
	
	for(int i = 0; i < n; i++){
		uint8_t inputSprite = memory[I+i];
		
		
		for(int j = 0; j < 8; j++){
			
			if (X + j >= 64 || Y + i >= 32) continue;
			
			uint8_t currentBit = (inputSprite >> (7-j)) & 1;
			uint8_t Xk = X+j;
			uint8_t Yk = Y+i;
			uint16_t startIndex = Yk*64+Xk;
			
			//printf("current val of CB: %d\n",currentBit);
			if(currentBit != 0){
				if(display[startIndex] == 1 && currentBit == 1){
					flag=1;
				}
				display[startIndex] ^= currentBit;
				//printf("PIXEL SET at %d,%d\n", Xk,Yk);
			}
		}
	}
	
	ReleaseMutex(DISPMutex);
	registers[VF]=flag;
	return;
}



void executePrimaryOpE(uint16_t opcode){
	uint8_t secondaryOp = opcode&0x00FF;
	uint8_t Vx = (opcode&0x0F00)>>8;
	uint16_t mask = 1 << (registers[Vx] & 0x0F);
	uint16_t keyStateLocal = handleKeypressesLocal();
	uint16_t state = keyStateLocal & mask;
	
	if(secondaryOp==0x9E && state != 0){
		pc+=2;
	}
	
	else if(secondaryOp==0xA1 && state == 0){
		pc+=2;
	}
	return;
}

void executePrimaryOpF(uint16_t opcode){
	uint8_t secondaryBits = opcode & 0x00FF;
	uint8_t Vx = (opcode & 0x0F00)>>8;
	
	switch(secondaryBits){
		//LD Vx DT
		case 0x07:
			registers[Vx]=DT;
			break;
		
		//Fx0A - LD Vx, K
		case 0x0A:
			uint16_t keyStateLocal = handleKeypressesLocal();
			if(keyStateLocal==0){
				pc-=2;
				break;
			}
			
			else{
				for(int i = 0; i < 16; i++){
					uint16_t mask = 1<<i;
					uint8_t pressed = (keyStateLocal & mask) != 0;
					
					if(pressed){
						registers[Vx]=i;
						break;
					}
				}
			}
			break;
		
		//LD DT Vx
		case 0x15:
			WaitForSingleObject(DTMutex, INFINITE);
			DT=registers[Vx];
			ReleaseMutex(DTMutex);
			break;
		
		//LD ST Vx
		case 0x18:
			WaitForSingleObject(STMutex, INFINITE);
			ST=registers[Vx];
			ReleaseMutex(STMutex);
			break;
		
		//ADD I Vx
		case 0x1E:
			I = (I + registers[Vx]) & 0x0FFF;
			break;
		
		//LD F Vx
		case 0x29:
			I=charMemStart+(registers[Vx]*5);
			//printf("set char memory\n");
			break;
		
		//LD B Vx
		case 0x33:
			memory[I]=registers[Vx]/100;
			memory[I+1]=(registers[Vx]/10)%10;
			memory[I+2]=registers[Vx]%10;
			break;
		
		//LD [I] Vx
		case 0x55:
			for(uint8_t index = 0; index <= Vx; index++){
				memory[I+index]=registers[index];
			}
			break;
			
		//LD Vx [I]
		case 0x65:
			for(uint8_t index = 0; index <= Vx; index++){
				registers[index]=memory[I+index];
			}
			break;
	}
	return;
}