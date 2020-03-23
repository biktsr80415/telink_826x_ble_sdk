require "hci_const"	
require "basic_debug_config"
function hci_le_clear_white_list(status, ...)
arg = {...} 

---------------------------------------------------------------------------------
-- cmdParaLen = 0		    ---

hci_type_cmd = HCI_TYPE_CMD
opcode_OCF = HCI_CMD_LE_CLEAR_WHITE_LIST
opcode_OGF = HCI_CMD_LE_OPCODE_OGF
cmd_param_len = 0
cmd_total_len = cmd_param_len + 4

numHCIcmds = 1
event_param_len = 1
total_param_len = 4
event_code = HCI_EVT_CMD_COMPLETE

cmd = array.new(4)
cmd[1] = hci_type_cmd
cmd[2] = opcode_OCF
cmd[3] = opcode_OGF
-------------------------------------------
cmd[4] = 0    -- cmdParaLen
-------------------------------------------
---------------------------------------------------------------------------------
print(string.format("\t\t\t\tCMD hci_le_clear_white_list") )
print("<-------------------------------------------------------------------------------------")
print(string.format("\t\t\t\t%02x  %02x  %02x  %02x", cmd[1],cmd[2],cmd[3],cmd[4]) )

len = tl_usb_bulk_out(handle,cmd, 4)

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
-- type_evt  evtCode(0e)  evtParamLen  numHciCmds   opCode_OCF   opCode_OGF         status
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
	print(string.format("HCI_Command_Complete_Event") )
	print("-------------------------------------------------------------------------------------->")
	print(string.format("Status: 0x%02x",resTbl[7])) 
	print(string.format("%02x  %02x  %02x  %02x  %02x  %02x  %02x", resTbl[1],resTbl[2],resTbl[3],resTbl[4],resTbl[5],resTbl[6],resTbl[7]) )
	
	if( (resTbl[3] == event_param_len + 3)  and resTbl[4] == numHCIcmds and resTbl[5] == opcode_OCF and 
		resTbl[6] == opcode_OGF and resTbl[7] == status)
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


	--resTbl[4]: numHciCmds 
	if(resTbl[4] == numHCIcmds)  
	then
		print(string.format("0x%02x",resTbl[4]), "OK, numHciCmds")  
	else
		print(string.format("0x%02x",resTbl[4]), "ERR, numHciCmds")   
	end

	--resTbl[5]: opCode_OCF 
	if(resTbl[5] == opcode_OCF)  
	then
		print(string.format("0x%02x",resTbl[5]), "OK, opcode OCF")  
	else
		print(string.format("0x%02x",resTbl[5]), "ERR, opcode OCF")   
	end


	--resTbl[6]: opCode_OGF 
	if(resTbl[6] == opcode_OGF)  
	then
		print(string.format("0x%02x",resTbl[6]), "OK, opcode_OGF")  
	else
		print(string.format("0x%02x",resTbl[6]), "ERR, opcode_OGF")  
	end

	--resTbl[7]: status
	if(resTbl[7] == status)  
	then
		print(string.format("0x%02x",resTbl[7]), "OK, status")  
	else
		print(string.format("0x%02x",resTbl[7]), "ERR, status") 
	end
end
------------------------------------- TEST  RESULT ---------------------------------
if(eventERR == 1)
then
	print("\n TEST  FAIL!")   
	tl_error(1)
	
else

	tl_error(0)
	
end

return status


end  --function end


