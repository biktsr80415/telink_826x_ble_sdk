require "hci_const"		


function hci_le_create_conn(status, ...)
arg = {...} 

hci_type_cmd = HCI_TYPE_CMD
opcode_OCF = HCI_CMD_LE_CREATE_CONNECTION
opcode_OGF = HCI_CMD_LE_OPCODE_OGF
cmd_param_len = 25
cmd_total_len = cmd_param_len + 4

numHCIcmds = 1
event_param_len = 1
total_param_len = 4
event_code = HCI_EVT_CMD_STATUS
-- le_sub_event = HCI_SUB_EVT_LE_CONNECTION_COMPLETE

---------------------------------------------------------------------------------
cmd = array.new(cmd_total_len)    -- 5
cmd[1] = hci_type_cmd
cmd[2] = opcode_OCF
cmd[3] = opcode_OGF
-------------------------------------------
cmd[4] = cmd_param_len    -- cmdParaLen
-------------------------------------------
for i,v in ipairs(arg) do
	cmd[4+i] = v
end
---------------------------------------------------------------------------------

print(string.format("\t\tCMD hci_le_create_conn") )
print("<-------------------------------------------------------------------------------------")
len = tl_usb_bulk_out(handle,cmd, cmd_total_len)



--repeat
--	resTbl,resLen = tl_usb_bulk_read()
--	tl_sleep_ms(100)
--until(resLen>0)
	
start = os.clock()
while(   os.clock() - start < 0.050)
do
   tl_sleep_ms(1)
   resTbl,resLen = tl_usb_bulk_read()

   if(resLen > 0)
   then
		break
   end
end
	
	

local eventERR = 0

------------------------------------------------------------------------------    event Params
-- type_evt  evtCode(0e)  evtParamLen   status      numHciCmds   opCode_OCF       opCode_OGF         
--    1			1		  	   1		   1			1			 1			       1     

--  total_param_len =  3 + event_param_len
--  resLen 			=  6 + event_param_len


if(resLen ~= (6 + event_param_len))
then
	print("Retrun param length: ", resLen, "\tERR")
	tl_error(1)
	return
end



if(resTbl[1] == HCI_TYPE_EVENT and resTbl[2] == event_code)
then
	print(string.format("HCI_Command_Status_Event") )
	print("-------------------------------------------------------------------------------------->")
	print(string.format("Status: 0x%02x",resTbl[4])) 
	
	if( resTbl[3] == total_param_len and resTbl[4] == status and resTbl[5] == numHCIcmds and 
		resTbl[6] == opcode_OCF and resTbl[7] == opcode_OGF)
	then
		eventERR = 0  --event OK
	else
		eventERR = 1
	end
	
else
	eventERR = 1
end


if(eventERR == 1 or PRINT_MODE == PRINT_LEVEL_1 )
then
	print("\n")

	if(PRINT_MODE == PRINT_LEVEL_1)
	then
		print("Retrun param length: ", resLen)
	end
	

	--resTbl[1]: hci type
	if(resTbl[1] == HCI_TYPE_EVENT)  
	then
		print(string.format("0x%02x",resTbl[1]), "OK, hci type event")  
	else
		print(string.format("0x%02x",resTbl[1]), "ERR, hci type event")   
	end


	--resTbl[2]: event type
	if(resTbl[2] == event_code)  
	then
		print(string.format("0x%02x",resTbl[2]), "OK, hci event code")  
	else
		print(string.format("0x%02x",resTbl[2]), "ERR, hci event code")   
	end

	--resTbl[3]: Retrun Param len 
	if(resTbl[3] == total_param_len)  
	then
		print(string.format("0x%02x",resTbl[3]), "OK, totol param len")  
	else
		print(string.format("0x%02x",resTbl[3]), "ERR, totol param len")  
	end


	--resTbl[4]: status 
	if(resTbl[4] == status)  
	then
		print(string.format("0x%02x",resTbl[4]), "OK, status")  
	else
		print(string.format("0x%02x",resTbl[4]), "ERR, status")   
	end

	--resTbl[5]: num hci cmd pkts
	if(resTbl[5] == numHCIcmds)  
	then
		print(string.format("0x%02x",resTbl[5]), "OK, num hci cmd pkts")  
	else
		print(string.format("0x%02x",resTbl[5]), "ERR, num hci cmd pkts") 
	end

	if(resTbl[6] == opcode_OCF)  
	then
		print(string.format("0x%02x",resTbl[6]), "OK, opcode_OCF")  
	else
		print(string.format("0x%02x",resTbl[6]), "ERR, opcode_OCF") 
	end

	if(resTbl[7] == opcode_OGF)  
	then
		print(string.format("0x%02x",resTbl[7]), "OK, opcode_OGF")  
	else
		print(string.format("0x%02x",resTbl[7]), "ERR, opcode_OGF") 
	end
end




------------------------------------- TEST  RESULT ---------------------------------
if(eventERR == 1)
then
   print("\n TEST  FAIL!")   
   tl_error(1)
else
   print("\n TEST  OK!")
end


return status



end  --function end