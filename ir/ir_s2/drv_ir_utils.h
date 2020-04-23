#ifndef __IR_UTILS_H__
#define __IR_UTILS_H__

#include "linux/hisilicon/securec.h"

#define hiir_debug HI_DEBUG_IR
#define hiir_info  HI_INFO_IR
#define hiir_error HI_ERR_IR
#define hiir_warn  HI_WARN_IR

hi_void  ir_insert_key_tail(ir_buffer *head, key_attr *key);
key_attr *ir_first_write_key(ir_buffer *head);
key_attr *ir_first_read_key(ir_buffer *head);
key_attr *ir_get_write_key_n(ir_buffer *head, hi_u32 n);
key_attr *ir_get_prev_write_key(ir_buffer *head);
key_attr *ir_get_read_key_n(ir_buffer *head, hi_u32 n);
key_attr *ir_next_reader_clr_inc(ir_buffer *head);
key_attr *ir_reader_inc(ir_buffer *head);
/* return 1 while d is in [min, max] */
static inline hi_s32 data_fallin(hi_u32 d, hi_u32 min, hi_u32 max)
{
    return (d >= min && d <= max);
}
#define TO_STR(is_pulse)    ((is_pulse) ? "pulse" : "space")
static inline hi_s32 geq_margin(hi_u32 d1, hi_u32 d2, hi_u32 margin)
{
    return d1 > (d2 - margin);
}

static inline hi_s32 eq_margin(hi_u32 d1, hi_u32 d2, hi_u32 margin)
{
    return ((d1 > (d2 - margin)) && (d1 < (d2 + margin)));
}
typedef struct {
    hi_u32  duration;
    hi_u32  pulse;          /* 1:pulse  0:space */
} ir_signal;
static inline hi_u32 is_transition(ir_signal *x, ir_signal *y)
{
    return x->pulse != y->pulse;
}

static inline hi_void decrease_duration(ir_signal *signal, hi_u32 duration)
{
    if (duration > signal->duration) {
        signal->duration = 0;
    } else {
        signal->duration -= duration;
    }
}
#endif
