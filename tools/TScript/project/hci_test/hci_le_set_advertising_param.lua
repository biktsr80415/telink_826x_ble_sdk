require "basic_debug_config"
require "hci_const"

function bls_ll_setAdvParam(intervalMin_low, intervalMin_high, intervalMax_low, intervalMax_high, advType, 
							ownAddrType, peerAddrType, peerAddr0, peerAddr1, peerAddr2, 
							peerAddr3, peerAddr4, peerAddr5, adv_channelMap, advFilterPolicy,
							status)

													
							
hci_type_cmd = HCI_TYPE_CMD
opcode_OCF = HCI_CMD_LE_SET_ADVERTISE_PARAMETERS
opcode_OGF = HCI_CMD_LE_OPCODE_OGF
cmd_param_len = 15
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
cmd[4] = cmd_param_len    -- cmdParaLen
-------------------------------------------
cmd[5] = intervalMin_low  --adv_min     80*0.625 = 50 ms
cmd[6] = intervalMin_high
cmd[7] = intervalMax_low  --adv_max   100*0.625 = 62.5 ms
cmd[8] = intervalMax_high
cmd[9] = advType  --adv_type 
cmd[10] = ownAddrType -- own_adr_type

cmd[11] = peerAddrType --peer adr type
cmd[12] = peerAddr0    --peer  adr
cmd[13] = peerAddr1
cmd[14] = peerAddr2
cmd[15] = peerAddr3
cmd[16] = peerAddr4
cmd[17] = peerAddr5

cmd[18] = adv_channelMap  -- adv chn 37/38
cmd[19] = advFilterPolicy  --adv filter policy
---------------------------------------------------------------------------------
--print("\n")


print(string.format("\t\t\t\tHCI_LE_Set_Advertising_Parameters") )
print("<-------------------------------------------------------------------------------------")
print(string.format("\t\t\t\t%02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x", cmd[1],cmd[2],cmd[3],cmd[4],cmd[5],cmd[6],cmd[7],cmd[8],cmd[9],cmd[10],cmd[11],cmd[12],cmd[13],cmd[14],cmd[15],cmd[16],cmd[17],cmd[18],cmd[19]) )
print(string.format("\t\t\t\tintervalMin: %04x", cmd[5]))
print(string.format("\t\t\t\tintervalMax: %04x", cmd[7]))
print(string.format("\t\t\t\tadvType: %02x",cmd[9]))
print(string.format("\t\t\t\townAddrType: %02x",cmd[10]))
print(string.format("\t\t\t\tpeerAddrType: %02x",cmd[11]))
print(string.format("\t\t\t\tpeerAddr: 0x%02x%02x%02x%02x%02x%02x",cmd[17],cmd[16],cmd[15],cmd[14],cmd[13],cmd[12]))
print(string.format("\t\t\t\tadv_channelMap: %02x",cmd[18]))
print(string.format("\t\t\t\tadvFilterPolicy: %02x",cmd[19]))



len = tl_usb_bulk_out(handle,cmd, cmd_total_len)

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
	
	if( resTbl[3] == total_param_len and resTbl[4] == numHCIcmds and resTbl[5] == opcode_OCF and 
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


end   -- func end