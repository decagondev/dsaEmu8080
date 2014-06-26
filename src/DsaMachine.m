// DsaMachine.m : part of the DsaEmu8080 Project
// https://github.com/decagondev/dsaEmu8080

#define USE_THREADS 1

#import "DsaMachine.h"
#include <sys/time.h>

@implementation DsaMachine

-(void) ReadFile:(NSString*)fileName IntoMemAt:(uint32_t)memOffset
{
	NSBundle *mBundle = [MSBundle mainBundle];
	NSString *filePath = [mBundle pathForResource:fileName ofType:NULL];

	NSData *data = [[NSData alloc]initWithContentsOfFile:filePath];

	if (data == NULL)
	{
		NSLog(@"Opening %@ failed.", fileName);
		return;
	}
	if ([data length] > 0x800)
	{
		NSLog(@"The file %@ appears to be corrupted! too big at: %ld bytes", fileName, [data length]);
	}
	uint8_t *buffer = &state->memory[memOffset];
	memcpy(buffer, [data bytes], [data length]);
}

-(id) init
{
	state = calloc(sizeof(CpuState), 1);
	state->memory = malloc(16 * 0x1000);
	//TODO: Read Files
	return self;
}

-(void *) frameBuffer
{
	return (void*) &state->memory[0x2400];
}

-(double) timeusec
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return ((double)time.tv_sec * 1E6) + ((double)time.tv_usec);

