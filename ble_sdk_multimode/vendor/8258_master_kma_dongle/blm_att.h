/*
 * blm_att.h
 *
 *  Created on: 2018-8-29
 *      Author: Administrator
 */

#ifndef BLM_ATT_H_
#define BLM_ATT_H_




void	att_keyboard (u16 conn, u8 *p);
void	att_keyboard_media (u16 conn, u8 *p);
void	att_mic (u16 conn, u8 *p);
void 	att_mouse(u16 conn, u8 *p);

void host_att_data_clear(void);

#endif /* BLM_ATT_H_ */
