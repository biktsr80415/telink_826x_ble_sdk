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

cmd = array.new(4)
cmd[1] = HCI_TYPE_CMD
cmd[2] = HCI_CMD_LE_READ_ADVERTISING_CHANNEL_TX_POWER
cmd[3] = HCI_CMD_LE_OPCODE_OGF
-------------------------------------------
cmd[4] = 0    -- cmdParaLen
-------------------------------------------

---------------------------------------------------------------------------------


print("\nhci_cmd_le_read_advertising_channel_TX_power")
tl_rs232_send(cmd, 4)

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
local evtParamLen = 1    --


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
if(resTbl[5] == HCI_CMD_LE_READ_ADVERTISING_CHANNEL_TX_POWER)  
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



print(resTbl[8], "dBm Tx power")




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
-- tl_usb_bulk_monitor_end()