echo off
set /P anykey=plug dongle, then press any key to continue
echo "Reset MCU..."
tcdb.exe wc 643 0 -u;wc 520 0 -u;wc 74 53 -u;wc 7e 5325 -u;wc 74 0 -u

echo "Download new file to dongle ..."
tcdb.exe wf 0 -ueb -i 8267_beacon_pad.bin 

echo "Download OTA file1 to address 0x20000 ..."
add_crc16.exe 8261_beacon.bin 
tcdb.exe wf 20000 -ueb -i 8261_beacon.bin 

echo "Download OTA file2 to address 0x40000 ..."
add_crc16.exe 8261_beacon_55aa.bin 
tcdb.exe wf 40000 -ueb -i 8261_beacon_55aa.bin 

set /P anykey=press any key to finish
