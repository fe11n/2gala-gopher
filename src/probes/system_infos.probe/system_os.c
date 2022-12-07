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
 * Author: dowzyx
 * Create: 2022-09-20
 * Description: os release infos of host
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "nprobe_fprintf.h"
#include "system_os.h"

#define METRICS_OS_NAME         "system_os"
#define ENTITY_OS_NAME          "host"
#define OS_RELEASE_PATH1        "/etc/os-release"
#define OS_RELEASE_PATH2        "/usr/lib/os-release"
#define OS_RELEASE_ID           "/usr/bin/cat %s | grep -w ID | awk -F'\"' \'{print $2}\'"
#define OS_RELEASE_PRETTYT_NAME "/usr/bin/cat %s | grep -w PRETTY_NAME | awk -F'\"' \'{print $2}\'"
#define OS_LATEST_VERSION       "/usr/bin/cat %s | grep -e %s.*version | awk -F'=' \'{print $2}\'"
#define OS_LATEST_KVERSION      "/usr/bin/cat %s | grep kernelversion | awk -F'=' \'{print $2}\'"
#define IS_CMD_HOSTNAME_EXIST   "which hostname 2>/dev/null"
#define OS_GET_ALL_ADDRS        "hostname -I"
#define OS_DETECT_VIRT_ENV      "systemd-detect-virt -v"
#define LEN_1MB                 (1024 * 1024)     // 1 MB


static struct node_infos g_nodeinfos = {0};

static int do_get_os_release_path(char path[], int path_len)
{
    // The file /etc/os-release takes precedence over /usr/lib/os-release
    path[0] = 0;
    if (!access(OS_RELEASE_PATH1, 0)) {
        (void)strncpy(path, OS_RELEASE_PATH1, path_len - 1);
        return 0;
    }
    if (!access(OS_RELEASE_PATH2, 0)) {
        (void)strncpy(path, OS_RELEASE_PATH2, path_len - 1);
        return 0;
    }
    ERROR("[SYSTEM_OS] os-release file isn't /etc/os-release or /usr/lib/os-release.\n");
    return -1;
}

static int do_read_line(char *command, char line[])
{
    FILE *f = NULL;

    if (command == NULL) {
        return -1;
    }

    f = popen(command, "r");
    if (f == NULL) {
        return -1;
    }
    line[0] = 0;
    if (fgets(line, LINE_BUF_LEN, f) == NULL) {
        (void)pclose(f);
        return -1;
    }
    SPLIT_NEWLINE_SYMBOL(line);
    (void)pclose(f);
    return 0;
}

static int get_os_release_info(struct node_infos *infos)
{
    char os_release_path[COMMAND_LEN];
    char os_latest_path[COMMAND_LEN];
    char cmd[COMMAND_LEN];
    char line[LINE_BUF_LEN];

    if (do_get_os_release_path(os_release_path, COMMAND_LEN) < 0) {
        ERROR("[SYSTEM_OS] get os-release file failed.\n");
        return -1;
    }

    cmd[0] = 0;
    (void)snprintf(cmd, COMMAND_LEN, OS_RELEASE_ID, os_release_path);
    if (do_read_line(cmd, line) < 0) {
        ERROR("[SYSTEM_OS] get os id failed.\n");
        return -1;
    }
    strncpy(infos->os_id, line, MAX_FIELD_LEN - 1);

    cmd[0] = 0;
    (void)snprintf(cmd, COMMAND_LEN, OS_RELEASE_PRETTYT_NAME, os_release_path);
    if (do_read_line(cmd, line) < 0) {
        ERROR("[SYSTEM_OS] get os pretty_name failed.\n");
        return -1;
    }
    strncpy(infos->os_pretty_name, line, MAX_FIELD_LEN - 1);

    os_latest_path[0] = 0;
    if (!strcasecmp(infos->os_id, "openEuler")) {
        (void)strncpy(os_latest_path, "/etc/openEuler-latest", COMMAND_LEN - 1);
    } else if (!strcasecmp(infos->os_id, "euleros")) {
        (void)strncpy(os_latest_path, "/etc/euleros-latest", COMMAND_LEN - 1);
    }

    cmd[0] = 0;
    (void)snprintf(cmd, COMMAND_LEN, OS_LATEST_VERSION, os_latest_path, "euler");
    if (do_read_line(cmd, line) < 0) {
        ERROR("[SYSTEM_OS] get os version failed.\n");
        return -1;
    }
    (void)strncpy(infos->os_version, line, MAX_FIELD_LEN - 1);
    
    cmd[0] = 0;
    (void)snprintf(cmd, COMMAND_LEN, OS_LATEST_KVERSION, os_latest_path);
    if (do_read_line(cmd, line) < 0) {
        ERROR("[SYSTEM_OS] get os kernelversion failed.\n");
        return -1;
    }
    (void)strncpy(infos->kernel_version, line, MAX_FIELD_LEN - 1);

    return 0;
}

static int parse_netmask(char *ip_addr)
{
    char *colon = strchr(ip_addr, '/');
    if (colon == NULL) {
        return 32;
    }
    return (atoi(colon + 1) > 32) ? 0 : atoi(colon + 1);
}

/* 检查IP是否在某网段内 */
static int check_ip_in_net_segment(char *ip_str, char *net_str)
{
    int ips[4];
    int nets[4];
    if (sscanf(ip_str, "%d.%d.%d.%d", &ips[0], &ips[1], &ips[2], &ips[3]) < 4) {
        ERROR("[SYSTEM_OS] sscanf ip_addr_str:%s faield.\n", ip_str);
        return false;
    }
    if (sscanf(net_str, "%d.%d.%d.%d", &nets[0], &nets[1], &nets[2], &nets[3]) < 4) {
        ERROR("[SYSTEM_OS] sscanf net_str:%s faield.\n", net_str);
        return false;
    }

    int mask = parse_netmask(net_str);
    for (int i = 0; i < 4; i++) {
        int temp = (mask - 8 > 0) ? 8 : (mask > 0) ? mask : 0;
        if ((ips[i] & temp) != (nets[i] & temp)) {
            return false;
        }
        mask -= 8;
    }
    return true;
}

static int get_ip_addr(struct node_infos *infos, struct probe_params * params)
{
    char line[LINE_BUF_LEN];

    if (do_read_line(IS_CMD_HOSTNAME_EXIST, line) < 0) {
        ERROR("[SYSTEM_OS] no hostname in this host.\n");
        return -1;
    }

    if (do_read_line(OS_GET_ALL_ADDRS, line) < 0) {
        ERROR("[SYSTEM_OS] get all addresses for this host failed.\n");
        return -1;
    }

    if (params == NULL || params->host_ip_list[0][0] == 0) {
        // 不需要过滤业务IP时，输出全部IP地址
        (void)strncpy(infos->ip_addr, line, MAX_IP_ADDRS_LEN - 1);
        return 0;
    }

    char *ip_addr_str = infos->ip_addr;
    int ip_addr_len = MAX_IP_ADDRS_LEN;
    int first_flag = 0;
    char *p = strtok(line, " ");
    while (p != NULL) {
        for (int i = 0; i < MAX_IP_NUM; i++) {
            if (params->host_ip_list[i][0] == 0) {
                break;
            }
            if (check_ip_in_net_segment(p, params->host_ip_list[i]) == true) {
                if (first_flag == 0) {
                    (void)__snprintf(&ip_addr_str, ip_addr_len, &ip_addr_len, "%s", p);
                    first_flag = 1;
                } else {
                    (void)__snprintf(&ip_addr_str, ip_addr_len, &ip_addr_len, " %s", p);
                }
                break;
            }
        }
        p = strtok(NULL, " ");
    }

    return 0;
}

static int get_resource_info(struct node_infos *infos)
{
    if (infos == NULL) {
        return -1;
    }
    infos->cpu_num = (u64)sysconf(_SC_NPROCESSORS_CONF);
    infos->total_memory = (u64)sysconf(_SC_PAGESIZE) * (u64)sysconf(_SC_PHYS_PAGES) / LEN_1MB;
    return 0;
}

static int get_host_name(struct node_infos *infos)
{
    if (infos == NULL) {
        return -1;
    }
    if (gethostname(infos->host_name, MAX_FIELD_LEN) < 0) {
        ERROR("[SYSTEM_OS] get hostname failed.\n");
        return -1;
    }
    return 0;
}

static int get_host_type(struct node_infos *infos)
{
    char line[LINE_BUF_LEN];

    if (infos == NULL) {
        return -1;
    }
    if (do_read_line(OS_DETECT_VIRT_ENV, line) < 0) {
        ERROR("[SYSTEM_OS] systemd detect virt environment failed.\n");
        return -1;
    }
    if (!strcmp(line, "none")) {
        infos->is_host_vm = 0;
    } else {
        infos->is_host_vm = 1;
    }
    return 0;
}

static char g_first_get = 1;
int system_os_probe(struct probe_params * params)
{
    // 部分节点(如系统、版本等)信息不会变更，仅在探针启动时获取一次
    if (g_first_get == 1) {
        (void)get_os_release_info(&g_nodeinfos);
        (void)get_resource_info(&g_nodeinfos);
        (void)get_host_type(&g_nodeinfos);

        g_first_get = 0;
    }
    (void)get_ip_addr(&g_nodeinfos, params);
    (void)get_host_name(&g_nodeinfos);

    nprobe_fprintf(stdout, "|%s|%s|%s|%s|%llu|%llu|%s|%s|%d|\n",
        METRICS_OS_NAME,
        g_nodeinfos.os_version,
        g_nodeinfos.host_name,
        g_nodeinfos.kernel_version,
        g_nodeinfos.cpu_num,
        g_nodeinfos.total_memory,
        g_nodeinfos.ip_addr,
        g_nodeinfos.is_host_vm == 0 ? "pm" : "vm",
        1); // metric is a fixed number
    return 0;
}