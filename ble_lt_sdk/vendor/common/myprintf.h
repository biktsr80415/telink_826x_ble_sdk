#ifndef MYPRINTF_H
#define MYPRINTF_H

#if (PRINT_DEBUG_INFO)

int mini_printf(const char *format, ...);
int mini_sprintf(char *out, const char *format, ...);
void PrintHex(unsigned char x);

#define printf			mini_printf
#define	printfArray		arrayPrint
#define sprintf         mini_sprintf

#define	arrayPrint(arrayAddr,len)					\
{													\
	unsigned char	i = 0;							\
	do{												\
		mini_printf("%x%s", ((unsigned char *)arrayAddr)[i++], i<len? "-":" "); \
	}while(i<len); \
	mini_printf("\n");	\
}
#else
#define printf
#define	printfArray
#define	PrintHex
#define sprintf
#endif

//#define debugBuffer (*(volatile unsigned char (*)[40])(0x8095d8))
#endif
