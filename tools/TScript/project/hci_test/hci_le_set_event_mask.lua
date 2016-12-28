require "hci_const"  

function hci_le_set_event_mask(status, ...)
arg = {...} 


---------------------------------------------------------------------------------
-- cmdParaLen = 0        ---

hci_type_cmd = HCI_TYPE_CMD
opcode_OCF = HCI_CMD_LE_SET_EVENT_MASK
opcode_OGF = HCI_CMD_LE_OPCODE_OGF
cmd_param_len = 8
cmd_total_len = cmd_param_len + 4

numHCIcmds = 1
event_param_len = 1
total_param_len = 4
event_code = HCI_EVT_CMD_COMPLETE
---------------------------------------------------------------------------------
cmd = array.new(cmd_total_len)
cmd[1] = hci_type_cmd
cmd[2] = opcode_OCF
cmd[3] = opcode_OGF
-------------------------------------------
cmd[4] = cmd_param_len    -- paramLen
-------------------------------------------
for i,v in ipairs(arg) do
   cmd[4+i] = v
end
---------------------------------------------------------------------------------


print("\nhci_cmd_le_set_event_mask")
len = tl_usb_bulk_out(handle,cmd,6 )

repeat
	resTbl,resLen = tl_usb_bulk_read()
	tl_sleep_ms(50)
until(resLen>0)


---------------------------------------------------------------------------------
print("\nRetrun param length: ", resLen)


--for i= 1, resLen do
--  print(string.format("0x%02x",resTbl[i])) 
--end


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
if(resTbl[3] == 4)  
then
   print(string.format("0x%02x",resTbl[3]), "OK, Param len = 4")  
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
if(resTbl[5] == HCI_CMD_LE_SET_EVENT_MASK)  
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


tl_usb_bulk_monitor_end()