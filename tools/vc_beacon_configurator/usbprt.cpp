/* Code to find the device path for a usbprint.sys controlled 
 * usb printer and print to it
 */

#include "stdafx.h"
#include <setupapi.h>

#include <winioctl.h>
#include "usbprt.h"

#define USBPRINT_IOCTL_INDEX  0x0000


#define IOCTL_USBPRINT_GET_LPT_STATUS  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+12,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_USBPRINT_GET_1284_ID     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+13,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_USBPRINT_VENDOR_SET_COMMAND CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+14,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_USBPRINT_VENDOR_GET_COMMAND CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+15,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_USBPRINT_SOFT_RESET         CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+16,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           


/* This define is required so that the GUID_DEVINTERFACE_USBPRINT variable is
 * declared an initialised as a static locally, since windows does not include it in any
 * of its libraries
 */
/*
#define SS_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
static const GUID DECLSPEC_SELECTANY name \
= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

SS_DEFINE_GUID(GUID_DEVINTERFACE_USBPRINT, 0x28d78fad, 0x5a12, 0x11D1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);
*/

//GUID DECLSPEC_SELECTANY GUID_DEVINTERFACE_USBPRINT = { 0x28d78fad, 0x5a12, 0x11D1, { 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2 } };
GUID GUID_DEVINTERFACE_USBPRINT = { 0x28d78fad, 0x5a12, 0x11D1, { 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2 } };

extern HANDLE NULLEVENT2;
HANDLE GetPrintDeviceHandle(unsigned short id)
{
  HDEVINFO devs;
  DWORD devcount;
  SP_DEVINFO_DATA devinfo;
  SP_DEVICE_INTERFACE_DATA devinterface;
  DWORD size;
  GUID intfce;
  PSP_DEVICE_INTERFACE_DETAIL_DATA interface_detail;
  HANDLE usbHandle = 0;
  HANDLE last_handle = 0;

  intfce = GUID_DEVINTERFACE_USBPRINT;
  devs = SetupDiGetClassDevs(&intfce, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if (devs == INVALID_HANDLE_VALUE) {
    return NULL;
  }
  devcount = 0;
  devinterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
  while (SetupDiEnumDeviceInterfaces(devs, 0, &intfce, devcount, &devinterface)) {
    /* The following buffers would normally be malloced to he correct size
     * but here we just declare them as large stack variables
     * to make the code more readable
     */
    char driverkey[2048];
    char interfacename[2048];
    char location[2048];
    //char description[2048];

    /* If this is not the device we want, we would normally continue onto the next one
     * so something like if (!required_device) continue; would be added here
     */
    devcount++;
    size = 0;
    /* See how large a buffer we require for the device interface details */
    SetupDiGetDeviceInterfaceDetail(devs, &devinterface, 0, 0, &size, 0);
    devinfo.cbSize = sizeof(SP_DEVINFO_DATA);
    interface_detail = (struct _SP_DEVICE_INTERFACE_DETAIL_DATA_A *)calloc(1, size);
    if (interface_detail) {
      interface_detail->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
      devinfo.cbSize = sizeof(SP_DEVINFO_DATA);
      if (!SetupDiGetDeviceInterfaceDetail(devs, &devinterface, interface_detail, size, 0, &devinfo)) {
		free(interface_detail);
		SetupDiDestroyDeviceInfoList(devs);
		return NULL;
      }
      /* Make a copy of the device path for later use */
      strcpy(interfacename, interface_detail->DevicePath);
      free(interface_detail);
      /* And now fetch some useful registry entries */
      size = sizeof(driverkey);
      driverkey[0] = 0;
      //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_DRIVER, &dataType, (LPBYTE)driverkey, size, 0)) {
	  if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_DRIVER, NULL, (LPBYTE)driverkey, size, 0)) {
		SetupDiDestroyDeviceInfoList(devs);
		return NULL;
      }
      size = sizeof(location);
      location[0] = 0;
      //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_LOCATION_INFORMATION, &dataType, (LPBYTE)location, size, 0)) {
	  //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_LOCATION_INFORMATION, NULL, (LPBYTE)location, size, 0)) {
		//SetupDiDestroyDeviceInfoList(devs);
		//return NULL;
      //}
	char * str_tlk = strstr (interfacename, "vid_248a&pid_");
	if (str_tlk) {
		last_handle = usbHandle;
		usbHandle = CreateFile(interfacename, 
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL, 
						OPEN_ALWAYS, 
						0, //FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
						//FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
						NULL);
	  }
      // usbHandle = CreateFile(interfacename, GENERIC_WRITE, FILE_SHARE_READ,
	  	//		     NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (usbHandle != INVALID_HANDLE_VALUE) {
		unsigned int dev = 0;
		ReadMem (usbHandle, 0x7e, (LPBYTE) &dev, 4, 2);
		if ((dev == id) || (id == 0xffff)) {
			SetupDiDestroyDeviceInfoList(devs);
			  return usbHandle;
		}
     }
    }
  }
  SetupDiDestroyDeviceInfoList(devs);
	return 0;
}


int WriteUSBMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
  
	DWORD	nBytes;
	 
	BYTE	buff[4096];
	if (fifo)
		buff[0] = 0x03;
	else
		buff[0] = 0x02;
	buff[1] = (addr>>8) & 0xff;
	buff[2] = addr & 0xff;
	buff[3] = lpB[0];
	buff[4] = lpB[0];
	buff[5] = lpB[0];
	buff[6] = lpB[0];
	buff[7] = lpB[0];
	

	int nW = (len > maxlen ? maxlen : len);
	for (int i=0; i<nW; i++)
		buff[8+i] = lpB[i];
	

	int bRet = DeviceIoControl(
		hdev, 
		//IOCTL_USBPRINT_GET_1284_ID, // dwIoControlCode
		IOCTL_USBPRINT_VENDOR_SET_COMMAND,	// dwIoControlCode
		buff,					// lpInBuffer
		nW+8,									// nInBufferSize
		NULL,
		0,
		(LPDWORD)  &nBytes,
		NULL 
	);
	if (!bRet)
		return 0;
	else
		return nW;
}

int ReadUSBMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
  
	DWORD	nBytes;
	BYTE	buff[16];
	if (fifo)
		buff[0] = 0x03;
	else
		buff[0] = 0x02;
	buff[1] = (addr>>8) & 0xff;
	buff[2] = addr & 0xff;
	buff[3] = lpB[0];
	
	DWORD nR = 8 + (len > maxlen ? maxlen :  len<1 ? 1 : len);
	
  
	//HANDLE hh = GetPrintDeviceHandle ();
	int bRet = DeviceIoControl(
		hdev, 
		//IOCTL_USBPRINT_GET_1284_ID, // dwIoControlCode
		IOCTL_USBPRINT_VENDOR_GET_COMMAND,	// dwIoControlCode
		buff,					// lpInBuffer
		16,									// nInBufferSize
		lpB,
		nR,
		(LPDWORD)  &nBytes,
		NULL 
	);

	//if (nBytes != nR - 8 || (lpB[nBytes-1]&4))
	if (!bRet || nBytes != nR - 8)
		return 0;
	else
		return nBytes;
}

int LogMsg (LPSTR sz,...);

int WriteUSBMemCheck(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
	BYTE rbuff[4096];
	int nc = 10;
	int l = 0;
	int retw, retr;

	while (l++<nc) { 
		retw = WriteUSBMem (hdev, addr, lpB, len, fifo, maxlen);
		if (len <= 8) 
			return retw;
		
		int ok = 1;
		retr = ReadUSBMem  (hdev, addr, rbuff, len, fifo, maxlen);
		for (int k=0; k<len; k++) {
			if (lpB[k] != rbuff[k]) {
				ok = 0;
				//LogMsg ("USB write error at adrress %x, retry", addr + k);
				break;
			}
		}
		if (ok) 
			return retw;
		else
			ok = ok;
	}
	return 0;
}

int WriteMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int type=1)
{
  
	BYTE ah = addr>>16;
	//WriteI2CMem (hdev, 0xffff, &ah, 1, 1);
	int ret = 0;

	int al = addr & 0xffff;
	int step = 1024;
	//int step = 3072;

	for (int i=0; i<len; i+=step) {
		int n = len - i > step ? step : len - i;
		int fadr = al + i;
		if (type & 0x200) fadr = al;
		//ret += WriteUSBMem (hdev, fadr, lpB+i, n, type & 0x200, step);
		int rw = WriteUSBMemCheck (hdev, fadr, lpB+i, n, type & 0x200, step);
		if (rw)
			ret += rw;
		else 
			break;
	}
	return ret;
}

int ReadMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int type=1)
{
  
	BYTE ah = addr>>16;
	//WriteI2CMem (hdev, 0xff08, &ah, 1, 1);
	int ret = 0;

	int al = addr & 0xffff;
	//int step = 32;
	int step = 3072;

	for (int i=0; i<len; i+=step) {
		int n = len - i > step ? step : len - i;
		int fadr = al + i;
		if (type & 0x200) fadr = al;
		ret += ReadUSBMem (hdev, fadr, lpB+i, n, type & 0x200, step);
	}
	return ret;
}

int ReadUartMem(HANDLE hdev, LPBYTE lpB)
{
  
	DWORD	nB;
	int ret = ReadFile(hdev, lpB, 2048, &nB, NULL);
	if (ret==0)
		return -1;
	else
		return nB;
	
}

int WriteUSBReg(HANDLE hdev, int addr, int dat)
{
  
	BYTE buff[16];
	*((int *) buff) = dat;
	
	int bRet = WriteUSBMem(hdev, addr, buff, 1);
	return bRet;
}

int ReadUSBReg(HANDLE hdev, int addr, int  & dat)
{
  
	BYTE buff[16];
	int bRet = ReadUSBMem(hdev, addr, buff, 1);
	dat = buff[0];
	return bRet;
}
