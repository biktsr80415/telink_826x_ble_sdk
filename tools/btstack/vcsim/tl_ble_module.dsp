# Microsoft Developer Studio Project File - Name="tl_ble_module" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=tl_ble_module - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tl_ble_module.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tl_ble_module.mak" CFG="tl_ble_module - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tl_ble_module - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "tl_ble_module - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 setupapi.lib /nologo /subsystem:windows /machine:I386 /out:"Release/tl_bulk.exe"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 setupapi.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/tl_bulk.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tl_ble_module - Win32 Release"
# Name "tl_ble_module - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "btstack"

# PROP Default_Filter "*.c"
# Begin Group "ble"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\btstack\src\ble\ad_parser.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\ad_parser.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\ancs_client.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\ancs_client.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\att_db.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\att_db.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\att_db_util.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\att_db_util.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\att_dispatch.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\att_dispatch.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\att_server.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\att_server.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\core.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\gatt_client.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\gatt_client.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\le_device_db.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\le_device_db_dummy.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\le_device_db_memory.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\sm.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\ble\sm.h
# End Source File
# End Group
# Begin Group "classic"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\btstack\src\classic\bnep.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\bnep.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\btstack_link_key_db.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\btstack_link_key_db_memory.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\btstack_link_key_db_memory.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\core.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hfp.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hfp.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hfp_ag.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hfp_ag.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hfp_gsm_model.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hfp_gsm_model.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hfp_hf.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hfp_hf.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hsp_ag.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hsp_ag.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hsp_hs.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\hsp_hs.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\pan.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\pan.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\rfcomm.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\rfcomm.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\sdp_client.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\sdp_client.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\sdp_client_rfcomm.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\sdp_client_rfcomm.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\sdp_server.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\sdp_server.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\sdp_util.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\sdp_util.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\spp_server.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\classic\spp_server.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\btstack\src\bluetooth.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_chipset.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_config.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_control.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_debug.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_defines.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_event.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_link_key_db_fs.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_linked_list.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_linked_list.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_memory.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_memory.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_memory_pool.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_memory_pool.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_run_loop.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_run_loop.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_run_loop_posix.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_run_loop_posix.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_slip.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_slip.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_uart_block.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_util.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\btstack_util.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\gap.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci_cmd.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci_cmd.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci_dump.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci_dump.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci_transport.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci_transport_h2_libusb.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci_transport_h4.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\hci_transport_h5.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\l2cap.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\l2cap.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\l2cap_signaling.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\l2cap_signaling.h
# End Source File
# Begin Source File

SOURCE=.\btstack\src\le_counter.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\main.cpp

!IF  "$(CFG)" == "tl_ble_module - Win32 Release"

!ELSEIF  "$(CFG)" == "tl_ble_module - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\btstack\src\stdint.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ScanDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\tl_ble_module.cpp
# End Source File
# Begin Source File

SOURCE=.\tl_ble_module.rc
# End Source File
# Begin Source File

SOURCE=.\tl_ble_moduleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\usbprt.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "btstack_h"

# PROP Default_Filter "*.h"
# End Group
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScanDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tl_ble_module.h
# End Source File
# Begin Source File

SOURCE=.\tl_ble_moduleDlg.h
# End Source File
# Begin Source File

SOURCE=.\usbprt.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\tl_ble_module.ico
# End Source File
# Begin Source File

SOURCE=.\res\tl_ble_module.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
