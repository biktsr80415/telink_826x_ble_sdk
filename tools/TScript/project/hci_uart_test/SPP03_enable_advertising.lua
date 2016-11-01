require "spp_cmd"  

rs232_tbl={}
rs232_tbl,rs232_tbl_cnt=tl_rs232_list()
-- for i,v in ipairs(rs232_tbl)
-- do
--    print(string.format("%s",v))
-- end
print(string.format("open the first device:%s",rs232_tbl[1]))
tl_rs232_open(rs232_tbl[1],12)


---------------------------------------------------------------------------------
-- cmdParaLen = 0        ---

cmd = array.new(5)
cmd[1] = ENABLE_ADV
cmd[2] = 0xff
cmd[3] = 1        -- enable
cmd[4] = 0
-------------------------------------------
cmd[5] = 0x01   
-------------------------------------------


print("\nhci_cmd_le_set random address")
tl_rs232_send(cmd, 5)

repeat
   resTbl,resLen = tl_rs232_recv()
until(resLen>0)


---------------------------------------------------------------------------------
print("\nRetrun param length: ", resLen)

------------------------------------------------------------------------------returnParams
-- type_evt   len      evtid     status  
--    ff      0x03     0x07xx      0 or 1        
--  evtParamLen =  4 + x
--  resLen =      7 + x
local evtParamLen = 0    --


local resultERR = 0

--resTbl[1]: spp type
if(resTbl[1] == 0xff)  
then
   print(string.format("0x%02x",resTbl[1]), "OK, spp type event")  
else
   print(string.format("0x%02x",resTbl[1]), "ERR")  
   resultERR = 1
end


--resTbl[2]: event type
if(resTbl[2] == 0x03)  
then
   print(string.format("0x%02x",resTbl[2]), "OK, spp command complete event")  
else
   print(string.format("0x%02x",resTbl[2]), "ERR")  
   resultERR = 1
end

--resTbl[3]: Retrun Param len 
if(resTbl[3] == ENABLE_ADV)  
then
   print(string.format("0x%02x",resTbl[3]), "OK, cmd right")  
else
   print(string.format("0x%02x",resTbl[3]), "ERR")  
   resultERR = 1
end


if(resTbl[4] == 0x07)  
then
   print(string.format("0x%02x",resTbl[4]), "OK")  
else
   print(string.format("0x%02x",resTbl[4]), "ERR")  
   resultERR = 1
end

--resTbl[5]: status
if(resTbl[5] == BLE_SUCCESS)  
then
   print(string.format("0x%02x",resTbl[5]), "OK, status is ble success")  
else
   print(string.format("0x%02x",resTbl[5]), "ERR")  
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