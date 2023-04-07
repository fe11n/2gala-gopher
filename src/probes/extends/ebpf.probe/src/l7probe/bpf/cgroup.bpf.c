/******************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * gala-gopher licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *     http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: wo_cow
 * Create: 2023-02-20
 * Description: pod lifecycle
 ******************************************************************************/
#ifdef BPF_PROG_USER
#undef BPF_PROG_USER
#endif
#define BPF_PROG_KERN
#include "include/pod.h"
#include "kern_sock.h"

char g_linsence[] SEC("license") = "GPL";

#define BPF_F_INDEX_MASK        0xffffffffULL
#define BPF_F_CURRENT_CPU       BPF_F_INDEX_MASK
#define MAX_CGRP_NUM 8192

struct {
    __uint(type, GOPHER_BPF_MAP_TYPE_PERF);
    __uint(key_size, sizeof(u32));
    __uint(value_size, sizeof(u32));
    __uint(max_entries, 64);
} cgroup_msg_map SEC(".maps");

#ifndef __USE_RING_BUF
struct {
    __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __uint(key_size, sizeof(u32));
    __uint(value_size, sizeof(struct cgroup_msg_data_t));
    __uint(max_entries, 1);
} tmp_map SEC(".maps");
#endif

static __always_inline int check_root_id(struct cgroup *cgrp)
{
    if (!cgrp)
        return 0;

    struct cgroup_root *root =  _(cgrp->root);
    if (!root)
        return 0;

    int root_id = _(root->hierarchy_id);
    return root_id == 1 ? 1 : 0;
}

static __always_inline void report_cgrp_change(void *ctx, enum cgrp_event_t cgrp_event, const char *path)
{
    u32 key = 0;
#ifdef __USE_RING_BUF
    struct cgroup_msg_data_t *msg_data = bpf_ringbuf_reserve(&cgroup_msg_map, sizeof(struct cgroup_msg_data_t), 0);
#else
    struct cgroup_msg_data_t *msg_data = bpf_map_lookup_elem(&tmp_map, &key);
#endif

    if (!msg_data)
        return;

    msg_data->cgrp_event = cgrp_event;
    bpf_probe_read_str(msg_data->cgrp_path, MAX_CGRP_PATH, path);

    (void)bpf_perf_event_output(ctx, &cgroup_msg_map, BPF_F_CURRENT_CPU, msg_data, sizeof(struct cgroup_msg_data_t));
}

KRAWTRACE(cgroup_mkdir, bpf_raw_tracepoint_args)
{
    struct cgroup *cgrp = (struct cgroup *)ctx->args[0];

    if (!check_root_id(cgrp))
        return 0;

    const char *path = (const char *)ctx->args[1];
    report_cgrp_change(ctx, CGRP_MK, path);
    return 0;
}

KRAWTRACE(cgroup_rmdir, bpf_raw_tracepoint_args)
{
    struct cgroup *cgrp = (struct cgroup *)ctx->args[0];
    if (!check_root_id(cgrp))
        return 0;

    const char *path = (const char *)ctx->args[1];
    report_cgrp_change(ctx, CGRP_RM, path);
    return 0;
}

