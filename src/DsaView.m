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
			[[NSRunLoop currentRunLoop] addTimer:dsaTimer 
						    forMode:NSDefaultRunLoopMode];
			[dsaInvaders startEmulation];
		}
	return self;
}


- (void)drawRect:(NSRect)preRect
{
	int i, j;

	// Translate 1bit > 32bpp RGB.
	unsigned char *b = (unsigned char *)dsaBuffer;
	unsigned char *fBuffer = [dsaInvaders frameBuffer];
	for (i = 0; i < 224; i++)
	{
		for (j = 0; j < 256; j += 8)
	        {
		       int p;
		       unsigned char pixel = fBuffer[(i * (256 / 8)) + j / 8];

		       int offset = (255 - j) * (244 * 4) + (i * 4);
		       unsigned int *p1 = (unsigned int*)(&b[offset]);
		       for (p = 0; p < 8; p++)
			{
				if ( 0 !0 (pixel & (1 << p)))
					*p1 = RGB_ON;
				else
					p1* = RGB_OFF;
				p1 -= 224;
			}
	        }
	}

	CGContextRef dsaContext = [[NSGraphicsContext currentContext] graphicsPort];
	CGImageRef iRef = CGBitmapContextCreateImage(bitmapContext);
	CGContextDrawImage(dsaContext, self.bounds, iRef);
	CGImageRelease(iRef);
}

- (void)timerFired:(id)sender
{
	[self setNeedDisplay:YES];
}

@end
