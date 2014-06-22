// DsaView.m

#import "DsaView.h"

#define RGB_ON 0xffffffffL
#define RGB_OFF 0x00000000L

@implement DsaView

- (id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame]
		if (self)
		{
	`		dsaInvaders = [[DsaMachine alloc] init];
			CGColorSpaceRef colourSpace = CGColorspaceCreateDeviceRGB();
			dsaBuffer = malloc(4 * 224 * 256);
			bitmapContext = CGBitmapContextCreate(dsaBuffer, 224, 256, 8, 224 * 4, colourSpace, kCGImageAlphaNoneSkipFirst);

			dsaTimer = [NSTimer timerWithTimeInterval:0.016
				    target:self
				    selector:@selector(timerFired:)
				    userInfo:nil
				    repeats:YES];
			[[NSRunLoop cur
