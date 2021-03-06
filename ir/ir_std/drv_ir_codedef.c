#ifndef __HI_IR_CODEDEF__
#define __HI_IR_CODEDEF__

/* device parameter */
typedef struct {
    hi_u16 leads_min;
    hi_u16 leads_max;

    hi_u16 leade_min;
    hi_u16 seade_max;

    hi_u16 bit0_min;
    hi_u16 bit0_max;

    hi_u16 bit1_min;
    hi_u16 bit1_max;

    hi_u16 sleade_min;
    hi_u16 sleade_max;

    hi_u16 code_len;
    hi_u16 code_type;
} ir_dev_para;

static ir_dev_para ir_dev_paras[] = {
    /* NEC with simple repeat code : u_pd6121_g */
    { 828, 972, 414, 486, 39, 84, 135, 203, 180, 270, 32, 0 },

    /* NEC with simple repeat code : D6121/BU5777/D1913 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 207, 243, 32, 0 },

    /* NEC with simple repeat code : LC7461M-C13 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 207, 243, 42, 0 },

    /* NEC with simple repeat code : AEHA */
    { 270, 405, 135, 203, 34, 51, 101, 152, 270, 405, 48, 0 },

    /* TC9012 : TC9012F/9243 */
    { 414, 486, 414, 486, 45, 67, 135, 203, 0, 0, 32, 1 },

    /* NEC with full repeat code : u_pd6121_g */
    { 828, 972, 414, 486, 45, 67, 135, 203, 0, 0, 32, 2 },

    /* NEC with full repeat code : LC7461M-C13 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 0, 0, 42, 2 },

    /* NEC with full repeat code : MN6024-C5D6 */
    { 270, 405, 270, 405, 68, 101, 203, 304, 0, 0, 22, 2 },

    /* NEC with full repeat code : MN6014-C6D6 */
    { 279, 419, 279, 419, 70, 105, 140, 210, 0, 0, 24, 2 },

    /* NEC with full repeat code : MATNEW */
    { 279, 419, 300, 449, 35, 52, 105, 157, 0, 0, 48, 2 },

    /* NEC with full repeat code : MN6030 */
    { 279, 419, 279, 419, 70, 105, 210, 314, 0, 0, 22, 2 },

    /* NEC with full repeat code : PANASONIC */
    { 282, 422, 282, 422, 70, 106, 211, 317, 0, 0, 22, 2 },

    /* SONY-D7C5 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 12, 3 },

    /* SONY-D7C6 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 13, 3 },

    /* SONY-D7C8 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 15, 3 },

    /* SONY-D7C13 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 20,  3 },
};
#endif /* __HI_IR_CODEDEF__ */
