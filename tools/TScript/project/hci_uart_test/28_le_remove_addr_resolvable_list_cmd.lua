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
cmdLen = 7;

cmdTotalLen = cmdheaderLen + cmdLen;
cmd = array.new(cmdTotalLen)
cmd[1] = HCI_TYPE_CMD
cmd[2] = HCI_CMD_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST
cmd[3] = HCI_CMD_LE_OPCODE_OGF
-------------------------------------------
cmd[4] = cmdLen    -- paramLen
-------------------------------------------
for i=1,cmdLen do
	cmd[i + 4] = i  -- test parameter for remote key
end

---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
expectHciType = HCI_TYPE_EVENT;
expectEvtType = HCI_EVT_CMD_COMPLETE;
expectOCFcode = cmd[2];

expectEvtParamLen = 1;
expectEvtHeaderLen = 3; 

expectEvtReturnLen = expectEvtHeaderLen + expectEvtParamLen;  --include evt complete header 3byte
-------------------------------------------

---------------------------------------------------------------------------------

print("\n HCI_CMD_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST ")
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
   print(string.format("0x%02x",resTbl[2]), "ERR")  
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
   print(string.format("0x%02x",resTbl[4]), "ERR")  
   resultERR = 1
end

--resTbl[5]: opCode_OCF 
if(resTbl[5] == expectOCFcode)  
then
   print(string.format("0x%02x",resTbl[5]), "OK, opcode match")  
else
   print(string.format("0x%02x",resTbl[5]), "ERR")  
   resultERR = 1
end


--resTbl[6]: opCode_OGF 
if(resTbl[6] == HCI_CMD_LE_OPCODE_OGF)  
then
   print(string.format("0x%02x",resTbl[6]), "OK, opcode OGF is 0x20")  
else
   print(string.format("0x%02x",resTbl[6]), "ERR")  
   resultERR = 1
end

--resTbl[7]: status
if(resTbl[7] == BLE_SUCCESS)  
then
   print(string.format("0x%02x",resTbl[7]), "OK, status is ble success")  
else
   print(string.format("0x%02x",resTbl[7]), "ERR")  
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