require "hci_const"	

function hci_le_read_local_p256_pkey(status, ...)
arg = {...} 

---------------------------------------------------------------------------------
-- cmdParaLen = 0		    ---

hci_type_cmd = HCI_TYPE_CMD
opcode_OCF = HCI_CMD_LE_READ_LOCAL_P256_PUBLIC_KEY
opcode_OGF = HCI_CMD_LE_OPCODE_OGF
cmd_param_len = 60
cmd_total_len = cmd_param_len + 4

numHCIcmds = 1
event_param_len = 66
total_param_len = 66
event_code = HCI_EVT_LE_META
sub_event_code = HCI_SUB_EVT_LE_READ_LOCAL_P256_KEY_COMPLETE

cmd = array.new(4)
cmd[1] = hci_type_cmd
cmd[2] = opcode_OCF
cmd[3] = opcode_OGF
-------------------------------------------
cmd[4] = 0    -- cmdParaLen
-------------------------------------------
---------------------------------------------------------------------------------
print(string.format("\t\tCMD hci_le_read_local_p256_pkey") )
print("<-------------------------------------------------------------------------------------")
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
-- type_evt  evtCode(0e)  evtParamLen          status
--    1			1		  	   1	      	       1     

--  total_param_len =  event_param_len
--  resLen 			=  3 + event_param_len

if(resLen ~= (3 + event_param_len))
then
	print("Retrun param length: ", resLen, "\tERR")
	tl_error(1)
	return
end 														

if(resTbl[1] == HCI_TYPE_EVENT and resTbl[2] == event_code)
then
	print(string.format("\t\tHCI_Command_Complete_Event") )
	print("-------------------------------------------------------------------------------------->")
	print(string.format("Status: 0x%02x",resTbl[7])) 
	if( resTbl[3] == event_param_len  and resTbl[4] == sub_event_code and resTbl[5] == status )
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


	--resTbl[4]: sub_event_code 
	if(resTbl[4] == sub_event_code)  
	then
		print(string.format("0x%02x",resTbl[4]), "OK, sub_event_code")  
	else
		print(string.format("0x%02x",resTbl[4]), "ERR, sub_event_code")   
	end


	--resTbl[5]: status
	if(resTbl[5] == status)  
	then
		print(string.format("0x%02x",resTbl[5]), "OK, status")  
	else
		print(string.format("0x%02x",resTbl[5]), "ERR, status") 
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


