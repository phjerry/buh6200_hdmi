/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: baseparam database operation function
 * Author: wan
 * Create: 2019-12-6
 */

#ifndef __DRV_MCE_DB_H__
#define __DRV_MCE_DB_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_DB_CHECK_LEN     4
#define HI_DB_MAX_SIZE      (12*1024)
#define HI_DB_MAX_NAME_LEN  32

typedef struct {
    hi_u32      data_len;
    hi_u8       *data;
} hi_db;

typedef struct {
    hi_db       *db;
    hi_char     name[HI_DB_MAX_NAME_LEN];
    hi_u32      data_size;
    hi_u8       *data;
} hi_db_table;

typedef struct {
    hi_char     name[HI_DB_MAX_NAME_LEN];
    hi_u32      value_size;
    hi_void     *value;
} hi_db_key;

hi_s32 hi_db_create(hi_db *db);

hi_s32 hi_db_destroy(hi_db *db);

hi_s32 hi_db_get_db_from_mem(hi_u8 *db_mem, hi_db *db);

hi_s32 hi_db_insert_table(hi_db *db, hi_db_table *table);

hi_s32 hi_db_delete_table(hi_db *db, hi_char table_name[]);

hi_s32 hi_db_get_table_by_name(hi_db *db, hi_char table_name[], hi_db_table *table);

hi_s32 hi_db_insert_key(hi_db_table *table, hi_db_key *key);

hi_s32 hi_db_delete_key(hi_db_table *table, hi_char key_name[]);

hi_s32 hi_db_get_key_by_name(hi_db_table *table, hi_char key_name[], hi_db_key *key);

#ifdef __cplusplus
}
#endif

#endif

