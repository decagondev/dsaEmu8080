/* oldmain_TEST1_YT_EP20.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct CpuFlags
{
	uint8_t z:1 // Zero Flag
	uint8_t s:1 // Sign Flag
	uint8_t p:1 // Parity Flag.
	uint8_t c:1 // Carry Flag
	uint8_t ac:1 // Aux Carry Flag
	uint8_t padding:3 // Padding
} CpuFlags;

CpuFlags CC_ZSPAC = {1,1,1,0,1}; // Implemented on Twitch.tv


typedef struct CpuState
{
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
	uint8_t *memory; // fixed type here: after episode 12
	struct CpuFlags cc;
	uint8_t enable_int;
}CpuState;

int parity(int q, int size)
{
	// parity function TODO: move to utils include.
	int p = 0;
	q = (q & ((1 << size)-1));
	for (int i = 0; i < size; i++)
	{
		if (q & 0x1) p++;
		q = q >> 1;
	}

	return (0 == (p & 0x1));	
}

void FileReadToMemory(CpuState* state, char* fName, uint32_t offs)
{
	FILE *filename = fopen(fName, "rb");
	if (filename = NULL)
	{
		printf("ERROR: Can not open %s\n", fName);
		exit(1);
	}
	fseek(filename, 0L, SEEK_END);
	int fSize = ftell(filename);
	fseek(filename, 0L, SEEK_SET);

	uint8_t *buffer = &state->memory[offs];
	fread(, fSize, 1, filename);
	fclose(filename);
}



void WriteToMem(CpuState* state, uint16_t addr, uint8_t val) // Implemented in Episode 18
{
	if(addr < 0x2000)
	{
		printf("Writing Out of Lower Bounds %x\n", addr);
		return;
	}
	if(addr >= 0x4000)
	{
		printf("Writing Out of Upper Bounds %x\n", addr);
		return;
	}

	state->memory[addr] = val;
}

// implemented in episode 20. Read and Write HL / Z S P Flag functions

uint8_t ReadFromHL(VpuState* state)
{
	uint16_t offset = (state->h << 8) | state->l;
	return state->memory[offset];
}

void WriteToHL(CpuState* state, uint8_t val)
{
	uint16_t offset = (state->h << 8) | state->l;
	WriteToMem(state, offset, val);
}

void FlagsZSP(CpuState *state, uint8_t val)
{
	state->cc.z = (val == 0);
	state->cc.s = (0x80 == (val & 0x80));
	state->cc.p = parity(value, 8);
}

void Pop(CpuState* state, uint8_t *high, uint8_t *low) // Implemented in Episode 18
{
	*low = state->memory[state->sp];
	*high = state->memory[state->sp +1];
}

void Push(CpuState* state, uint8_t high, uint8_t low) // Implemented in Episode 
{
	WriteToMem(state, state->sp - 1, high);
	WriteToMem(state, state->sp - 2, low);
	state->sp -= 2;
}

int DissAsm(unsigned char *buff, int pc) 
{
	char *code = buff[pc];
	int opCodeSize = 1;
	printf("Data: \n");
	switch (*code)
	{
		// B,C Cases
		case 0x00: printf("NOP"); break;
		case 0x01: printf("LXI    B,#$%02x%02x", code[2], code[1]); opCodeSize=3; break; // implemented in episode 8
		case 0x02: printf("STAX   B"); break;
		case 0x03: printf("INX    B"); break;
		case 0x04: printf("INR    B"); break;
		case 0x05: printf("DCR    B"); break; // TODO: implement in ep 9.
		case 0x06: printf("MVI    B,#$%02x", code[1]); opCodeSize=2; break; // implemented in episode 8
		case 0x07: printf("RLC"); break;
		case 0x08: printf("NOP"); break;
		case 0x09: printf("DAD    B"); break;
		case 0x0a: printf("LDAX   B"); break;
		case 0x0b: printf("DCX    B"); break;
		case 0x0c: printf("INR    C"); break;
		case 0x0d: printf("DCR    C"); break;
		case 0x0e: printf("MVI    C,#$%02x", code[1]); opCodeSize = 2;	break;
		case 0x0f: printf("RRC"); break;

		// D, E Cases
		case 0x10: printf("NOP"); break;
		case 0x11: printf("LXI    D,#$%02x%02x", code[2], code[1]); opCodeSize=3; break;
		case 0x12: printf("STAX   D"); break;
		case 0x13: printf("INX    D"); break;
		case 0x14: printf("INR    D"); break;
		case 0x15: printf("DCR    D"); break;
		case 0x16: printf("MVI    D,#$%02x", code[1]); opCodeSize=2; break;
		case 0x17: printf("RAL"); break;
		case 0x18: printf("NOP"); break;
		case 0x19: printf("DAD    D"); break;
		case 0x1a: printf("LDAX   D"); break;
		case 0x1b: printf("DCX    D"); break;
		case 0x1c: printf("INR    E"); break;
		case 0x1d: printf("DCR    E"); break;
		case 0x1e: printf("MVI    E,#$%02x", code[1]); opCodeSize = 2; break;
		case 0x1f: printf("RAR"); break;
		case 0x20: printf("NOP"); break;
		case 0x21: printf("LXI    H,#$%02x%02x", code[2], code[1]); opCodeSize=3; break;
		case 0x22: printf("SHLD   $%02x%02x", code[2], code[1]); opCodeSize=3; break;
		case 0x23: printf("INX    H"); break;
		case 0x24: printf("INR    H"); break;
		case 0x25: printf("DCR    H"); break;
		case 0x26: printf("MVI    H,#$%02x", code[1]); opCodeSize=2; break;
		case 0x27: printf("DAA"); break;
		case 0x28: printf("NOP"); break;
		case 0x29: printf("DAD    H"); break;
		case 0x2a: printf("LHLD   $%02x%02x", code[2], code[1]); opCodeSize=3; break;
		case 0x2b: printf("DCX    H"); break;
		case 0x2c: printf("INR    L"); break;
		case 0x2d: printf("DCR    L"); break;
		case 0x2e: printf("MVI    L,#$%02x", code[1]); opCodeSize = 2; break;
		case 0x2f: printf("CMA"); break;

		case 0x30: printf("NOP"); break;
		case 0x31: printf("LXI    SP,#$%02x%02x", code[2], code[1]); opCodeSize=3; break;
		case 0x32: printf("STA    $%02x%02x", code[2], code[1]); opCodeSize=3; break;
		case 0x33: printf("INX    SP"); break;
		case 0x34: printf("INR    M"); break;
		case 0x35: printf("DCR    M"); break;
		case 0x36: printf("MVI    M,#$%02x", code[1]); opCodeSize=2; break;
		case 0x37: printf("STC"); break;
		case 0x38: printf("NOP"); break;
		case 0x39: printf("DAD    SP"); break;
		case 0x3a: printf("LDA    $%02x%02x", code[2], code[1]); opCodeSize=3; break;
		case 0x3b: printf("DCX    SP"); break;
		case 0x3c: printf("INR    A"); break;
		case 0x3d: printf("DCR    A"); break;
		case 0x3e: printf("MVI    A,#$%02x", code[1]); opCodeSize = 2; break;
		case 0x3f: printf("CMC"); break;

		// MOV operation blocks		
		case 0x40: printf("MOV    B,B"); break;
		case 0x41: printf("MOV    B,C"); break;
		case 0x42: printf("MOV    B,D"); break;
		case 0x43: printf("MOV    B,E"); break;
		case 0x44: printf("MOV    B,H"); break;
		case 0x45: printf("MOV    B,L"); break;
		case 0x46: printf("MOV    B,M"); break;
		case 0x47: printf("MOV    B,A"); break;
		case 0x48: printf("MOV    C,B"); break;
		case 0x49: printf("MOV    C,C"); break;
		case 0x4a: printf("MOV    C,D"); break;
		case 0x4b: printf("MOV    C,E"); break;
		case 0x4c: printf("MOV    C,H"); break;
		case 0x4d: printf("MOV    C,L"); break;
		case 0x4e: printf("MOV    C,M"); break;
		case 0x4f: printf("MOV    C,A"); break;

		case 0x50: printf("MOV    D,B"); break;
		case 0x51: printf("MOV    D,C"); break;
		case 0x52: printf("MOV    D,D"); break;
		case 0x53: printf("MOV    D.E"); break;
		case 0x54: printf("MOV    D,H"); break;
		case 0x55: printf("MOV    D,L"); break;
		case 0x56: printf("MOV    D,M"); break;
		case 0x57: printf("MOV    D,A"); break;
		case 0x58: printf("MOV    E,B"); break;
		case 0x59: printf("MOV    E,C"); break;
		case 0x5a: printf("MOV    E,D"); break;
		case 0x5b: printf("MOV    E,E"); break;
		case 0x5c: printf("MOV    E,H"); break;
		case 0x5d: printf("MOV    E,L"); break;
		case 0x5e: printf("MOV    E,M"); break;
		case 0x5f: printf("MOV    E,A"); break;

		case 0x60: printf("MOV    H,B"); break;
		case 0x61: printf("MOV    H,C"); break;
		case 0x62: printf("MOV    H,D"); break;
		case 0x63: printf("MOV    H.E"); break;
		case 0x64: printf("MOV    H,H"); break;
		case 0x65: printf("MOV    H,L"); break;
		case 0x66: printf("MOV    H,M"); break;
		case 0x67: printf("MOV    H,A"); break;
		case 0x68: printf("MOV    L,B"); break;
		case 0x69: printf("MOV    L,C"); break;
		case 0x6a: printf("MOV    L,D"); break;
		case 0x6b: printf("MOV    L,E"); break;
		case 0x6c: printf("MOV    L,H"); break;
		case 0x6d: printf("MOV    L,L"); break;
		case 0x6e: printf("MOV    L,M"); break;
		case 0x6f: printf("MOV    L,A"); break;

		case 0x70: printf("MOV    M,B"); break;
		case 0x71: printf("MOV    M,C"); break;
		case 0x72: printf("MOV    M,D"); break;
		case 0x73: printf("MOV    M.E"); break;
		case 0x74: printf("MOV    M,H"); break;
		case 0x75: printf("MOV    M,L"); break;
		case 0x76: printf("HLT");        break;
		case 0x77: printf("MOV    M,A"); break;
		case 0x78: printf("MOV    A,B"); break;
		case 0x79: printf("MOV    A,C"); break;
		case 0x7a: printf("MOV    A,D"); break;
		case 0x7b: printf("MOV    A,E"); break;
		case 0x7c: printf("MOV    A,H"); break;
		case 0x7d: printf("MOV    A,L"); break;
		case 0x7e: printf("MOV    A,M"); break;
		case 0x7f: printf("MOV    A,A"); break;

		// ADD opperation blocks
		case 0x80: printf("ADD    B"); break;
		case 0x81: printf("ADD    C"); break;
		case 0x82: printf("ADD    D"); break;
		case 0x83: printf("ADD    E"); break;
		case 0x84: printf("ADD    H"); break;
		case 0x85: printf("ADD    L"); break;
		case 0x86: printf("ADD    M"); break;
		case 0x87: printf("ADD    A"); break;
		case 0x88: printf("ADC    B"); break;
		case 0x89: printf("ADC    C"); break;
		case 0x8a: printf("ADC    D"); break;
		case 0x8b: printf("ADC    E"); break;
		case 0x8c: printf("ADC    H"); break;
		case 0x8d: printf("ADC    L"); break;
		case 0x8e: printf("ADC    M"); break;
		case 0x8f: printf("ADC    A"); break;

		// SUB Opperation blocks
		case 0x90: printf("SUB    B"); break;
		case 0x91: printf("SUB    C"); break;
		case 0x92: printf("SUB    D"); break;
		case 0x93: printf("SUB    E"); break;
		case 0x94: printf("SUB    H"); break;
		case 0x95: printf("SUB    L"); break;
		case 0x96: printf("SUB    M"); break;
		case 0x97: printf("SUB    A"); break;
		case 0x98: printf("SBB    B"); break;
		case 0x99: printf("SBB    C"); break;
		case 0x9a: printf("SBB    D"); break;
		case 0x9b: printf("SBB    E"); break;
		case 0x9c: printf("SBB    H"); break;
		case 0x9d: printf("SBB    L"); break;
		case 0x9e: printf("SBB    M"); break;
		case 0x9f: printf("SBB    A"); break;

		case 0xa0: printf("ANA    B"); break;
		case 0xa1: printf("ANA    C"); break;
		case 0xa2: printf("ANA    D"); break;
		case 0xa3: printf("ANA    E"); break;
		case 0xa4: printf("ANA    H"); break;
		case 0xa5: printf("ANA    L"); break;
		case 0xa6: printf("ANA    M"); break;
		case 0xa7: printf("ANA    A"); break;
		case 0xa8: printf("XRA    B"); break;
		case 0xa9: printf("XRA    C"); break;
		case 0xaa: printf("XRA    D"); break;
		case 0xab: printf("XRA    E"); break;
		case 0xac: printf("XRA    H"); break;
		case 0xad: printf("XRA    L"); break;
		case 0xae: printf("XRA    M"); break;
		case 0xaf: printf("XRA    A"); break;

		case 0xb0: printf("ORA    B"); break;
		case 0xb1: printf("ORA    C"); break;
		case 0xb2: printf("ORA    D"); break;
		case 0xb3: printf("ORA    E"); break;
		case 0xb4: printf("ORA    H"); break;
		case 0xb5: printf("ORA    L"); break;
		case 0xb6: printf("ORA    M"); break;
		case 0xb7: printf("ORA    A"); break;
		case 0xb8: printf("CMP    B"); break;
		case 0xb9: printf("CMP    C"); break;
		case 0xba: printf("CMP    D"); break;
		case 0xbb: printf("CMP    E"); break;
		case 0xbc: printf("CMP    H"); break;
		case 0xbd: printf("CMP    L"); break;
		case 0xbe: printf("CMP    M"); break;
		case 0xbf: printf("CMP    A"); break;

		case 0xc0: printf("RNZ"); break;
		case 0xc1: printf("POP    B"); break;
		case 0xc2: printf("JNZ    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xc3: printf("JMP    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xc4: printf("CNZ    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xc5: printf("PUSH   B"); break;
		case 0xc6: printf("ADI    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xc7: printf("RST    0"); break;
		case 0xc8: printf("RZ"); break;
		case 0xc9: printf("RET"); break;
		case 0xca: printf("JZ     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xcb: printf("JMP    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xcc: printf("CZ     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xcd: printf("CALL   $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xce: printf("ACI    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xcf: printf("RST    1"); break;

		case 0xd0: printf("RNC"); break;
		case 0xd1: printf("POP    D"); break;
		case 0xd2: printf("JNC    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xd3: printf("OUT    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xd4: printf("CNC    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xd5: printf("PUSH   D"); break;
		case 0xd6: printf("SUI    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xd7: printf("RST    2"); break;
		case 0xd8: printf("RC");  break;
		case 0xd9: printf("RET"); break;
		case 0xda: printf("JC     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xdb: printf("IN     #$%02x",code[1]); opCodeSize = 2; break;
		case 0xdc: printf("CC     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xdd: printf("CALL   $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xde: printf("SBI    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xdf: printf("RST    3"); break;

		case 0xe0: printf("RPO"); break;
		case 0xe1: printf("POP    H"); break;
		case 0xe2: printf("JPO    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xe3: printf("XTHL");break;
		case 0xe4: printf("CPO    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xe5: printf("PUSH   H"); break;
		case 0xe6: printf("ANI    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xe7: printf("RST    4"); break;
		case 0xe8: printf("RPE"); break;
		case 0xe9: printf("PCHL");break;
		case 0xea: printf("JPE    $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xeb: printf("XCHG"); break;
		case 0xec: printf("CPE     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xed: printf("CALL   $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xee: printf("XRI    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xef: printf("RST    5"); break;

		case 0xf0: printf("RP");  break;
		case 0xf1: printf("POP    PSW"); break;
		case 0xf2: printf("JP     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xf3: printf("DI");  break;
		case 0xf4: printf("CP     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xf5: printf("PUSH   PSW"); break;
		case 0xf6: printf("ORI    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xf7: printf("RST    6"); break;
		case 0xf8: printf("RM");  break;
		case 0xf9: printf("SPHL");break;
		case 0xfa: printf("JM     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xfb: printf("EI");  break;
		case 0xfc: printf("CM     $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xfd: printf("CALL   $%02x%02x",code[2],code[1]); opCodeSize = 3; break;
		case 0xfe: printf("CPI    #$%02x",code[1]); opCodeSize = 2; break;
		case 0xff: printf("RST    7"); break;

	}
	printf("\n")
	return opCodeSize;
}

void LogicalFlagsDSA(CpuState *state)
{
	//TODO: move in to utility header at a later stage
	state->cc.cy = state->cc.ac = 0;
	state->z = (state-> == 0);
	state->s = (0x80 == (state->a & 0x80));
	state->cc.p = parity(state->a, 8);
}

void ArithmaticFlagsDSA(CpuState *state, uint16_t res)
{
	// TODO: move in to utility header at a later stage
	state->cc.cy = (state->cc.ac = (res > 0xff);;
	state->z = ((res & 0xff) == 0);
	state->s = (0x80 == (res & 0x80));
	state->cc.p = parity(res & 0xff, 8);
}

void UnimplementedIns(CpuState *state)
{
	/* TODO: Unimplimented Ins - print message, decrement pc (Program Counter)
	   DissAsm(the instruction), print a new line and exit with an error. */
	printf("Instruction Not Implemented!\n"); // print reason for error
	state->pc--; // decrement pc
	DissAsm(state->memory, state->pc); //DissAsm the ins
	printf("\n");
	exit(1); // exit returning error
}

int Emu8080Op()
{
	//TODO: add more insatruction implementations over the course of the series

	int cycles = 4; // Number of cycles in std op
	unsigned char *opCode = &state->memory[state->pc]; //Set opCode to read next

	DissAsm();

	state->pc++; // increment PC

	switch (*opCode)
	{
		case 0x00: return;

		case 0x01: // implemented in episode 8 : LXI B, D16
			state->c = opCode[1]; // load opCode[1] in to reg c
			state->b = opCode[2]; // load opCode[2] in to reg b
			state->pc += 2; // inc pc by 2.
			break; 

		case 0x02: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x03: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x04: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x05: // implemented in episode 9 : DCR B
			{
				uint8_t res = state->b - 1;
				state->cc.z = (res == 0);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = parity(res, 8);
				state->b = res;
			}
			break;

		case 0x06: // implemented in episode 8 MVI C, D8
			state->b = opCode[1]; // Load opCode[1] in to reg b
			state->pc++; // inc pc 
			break; 

		case 0x07: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x08: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x09: // implemented in episode 10 : DAD B
			{
				uint32_t hl = (state->h << 8) | state->l;
				uint32_t bc = (state->b << 8) | state->c;
				uint32_t res = hl + bc;
				state->h = (res & 0xff00) >> 8;
				state->l = res & 0xff;
				state->cc.cy = ((res & 0xffff0000) > 0);
			}
			break;

		case 0x0a: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x0b: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x0c: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x0d: // implemented in episode 11 : DCR C
			{
				uint8_t res = state->c - 1;
				state->z = (res == 0);
				state->cc.s = (0x80 == (res & 0x80));
				state->cc.p = parity(res, 8);
				state->c = res;
			}
			break; 

		case 0x0e: // Implemented in episode 12 : MVI C, byte
			state->c = opCode[1];
			state->pc++;
			break; 

		case 0x0f: // Implemented in episode 12 : RRC
			{
				uin8_t n = state->a;
				state->a = ((n & 1)) << 7) | (n >> 1);
				state->cc.cy = (1 == (n & 1));
			}
			break; 

		case 0x10: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x11: // Implemented in episode 12 : LXI D, word
			state->e = opCode[1];
			state->d = opCode[2];
			state->pc += 2;
			break; 

		case 0x12: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x13: // Implemented in Episode 13 : INX D
			state->e++;
			if (state->e == 0)
				state->d++;
			break; // TODO: Implement Instruction
		case 0x14: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x15: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x16: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x17: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x18: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x19: // Implemented in episode 11 : DAD D
			{
				uint32_t hl = (state->h << 8) | state->l;
				uint32_t bc = (state->d << 8) | state->e;
				uint32_t res = hl + de;
				state->h = (res & 0xff00) >> 8;
				state->l = res & 0xff;
				state->cc.cy = ((res & 0xffff0000) != 0);
			}		
			break;

		case 0x1a: // Implemented in Episode 13 : LDAX D
			{
				uint16_t offset = (state-> d << 8) | state->e;
				state->a = state->memory[offset];
			}
			break; 

		case 0x1b: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x1c: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x1d: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x1e: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x1f: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x20: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x21: //Implemented Instruction for Episode 14 : LXI H, word
			state->l = opCode[1];
			state->h = opCode[2];
			state->pc += 2;
			break; 

		case 0x22: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x23: //Implemenede Instruction for Episode 14 : INX H
			state->l++;
			if (state->l == 0)
				state->h++;
			break; 
		case 0x24: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x25: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x26: //Implemented Instruction for Episode 14 : MVI H, byte
			state->h = opCode[1];
			state->pc++;
			break; 
		case 0x27: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x28: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x29: // Implemented Instruction for Episode 15 : DAD H
			{
				uint32_t hl = (stste->h << 8) | state->l;
				uint32_t res = hl + hl;
				state->h = (res & 0xff00) >> 8;
				state->l = res & 0xff;
				state->cc.cy = ((res & 0xffff0000) != 0);
			}
			break; 

		case 0x2a: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x2b: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x2c: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x2d: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x2e: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x2f: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x30: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x31: // Implemented Instruction for Episode 15 : LXI SP, word
			state->sp = (opCode[2] << 8 ) | opCode[1];
			state->pc += 2;
			break; 

		case 0x32: // Implemented Instruction for Episode 15 : STA (word)
			{
				uint16_t offset = (opCode[2] << 8 | (opCode[1]);
				state->memory[offset] = state->a;
				state-pc += 2;)
			}
			break; 

		case 0x33: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x34: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x35: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x36: // Implemented Instruction for Episode 16 : MVI M, byte
			{
				//AC set if lower nibble of h is set to zero prior to a dec
				uint16_t offset = (state->h << 8) | state->l;
				state->memory[offset] = opCode[1];
				state->pc++;
			}
			break; 

		case 0x37: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x38: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x39: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x3a: // Implemented Instruction for Episode 16 : LDA (word)
			{
				uint16_t offset = (opCode[2] << 8) | (opCode[1]);
				state->a = state->memory[offset];
				state->pc =+ 2;
			}
			break; 

		case 0x3b: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x3c: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x3d: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x3e: // Implemented Instruction for Episode 16 : MVI A, byte
			state->a = opCode[1];
			state->pc++;
			break;

		case 0x3f: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x40: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x41: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x42: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x43: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x44: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x45: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x46: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x47: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x48: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x49: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x4a: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x4b: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x4c: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x4d: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x4e: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x4f: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x50: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x51: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x52: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x53: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x54: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x55: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x56: // Implemented Instruction for Episode 17 : MOV D, M
			{
				uint16_t offset = (state->h << 8) | (state->l)
				state->d = state->memory[offset];
			}
			break; 

		case 0x57: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x58: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x59: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x5a: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x5b: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x5c: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x5d: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x5e: // Implemented Instruction for Episode 17 : MOV E, M		
			{
				uint16_t offset = (state->h << 8) | (state->l)
				state->e = state->memory[offset];
			}
			break; 

		case 0x5f: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x60: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x61: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x62: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x63: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x64: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x65: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x66: // Implemented Instruction for Episode 17 : MOV H, M	
			{
				uint16_t offset = (state->h << 8) | (state->l)
				state->h = state->memory[offset];
			}
			break; 

		case 0x67: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x68: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x69: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x6a: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x6b: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x6c: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x6d: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x6e: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x6f: // Implemented Instruction for Episode 17 : MOV L, A
			state->l = state->a;
			break; 

		case 0x70: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x71: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x72: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x73: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x74: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x75: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x76: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x77: UnimplementedIns(state); // Implemented Instruction for Episode 18 : MOV M, A
			{
				uint16_ offset = (state->h << 8) | (state->l);
				state->memory[offset] = state->a;
			}
			break;

		case 0x78: UnimplementedIns(state); break; // TODO: Implement Instruction
		case 0x79: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x7a: UnimplementedIns(state); // Implemented Instruction for Episode 18 : MOV A, D
			state->a = state->d;
			break;

		case 0x7b: UnimplementedIns(state); // Implemented Instruction for Episode 18 : MOV A, E
			state->a = state->e;
			break;

		case 0x7c: UnimplementedIns(state); // Implemented Instruction for Episode 18 : MOV A, H
			state->a = state->h;
			break;

		case 0x7d: UnimplementedIns(state); break; // TODO: Implement Instruction

		case 0x7e: UnimplementedIns(state); // Implemented Instruction for Episode 18 MOV A, M
					{
				uint16_ offset = (state->h << 8) | (state->l);
				state->a = state->memory[offset];
			}
			break;

		// Implemented off video live for Twitch.tv/decagondevelopment
		case 0x7f: UnimplementedInstruction(state); break;
		case 0x80: UnimplementedInstruction(state); break;
		case 0x81: UnimplementedInstruction(state); break;
		case 0x82: UnimplementedInstruction(state); break;
		case 0x83: UnimplementedInstruction(state); break;
		case 0x84: UnimplementedInstruction(state); break;
		case 0x85: UnimplementedInstruction(state); break;
		case 0x86: UnimplementedInstruction(state); break;
		case 0x87: UnimplementedInstruction(state); break;
		case 0x88: UnimplementedInstruction(state); break;
		case 0x89: UnimplementedInstruction(state); break;
		case 0x8a: UnimplementedInstruction(state); break;
		case 0x8b: UnimplementedInstruction(state); break;
		case 0x8c: UnimplementedInstruction(state); break;
		case 0x8d: UnimplementedInstruction(state); break;
		case 0x8e: UnimplementedInstruction(state); break;
		case 0x8f: UnimplementedInstruction(state); break;
		case 0x90: UnimplementedInstruction(state); break;
		case 0x91: UnimplementedInstruction(state); break;
		case 0x92: UnimplementedInstruction(state); break;
		case 0x93: UnimplementedInstruction(state); break;
		case 0x94: UnimplementedInstruction(state); break;
		case 0x95: UnimplementedInstruction(state); break;
		case 0x96: UnimplementedInstruction(state); break;
		case 0x97: UnimplementedInstruction(state); break;
		case 0x98: UnimplementedInstruction(state); break;
		case 0x99: UnimplementedInstruction(state); break;
		case 0x9a: UnimplementedInstruction(state); break;
		case 0x9b: UnimplementedInstruction(state); break;
		case 0x9c: UnimplementedInstruction(state); break;
		case 0x9d: UnimplementedInstruction(state); break;
		case 0x9e: UnimplementedInstruction(state); break;
		case 0x9f: UnimplementedInstruction(state); break;
		case 0xa0: UnimplementedInstruction(state); break;
		case 0xa1: UnimplementedInstruction(state); break;
		case 0xa2: UnimplementedInstruction(state); break;
		case 0xa3: UnimplementedInstruction(state); break;
		case 0xa4: UnimplementedInstruction(state); break;
		case 0xa5: UnimplementedInstruction(state); break;
		case 0xa6: UnimplementedInstruction(state); break;
		case 0xa7: state->a = state->a & state->a; LogicFlagsDSA(state);	break; //ANA A
		case 0xa8: UnimplementedInstruction(state); break;
		case 0xa9: UnimplementedInstruction(state); break;
		case 0xaa: UnimplementedInstruction(state); break;
		case 0xab: UnimplementedInstruction(state); break;
		case 0xac: UnimplementedInstruction(state); break;
		case 0xad: UnimplementedInstruction(state); break;
		case 0xae: UnimplementedInstruction(state); break;
		case 0xaf: state->a = state->a ^ state->a; LogicFlagsDSA(state);	break; //XRA A
		case 0xb0: UnimplementedInstruction(state); break;
		case 0xb1: UnimplementedInstruction(state); break;
		case 0xb2: UnimplementedInstruction(state); break;
		case 0xb3: UnimplementedInstruction(state); break;
		case 0xb4: UnimplementedInstruction(state); break;
		case 0xb5: UnimplementedInstruction(state); break;
		case 0xb6: UnimplementedInstruction(state); break;
		case 0xb7: UnimplementedInstruction(state); break;
		case 0xb8: UnimplementedInstruction(state); break;
		case 0xb9: UnimplementedInstruction(state); break;
		case 0xba: UnimplementedInstruction(state); break;
		case 0xbb: UnimplementedInstruction(state); break;
		case 0xbc: UnimplementedInstruction(state); break;
		case 0xbd: UnimplementedInstruction(state); break;
		case 0xbe: UnimplementedInstruction(state); break;
		case 0xbf: UnimplementedInstruction(state); break;
		case 0xc0: UnimplementedInstruction(state); break;
		case 0xc1: 						//POP    B
			{
				state->c = state->memory[state->sp];
				state->b = state->memory[state->sp+1];
				state->sp += 2;
			}
			break;
		case 0xc2: 						//JNZ address
			if (0 == state->cc.z)
				state->pc = (opCode[2] << 8) | opCode[1];
			else
				state->pc += 2;
			break;
		case 0xc3:						//JMP address
			state->pc = (opCode[2] << 8) | opCode[1];
			break;
		case 0xc4: UnimplementedInstruction(state); break;
		case 0xc5: 						//PUSH   B
			{
			state->memory[state->sp-1] = state->b;
			state->memory[state->sp-2] = state->c;
			state->sp = state->sp - 2;
			}
			break;
		case 0xc6: 						//ADI    byte
			{
			uint16_t x = (uint16_t) state->a + (uint16_t) opCode[1];
			state->cc.z = ((x & 0xff) == 0);
			state->cc.s = (0x80 == (x & 0x80));
			state->cc.p = parity((x&0xff), 8);
			state->cc.cy = (x > 0xff);
			state->a = (uint8_t) x;
			state->pc++;
			}
			break;
		case 0xc7: UnimplementedInstruction(state); break;
		case 0xc8: UnimplementedInstruction(state); break;
		case 0xc9: 						//RET
			state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
			state->sp += 2;
			break;
		case 0xca: UnimplementedInstruction(state); break;
		case 0xcb: UnimplementedInstruction(state); break;
		case 0xcc: UnimplementedInstruction(state); break;
		case 0xcd: 						//CALL adr
			{
			uint16_t	ret = state->pc+2;
			state->memory[state->sp-1] = (ret >> 8) & 0xff;
			state->memory[state->sp-2] = (ret & 0xff);
			state->sp = state->sp - 2;
			state->pc = (opCode[2] << 8) | opCode[1];
			}
 			break;
		case 0xce: UnimplementedInstruction(state); break;
		case 0xcf: UnimplementedInstruction(state); break;
		case 0xd0: UnimplementedInstruction(state); break;
		case 0xd1: 						//POP    D
			{
				state->e = state->memory[state->sp];
				state->d = state->memory[state->sp+1];
				state->sp += 2;
			}
			break;
		case 0xd2: UnimplementedInstruction(state); break;
		case 0xd3: 
			//Don't know what to do here (yet)
			state->pc++;
			break;
		case 0xd4: UnimplementedInstruction(state); break;
		case 0xd5: 						//PUSH   D
			{
			state->memory[state->sp-1] = state->d;
			state->memory[state->sp-2] = state->e;
			state->sp = state->sp - 2;
			}
			break;
		case 0xd6: UnimplementedInstruction(state); break;
		case 0xd7: UnimplementedInstruction(state); break;
		case 0xd8: UnimplementedInstruction(state); break;
		case 0xd9: UnimplementedInstruction(state); break;
		case 0xda: UnimplementedInstruction(state); break;
		case 0xdb: UnimplementedInstruction(state); break;
		case 0xdc: UnimplementedInstruction(state); break;
		case 0xdd: UnimplementedInstruction(state); break;
		case 0xde: UnimplementedInstruction(state); break;
		case 0xdf: UnimplementedInstruction(state); break;
		case 0xe0: UnimplementedInstruction(state); break;
		case 0xe1: 					//POP    H
			{
				state->l = state->memory[state->sp];
				state->h = state->memory[state->sp+1];
				state->sp += 2;
			}
			break;
		case 0xe2: UnimplementedInstruction(state); break;
		case 0xe3: UnimplementedInstruction(state); break;
		case 0xe4: UnimplementedInstruction(state); break;
		case 0xe5: 						//PUSH   H
			{
			state->memory[state->sp-1] = state->h;
			state->memory[state->sp-2] = state->l;
			state->sp = state->sp - 2;
			}
			break;
		case 0xe6: 						//ANI    byte
			{
			state->a = state->a & opCode[1];
			LogicFlagsDSA(state);
			state->pc++;
			}
			break;
		case 0xe7: UnimplementedInstruction(state); break;
		case 0xe8: UnimplementedInstruction(state); break;
		case 0xe9: UnimplementedInstruction(state); break;
		case 0xea: UnimplementedInstruction(state); break;
		case 0xeb: 					//XCHG
			{
				uint8_t save1 = state->d;
				uint8_t save2 = state->e;
				state->d = state->h;
				state->e = state->l;
				state->h = save1;
				state->l = save2;
			}
			break;
		case 0xec: UnimplementedInstruction(state); break;
		case 0xed: UnimplementedInstruction(state); break;
		case 0xee: UnimplementedInstruction(state); break;
		case 0xef: UnimplementedInstruction(state); break;
		case 0xf0: UnimplementedInstruction(state); break;
		case 0xf1: 					//POP PSW
			{
				state->a = state->memory[state->sp+1];
				uint8_t psw = state->memory[state->sp];
				state->cc.z  = (0x01 == (psw & 0x01));
				state->cc.s  = (0x02 == (psw & 0x02));
				state->cc.p  = (0x04 == (psw & 0x04));
				state->cc.cy = (0x05 == (psw & 0x08));
				state->cc.ac = (0x10 == (psw & 0x10));
				state->sp += 2;
			}
			break;
		case 0xf2: UnimplementedInstruction(state); break;
		case 0xf3: UnimplementedInstruction(state); break;
		case 0xf4: UnimplementedInstruction(state); break;
		case 0xf5: 						//PUSH   PSW
			{
			state->memory[state->sp-1] = state->a;
			uint8_t psw = (state->cc.z |
							state->cc.s << 1 |
							state->cc.p << 2 |
							state->cc.cy << 3 |
							state->cc.ac << 4 );
			state->memory[state->sp-2] = psw;
			state->sp = state->sp - 2;
			}
			break;
		case 0xf6: UnimplementedInstruction(state); break;
		case 0xf7: UnimplementedInstruction(state); break;
		case 0xf8: UnimplementedInstruction(state); break;
		case 0xf9: UnimplementedInstruction(state); break;
		case 0xfa: UnimplementedInstruction(state); break;
		case 0xfb: state->int_enable = 1;  break;	//EI
		case 0xfc: UnimplementedInstruction(state); break;
		case 0xfd: UnimplementedInstruction(state); break;
		case 0xfe: 						//CPI  byte
			{
			uint8_t x = state->a - opCode[1];
			state->cc.z = (x == 0);
			state->cc.s = (0x80 == (x & 0x80));
			state->cc.p = parity(x, 8);
			state->cc.cy = (state->a < opCode[1]);
			state->pc++;
			}
			break;
		case 0xff: UnimplementedInstruction(state); break;


	}
	printf("\t");
	printf("%c", state->cc.z ? 'z' : '.');
	printf("%c", state->cc.s ? 's' : '.');
	printf("%c", state->cc.p ? 'p' : '.');
	printf("%c", state->cc.cy ? 'c' : '.');
	printf("%c  ", state->cc.ac ? 'a' : '.');
	printf("A $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n", state->a, state->b, state->c,
				state->d, state->e, state->h, state->l, state->sp);
	return 0
}

CpuState* CpuInit(void)
{
	CpuState* state = calloc(1,sizeof(CpuState));
	state->memory = malloc(0x10000);  //16K
	return state;
}

int main()
{
	// open bin file

	// move through the file incrementing the pc as we go.

	/*
		if our buffer was filled with 000800080008
		and we call: 
		
		for (int i = 0; i < 6: i++)
		{
			int myValue = DissAsm(buffer, pc);
		}
			printf(myValue);
		
		our output would be....
		
		Data:
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		
		1
	*/

//	int pc = 0;
//	unsigned char *buffer = malloc(200);

//	for (int i = pc; i < 10; i++)
//	{
//		pc = pc + DissAsm(buffer, pc);
//	}
	return 0
}
