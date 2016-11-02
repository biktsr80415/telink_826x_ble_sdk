/*
 * smp_test.c
 *
 *  Created on: 2016-9-30
 *      Author: Telink
 */

/**********************************************************************
 * INCLUDES
 */
#include "../../proj/tl_common.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/ble/blt_smp_nv.h"

/**********************************************************************
 * GLOBAL DEFINITION
 */
#define		BLT_SECURITY_ENABLE				0

#if (BLT_SECURITY_ENABLE)

	#define		BLT_SMP_PAIR_METHOD				JUST_WORKS

	#if (BLT_SMP_PAIR_METHOD == JUST_WORKS)

		#define		BLT_SMP_IO_CAPABILTY			IO_CAPABLITY_NO_IN_NO_OUT
		#define		BLT_SMP_MITM_EN					0

	#elif (BLT_SMP_PAIR_METHOD == PK_RESP_INPUT)

		#define		BLT_SMP_IO_CAPABILTY			IO_CAPABLITY_KEYBOARD_ONLY
		#define		BLT_SMP_MITM_EN					1

	#elif(BLT_SMP_PAIR_METHOD == PK_INIT_INPUT)

		#define		BLT_SMP_IO_CAPABILTY			IO_CAPABLITY_KEYBOARD_DISPLAY
		#define		BLT_SMP_MITM_EN					1

	#endif



	#define		BLT_SMP_DISTRIBUTE_LTK				1
	#define		BLT_SMP_DISTRIBUTE_IRK				1
	#define		BLT_SMP_DISTRIBUTE_CSRK				0

	#define		BLT_SMP_EXPECT_LTK				1
	#define		BLT_SMP_EXPECT_IRK				1
	#define		BLT_SMP_EXPECT_CSRK				0

	#define		BLT_SMP_MAX_KEY_SIZE			16

	#define		BLT_SMP_KEY						123456

	#define		BLT_SMP_BOND_FLAG				0x01

#endif


/**********************************************************************
 * LOCAL FUNCTIONS
 */

/*
 *	API Used to init callback about ll to smp hci.
 * */
void smpRegisterCbInit()
{
	//smp function enable.
	blt_set_secReq_enable(BLT_SECURITY_ENABLE);

	blt_registerLtkReqEvtCb ( bls_smp_getLtkReq );

	//smp parameter init
	blt_smp_paramInit ();

	//smp parameter flash clean
	blt_smp_paramFlashClean ();  // clean flash if flash full
#if BLT_SECURITY_ENABLE
	//smp parameter config
	blt_smp_setMaxKeySize 	(BLT_SMP_MAX_KEY_SIZE);
	blt_smp_setDistributeKey (BLT_SMP_DISTRIBUTE_LTK, BLT_SMP_DISTRIBUTE_IRK, BLT_SMP_DISTRIBUTE_CSRK);
	blt_smp_expectDistributeKey (BLT_SMP_EXPECT_LTK, BLT_SMP_EXPECT_IRK, BLT_SMP_EXPECT_CSRK);
	blt_smp_setIoCapability (BLT_SMP_IO_CAPABILTY);
	blt_smp_enableAuthMITM (BLT_SMP_MITM_EN, BLT_SMP_KEY);
	blt_smp_enableBonding (BLT_SMP_BOND_FLAG);
#endif
}
