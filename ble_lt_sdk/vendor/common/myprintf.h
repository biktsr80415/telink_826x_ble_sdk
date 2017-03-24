#ifndef MYPRINTF_H
#define MYPRINTF_H

#if (PRINT_DEBUG_INFO)

void my_array_printf(char*data, int len);
int Tl_printf(const char *format, ...);

#define printf			Tl_printf
#define	printfArray		arrayPrint
//#define	logPrint		Tl_printf
//#define	logPrintArray	arrayPrint
#define	arrayPrint(arrayAddr,len)					\
{													\
	Tl_printf("\n*********************************\n");		\
	unsigned char	i = 0;							\
	do{												\
		Tl_printf(" %x",((unsigned char *)arrayAddr)[i++]);						\
	}while(i<len);										\
	Tl_printf("\n*********************************\n");		\
}
#else
#define printf
#define	printfArray
#endif

//#define debugBuffer (*(volatile unsigned char (*)[40])(0x8095d8))
#endif
