require "basic_debug_config"	
require "hci_const"

-- check_en = 1鏃讹紝妫�祴address_type鍜宎ddrss鐨勫�锛屽惁鍒欎笉妫�祴

function wait_for_le_report_event(time_ms, check_en, check_evtType, check_adrType, check_adr_high4, check_adr_low2)


---- print(string.format("chech_address_Type:0x%02x", check_adrType))   --debug

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

------------------------------------------------------------------------------    event Params
-- type_evt  evtCode  evtParamLen  Subevent_Code   ......
--    1			1		  1		       1		    


if (check_en == CHECK_NO_MORE_EVENT) then
	if(resLen == 0) then
		print("No more ADV report event come, OK")
	else
		print(string.format("Still event come, ERR, data len:%02d", resLen))
		tl_error(1)
		tl_log_color(0x000000ff)   --Turn to Red color to indicate ERR
		
		for i =1, resLen, 1  do
			print(string.format("resTbl[%02d]: 0x%02x", i, resTbl[i])) 
		end
		
	end
	
	return
else
	if(resLen < 4) then
		if(resLen == 0) then
			print("Retrun param length:", resLen, "No event come!")
		else
			print("Retrun param length:", resLen, "ERR, too short")
		end
		tl_error(1)
		tl_log_color(0x000000ff)   --Turn to Red color to indicate ERR
		return
	end
end


evtParamLen = resTbl[3]
Subevent_Code = resTbl[4]


-- total_param_len = event_param_len + 19
if(resLen ~=  (evtParamLen+3))
then
	print(string.format("Retrun param length %d unmatch with param len %d ", resLen, evtParamLen)) 
	tl_error(1)
	tl_log_color(0x000000ff)   --Turn to Red color to indicate ERR
	return 0, 0
end




if(resTbl[1] == HCI_TYPE_EVENT and resTbl[2] == HCI_EVT_LE_META) then

	if(WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
		print(string.format("event type 0x%02x and event code 0x%02x", resTbl[1], resTbl[2])) 
	end

	if(Subevent_Code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)  then
		Num_Reports = resTbl[5]
		Event_Type = resTbl[6]
		Address_Type = resTbl[7]
		Address_low2 = resTbl[8] + resTbl[9] * 256
		Address_high4 = resTbl[10] + resTbl[11] * 256 + resTbl[12]*65536 + resTbl[13] * 256 * 65536
		Length = resTbl[14]
		Data0 = resTbl[15]
		RSSI = resTbl[15 + Length]
	
		print(string.format("HCI_LE_Advertising_Report_Event") )
		print("===========================================>")
		print(string.format("Event_type:0x%02x, Addr_type:0x%02x, Addr: 0x%08x%04x, data0:0x%02x, rssi:0x%02x",Event_Type,Address_Type, Address_high4, Address_low2, Data0, RSSI)) 
		--print("<===========================================")		
				
		if(Num_Reports ~= 1 or Length + 12 ~= evtParamLen) then	
			eventERR = 1
			
			if(Num_Reports ~= 1 ) then
				print(string.format("Num_Reports:%d, ERR", Num_Reports)) 
			elseif(Length + 12 ~= evtParamLen) then
				print(string.format("data length:%d, ERR", Length)) 
			end
			
		end
		
		if(check_en == DATA_CHECK_ENABLE)  then
			if(check_evtType ~= Event_Type or check_adrType~=Address_Type or Address_low2~= check_adr_low2 or Address_high4~=check_adr_high4) then
			--if(check_evtType ~= Event_Type or  Address_low2~= check_adr_low2 or Address_high4~=check_adr_high4) then
				eventERR = 1
			end	
			
			---- print(string.format("chech_address_Type:0x%02x", check_adrType)) --debug
			
			if(check_evtType ~= Event_Type ) then
				print(string.format("Event_Type:0x%02x, ERR",Event_Type)) 
			elseif(check_adrType ~= Address_Type) then
				print(string.format("Address_Type:%d, ERR",Address_Type))
			elseif(Address_high4~=check_adr_high4) then
				print(string.format("Address_high4:0x%08x, ERR", Address_high4))		
			elseif(Address_low2~= check_adr_low2) then
				print(string.format("Address_low2:0x%04x, check_adr_low2:0x%04x  ERR", Address_low2, check_adr_low2))	
			end
		end
		
		
		if(eventERR==1 or WAIT_EVENT_DEBUGMODE == PRINT_LEVEL_1) then
			print(string.format("Num_Reports:%d, data length:%d", Num_Reports, Length)) 
			print(string.format("Event_Type:0x%02x, Address_Type:%d",Event_Type, Address_Type)) 
		end
						
				
	else
		eventERR = 1
		print(string.format("event type %d or event code(LE Meta Event) %d ERR", resTbl[1], resTbl[2]))
	end
	
	
else
	print(string.format("event type 0x%02x or event code 0x%02x ERR", resTbl[1], resTbl[2])) 
	eventERR = 1
end




------------------------------------- TEST  RESULT ---------------------------------
if(eventERR == 1)
then
	print("\n TTTTTTTTTTTTTTTTTTTEST  FAIL!")   
	tl_error(1)
	tl_log_color(0x000000ff)   --Turn to Red color to indicate ERR
else
	tl_error(0)
end



return Subevent_Code, connection_handle, event_status


end   -- func end