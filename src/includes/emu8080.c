#include "emu8080.h"

unsigned char cycles8080[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
	
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
	
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, 
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11, 
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11, 
};

//parity function
static int parity(int x, int size)
{
	int i;
	int p = 0;
	x = (x & ((1<<size)-1));
	for (i=0; i<size; i++)
	{
		if (x & 0x1) p++;
		x = x >> 1;
	}
	return (0 == (p & 0x1));
}

static void LogicFlagsDSA(CpuState *state)
{
	state->cc.cy = state->cc.ac = 0;
	state->cc.z = (state->a == 0);
	state->cc.s = (0x80 == (state->a & 0x80));
	state->cc.p = parity(state->a, 8);
}

static void ArithmaticFlagsDSA(CpuState *state, uint16_t res)
{
	state->cc.cy = (res > 0xff);
	state->cc.z = ((res&0xff) == 0);
	state->cc.s = (0x80 == (res & 0x80));
	state->cc.p = parity(res&0xff, 8);
}

static void UnimplementedIns(CpuState* state)
{
	//pc will have advanced one, so undo that
	printf ("Error: Unimplemented instruction\n");
	state->pc--;
	DissAsm(state->memory, state->pc);
	printf("\n");
	exit(1);
}


static void WriteMem(CpuState *state, uint16_t addr, uint8_t val)
{
	state->memory[addr] = val;
}

static uint8_t ReadFromHL(CpuState *state)
{
	uint16_t offset = (state->h << 8) | state->l;
	return state->memory[offset];
}

static uint8_t WriteToHL(CpuState *state, uint8_t val)
{
	uint16_t offset = (state->h << 8) | state->l;
	WriteMem(state, offset, val);
}



static void Push(CpuState *state, uint8_t high, uint8_t low)
{
	WriteMem(state, state->sp-1, high);
	WriteMem(state, state->sp-2, low);
	state->sp = state->sp -2;

}

static void Pop(CpuState *state, uint8_t *high, uint8_t *low)
{
	*low = state->memory[state->sp];
	*high = state->memory[state->sp+1];
	state->sp += 2;
}

static void FlagsZSP(CpuState *state, uint8_t val)
{
	state->cc.z = (val == 0);
	state->cc.s (0x80 == (val & 0x80));
	state->cc.p = parity(val, 8);
}	


void GenerateInterrupt(CpuState* state, int interruptNum)
{
	// perform "PUSH PC"
	Push(state, (state->pc & 0xff00) >> 8, (state->pc & 0xff));
	//Set PC to low memory Vector.
	state->pc = 8 * interruptNum;

	// "DI"
	state->int_enable = 0;
}
