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
 * Author: Hubble_Zhu
 * Create: 2021-04-26
 * Description: provide gala-gopher test
 ******************************************************************************/
#ifndef __TEST_PROBES_H__
#define __TEST_PROBES_H__

void TestSystemMeminfoProbe(void);
void TestSystemCpuProbe(void);
void TestSystemDiskIOStatProbe(void);
void TestSystemDiskProbe(void);
void TestSystemNetProbe(void);
void TestSystemdNetTcpProbe(void);
void TestSystemProcProbe(void);
void TestVirtInfoProbe(void);
void TestEventProbe(void);

#endif

