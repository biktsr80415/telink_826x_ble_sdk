echo off
set /P anykey=connect beacon to SWB, then press any key to continue

echo "Erase flash ..."
tcdb.exe wf 0 -e -s 128k 

echo "Download OTA boot code to address 0x1a000 ..."
tcdb.exe wf 1a000 -eb -i 8261_ota_boot.bin 

echo "Download OTA boot code to address 0x1a000 ..."
tcdb.exe wf 0 -eb -i 8261_beacon.bin 

set /P anykey=press any key to finish
