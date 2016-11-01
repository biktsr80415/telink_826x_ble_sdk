require "hci_const"	
require "print_table"
rs232_tbl={}
rs232_tbl,rs232_tbl_cnt=tl_rs232_list()
-- for i,v in ipairs(rs232_tbl)
-- do
--    print(string.format("%s",v))
-- end
-- print(string.format("open the first device:%s",rs232_tbl[1]))
tl_rs232_open(rs232_tbl[1],12)


---------------------------------------------------------------------------------
cmdheaderLen = 4;
cmdLen = 32;

cmdTotalLen = cmdheaderLen + cmdLen;
cmd = array.new(cmdTotalLen)
cmd[1] = HCI_TYPE_CMD
cmd[2] = HCI_CMD_LE_ENCRYPT
cmd[3] = HCI_CMD_LE_OPCODE_OGF
-------------------------------------------
cmd[4] = cmdLen    -- paramLen
-------------------------------------------
--key 16 bytes
cmd[5] = 0x05; 
cmd[6] = 0x05; 
cmd[7] = 0x05; 
cmd[8] = 0x05; 
cmd[9] = 0x05; 
cmd[10] = 0x05; 
cmd[11] = 0x05; 
cmd[12] = 0x05; 

cmd[13] = 0x05; 
cmd[14] = 0x05; 
cmd[15] = 0x05; 
cmd[16] = 0x05; 
cmd[17] = 0x05; 
cmd[18] = 0x05; 
cmd[19] = 0x05; 
cmd[20] = 0x05; 

--plainText 
cmd[21] = 0x05; 
cmd[22] = 0x05; 
cmd[23] = 0x05; 
cmd[24] = 0x05; 
cmd[25] = 0x05; 
cmd[26] = 0x05; 
cmd[27] = 0x05; 
cmd[28] = 0x05; 

cmd[29] = 0x05; 
cmd[30] = 0x05; 
cmd[31] = 0x05; 
cmd[32] = 0x05; 
cmd[33] = 0x05; 
cmd[34] = 0x05; 
cmd[35] = 0x05; 
cmd[36] = 0x05; 
---------------------------------------------------------------------------------


---------------------------------------------------------------------------------
expectHciType = HCI_TYPE_EVENT;
expectEvtType = HCI_EVT_CMD_COMPLETE;
expectOCFcode = cmd[2];

expectEvtParamLen = 17;
expectEvtHeaderLen = 3; 

expectEvtReturnLen = expectEvtHeaderLen + expectEvtParamLen;  --include evt complete header 3byte
-------------------------------------------

---------------------------------------------------------------------------------

print("\n HCI_CMD_LE_ENCRYPT ")
tl_rs232_send(cmd,cmdTotalLen )

repeat
	resTbl,resLen = tl_rs232_recv()
	tl_sleep_ms(50)
until(resLen>0)


---------------------------------------------------------------------------------
print("\nRetrun param length: ", resLen)


--for i= 1, resLen do
--  print(string.format("0x%02x",resTbl[i])) 
--end


local resultERR = 0

--resTbl[1]: hci type
if(resTbl[1] == expectHciType)  
then
   print(string.format("0x%02x",resTbl[1]), "hci type event OK")  
else
   print(string.format("0x%02x",resTbl[1]), "hci type event ERR")  
   resultERR = 1
end


--resTbl[2]: event type
if(resTbl[2] == expectEvtType)  
then
   print(string.format("0x%02x",resTbl[2]), "OK, hci command complete event")  
else
   print(string.format("0x%02x",resTbl[2]), "ERR, hci command complete event")  
   resultERR = 1
end

--resTbl[3]: Retrun Param len 
if(resTbl[3] == expectEvtReturnLen)  
then
   print(string.format("0x%02x",resTbl[3]), "OK, Param len")  
else
   print(string.format("0x%02x",resTbl[3]), "Return Param ERR")  
   resultERR = 1
end



--resTbl[4]: numHciCmds 
if(resTbl[4] == 1)  
then
   print(string.format("0x%02x",resTbl[4]), "OK, numHciCmds ")  
else
   print(string.format("0x%02x",resTbl[4]), "ERR, numHciCmds ")  
   resultERR = 1
end

--resTbl[5]: opCode_OCF 
if(resTbl[5] == expectOCFcode)  
then
   print(string.format("0x%02x",resTbl[5]), "OK, opcode match")  
else
   print(string.format("0x%02x",resTbl[5]), "ERR, opcode match")  
   resultERR = 1
end


--resTbl[6]: opCode_OGF 
if(resTbl[6] == HCI_CMD_LE_OPCODE_OGF)  
then
   print(string.format("0x%02x",resTbl[6]), "OK, opcode OGF is 0x20")  
else
   print(string.format("0x%02x",resTbl[6]), "ERR, opcode OGF is 0x20")  
   resultERR = 1
end

--resTbl[7]: status
if(resTbl[7] == BLE_SUCCESS)  
then
   print(string.format("0x%02x",resTbl[7]), "OK, status is ble success")  
else
   print(string.format("0x%02x",resTbl[7]), "ERR, status is ble fail")  
   resultERR = 1
end


---------------------------------------------------------------------------------

------ display whole test result
if(resultERR == 1)  
then
   print("\n TEST  FAIL!")   
   tl_error(1)
else
   print("\n ")
   PrintTable(resTbl)
   print("\n TEST  OK!")
end

tl_rs232_close()
--tl_usb_bulk_monitor_end()