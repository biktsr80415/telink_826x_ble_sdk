require "basic_debug_config"	


function wait_for_event(time_ms, expect_status)


--HCI_EVT_LE_META   				--0x3E
--HCI_CMD_DISCONNECTION_COMPLETE    --0x05 


start = os.clock()
while(   os.clock() - start < (time_ms/1000) )
do
   tl_sleep_ms(1)
   resTbl,resLen = tl_usb_bulk_read()

   if(resLen > 0)
   then
		break
   end
end
	
	

local eventERR = 0
local Subevent_Code
local connection_handle = 0
local event_status = BLE_SUCCESS
local role
local terminate_reason


------------------------------------------------------------------------------    event Params
-- type_evt  evtCode  evtParamLen  Subevent_Code   ......
--    1			1		  1		       1		    



if(resLen < 4) then
	if(resLen == 0) then
		print("Retrun param length:", resLen, "No event come!")
	else
		print("Retrun param length:", resLen, "ERR, too short")
	end
	tl_error(1)
	return 0, 0
end


evtParamLen = resTbl[3]
Subevent_Code = resTbl[4]


-- total_param_len = event_param_len + 19
if(resLen ~=  (evtParamLen+3))
then
	print(string.format("Retrun param length %d unmatch with param len %d ", resLen, evtParamLen))
    print(string.format("0x%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[1],resTbl[2], resTbl[3],resTbl[4], resTbl[5],resTbl[6], resTbl[7],resTbl[8]))	
	print(string.format("0x%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[9], resTbl[10],resTbl[11], resTbl[12],resTbl[13],resTbl[14],resTbl[15], resTbl[16]))
	tl_error(1)
	return 0, 0
end




if(resTbl[1] == HCI_TYPE_EVENT and resTbl[2] == HCI_EVT_LE_META) then

	if(WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
		print(string.format("event type 0x%02x and event code 0x%02x", resTbl[1], resTbl[2])) 
	end

	if(Subevent_Code == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)  then
		event_param_len = 19
		event_status = resTbl[5]
		role = resTbl[8]
		connection_handle = resTbl[7]*256 + resTbl[6]
		print(string.format("HCI_LE_Connection_Complete_Event") )
		print("===========================================>")
		print(string.format("Status: 0x%02x",event_status)) 
		
		--if(event_status ~= BLE_SUCCESS or role ~= LL_ROLE_SLAVE or connection_handle~=BLS_CONN_HANDLE)	then
		if(event_status ~= BLE_SUCCESS)	then
			eventERR = 1
			tl_error(1)
		else
		   print(string.format("\t\t\t\tconnection_handle: %04x", connection_handle))
		   print(string.format("\t\t\t\trole 0x%02x",role))
		   print(string.format("\t\t\t\tPeer_Address_Type %02x", resTbl[9]))
		   print(string.format("\t\t\t\tpeerAddr: 0x%02x%02x%02x%02x%02x%02x",resTbl[15],resTbl[14],resTbl[13],resTbl[12],resTbl[11],resTbl[10]))
		   print(string.format("\t\t\t\tConn_Interval: 0x%04x", resTbl[16]+ resTbl[17]*256))
		   print(string.format("\t\t\t\tConn_Latency: 0x%04x", resTbl[18]+ resTbl[19]*256))
		   print(string.format("\t\t\t\tSupervision_Timeout: 0x%04x", resTbl[20]+ resTbl[21]*256))
		   print(string.format("\t\t\t\tMaster_Clock_Accuracy: %02x", resTbl[22]))  
		end
		
		if(eventERR==1 or WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
			print(string.format("role: 0x%02x, Connection Hanlde: 0x%04x", role, connection_handle))
			print(string.format("Peer Adr Type and value: 0x%02x, 0x%02x %02x %02x %02x %02x %02x", resTbl[9], resTbl[15],resTbl[14], resTbl[13],resTbl[12], resTbl[11],resTbl[10])) 			
			print(string.format("Interval: 0x%04x, Latency: 0x%04x, Timeout: 0x%04x", 
					resTbl[17]*256 + resTbl[16], resTbl[19]*256 + resTbl[18], resTbl[21]*256 + resTbl[20]))
		end
		
	elseif(Subevent_Code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)  then
		Num_Reports = resTbl[5]
		Event_Type = resTbl[6]
		Address_Type = resTbl[7]
				

				
	elseif(Subevent_Code == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)  then
		event_param_len = 10
		event_status = resTbl[5]
		connection_handle = resTbl[7]*256 + resTbl[6]
		print(string.format("HCI_LE_Connection_Update_Complete_Event") )
		print("===========================================>")
		print(string.format("Status: 0x%02x",event_status)) 
		
		if(event_status ~= BLE_SUCCESS or connection_handle~=BLS_CONN_HANDLE)	then
			eventERR = 1
			tl_error(1)
		end
		
		if(eventERR==1 or WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
			print(string.format("Connection Hanlde: 0x%04x", connection_handle))
			print(string.format("Interval: 0x%04x, Latency: 0x%04x, Timeout: 0x%04x", 
					resTbl[9]*256 + resTbl[8], resTbl[11]*256 + resTbl[10], resTbl[13]*256 + resTbl[12]))
		end
			
	elseif(Subevent_Code == HCI_SUB_EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE)  then
		event_param_len = 12
		connection_handle = resTbl[7]*256 + resTbl[6]
		event_status = resTbl[5]
		print(string.format("HCI_LE_Read_Remote_Used_Features_Complete_Event") )
		print("===========================================>")
		print(string.format("Status: 0x%02x",event_status)) 
		
		if(event_status ~= expect_status or connection_handle~=BLS_CONN_HANDLE)	then
			eventERR = 1
			tl_error(1)
		end
		
		if(eventERR==1 or WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
			print(string.format("Connection Hanlde: 0x%04x", connection_handle))
			print(string.format("Features: 0x%02x %02x %02x %02x %02x %02x %02x %02x", resTbl[15], resTbl[14], resTbl[13],resTbl[12], resTbl[11], resTbl[10], resTbl[9],resTbl[8]) )
		end
	
	
	elseif(Subevent_Code == HCI_SUB_EVT_LE_LONG_TERM_KEY_REQUESTED)  then
		event_param_len = 13
		print(string.format("HCI_LE_Long_Term_Key_Request_Event") )
		print("===========================================>")
		--print(string.format("Connection Hanlde: 0x%02x 0x%02x", resTbl[6], resTbl[5]))
		print(string.format("Rand_Num: %02x %02x %02x %02x %02x %02x %02x %02x",
				resTbl[14], resTbl[13],resTbl[12], resTbl[11], resTbl[10], resTbl[9],resTbl[8]),resTbl[7] )
		print(string.format("EDIV: 0x%02x %02x", resTbl[16], resTbl[15]) )
	
	elseif(Subevent_Code == HCI_SUB_EVT_LE_REMOTE_CONNECTION_PARAM_REQUEST)  then
		event_param_len = 11
		print(string.format("HCI_LE_Remote_Connection_Paramters_Request_Event") )
		print("===========================================>")
				
		if(WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
			--print(string.format("Connection Hanlde: 0x%02x 0x%02x", resTbl[6], resTbl[5]))
			print(string.format("Interval_min: 0x%02x, Interval_max: 0x%02x", resTbl[7], resTbl[9]))
			print(string.format("Latency: 0x%02x, Timeout: 0x%02x %02x", resTbl[11], resTbl[14], resTbl[13]))
		end	
		
	elseif(Subevent_Code == HCI_SUB_EVT_LE_DATA_LENGTH_CHANGE)  then
		event_param_len = 11
		connection_handle = resTbl[6]*256 + resTbl[5]
		print(string.format("HCI_LE_Data_Length_Change_Event") )
		print("===========================================>")
		print(string.format("MaxTxOctets: 0x%02x %02x, MaxTxTime: 0x%02x %02x", resTbl[8],resTbl[7], resTbl[10],resTbl[9]))
		print(string.format("MaxRxOctets: 0x%02x %02x, MaxRxTime: 0x%02x %02x", resTbl[12],resTbl[11], resTbl[14],resTbl[13]))
		
		
		if(event_status ~= expect_status or connection_handle~=BLS_CONN_HANDLE)	then
			eventERR = 1
			tl_error(1)
		end
		
		if(eventERR==1 or WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
			print(string.format("Connection Hanlde: 0x%04x", connection_handle))
			print(string.format("Status: 0x%02x",event_status)) 
		end		

	elseif(Subevent_Code == HCI_SUB_EVT_LE_READ_LOCAL_P256_KEY_COMPLETE)  then
	    event_param_len = 66
		event_status = resTbl[5]
		print(string.format("HCI_LE_Read_Local_P256_Key_Complete_Event") )
		print("===========================================>")
		print(string.format("Status: 0x%02x",event_status)) 
		if(event_status ~= expect_status) then
			eventERR = 1
			tl_error(1)
		else
			print(string.format("Local_P256_Key:")) 
			print(string.format("0x%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[6],resTbl[7], resTbl[8],resTbl[9], resTbl[10],resTbl[11], resTbl[12],resTbl[13]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[14],resTbl[15], resTbl[16],resTbl[17], resTbl[18],resTbl[19], resTbl[20],resTbl[21]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[22],resTbl[23], resTbl[24],resTbl[25], resTbl[26],resTbl[27], resTbl[28],resTbl[29]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[30],resTbl[31], resTbl[32],resTbl[33], resTbl[34],resTbl[35], resTbl[36],resTbl[37]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[38],resTbl[39], resTbl[40],resTbl[41], resTbl[42],resTbl[43], resTbl[44],resTbl[45]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[46],resTbl[47], resTbl[48],resTbl[49], resTbl[50],resTbl[51], resTbl[52],resTbl[53]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[54],resTbl[55], resTbl[56],resTbl[57], resTbl[58],resTbl[59], resTbl[60],resTbl[61]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[62],resTbl[63], resTbl[64],resTbl[65], resTbl[66],resTbl[67], resTbl[68],resTbl[69]))
		end
		
		if(eventERR==1 or WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
			print(string.format("Status: 0x%02x",event_status)) 
		end		
			
	elseif(Subevent_Code == HCI_SUB_EVT_LE_GENERATE_DHKEY_COMPLETE)  then
	    event_param_len = 34
		event_status = resTbl[5] --LE_Generate_DHKey command completed successfully.
		print(string.format("HCI_LE_Generate_DHKey_Complete_Event") )
		print("===========================================>")
		print(string.format("Status: 0x%02x",event_status)) 
		if(event_status ~= expect_status) then
			eventERR = 1
			tl_error(1)
		else
			print(string.format("DHKey:")) 
			print(string.format("0x%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[6],resTbl[7], resTbl[8],resTbl[9], resTbl[10],resTbl[11], resTbl[12],resTbl[13]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[14],resTbl[15], resTbl[16],resTbl[17], resTbl[18],resTbl[19], resTbl[20],resTbl[21]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[22],resTbl[23], resTbl[24],resTbl[25], resTbl[26],resTbl[27], resTbl[28],resTbl[29]))
			print(string.format("%02x%02x%02x%02x%02x%02x%02x%02x", resTbl[30],resTbl[31], resTbl[32],resTbl[33], resTbl[34],resTbl[35], resTbl[36],resTbl[37]))
		end
		
		if(eventERR==1 or WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
			print(string.format("Status: 0x%02x",event_status)) 
		end		
		
	elseif(Subevent_Code == HCI_SUB_EVT_LE_ENHANCED_CONNECTION_COMPLETE)  then
	
	elseif(Subevent_Code == HCI_SUB_EVT_LE_DIRECT_ADVERTISE_REPORT)  then
	
	
	
	else
		eventERR = 1
		print(string.format("event type %d or event code(LE Meta Event) %d ERR", resTbl[1], resTbl[2]))
	end
	
	
	
elseif(resTbl[1] == HCI_TYPE_EVENT and resTbl[2] == HCI_CMD_DISCONNECTION_COMPLETE)	 then
	Subevent_Code = HCI_CMD_DISCONNECTION_COMPLETE   --实际是event code

	event_param_len = 7
	event_status = resTbl[4]
	connection_handle = resTbl[6]*256 + resTbl[5]
	terminate_reason = resTbl[7]
	print(string.format("HCI_Disconnection_Complete_Event") )
	print("===========================================>")
	print(string.format("Status: 0x%02x",event_status)) 
		
	--if(event_status ~= BLE_SUCCESS or connection_handle~=BLS_CONN_HANDLE)	then
	if(event_status ~= BLE_SUCCESS)	then
		eventERR = 1
		tl_error(1)
	end
		
	if(eventERR==1 or WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
		print(string.format("Connection Hanlde: 0x%04x", connection_handle))
		print(string.format("reason: 0x%02x", terminate_reason))
	end
	
else
	print(string.format("event type 0x%02x or event code 0x%02x ERR", resTbl[1], resTbl[2])) 
	eventERR = 1
end



------------------------------------- TEST  RESULT ---------------------------------
if(eventERR == 1)
then
	print("\n TEST  FAIL!")   
	tl_error(1)
else
	tl_error(0)
end



return Subevent_Code, connection_handle, event_status


end   -- func end