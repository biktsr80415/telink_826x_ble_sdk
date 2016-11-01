require "hci_const"	

rs232_tbl={}
rs232_tbl,rs232_tbl_cnt=tl_rs232_list()
-- for i,v in ipairs(rs232_tbl)
-- do
--    print(string.format("%s",v))
-- end
-- print(string.format("open the first device:%s",rs232_tbl[1]))
tl_rs232_open(rs232_tbl[1],12)


---------------------------------------------------------------------------------
-- cmdParaLen = 0		    ---

cmd = array.new(19)
cmd[1] = HCI_TYPE_CMD
cmd[2] = HCI_CMD_LE_SET_ADVERTISE_PARAMETERS
cmd[3] = HCI_CMD_LE_OPCODE_OGF
-------------------------------------------
cmd[4] = 15    -- cmdParaLen
-------------------------------------------
cmd[5] = 80  --adv_min     80*0.625 = 50 ms
cmd[6] = 0x00
cmd[7] = 100  --adv_max   100*0.625 = 62.5 ms
cmd[8] = 0x00
cmd[9] = 0x00  --adv_type 
cmd[10] = 0x00 -- own_adr_type

cmd[11] = 0x00 --peer adr type
cmd[12] = 0    --peer  adr
cmd[13] = 0
cmd[14] = 0
cmd[15] = 0
cmd[16] = 0
cmd[17] = 0

cmd[18] = 0x07  -- adv chn 37/38
cmd[19] = 0  --adv filter policy
---------------------------------------------------------------------------------


print("\nhci_cmd_le_set random address")
tl_rs232_send(cmd, 19)

repeat
	resTbl,resLen = tl_rs232_recv()
	tl_sleep_ms(50)
until(resLen>0)


---------------------------------------------------------------------------------
print("\nRetrun param length: ", resLen)

------------------------------------------------------------------------------returnParams
-- type_evt  evtCode(0e)  evtParamLen  numHciCmds   opCode_OCF   opCode_OGF    status  others
--    1			1		  	   1		   1			1			 1			 1       x 

--  evtParamLen =  4 + x
--  resLen = 	   7 + x
local evtParamLen = 0    --


local resultERR = 0

--resTbl[1]: hci type
if(resTbl[1] == HCI_TYPE_EVENT)  
then
   print(string.format("0x%02x",resTbl[1]), "OK, hci type event")  
else
   print(string.format("0x%02x",resTbl[1]), "ERR")  
   resultERR = 1
end


--resTbl[2]: event type
if(resTbl[2] == HCI_EVT_CMD_COMPLETE)  
then
   print(string.format("0x%02x",resTbl[2]), "OK, hci command complete event")  
else
   print(string.format("0x%02x",resTbl[2]), "ERR")  
   resultERR = 1
end

--resTbl[3]: Retrun Param len 
if(resTbl[3] == evtParamLen + 4)  
then
   print(string.format("0x%02x",resTbl[3]), "OK, evtParamLen")  
else
   print(string.format("0x%02x",resTbl[3]), "ERR")  
   resultERR = 1
end


--resTbl[4]: numHciCmds 
if(resTbl[4] == 1)  
then
   print(string.format("0x%02x",resTbl[4]), "OK, numHciCmds = 1")  
else
   print(string.format("0x%02x",resTbl[4]), "ERR")  
   resultERR = 1
end

--resTbl[5]: opCode_OCF 
if(resTbl[5] == HCI_CMD_LE_SET_ADVERTISE_PARAMETERS)  
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
   print("\n TEST  OK!")
end


tl_rs232_close()