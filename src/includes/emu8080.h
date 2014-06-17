#ifndef EMU8080__H__
#define EMU8080__H__
// emu8080.h : Dragon, Part of the DSA Emu8080 Project

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct CpuCondition {
	uint8_t		cy:1;
	uint8_t		pad:1;
	uint8_t		p:1;
	uint8_t		pad2:1;
	uint8_t		ac:1;
	uint8_t		pad3:1;
	uint8_t		z:1;
	uint8_t		s:1;
} CpuCondition;


typedef struct CpuState {
	uint8_t		a;
	uint8_t		b;
	uint8_t		c;
	uint8_t		d;
	uint8_t		e;
	uint8_t		h;
	uint8_t		l;
	uint16_t	sp;
	uint16_t	pc;
	uint8_t		*memory;
	struct CpuCondition	cc;
	uint8_t		int_enable;
} CpuState;

int Emu8080Op(CpuState* state);
void GenerateInterrupt(CpuState* state, int interruptNum);

#endif
