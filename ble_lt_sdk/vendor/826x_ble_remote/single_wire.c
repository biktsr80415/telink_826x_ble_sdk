/*
 * single_wire.c
 *
 *  Created on: 2014-9-15
 *      Author: Telink
 */

#include "../../proj/tl_common.h"
#include "../../proj/drivers/keyboard.h"

extern const u8 DRIVE_PINS_CNT;
extern const u8 SCAN_PINS_CNT;
extern u32 drive_pins[];  // pins used for keyboard, work with input for forever
extern u32 scan_pins[];   // pins used for keyboard, work with input for forever

////////////////////////////////////////////////////
// swire function
////////////////////////////////////////////////////
#define IO_SWD                 0xb0
#define IO_SWC                 0xb1
#define IO_SWSPEED             0xb2

#define SWIRE_PIN              GPIO_PA7

#define WAIT_SWIRE_DONE  while (read_reg8(IO_SWC) & 3)


#define SWM_RST() do { \
    write_reg8(0x61, read_reg8(0x61)|0x80); \
    write_reg8(0x61, read_reg8(0x61)&0x7F); \
    } while (0)
#define SWM_AS_GPIO() do { \
    gpio_set_func(SWIRE_PIN, AS_GPIO); \
    gpio_set_output_en(SWIRE_PIN, 1); \
    } while (0)

#define SWM_AS_SWIRE() do { \
    gpio_set_output_en(SWIRE_PIN, 1); \
    gpio_set_func(SWIRE_PIN, AS_SWM); \
    } while (0)

#define SWM_OUT_HIGH() do { \
    gpio_write(SWIRE_PIN, 1); \
    } while (0)

#define SWM_OUT_LOW() do { \
    gpio_write(SWIRE_PIN, 0); \
    } while (0)

void swire_speed(unsigned char data)
{
    write_reg8(IO_SWSPEED, data & 0x7F);
}

int swire_sync (int usb)
{
    //reset SWIRE and release reset
    SWM_RST();
    SWM_AS_GPIO();

    if (usb) {
        SWM_OUT_LOW();
        sleep_us(40000);                    //wait 40ms
        SWM_OUT_HIGH();
        sleep_us(80000);        //wait 80ms
        SWM_OUT_LOW();
        sleep_us(40000);        //wait 40ms
        SWM_OUT_HIGH();
        sleep_us(20000);        //wait 20ms
    }
    SWM_OUT_HIGH();
    sleep_us(10);

    int i;
    for (i = 0; i < 12; i ++) {
        SWM_OUT_LOW();
        sleep_us(4);
        SWM_OUT_HIGH();
        sleep_us(1);
    }
    SWM_AS_SWIRE();
    return 1;
}

void swire_write (unsigned short adr, unsigned char *ptr, int cnt)
{
    int i;
    unsigned short  adr_h = (adr & 0xff00)>>8;
    SWM_RST();

    write_reg8(IO_SWD, 0xff);
    write_reg8(IO_SWC, 0x05);
    WAIT_SWIRE_DONE;

    write_reg8(IO_SWD, 0x5a);
    write_reg8(IO_SWC, 0x05);
    WAIT_SWIRE_DONE;

    write_reg8(IO_SWD, adr_h);
    write_reg8(IO_SWC, 0x01);
    WAIT_SWIRE_DONE;

    write_reg8(IO_SWD, adr&0xff);
    write_reg8(IO_SWC, 0x01);
    WAIT_SWIRE_DONE;

    write_reg8(IO_SWD, 0x00);    //0 for write
    write_reg8(IO_SWC, 0x01);
    WAIT_SWIRE_DONE;

    for(i = 0; i < cnt; i ++) {
        write_reg8(IO_SWD, *(ptr++));
        write_reg8(IO_SWC, 0x01);
        WAIT_SWIRE_DONE;
    }

    write_reg8(IO_SWD, 0xff);
    write_reg8(IO_SWC, 0x05);
    WAIT_SWIRE_DONE;
}

int swire_read (unsigned short adr, unsigned char *ptr, int cnt)
{
    int i;
    unsigned short adr_h = (adr & 0xff00) >> 8;
    SWM_RST();

    write_reg8(IO_SWD, 0xff);
    write_reg8(IO_SWC, 0x05);
    WAIT_SWIRE_DONE;

    write_reg8(IO_SWD, 0x5a);
    write_reg8(IO_SWC, 0x05);
    WAIT_SWIRE_DONE;

    write_reg8(IO_SWD, adr_h);
    write_reg8(IO_SWC, 0x01);
    WAIT_SWIRE_DONE;

    write_reg8(IO_SWD, adr & 0xff);
    write_reg8(IO_SWC, 0x01);
    WAIT_SWIRE_DONE;

    write_reg8(IO_SWD, 0x80);    //0 for write
    write_reg8(IO_SWC, 0x01);
    WAIT_SWIRE_DONE;

    for(i = 0; i < cnt; i ++) {
        write_reg8(IO_SWC, 0x02);
        unsigned long t = clock_time();
        while (clock_time () - t < 300000) {
            if (!(read_reg8(IO_SWC) & 2)) {
                *ptr = read_reg8 (IO_SWD);
                ptr++;
                break;
            }
        }
        if(clock_time () - t >= 300000) {
            SWM_RST();
            swire_sync(0);
            return i;
        }
    }
    write_reg8(IO_SWD, 0xff);
    write_reg8(IO_SWC, 0x05);
    WAIT_SWIRE_DONE;

    return cnt;
}

void reset_swm_for_keyboard()
{
    u8 i, up_down, find = 0;
    for (i = 0; i < DRIVE_PINS_CNT; i ++) {
        if (drive_pins[i] != SWIRE_PIN)
            continue;
        up_down = MATRIX_ROW_PULL;
        find = 1;
        break;
    }

    do {
        if (find)
            break;
        for (i = 0; i < SCAN_PINS_CNT; i ++) {
            if (scan_pins[i] != SWIRE_PIN)
                continue;
            up_down = MATRIX_COL_PULL;
            find = 1;
            break;
        }
    } while (0);

    do {
        if (find == 0)
            break;
        gpio_set_func(SWIRE_PIN, AS_GPIO);
        gpio_set_input_en(SWIRE_PIN, 1);
        gpio_set_output_en(SWIRE_PIN, 0);
        gpio_setup_up_down_resistor(SWIRE_PIN, up_down);
    } while (0);
}

#if 0
int cmd_func_swire_sync(void)
{
    SwireSync(1);
    return 0;
}

int cmd_func_ram_read(void)
{
    int adr = evk_str2n(evk_argv[1]);
    char * var = (evk_argv[2]);
    int idx = evk_str2n(evk_argv[3]);
    //unsigned char buf[2];
    DEBUG_OUT(1,"\tv[%d] = ram_read(%x)\n",idx,adr);
    evk_r[0] = 0;//@2017-5-27 add for new write_log .
    if(idx<0 || idx>=EVK_V_MAX)
    {
        DEBUG_OUT(1,"\tindex of v[] is out of range!\n");
        return  err_flg_abnormal;
    }

#if(EVK_HUB_CLIENT)
    if(evk_filenamencmp (var,"v",4)==0)  // if the format is v[]
    {
        //DEBUG_OUT(1,"\tread begin:");
        unsigned char temp[4];
        //if(SwireReadBytes(adr,evk_v+idx,1)==1)
        if(SwireReadBytes(adr,temp,1)==1)
        {
            //evk_v[idx] = (int)(*temp);
            evk_v[idx] = temp[0];
            evk_b[0]   = evk_v[idx];//@2017-5-18 add .
            DEBUG_OUT(1,"\tv[%d] is [%x]\n",idx,evk_v[idx]);
            evk_r[0] = 1;
            return 0;
        }
        evk_r[0] = 2;
    }
#else
    if(evk_filenamencmp (var,"v",4)==0)  // if the format is v[]
    {
        //DEBUG_OUT(1,"\tread begin:");
        unsigned char temp[4];
        //if(SwireReadBytes(adr,evk_v+idx,1)==1)
        if(SwireReadBytes(adr,temp,1)==1)
        {
            //evk_v[idx] = (int)(*temp);
            evk_v[idx] = temp[0];
            DEBUG_OUT(1,"\tv[%d] is [%x]\n",idx,evk_v[idx]);
            return 0;
        }
    }
#endif

    return  err_flg_abnormal;
}

int cmd_func_ram_write(void)
{
    int adr = evk_str2n(evk_argv[1]);
    int dat = evk_str2n(evk_argv[2]);
    unsigned char buf[2];
    //unsigned char check[2];


    buf[0] = (unsigned char) (dat & 0xff);

    DEBUG_OUT(1,"\t ram write: adr[%x] dat [%x]\n",adr,buf[0]);

    SwireWriteBytes(adr,buf,1);


    return 0;
}

int cmd_func_dut_reset(void)
{
    u8 temp_buff[4];
    int i;
    DEBUG_OUT(1,"\tReset dut!\n");
        //*((volatile unsigned char  *)(0x800066)) = 0x08;
        //*((volatile unsigned char  *)(0x80006f)) = 0x20;
    temp_buff[0]=0x08;
    for(i=0;i<50;i++)
    {
        SwireWriteBytes(0x66,temp_buff,1);  //try to reset dut
    }

    temp_buff[0]=0x20;
    for(i=0;i<50;i++)
    {
        SwireWriteBytes(0x6f,temp_buff,1);  //try to reset dut
    }
    return 0;
}
#endif
