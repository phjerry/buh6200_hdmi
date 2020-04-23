#ifndef __IR_PROTOCOL_H__
#define __IR_PROTOCOL_H__
/* microsecond on each. */
typedef struct {
    hi_s32 pluse;
    hi_s32 space;
    /* this is used to specify delta value of this phase */
    hi_s32 factor;
    /* atomic cuclated from pluse/spcace/factor */
    hi_s32 maxp;
    hi_s32 minp;
    hi_s32 maxs;
    hi_s32 mins;
} phase_attr;
typedef struct {
    phase_attr header;
    phase_attr second_header;
    hi_s32     second_header_at;
    phase_attr b0;
    phase_attr b1;
    phase_attr burst;
    phase_attr repeat;
    /* mininal time silce, micorsecond unit. */
    hi_s32     min_slice;
    /* how many bits this protocol will report in a frame. */
    hi_s32     wanna_bits;
    /*
     * how many symbols passed will meet an burst.
     * NEC like protocol, this is the same with wanna_bits.
     * but rc6 may not.
     */
    hi_s32     burst_offset;
} ir_symbol_attr;
typedef enum {
    /* protocol is unique. */
    INFR_UNIQUE = 1,
    /* some protocol is similar to this. */
    INFR_SIMILAR = (1 << 1),
    /* for som reason this protocol cannot used. */
    INFR_UNSUPPORT = (1 << 2),

    INFR_SIMPLE_REPEAT = (1 << 3),

    INFR_FULL_REPEAT = (1 << 4),
    INFR_NO_REPEAT = (1 << 5),
    /* bits number of frame data is not fix. */
    INFR_FLEX_DATA  = (1 << 6),

    INFR_HAS_ERR_HANDLE = (1 << 7),
    /*
     * - this is used to significate this protocol
     * has some problems and cannot be used.
     * - maybe not use.
     */
    INFR_IFFY = (1 << 31)
} infrared_flag;

/*
 * frame parser.
 * -parameters:
 * ir: ir priv data, parser may use key_stat_timer or some other information.
 * prot: infrared code feature.
 * symbol_head: symbols to parse.
 * key_head: where the keys parsed out to store.
 *
 * - call context:
 *  1.  caller should guarantee an entire frame is received
 * before call this handle.
 * for nec simple repeat like infrared code, caller guarantee at least
 * a full frame or a repeat frame is received. if a full frame is received,
 * this handle should judge a repeat frame is successfully received or not.
 *
 *  2.  irq disabled.
 *
 * - requirement: each handle should increase symb_head->reader
 * while a frame is parsed success.
 *
 * - functional requirements:
 *  1. this handle should parse a frame start from symb_head->reader,
 * if the frame have be parsed successfully, remember to increase
 * symb_head->reader.
 *  2. judge each frame's key state.
 *  3. according ir->key_up_event and ir->key_repeat_event/
 * ir->key_repeat_interval to decide if a key up event will be sent
 * or a repeat key event will be sent, and the frequence of repeat key.
 *  4. formate bit order, big endian or litter endian.
 * - return value:
 *  0 while at least a frame have been parsed successfully.
 *  others on error occured.
 */
struct ir_protocol_declaration;
typedef hi_s32 (*frame_parser)(ir_priv *ir, struct ir_protocol_declaration *prot,
                               ir_buffer *symb_head, ir_buffer *key_head);

/* phase match result */
typedef enum {
    /* frame match successful */
    IR_MATCH_MATCH,
    /* more data is needed. cannot judge match or not */
    IR_MATCH_NEED_MORE_DATA,
    /* frame not match. */
    IR_MATCH_NOT_MATCH
} ir_match_result;
typedef enum {
    IR_MTT_HEADER,
    IR_MTT_BURST,
    IR_MTT_FRAME
} ir_match_type;
/*
 * phase_match:
 * to decide whether the frame start from buffer->reader
 * is a frame of the protocol ip or a repeat frame start
 * if the infrared code constains repeat frame.
 *
 * - call context: irq disabled.
 */
typedef ir_match_result(*phase_match)(ir_match_type type,ir_buffer *buffer,
                                      struct ir_protocol_declaration *ip);
/*
 * match_error_handle:
 * - case call phase_match fail, driver will call this handle
 * if ip->flag & INFR_HAS_ERR_HANDLE is !0.
 * - return value:
 * while return 0, the driver will try to call frame_parser
 * with @type is set to IR_MTT_FRAME, if @type is IR_MTT_HEADER,
 * driver will call phase_match again but
 * @type is set to IR_MTT_FRAME.
 * while return !0, this frame will be parse to
 * the next infrared code parser.
 * - call context:
 * irq disabled.
 */
typedef ir_match_result(*match_error_handle)(ir_match_type type,ir_buffer *buffer,
                                             struct ir_protocol_declaration *ip);
/*
 * parse_error_handle:
 * - case call frame_parser fail, driver will call this handle
 * if ip->flag & INFR_HAS_ERR_HANDLE is !0.
 * - call context:
 * irq disabled
 */
typedef hi_void (*parse_error_handle)(ir_priv *ir,
                                      struct ir_protocol_declaration *prot,
                                      ir_buffer *symb_head,
                                      ir_buffer *key_head);

typedef struct ir_protocol_declaration {
    struct  list_head node;
    hi_char *ir_code_name;

    ir_protocol_idx idx;
    infrared_flag flag;
    ir_symbol_attr attr;

    phase_match match;
    /*
     * the private data may used by parser
     * which bind with infra code or protocol.
     */
    hi_u32 priv;
    frame_parser handle;
    match_error_handle match_error;
    parse_error_handle parse_error;

    hi_u32 key_hold_timeout_time;

    /* is this protocol disabled ? */
    hi_s32 disabled;
} ir_protocol;

hi_s32 ir_protocol_init(hi_void);
hi_s32 ir_protocol_exit(hi_void);

ir_protocol *ir_prot_first(hi_void);
ir_protocol *ir_prot_next(ir_protocol *curr);

hi_s32 ir_prot_valid(ir_protocol *ip);

hi_s32 key_match_phase(key_attr *key,phase_attr *phase);
/*******************************************************************/
/* NEC handles */
/* nec matcher for nec simple and full. */
ir_match_result nec_match(ir_match_type type,
                          ir_buffer *buf,
                          ir_protocol *ip);
/* nec simple parser. */
hi_s32 nec_frame_simple_parse(ir_priv *ir, ir_protocol *ip,
                              ir_buffer *readfrom, ir_buffer *writeto);
/* nec full parser. */
hi_s32 nec_frame_full_parse(ir_priv *ir, ir_protocol *ip,
                            ir_buffer *readfrom, ir_buffer *writeto);

/* nec protocol with 2headers */
hi_s32 nec_frame_2headers_full_parse(ir_priv *ir, ir_protocol *ip,
                                     ir_buffer *rd, ir_buffer *wr);

hi_s32 nec_frame_2headers_simple_parse(ir_priv *ir, ir_protocol *ip,
                                       ir_buffer *rd, ir_buffer *wr);

ir_match_result nec_2headers_match(ir_match_type type,
                                   ir_buffer *buf,
                                   ir_protocol *ip);

hi_void nec_init(hi_void);
hi_void nec_exit(hi_void);
/*****************************************************************/
/* RC6 handles */
hi_s32 parse_rc6(ir_priv *ir, ir_protocol *ip,
                 ir_buffer *symb_head, ir_buffer *key_head);

ir_match_result rc6_match(ir_match_type type,
                          ir_buffer *head,
                          ir_protocol *ip);
/*
 * call condition:
 * if rc6_match return not match, this routine will be called.
 */
ir_match_result rc6_match_error_handle(ir_match_type type,
                                       ir_buffer *buffer,
                                       ir_protocol *ip);
/*
 * call condition:
 * if parse_rc6 returns !0, this routine can be called.
 */
hi_void rc6_parse_error_handle(ir_priv *ir, ir_protocol *prot,
                               ir_buffer *symb_head, ir_buffer *key_head);

hi_void rc6_init(hi_void);
hi_void rc6_exit(hi_void);
/*****************************************************************/
/* rstep handles */
hi_s32 parse_rstep(ir_priv *ir, ir_protocol *ip,
                   ir_buffer *symb_head, ir_buffer *key_head);
ir_match_result rstep_match(ir_match_type type,
                            ir_buffer *head,
                            ir_protocol *ip);
ir_match_result rstep_match_error_handle(ir_match_type type,
                                         ir_buffer *buffer,
                                         ir_protocol *ip);
hi_void rstep_parse_error_handle(ir_priv *ir, ir_protocol *prot,
                                 ir_buffer *symb_head, ir_buffer *key_head);
hi_void rstep_init(hi_void);
hi_void rstep_exit(hi_void);
/*****************************************************************/
/* RC5 handles */
hi_s32 parse_rc5(ir_priv *ir, ir_protocol *ip,
                 ir_buffer *symb_head, ir_buffer *key_head);

ir_match_result rc5_match(ir_match_type type,
                          ir_buffer *head,
                          ir_protocol *ip);
/*
 * call condition:
 * if rc5_match return not match, this routine will be called.
 */
ir_match_result rc5_match_error_handle(ir_match_type type,
                                       ir_buffer *buffer,
                                       ir_protocol *ip);
/*
 * call condition:
 * if parse_rc5 returns !0, this routine can be called.
 */
hi_void rc5_parse_error_handle(ir_priv *ir, ir_protocol *prot,
                               ir_buffer *symb_head, ir_buffer *key_head);

/* extended rc5 handles */
hi_s32 parse_extended_rc5(ir_priv *ir, ir_protocol *ip,
                          ir_buffer *symb_head, ir_buffer *key_head);

ir_match_result extended_rc5_match(ir_match_type type,
                                   ir_buffer *head,
                                   ir_protocol *ip);
/*
 * call condition:
 * if extended_rc5_match return not match, this routine will be called.
 */
ir_match_result extended_rc5_match_error_handle(ir_match_type type,
                                                ir_buffer *buffer,
                                                ir_protocol *ip);
/*
 * call condition:
 * if extended_rc5_match returns !0, this routine can be called.
 */
hi_void extended_rc5_parse_error_handle(ir_priv *ir, ir_protocol *prot,
                                        ir_buffer *symb_head, ir_buffer *key_head);

hi_void rc5_init(hi_void);
hi_void rc5_exit(hi_void);

/*****************************************************************/
/* sony handles */
hi_void sony_init(hi_void);
hi_void sony_exit(hi_void);
ir_match_result sony_match(ir_match_type type,
                           ir_buffer *buf,
                           ir_protocol *ip);
hi_s32 sony_frame_full_parse(ir_priv *ir, ir_protocol *ip,
                             ir_buffer *rd, ir_buffer *wr);
/*****************************************************************/
/* sony handles */
hi_void tc9012_init(hi_void);
hi_void tc9012_exit(hi_void);
ir_match_result tc9012_match(ir_match_type type,
                             ir_buffer *buf,
                             ir_protocol *ip);
hi_s32 tc9012_frame_simple_parse(ir_priv *ir, ir_protocol *ip,
                                 ir_buffer *rd, ir_buffer *wr);
/*****************************************************************/
/*  xmp   */
hi_void xmp_init(hi_void);
hi_void xmp_exit(hi_void);
ir_match_result xmp_match(ir_match_type type,
                          ir_buffer *buf,
                          ir_protocol *ip);
hi_s32 xmp_parse(ir_priv *ir, ir_protocol *ip,
                 ir_buffer *rd, ir_buffer *wr);
/*****************************************************************/
/* credit handles */
hi_void credit_init(hi_void);
hi_void credit_exit(hi_void);
ir_match_result credit_match(ir_match_type type,
                             ir_buffer *buf,
                             ir_protocol *ip);
hi_s32 credit_frame_parse(ir_priv *ir, ir_protocol *ip,
                          ir_buffer *rd, ir_buffer *wr);
#endif
