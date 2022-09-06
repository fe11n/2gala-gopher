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
 * Author: luzhihao
 * Create: 2022-07-13
 * Description: proc map defined
 ******************************************************************************/
#ifndef __PROC_MAP_H__
#define __PROC_MAP_H__

#pragma once

#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "proc.h"

#define __PROC_MAX      1000
struct bpf_map_def SEC("maps") g_proc_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(u32),    // proc id
    .value_size = sizeof(struct proc_data_s),
    .max_entries = __PROC_MAX,
};

static __always_inline __maybe_unused struct proc_data_s* get_proc_entry(u32 proc_id)
{
    return (struct proc_data_s *)bpf_map_lookup_elem(&g_proc_map, &proc_id);
}

static __always_inline __maybe_unused int proc_add_entry(u32 proc_id, const char *comm)
{
    struct proc_data_s proc_data = {.proc_id = proc_id};
    __builtin_memcpy(proc_data.comm, comm, TASK_COMM_LEN);

    return bpf_map_update_elem(&g_proc_map, &proc_id, &proc_data, BPF_ANY);
}

static __always_inline __maybe_unused int proc_put_entry(u32 proc_id)
{
    return bpf_map_delete_elem(&g_proc_map, &proc_id);
}

#endif
