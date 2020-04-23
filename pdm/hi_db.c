/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description: baseparam database operation function
* Author: wan
* Create: 2019-12-6
*/

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#endif

#include "hi_module.h"
#include "hi_db.h"
#include "hi_osal.h"

/*
 * the db struct:
 * DB :   | dbcheck | datalen | table 0 | ... ... | table n |
 * TABLE: | name | datasize | key 0 | ... ...| key n |
 * KEY:   | name | valuelen | value |
 */
#ifdef __KERNEL__
#define DB_MALLOC(size)     osal_vmalloc(HI_ID_PDM, size)
#define DB_FREE(p)          osal_vfree(HI_ID_PDM, p)
#else
#define DB_MALLOC(size)     malloc(size)
#define DB_FREE(p)          free(p)
#endif


#define HI_ERR_DB(format, arg...)
#define HI_INFO_DB(format, arg...)


static hi_char g_db_check[HI_DB_CHECK_LEN] = {"###"};


hi_s32 hi_db_create(hi_db *db)
{
    if (db == HI_NULL) {
        HI_ERR_DB("ERR: para is null\n");
        return HI_FAILURE;
    }

    db->data = DB_MALLOC(HI_DB_MAX_SIZE);
    if (db->data == HI_NULL) {
        HI_ERR_DB("ERR: malloc error\n");
        return HI_FAILURE;
    }

    memset(db->data, 0x00, HI_DB_MAX_SIZE);

    memcpy(db->data, (hi_u8 *)g_db_check, sizeof(g_db_check));

    db->data_len = HI_DB_CHECK_LEN + sizeof(hi_u32);
    memcpy(db->data + HI_DB_CHECK_LEN, (hi_u8 *)(&(db->data_len)), sizeof(hi_u32));

    return HI_SUCCESS;
}

hi_s32 hi_db_destroy(hi_db *db)
{
    if (db == HI_NULL) {
        HI_ERR_DB("ERR: para is null\n");
        return HI_FAILURE;
    }

    if (db->data_len > HI_DB_MAX_SIZE) {
        HI_ERR_DB("ERR: data len too large\n");
        return HI_FAILURE;
    }

    db->data_len = 0;

    if (db->data != HI_NULL) {
        DB_FREE(db->data);
        db->data = HI_NULL;
    }

    return HI_SUCCESS;
}

hi_s32 hi_db_get_db_from_mem(hi_u8 *db_mem, hi_db *db)
{
    hi_char db_check[HI_DB_CHECK_LEN] = {0};

    if ((db_mem == HI_NULL) || (db == HI_NULL)) {
        HI_ERR_DB("ERR: para is null\n");
        return HI_FAILURE;
    }

    memcpy(db_check, (hi_char *)db_mem, HI_DB_CHECK_LEN);

    if (osal_strncmp(db_check, strlen(g_db_check), g_db_check, strlen(g_db_check)) != 0) {
        HI_ERR_DB("ERR: this is not a db\n");
        return HI_FAILURE;
    }

    db->data_len = *(hi_u32 *)(db_mem + HI_DB_CHECK_LEN);
    if (db->data_len > HI_DB_MAX_SIZE) {
        HI_ERR_DB("ERR: db size is error\n");
        return HI_FAILURE;
    }

    db->data = db_mem;

    return HI_SUCCESS;
}

hi_s32 hi_db_insert_table(hi_db *db, hi_db_table *table)
{
    hi_u32 need_len;

    if ((db == HI_NULL) || (table == HI_NULL)) {
        HI_ERR_DB("ERR: para is null\n");
        return HI_FAILURE;
    }

    need_len = HI_DB_MAX_NAME_LEN + sizeof(hi_u32);
    if (db->data_len + need_len > HI_DB_MAX_SIZE) {
        HI_ERR_DB("ERR: db is full\n");
        return HI_FAILURE;
    }

    table->db = db;

    memcpy((db->data + db->data_len), table->name, HI_DB_MAX_NAME_LEN);
    db->data_len += HI_DB_MAX_NAME_LEN;

    table->data_size = 0;
    memcpy((db->data + db->data_len), (hi_u8 *)(&(table->data_size)), sizeof(hi_u32));
    db->data_len += sizeof(hi_u32);

    /* we need to update db datalen */
    memcpy(db->data + HI_DB_CHECK_LEN, (hi_u8 *)(&(db->data_len)), sizeof(hi_u32));

    table->data = db->data + db->data_len;

    return HI_SUCCESS;
}

hi_s32 hi_db_delete_table(hi_db *db, hi_char table_name[])
{
    HI_ERR_DB("ERR: delete table is not support\n");
    return HI_FAILURE;
}

hi_s32 hi_db_get_table_by_name(hi_db *db, hi_char table_name[], hi_db_table *table)
{
    hi_db_table         table_tmp;
    hi_u8               *data = HI_NULL;

    if ((db == HI_NULL) || (table == HI_NULL) || (table_name == HI_NULL)) {
        HI_ERR_DB("ERR: para is null\n");
        return HI_FAILURE;
    }

    data = db->data + HI_DB_CHECK_LEN + sizeof(hi_u32);

    while ((hi_u32)(data - db->data) < db->data_len) {
        table_tmp.db = db;

        memset(table_tmp.name, 0, sizeof(table_tmp.name));
        memcpy(table_tmp.name, data, HI_DB_MAX_NAME_LEN - 1);
        data += HI_DB_MAX_NAME_LEN;

        table_tmp.data_size = *((hi_u32 *)(data));
        data += sizeof(hi_u32);

        table_tmp.data = data;

        /* find the table */
        if (osal_strncmp(table_tmp.name, strlen(table_name), table_name, strlen(table_name)) == 0) {
            *table = table_tmp;
            break;
        } else {
            data += table_tmp.data_size;
        }
    }

    if ((hi_u32)(data - db->data) >= db->data_len) {
        HI_ERR_DB("ERR: can not find table %s\n", table_name);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 hi_db_insert_key(hi_db_table *table, hi_db_key *key)
{
    hi_u32  need_len;

    if ((table == HI_NULL) || (key == HI_NULL)) {
        HI_ERR_DB("ERR: para is null\n");
        return HI_FAILURE;
    }

    need_len = HI_DB_MAX_NAME_LEN + sizeof(hi_u32) + key->value_size;
    if (table->db->data_len + need_len > HI_DB_MAX_SIZE) {
        HI_ERR_DB("ERR: db is full\n");
        return HI_FAILURE;
    }

    memcpy((table->data + table->data_size), key->name, HI_DB_MAX_NAME_LEN);
    table->data_size += HI_DB_MAX_NAME_LEN;

    memcpy((table->data + table->data_size), (hi_u8 *)(&(key->value_size)), sizeof(hi_u32));
    table->data_size += sizeof(hi_u32);

    memcpy((table->data + table->data_size), (hi_u8 *)(key->value), key->value_size);
    table->data_size += key->value_size;

    /* we need update table datasize */
    memcpy(table->data - sizeof(hi_u32), (hi_u8 *)(&(table->data_size)), sizeof(hi_u32));

    /* we need update db datalen */
    table->db->data_len += need_len;
    memcpy(table->db->data + HI_DB_CHECK_LEN, (hi_u8 *)(&(table->db->data_len)), sizeof(hi_u32));

    return HI_SUCCESS;
}

hi_s32 hi_db_delete_key(hi_db_table *table, hi_char key_name[])
{
    HI_ERR_DB("ERR: delete key is not support\n");
    return HI_FAILURE;
}

hi_s32 hi_db_get_key_by_name(hi_db_table *table, hi_char key_name[], hi_db_key *key)
{
    hi_db_key           key_tmp;
    hi_u8               *data = HI_NULL;
    hi_u32              str_len = 0;

    if ((table == HI_NULL) || (key == HI_NULL) || (key_name == HI_NULL)) {
        HI_ERR_DB("ERR: para is null\n");
        return HI_FAILURE;
    }

    data = table->data;

    while ((hi_u32)(data - table->data) < table->data_size) {
        memset(key_tmp.name, 0, sizeof(key_tmp.name));
        memcpy(key_tmp.name, data, HI_DB_MAX_NAME_LEN - 1);
        data += HI_DB_MAX_NAME_LEN;

        key_tmp.value_size = *((hi_u32 *)data);
        data += sizeof(hi_u32);

        key_tmp.value = data;

        str_len = strlen(key_tmp.name) > strlen(key_name) ? strlen(key_tmp.name) : strlen(key_name);
        /* find the key */
        if (osal_strncmp(key_tmp.name, str_len, key_name, str_len) == 0) {
            *key = key_tmp;
            break;
        } else {
            data += key_tmp.value_size;
        }
    }

    if ((hi_u32)(data - table->data) >= table->data_size) {
        HI_INFO_DB("ERR: can not find key %s\n", key_name);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
