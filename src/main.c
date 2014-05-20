/* main.c */
#include <stdio.h>
#include <stdlib.h>

	void DissAsm(unsigned char *buff, int pc) 
	{
		char *code = buff[pc];
		int opCodeSize = 1;
		printf("Data: \n");
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
			
		}
		
		printf("\n")
		return opCodeSize;
	}
		
	int main()
	{
		// open bin file
		
		
		// read bin file in to the *buffer (000800080008)
		
		
		
		
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
		
		int pc = 0;
		unsigned char *buffer = malloc(200);
		
		for (int i = pc; i < 10; i++)
		{
			pc = pc + DissAsm(buffer, pc);
		}
	}
