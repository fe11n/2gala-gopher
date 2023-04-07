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
 * Author: luzhihao
 * Create: 2023-02-22
 * Description: Socket defined
 ******************************************************************************/
#ifndef __L7_CONNECT_H__
#define __L7_CONNECT_H__

#pragma once

#include "include/l7.h"

#if (CURRENT_KERNEL_VERSION  >= KERNEL_VERSION(5, 10, 0))
#define __USE_RING_BUF
#endif

#ifdef __USE_RING_BUF
#define GOPHER_BPF_MAP_TYPE_PERF   BPF_MAP_TYPE_RINGBUF
#else
#define GOPHER_BPF_MAP_TYPE_PERF   BPF_MAP_TYPE_PERF_EVENT_ARRAY
#endif

#define L7_CONN_BPF_PATH          "/sys/fs/bpf/gala-gopher/__l7_connect"


#ifndef BPF_PROG_KERN
struct sockaddr {
    unsigned short  sa_family;
    char            sa_data[14];
};

struct sockaddr_in {
    unsigned short  sin_family;
    unsigned short  sin_port;
    unsigned int    sin4_addr;
    unsigned char   pad[8];
};

struct sockaddr_in6 {
    unsigned short  sin_family;
    unsigned short  sin_port;
    unsigned int    sin_flowinfo;
    unsigned char   sin6_addr[IP6_LEN];
    unsigned int    sin6_scope_id;
};
#endif

union sockaddr_t {
    struct sockaddr sa;
    struct sockaddr_in in4;
    struct sockaddr_in6 in6;
};

enum l7_direction_t {
    L7_EGRESS,
    L7_INGRESS,
};

enum l4_role_t {
    L4_UNKNOW = 0,              // udp
    L4_CLIENT,
    L4_SERVER,
};

enum l7_role_t {
    L7_UNKNOW = 0,
    L7_CLIENT,
    L7_SERVER,
};

struct conn_id_s {
    int tgid;                   // process id
    int fd;
};

struct conn_info_s {
    struct conn_id_s id;
    char is_ssl;
    char is_reported;
    char pad[2];
    enum l4_role_t l4_role;     // TCP client or server; udp unknow
    enum l7_role_t l7_role;     // RPC client or server
    enum proto_type_t protocol; // L7 protocol type
    union sockaddr_t remote_addr; // TCP remote address; UDP datagram address
};

typedef u64 conn_ctx_t;         // pid & tgid

// The information of socket connection
struct sock_conn_s {
    struct conn_info_s info;

    // The number of bytes written/read on this socket connection.
    u64 wr_bytes;
    u64 rd_bytes;
};

enum conn_evt_e {
    CONN_EVT_OPEN,
    CONN_EVT_CLOSE,
};

struct conn_open_s {
    union sockaddr_t addr;
    enum l4_role_t l4_role;     // TCP client or server; udp unknow
    char is_ssl;
    char pad[3];
};

struct conn_close_s {
    u64 wr_bytes;
    u64 rd_bytes;
};

// Exchange data between user mode/kernel using
// 'conn_control_events' perf channel.
struct conn_ctl_s {
    struct conn_id_s conn_id;
    u64 timestamp_ns;

    enum conn_evt_e type;
    struct conn_open_s open;
    struct conn_close_s close;
};

// Exchange data between user mode/kernel using
// 'conn_stats_events' perf channel.
struct conn_stats_s {
    struct conn_id_s conn_id;
    u64 timestamp_ns;

    // The number of bytes written on this connection.
    u64 wr_bytes;
    // The number of bytes read on this connection.
    u64 rd_bytes;
};

// Exchange data between user mode/kernel using
// 'conn_data_events' perf channel.
#define LOOP_LIMIT 4
#define CONN_DATA_MAX_SIZE  (10 * 1024)
struct conn_data_s {
    struct conn_id_s conn_id;

    u64 timestamp_ns;   // The timestamp when syscall completed.

    enum proto_type_t proto;
    enum l7_role_t l7_role;     // RPC client or server
    enum l7_direction_t direction;  // Only for tcp connection

    u64 offset_pos;     // The position is for the first data of this message.
    size_t data_size;   // The actually data size, maybe less than msg_size.
    char data[CONN_DATA_MAX_SIZE];
};

// struct conns_hash_t {
//     H_HANDLE;
//     struct conn_id_s conn_id; // key
//     struct conn_data_s conn_data; // TODO
// };
#endif
