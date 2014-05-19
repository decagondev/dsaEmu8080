/* main.c */
#include <stdio.h>
#include <stdlib.h>

	void myFunction() 
	{
		char *code;
		int opCodeSize = 1;
		switch (*code)
		{
			case 0x00: printf("NOP"); break;
			case 0x01: printf("LXI    B,#$%02x%02x", code[2], code[1]); opCodeSize = 3; break;
			case 0x02: printf("STAX   B"); break;
			case 0x03: printf("INX    B"); break;
			case 0x04: printf("INR    B"); break;
			case 0x05: printf("DCR    B"); break;
			case 0x06: printf("MVI    B,#$%02x", code[1]); opCodeSize = 2; break;
			case 0x07: printf("RLC"); break;
			case 0x08: printf("NOP"); break;
			case 0x09: printf("DAD    B"); break;
			case 0x0a: printf("LDAX   B"); break;
			case 0x0b: printf("DCX    B"); break;
			case 0x0c: printf("INR    C"); break;
			case 0x0d: printf("DCR    C"); break;
			case 0x0e: printf("MVI    C,#$%02x", code[1]); opCodeSize = 2;	break;
			case 0x0f: printf("RRC"); break;
			
			case 0x10: printf(); break;
		}
		
		
		
	}
		
