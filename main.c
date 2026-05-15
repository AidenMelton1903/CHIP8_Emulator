#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <synchapi.h>
#include <processthreadsapi.h>
#include <unistd.h>

#include "opcodeCollections.h"
#include "characterSetInitialization.h"
#include "customMacros.h"


//overall memory and register definitions
uint8_t memory[4096];
uint8_t registers[16];
uint16_t I;
uint16_t pc;

//stack memory
uint16_t stack[stackSize];
uint8_t sp = 0;

//timers and execution flag
uint8_t DT = 0;
HANDLE DTMutex;
uint8_t ST = 0;
HANDLE STMutex;
uint8_t executing = 1;

//display stuff
uint8_t display[32*64];
HANDLE DISPMutex;

//array clearing functions, mostly used during initialization
//I could have used something in the C standard lib but this way I can be sure I know how it's going to behave
void clearArray8(uint8_t* arrPointer, int size){
	for(int i = 0; i < size; i++){
		arrPointer[i]=0;
	}
}

void clearArray16(uint16_t* arrPointer, int size){
	for(int i = 0; i < size; i++){
		arrPointer[i]=0;
	}
}

//functions to initialize the CPU and prepare for the main program loop
void init(){
	clearArray8(memory,4096);
	clearArray8(registers,16);
	I = 0;
	pc = startingLoc;
	clearArray16(stack,stackSize);
	
	//init visual memory
	clearArray8(display,32*64);
	initCharSet();
}

void loadProg(char* romString){
	char fpath[1024+4];
	snprintf(fpath,6,"%s","roms/");
	strcat(fpath,romString);
	FILE* fptr = fopen(fpath, "rb");
    if (!fptr) {
        printf("Failed to open file \"%s\"\n",fpath);
        return;
    }

    fseek(fptr, 0, SEEK_END);
    size_t size = ftell(fptr);
    rewind(fptr);

    fread(memory +startingLoc, 1, size, fptr);

    printf("Loaded %ld bytes into memory starting at %03x\n", size, startingLoc);

    fclose(fptr);
	
	for (int i = 0x200; i < 0x400; i++) {
		//printf("%03x: %02x\n", i, memory[i]);
	}
}

//In true C fashion, refreshScreen() is now deprecated, but I refuse to remove the '2' from this function because i think it is pretty :)
void refreshScreen2(){
	printf("\x1b[H");
	char dispBuffer[65*32];
	uint32_t index = 0;
	
	WaitForSingleObject(DISPMutex, INFINITE);
	for(int i = 0; i < 32; i++){
		for(int j = 0; j < 64; j++){
			if(display[(64*i)+j]==0){
				dispBuffer[index]=' ';
			}
			else{
				dispBuffer[index]='#';
			}
			index++;
		}
		
		dispBuffer[index++]='\n';
	}
	
	ReleaseMutex(DISPMutex);
	write(STDOUT_FILENO, dispBuffer, index);
	return;
}

//code for fetch/decode/execute starts here
uint16_t fetch(){
	uint16_t code = memory[pc+1];
	code |= memory[pc] << 8;
	pc+=2;
	return code;
}

//decode and execute are within the same function (or function(s), rather)
void cycle(){
	uint16_t opcode = fetch();
	uint8_t registerNum;
	uint8_t value;
	uint8_t pcIncrement;
	uint16_t addr;
	//printf("pc location:%x opcode: %x\n",pc,opcode);
	switch (opcode & 0xF000){
		case 0x0000:
			if(opcode==0){
				printf("NOOP\n");
				break;
			}
			
			executePrimaryOp0(opcode);
			break;
		
		//JMP nnn
		case 0x1000:
			executePrimaryOp1(opcode);
			break;
		
		//CALL nnn
		case 0x2000:
			executePrimaryOp2(opcode);
			break;
		
		//SE Vx kk
		case 0x3000:
			executePrimaryOp3(opcode);
			break;
			
		//SNE Vx kk
		case 0x4000:
			executePrimaryOp4(opcode);
			break;
		
		case 0x5000:
			executePrimaryOp5(opcode);
			break;
		
		// LD Vx kk
		case 0x6000:
			executePrimaryOp6(opcode);
			break;
		
		//ADD Vx kk
		case 0x7000:
			executePrimaryOp7(opcode);
			break;
		
		//execute 8xxx opcodes
		case 0x8000:
			executePrimaryOp8(opcode);
			break;
		
		//SNE Vx Vy
		case 0x9000:
			executePrimaryOp9(opcode);
			break;
		
		//LD I nnn
		case 0xA000:
			executePrimaryOpA(opcode);
			break;
		
		//JMP nnn + V0
		case 0xB000:
			executePrimaryOpB(opcode);
			break;
			
		//RND Vx kk
		case 0xC000:
			executePrimaryOpC(opcode);
			break;
		
		//category D opcodes
		case 0xD000:
			executePrimaryOpDClip(opcode);
			break;
		
		//category E opcodes
		case 0xE000:
			executePrimaryOpE(opcode);
			break;
		
		//category F opcodes
		case 0xF000:
			executePrimaryOpF(opcode);
			break;
		
		default:
			printf("unknown");
			printf(" instruction, unimplimented.\n");
			break;
	}
}

//functions to create threads
DWORD WINAPI DTTimerThread(void*){
	while(1){
		Sleep(16);
		if(DT>0){
			WaitForSingleObject(DTMutex, INFINITE);
			DT--;
			ReleaseMutex(DTMutex);
		}
	}
}

DWORD WINAPI STTimerThread(void*){
	while(1){
		Sleep(16);
		if(ST>0){
			WaitForSingleObject(STMutex, INFINITE);
			ST--;
			ReleaseMutex(STMutex);
		}
	}
}

void createTimerThreads(){
	HANDLE DTthread;
    DWORD DTthreadId;

    CreateThread(
        NULL,               // default security attributes
        0,                  // default stack size
        DTTimerThread,     // thread start function
        NULL,               // parameter to thread
        0,                  // creation flags
        &DTthreadId           // thread ID output
    );
	
	CreateThread(
        NULL,               // default security attributes
        0,                  // default stack size
        STTimerThread,     // thread start function
        NULL,               // parameter to thread
        0,                  // creation flags
        &DTthreadId           // thread ID output
    );
}

DWORD WINAPI processorLoop(void*){
	while(1){
		cycle();
	}
}

void createCPUThread(){
	HANDLE DTthread;
    DWORD DTthreadId;
	
	CreateThread(
        NULL,               // default security attributes
        0,                  // default stack size
        processorLoop,     // thread start function
        NULL,               // parameter to thread
        0,                  // creation flags
        &DTthreadId           // thread ID output
    );
}

DWORD WINAPI displayLoop(void*){
	while(1){
		refreshScreen2();
		Sleep(17);
	}
}

void createDisplayThread(){
	HANDLE DTthread;
    DWORD DTthreadId;
	
	CreateThread(
        NULL,               // default security attributes
        0,                  // default stack size
        displayLoop,     // thread start function
        NULL,               // parameter to thread
        0,                  // creation flags
        &DTthreadId           // thread ID output
    );
}

//debug functions
void printTimers(){
	printf("timer: %d, audio timer:%d\n",DT,ST);
}

void dumpMem(int low,int high){
	for(int idx = low; idx < high; idx++){
		printf("location: %X, value: %X\n",idx,memory[idx]);
	}
}

int populatePrgEntryTable(char** strArray, int n){
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFile("roms\\*",&ffd);
	
	if(hFind == INVALID_HANDLE_VALUE){
		printf("error parsing program name\n");
		return 0;
	}
	
	int i = 0;
	while(i < n && FindNextFile(hFind, &ffd) != 0){
		//check if filename is . or ..
		if(!strcmp(ffd.cFileName,".")){
			continue;
		}
		else if(!strcmp(ffd.cFileName,"..")){
			continue;
		}
		
		//check if entry is the right filetype
		
		//make copy of string
		char stringCopy[1024];
		snprintf(stringCopy,sizeof(stringCopy),"%s",ffd.cFileName);
		
		//save file name
		char progName[1024];
		char* currentTok = strtok(stringCopy,".");
		snprintf(progName,sizeof(progName),"%s",currentTok);
		//get extension
		while(currentTok != NULL){
			currentTok = strtok(NULL,".");
		}
		
		char progExtension[4];
		snprintf(progExtension,sizeof(progExtension),"%s",currentTok);
		
		//continue if extension is wrong
		if(!strcmp(progExtension,"ch8")){
			continue;
		}
		
		printf("%d: %s\n",i+1,progName);
		//add to array of valid titles
		strArray[i]=strdup(ffd.cFileName);
		i++;
	}
	
	FindClose(hFind);
	return i;
}

void prgEntryCleanup(char** strArray, int n){
	for(int i = 0; i < n-1; i++){
		if(strArray[i]==0){
			break;
		}
		free(strArray[i]);
	}
}

void prgLoadMenu(char* prgNameBuff, int prgNameBuffSize){
	char* strArray[1024];
	int sizeOfArray = sizeof(strArray)/sizeof(char*);
	int numElements = populatePrgEntryTable(strArray,sizeOfArray);
	
	//user input goto loop starts here
	promptForUserInput:
	char userInputBuff[20];
	userInputBuff[0]='\0';
	printf("choice: ");
	fgets(userInputBuff,sizeof(userInputBuff),stdin);
	
	//convert to long long and check if it's valid
	char* endPtr;
	long long convertedInt = strtol(userInputBuff,&endPtr,10);
	//decrement it since the options presented start at 1 for usability
	convertedInt--;
	if(*endPtr != '\0' && *endPtr != '\n'){
		printf("invalid index, not a number.\n");
		goto promptForUserInput;
	}
	
	if(convertedInt >= numElements || convertedInt < 0){
		printf("invalid index, out of range.\n");
		goto promptForUserInput;
	}
	
	snprintf(prgNameBuff,prgNameBuffSize,"%s",strArray[convertedInt]);
	printf("chosen: %s\n",prgNameBuff);
	prgEntryCleanup(strArray,numElements);
}

int main(){
	char programName[1024];
	prgLoadMenu(programName,sizeof(programName));
	init();
	loadProg(programName);
	createTimerThreads();
	createCPUThread();
	
	//decided to let the display have the main thread, the others get their own
	while(1){
		refreshScreen2();
		Sleep(17);
	}
}