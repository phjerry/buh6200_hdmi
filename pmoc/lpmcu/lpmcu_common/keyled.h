/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file for keyled module
 */

#ifndef __KEYLED_H__
#define __KEYLED_H__

#include "hi_type.h"
#include "base.h"
#include <io8051.h>
#include <bitdef.h>
#include "timer.h"

#define KEYLED_CT1642_INNER

/*
 * #define KEYLED_74HC164
 * #define KEYLED_PT6964
 * #define KEYLED_CT1642_INNER
 * #define KEYLED_FD650
 * #define KEYLED_PT6961
 * #define KEYLED_ANSG08
 */

#define NO_DISPLAY 0
#define DIGITAL_DISPLAY 1
#define TIME_DISPLAY 2

extern hi_u8  g_keyled_wakeup_key;

extern hi_void keyled_early_display(hi_void);
extern hi_void timer_display(hi_void);
extern hi_void chan_display(hi_void);
extern hi_void no_display(hi_void);
extern hi_void dbg_display(hi_u16 value);
extern hi_void keyled_init(hi_void);
extern hi_void keyled_disable(hi_void);
extern hi_void keyled_isr(hi_void);

#endif
