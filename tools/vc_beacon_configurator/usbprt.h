/* Code to find the device path for a usbprint.sys controlled 
 * usb printer and print to it
 */


HANDLE GetPrintDeviceHandle(unsigned short id);

int WriteUSBReg(HANDLE hdev, int addr, int dat);
int ReadUSBReg(HANDLE hdev, int addr, int  & dat);

int WriteUSBMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo, int maxlen);
int ReadUSBMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo, int maxlen);

int WriteMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int type);
int ReadMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int type);
int ReadUartMem(HANDLE hdev, LPBYTE lpB);