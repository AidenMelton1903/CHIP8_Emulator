#ifndef OPCODE_COLLECTIONS_H
#define OPCODE_COLLECTIONS_H

#include <stdint.h>
#include "customMacros.h"

//I decided to implement most of the opcodes here because they're small and I thought they would be good to inline.
//realistically I'm sure the compiler would have inlined most of them anyways if it helped but I do like to be sure.

void executePrimaryOp0(uint16_t opcode);

//JMP nnn
static inline void executePrimaryOp1(uint16_t opcode){
	pc = (opcode & 0x0FFF);
	return;
}

//CALL
static inline void executePrimaryOp2(uint16_t opcode){
	//printf("CALL\n");
	if(sp>=stackSize){
		return;
	}
	
	uint16_t nnn = opcode & 0x0FFF;
	stack[sp++]=pc;
	pc=nnn;
	return;
}

//3xkk - SE Vx, byte
static inline void executePrimaryOp3(uint16_t opcode){
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	
	if(registers[Vx] == kk){
		pc+=2;
	}
	return;
}

//4xkk - SNE Vx, byte
static inline void executePrimaryOp4(uint16_t opcode){
	uint8_t Vx = (opcode&0x0F00)>>8;
	uint8_t kk = opcode & 0x00FF;
	
	//printf("SNE: regsister at %d, (Vx) is %d, kk is %d\n",(opcode&0x0F00)>>8,registers[Vx],kk);
	
	if (registers[Vx] != kk){
		pc+=2;
	}
	return;
}

//5xy0 SE Vx, Vy
static inline void executePrimaryOp5(uint16_t opcode){
	if ((opcode & 0x000F) != 0) return;
	
	uint8_t X = (opcode & 0x0F00)>>8;
	uint8_t Y = (opcode & 0x00F0)>>4;
	
	if(registers[X] == registers[Y]){
		pc+=2;
	}
	return;
}

//6xkk - LD Vx, byte
static inline void executePrimaryOp6(uint16_t opcode){
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	
	registers[Vx] = kk;
	return;
}

//7xkk - ADD Vx, byte
static inline void executePrimaryOp7(uint16_t opcode){
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	
	registers[Vx] += kk;
	return;
}

void executePrimaryOp8(uint16_t opcode);

//9xy0 - SNE Vx, Vy
static inline void executePrimaryOp9(uint16_t opcode){
	if ((opcode & 0x000F) != 0) return;
	
	uint8_t Vx = (opcode&0x0F00)>>8;
	uint8_t Vy = (opcode&0x00F0)>>4;
	
	if (registers[Vx] != registers[Vy]) {
		pc += 2;
	}
	return;
}

//Annn - LD I, addr
static inline void executePrimaryOpA(uint16_t opcode){
	I=(opcode & 0x0FFF);
	return;
	//printf("loading value %x into I\n",I);
}

//Bnnn - JP V0, addr
static inline void executePrimaryOpB(uint16_t opcode){
	pc=(opcode & 0x0FFF)+registers[0];
	return;
}

//Cxkk - RND Vx, byte
static inline void executePrimaryOpC(uint16_t opcode){
	uint8_t randNum = rand() % 256;
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t kk = (opcode & 0x00FF);
	
	randNum &= kk;
	registers[Vx]=randNum;
	return;
}


void executePrimaryOpDWrap(uint16_t opcode);
void executePrimaryOpDClip(uint16_t opcode);

void executePrimaryOpE(uint16_t opcode);

void executePrimaryOpF(uint16_t opcode);

#endif