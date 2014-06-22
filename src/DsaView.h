// DsaView.h : part of the DSA Emu 8080 Project
//

#import <Cocoa/Cocoa.h>
#import "DsaMachine.h"

@interface DsaView : NSView
{
	NSTimer *dsaTimer;
	DsaMachine *dsaInvaders;

	CGContextRef bitmapContext;
	unsigned char *dsaBuffer;
}

- (void)timerFired:(id)sender;

@end
