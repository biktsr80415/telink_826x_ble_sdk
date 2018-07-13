
#include "bsp.h"
#include "clock.h"
#include "analog.h"

int LoadTblCmdSet (	const TBLCMDSET * pt, int size){
	int l=0;

	while (l<size) {
		unsigned int  cadr = ((unsigned int)0x800000) | pt[l].adr;
		unsigned char cdat = pt[l].dat;
		unsigned char ccmd = pt[l].cmd;
		unsigned char cvld =(ccmd & TCMD_UNDER_WR);
		ccmd &= TCMD_MASK;
		if (cvld) {
			if (ccmd == TCMD_WRITE) {
				write_reg8 (cadr, cdat);
			}
			else if (ccmd == TCMD_WAREG) {
				WriteAnalogReg (cadr, cdat);
			}
			else if (ccmd == TCMD_WAIT) {
				WaitUs (pt[l].adr*256 + cdat);
			}
		}
		l++;
	}
	return size;

}




void sub_wr_ana(unsigned int addr, unsigned char value, unsigned char e, unsigned char s)
{
	unsigned char v, mask, tmp1, target, tmp2;

	v = ReadAnalogReg(addr);
	mask = BIT_MASK_LEN(e - s + 1);
	tmp1 = value & mask;

	tmp2 = v & (~BIT_RNG(s,e));

	target = (tmp1 << s) | tmp2;
	WriteAnalogReg(addr, target);
}



void sub_wr(unsigned int addr, unsigned char value, unsigned char e, unsigned char s)
{
	unsigned char v, mask, tmp1, target, tmp2;

	v = read_reg8(addr);
	mask = BIT_MASK_LEN(e - s + 1);
	tmp1 = value & mask;

	tmp2 = v & (~BIT_RNG(s,e));

	target = (tmp1 << s) | tmp2;
	write_reg8(addr, target);
}
