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
 * Description: bpf load prog
 ******************************************************************************/
#ifndef __BPF_PROG__H
#define __BPF_PROG__H

#pragma once

#include <bpf/libbpf.h>
#include <bpf/bpf.h>

#include "common.h"
#include "args.h"

#define THREAD_OUTPUT_PATH "/sys/fs/bpf/gala-gopher/__taskprobe_thread_output"
#define PROC_OUTPUT_PATH "/sys/fs/bpf/gala-gopher/__taskprobe_proc_output"
#define ARGS_PATH "/sys/fs/bpf/gala-gopher/__taskprobe_args"
#define THREAD_PATH "/sys/fs/bpf/gala-gopher/__taskprobe_thread"
#define PROC_PATH "/sys/fs/bpf/gala-gopher/__taskprobe_proc"

struct bpf_prog_s* load_glibc_bpf_prog(struct probe_params *args);
struct bpf_prog_s* load_thread_bpf_prog(struct probe_params *args);
struct bpf_prog_s* load_proc_bpf_prog(void *probe);

#endif
