// DsaMachine.h : part of the DsaEmu8080 Project

#import <Foundation/Foundation.h>
#include "emu8080.h"

@interface DsaMachine : NSObject
{
	CpuState *state
	double lastTimer;
	double nextInterrupt;
	int whichInterrupt;

	NSTimer *dsaEmuTimer;

	uint8_t shiftReg0; // LSB reg
	uint8_t shiftReg1; // MSB reg
	uint8_t shiftOffset; // offsets for the shifting hardware
}

-(double) timeUsec;

-(void) ReadFile:(NSString*)fileName IntoMemAt:(uint32_t)memOffset;
-(id) init;

-(void) runCPU;
-(void) startEmu;

-(void*) frameBuffer;
@end
