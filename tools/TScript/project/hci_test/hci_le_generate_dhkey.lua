require "hci_const"	

function hci_le_generate_dhkey(status, ...)
arg = {...} 

---------------------------------------------------------------------------------
-- cmdParaLen = 0		    ---

hci_type_cmd = HCI_TYPE_CMD
opcode_OCF = HCI_CMD_LE_GENERATE_DHKEY
opcode_OGF = HCI_CMD_LE_OPCODE_OGF
cmd_param_len = 64
cmd_total_len = cmd_param_len + 4

numHCIcmds = 1
event_param_len = 4
total_param_len = 4
event_code = HCI_EVT_CMD_STATUS
--event_code = HCI_EVT_LE_META
--sub_event_code = HCI_SUB_EVT_LE_GENERATE_DHKEY_COMPLETE

cmd = array.new(cmd_total_len)
cmd[1] = hci_type_cmd
cmd[2] = opcode_OCF
cmd[3] = opcode_OGF
cmd[4] = cmd_param_len    -- cmdParaLen

for i,v in ipairs(arg) do
	cmd[4+i] = v
end

len = tl_usb_bulk_out(handle,cmd, cmd_total_len)

print(string.format("\t\t\t\tCMD hci_le_generate_dhkey") )
print("<-------------------------------------------------------------------------------------")
print(string.format("\t\t\t\t%02x %02x %02x %02x", cmd[1],cmd[2],cmd[3],cmd[4]) )
print("\t\t\t\tRemote_P-256_Public_Key:")
print(string.format("\t\t\t\t0x%02x%02x%02x%02x%02x%02x%02x%02x",cmd[5],cmd[6], cmd[7],  cmd[8], cmd[9],  cmd[10],cmd[11], cmd[12]))
print(string.format("\t\t\t\t%02x%02x%02x%02x%02x%02x%02x%02x", cmd[13],cmd[14],cmd[15], cmd[16],cmd[17], cmd[18],cmd[19], cmd[20]))
print(string.format("\t\t\t\t%02x%02x%02x%02x%02x%02x%02x%02x", cmd[21],cmd[22],cmd[23], cmd[24],cmd[25], cmd[26],cmd[27], cmd[28]))
print(string.format("\t\t\t\t%02x%02x%02x%02x%02x%02x%02x%02x", cmd[29],cmd[30],cmd[31], cmd[32],cmd[33], cmd[34],cmd[35], cmd[36]))
print(string.format("\t\t\t\t%02x%02x%02x%02x%02x%02x%02x%02x", cmd[37],cmd[38],cmd[39], cmd[40],cmd[41], cmd[42],cmd[43], cmd[44]))
print(string.format("\t\t\t\t%02x%02x%02x%02x%02x%02x%02x%02x", cmd[45],cmd[46],cmd[47],cmd[48], cmd[49],cmd[50], cmd[51], cmd[52]))
print(string.format("\t\t\t\t%02x%02x%02x%02x%02x%02x%02x%02x", cmd[53],cmd[54],cmd[55], cmd[56],cmd[57], cmd[58],cmd[59], cmd[60]))
print(string.format("\t\t\t\t%02x%02x%02x%02x%02x%02x%02x%02x", cmd[61],cmd[62],cmd[63], cmd[64],cmd[65], cmd[66],cmd[67], cmd[68]))


start = os.clock()
while(   os.clock() - start < 5)
do
   tl_sleep_ms(1)
   resTbl,resLen = tl_usb_bulk_read()

   if(resLen > 0)
   then
		break
   end
end



local eventERR = 0

--------------------------------------------   ------------------- event Params ===-----------------   
-- type_evt  evtCode(0e)  evtParamLen          status     Num_HCI_Command_Packets    Command_Opcode:
--    1			1		  	   1	      	       1                1                       2

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
	print(string.format("HCI_Command_Status_Event") )
	print("-------------------------------------------------------------------------------------->")
	print(string.format("Status: 0x%02x",resTbl[4])) 
	--print(string.format("%02x  %02x  %02x  %02x  %02x  %02x  %02x", resTbl[1],resTbl[2],resTbl[3],resTbl[4],resTbl[5],resTbl[6],resTbl[7]) )
	
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

	tl_error(0)
	
end

return status


end  --function end


