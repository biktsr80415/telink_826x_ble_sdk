SMP_MUDULE_HEAD_L		=	0xfe
SMP_MUDULE_HEAD_H		=	0xff

SMP_MUDULE_OPCODE_EN_MITM				=	0x01
SMP_MUDULE_OPCODE_EN_OOB				=	0x02
SMP_MUDULE_OPCODE_EN_BOND				=	0x03
SMP_MUDULE_OPCODE_KEY_DISTRIBUTE		=	0x04
SMP_MUDULE_OPCODE_IO_CAPABLITY			=	0x05

SMP_SUCCESS		= 0x00

IO_CAPABILITY_DISPLAY_ONLY 				= 0
IO_CAPABILITY_DISPLAY_YES_NO			= 1
IO_CAPABILITY_KEYBOARD_ONLY				= 2
IO_CAPABILITY_NO_INPUT_NO_OUTPUT		= 3
IO_CAPABILITY_KEYBOARD_DISPLAY			= 4
IO_CAPABILITY_UNKNOWN 					= 0xff

function smp_set_MITM(handle, enable, tk_value)
	print ("\nset MITM")
	cmd = array.new(9)
	cmd[1] = SMP_MUDULE_HEAD_L
	cmd[2] = SMP_MUDULE_HEAD_H
	
	cmd[3] = 5    -- cmdParaLen
	cmd[4] = SMP_MUDULE_OPCODE_EN_MITM   --opcode
	
	cmd[5] = enable		--disable
	
	for i=1, 4 do
		cmd[5+i] = tk_value[i]
	end

	tl_usb_bulk_out(handle,cmd,9)
	
	repeat
		resTable,resLen = tl_usb_bulk_read()
		tl_sleep_ms(50)
	until(resLen>0)
		
	status = resTable[5]
	
	if(status == SMP_SUCCESS)
	then
		print ("set MITM success.", string.format("0x%02x",enable))
	else
		print ("set MITM failure.", string.format("0x%02x",status))
	end 
	print ("set MITM end")
end

function smp_set_OOB(handle, enable, oob_value)
	print ("\n set oob ")
	array_len = 4+1+16
	cmd = array.new(array_len)
	cmd[1] = SMP_MUDULE_HEAD_L
	cmd[2] = SMP_MUDULE_HEAD_H
	
	cmd[3] = 5    -- cmdParaLen
	cmd[4] = SMP_MUDULE_OPCODE_EN_OOB   --opcode
	
	cmd[5] = enable		--disable
	
	for i=1, 16 do
		cmd[5+i] = oob_value[i]
	end
	
	tl_usb_bulk_out(handle,cmd,array_len)

	repeat
		resTable,resLen = tl_usb_bulk_read()
		tl_sleep_ms(50)
	until(resLen>0)
	
	status = resTable[5]
	
	if(status == SMP_SUCCESS)
	then
		print ("set oob success.", string.format("0x%02x",enable))
	else
		print ("set oob failure.", string.format("0x%02x",status))
		
		--print ("return len :", string.format("0x%02x",resLen))
		--for i=1, resLen do
		--	print (string.format("0x%02x",resTable[i]))
		--end
	end 
	print ("set oob end.")
end

function smp_set_bond(handle, enable)
	print("\n set bond")
	array_len = 4+1
	cmd = array.new(array_len)
	cmd[1] = SMP_MUDULE_HEAD_L
	cmd[2] = SMP_MUDULE_HEAD_H
	
	cmd[3] = 5    -- cmdParaLen
	cmd[4] = SMP_MUDULE_OPCODE_EN_BOND   --opcode
	
	cmd[5] = enable		--disable

	tl_usb_bulk_out(handle,cmd,array_len)
	
	repeat
		resTable,resLen = tl_usb_bulk_read()
		tl_sleep_ms(50)
	until(resLen>0)
	
	status = resTable[5]
	
	if(status == SMP_SUCCESS)
	then
		print ("set bonding success.")
	else
		print ("set bonding failure.", string.format("0x%02x",status))
	end 

end

function smp_set_keyDistribute(handle, LTK_en, IRK_en, CSRK_en)
	print( "\n set disribute key")

	array_len = 4+3
	cmd = array.new(array_len)
	cmd[1] = SMP_MUDULE_HEAD_L
	cmd[2] = SMP_MUDULE_HEAD_H
	
	cmd[3] = 4    -- cmdParaLen
	cmd[4] = SMP_MUDULE_OPCODE_KEY_DISTRIBUTE   --opcode
	
	cmd[5] = LTK_en		
	cmd[6] = IRK_en		
	cmd[7] = CSRK_en		
	
	tl_usb_bulk_out(handle,cmd,array_len)

	repeat
		resTable,resLen = tl_usb_bulk_read()
		tl_sleep_ms(50)
	until(resLen>0)
	
	status = resTable[5]
	
	if(status == SMP_SUCCESS)
	then
		print ("set key distribute success." )
	else
		print ("set key distribute failure.")
		
		--print ("return len :", string.format("0x%02x",resLen))
		--for i=1, resLen do
		--	print (string.format("0x%02x",resTable[i]))
		--end
	end 
	
	print( "set disribute key end")

end

function smp_set_IO_capability(handle, IO_Capability)
	print( "\n set IO capability")
	array_len = 4+1
	cmd = array.new(array_len)
	cmd[1] = SMP_MUDULE_HEAD_L
	cmd[2] = SMP_MUDULE_HEAD_H
	
	cmd[3] = 4    -- cmdParaLen
	cmd[4] = SMP_MUDULE_OPCODE_IO_CAPABLITY    --opcode
	
	cmd[5] = IO_Capability		
	
	tl_usb_bulk_out(handle,cmd,array_len)

	repeat
		resTable,resLen = tl_usb_bulk_read()
		tl_sleep_ms(50)
	until(resLen>0)
	
	status = resTable[5]
	
	if(status == SMP_SUCCESS)
	then
		print ("set IO capability success." )
	else
		print ("set IO capability failure.")
		
		--print ("return len :", string.format("0x%02x",resLen))
		--for i=1, resLen do
		--	print (string.format("0x%02x",resTable[i]))
		--end
	end 
	
	print( "set IO capability end")

end