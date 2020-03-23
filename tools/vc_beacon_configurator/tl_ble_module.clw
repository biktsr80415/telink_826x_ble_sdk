; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTl_ble_moduleDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "tl_ble_module.h"

ClassCount=3
Class1=CTl_ble_moduleApp
Class2=CTl_ble_moduleDlg

ResourceCount=3
Resource2=IDD_TL_BLE_MODULE_DIALOG
Resource1=IDR_MAINFRAME
Class3=CScanDlg
Resource3=IDD_TL_SCAN_DLG

[CLS:CTl_ble_moduleApp]
Type=0
HeaderFile=tl_ble_module.h
ImplementationFile=tl_ble_module.cpp
Filter=N

[CLS:CTl_ble_moduleDlg]
Type=0
HeaderFile=tl_ble_moduleDlg.h
ImplementationFile=tl_ble_moduleDlg.cpp
Filter=D
LastObject=IDC_OTA2
BaseClass=CDialog
VirtualFilter=dWC



[DLG:IDD_TL_BLE_MODULE_DIALOG]
Type=1
Class=CTl_ble_moduleDlg
ControlCount=26
Control1=IDC_CLOSEDLG,button,1342242817
Control2=IDOK,button,1073807361
Control3=IDC_LOGCLEAR,button,1342242816
Control4=IDC_COMMAND,listbox,1084293377
Control5=IDC_INIFILE,combobox,1344340226
Control6=IDC_LOG,edit,1353713860
Control7=IDC_OPEN,button,1342242816
Control8=IDC_SCAN,button,1073807360
Control9=IDC_CHANNEL,combobox,1075904514
Control10=IDC_CONN_INTERVAL,edit,1082196096
Control11=IDC_CONN_TIMEOUT,edit,1082196096
Control12=IDC_CONN_CHNMASK,edit,1082196096
Control13=IDC_STOP,button,1073807360
Control14=IDC_LOGSAVE,button,1342242816
Control15=IDC_LOGSAVEFILE,button,1342242816
Control16=IDC_LOGEN,button,1342242819
Control17=IDC_TEXT_BO,edit,1350635652
Control18=IDC_BULKOUT,button,1342242816
Control19=IDC_HEX,button,1342242819
Control20=IDC_CMDINPUT,edit,1350631552
Control21=IDC_CMDSEL,button,1342242816
Control22=IDC_Device,button,1342177280
Control23=IDC_OTA1,button,1342177280
Control24=IDC_OTA2,button,1342177280
Control25=IDC_OTA1SART,button,1342177280
Control26=IDC_OTA2START,button,1342177280

[DLG:IDD_TL_SCAN_DLG]
Type=1
Class=CScanDlg
ControlCount=3
Control1=IDCANCEL,button,1342242816
Control2=IDC_SCANLIST,listbox,1352728833
Control3=IDC_CONNECT,button,1342242816

[CLS:CScanDlg]
Type=0
HeaderFile=ScanDlg.h
ImplementationFile=ScanDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_SCANLIST
VirtualFilter=dWC

