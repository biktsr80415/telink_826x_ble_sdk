#ifndef _APP_H
#define _APP_H

#include "drivers.h"

/* Audio Operation Function ------------------------------------------------- */
extern void ui_enable_mic(u8 en);
extern void voice_press_proc(void);

extern void deep_wakeup_proc(void);

extern void user_init();
extern void main_loop (void);

#endif /* APP_H_ */
