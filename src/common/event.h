/******************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * gala-gopher licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *     http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: Mr.lu
 * Create: 2022-05-16
 * Description:
 ******************************************************************************/
#ifndef __GOPHER_EVT_H__
#define __GOPHER_EVT_H__

#pragma once

#include "hash.h"

#define MAX_ENTITY_NAME_LEN     128
#define MAX_EVT_NUM             1000

enum evt_sec_e {
    EVT_SEC_INFO = 0,
    EVT_SEC_WARN,
    EVT_SEC_ERROR,
    EVT_SEC_FATAL,

    EVT_SEC_MAX
};

struct evt_ts_hash_t {
    H_HANDLE;
    char entity_id[MAX_ENTITY_NAME_LEN];
    time_t evt_ts;
};

void report_logs(const char* entityName,
                 const char* entityId,
                 const char* metrics,
                 enum evt_sec_e sec,
                 const char * fmt, ...);

void init_event_mgr(unsigned int time_out, char *lang_type);

#endif
