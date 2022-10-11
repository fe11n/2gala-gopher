/******************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.
 * gala-gopher licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *     http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: Mr.lu
 * Create: 2022-08-18
 * Description: stack defined
 ******************************************************************************/
#ifndef __GOPHER_STACK_H__
#define __GOPHER_STACK_H__

#pragma once

#include "common.h"

#define AGGRE_PERIOD    (1 * 60 * 1000) // 1min
#define SAMPLE_PERIOD   (10)            // 10ms
#define TMOUT_PERIOD    (AGGRE_PERIOD / 1000) // Second as unit
#define PROC_CACHE_MAX_COUNT    10    // Cache 10 proc symbols
#define DIV_ROUND_UP(NUM, DEN) ((NUM + DEN - 1) / DEN)

#define PERCPU_SAMPLE_COUNT     (2 * DIV_ROUND_UP(AGGRE_PERIOD, SAMPLE_PERIOD))

struct stack_pid_s {
    u64 real_start_time;
    int proc_id;
    char comm[TASK_COMM_LEN];
};

struct stack_id_s {
    int user_stack_id;
    int kern_stack_id;
    struct stack_pid_s pid;
};

#endif
