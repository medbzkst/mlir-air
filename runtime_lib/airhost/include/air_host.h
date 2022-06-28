//===- air_host.h -----------------------------------------------*- C++ -*-===//
//
// Copyright (C) 2020-2022, Xilinx Inc.
// Copyright (C) 2022, Advanced Micro Devices, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//===----------------------------------------------------------------------===//

#ifndef AIR_HOST_H
#define AIR_HOST_H

#include "acdc_queue.h"
#include "hsa_defs.h"

#include "test_library.h"

#include <stdlib.h>
#include <string>

extern "C" {

#define AIR_VCK190_SHMEM_BASE  0x020100000000LL
#define AIR_VCK190_L2_DMA_BASE 0x020240000000LL
#define AIR_VCK190_DDR_BASE 0x2000LL
#define AIR_BBUFF_BASE 0x81C000000LL
// library operations

aie_libxaie_ctx_t *air_init_libxaie1();
void air_deinit_libxaie1(aie_libxaie_ctx_t*);

// runtime operations
//

hsa_status_t air_get_agents(void *data);
hsa_status_t air_get_agent_info(queue_t *queue, air_agent_info_t attribute, void* data);

// queue operations
//

hsa_status_t air_queue_create(uint32_t size, uint32_t type, queue_t **queue, uint64_t paddr);

hsa_status_t air_queue_dispatch(queue_t *queue, uint64_t doorbell, dispatch_packet_t *pkt);
hsa_status_t air_queue_wait(queue_t *queue, dispatch_packet_t *pkt);
hsa_status_t air_queue_dispatch_and_wait(queue_t *queue, uint64_t doorbell, dispatch_packet_t *pkt);

// packet utilities
//

struct l2_dma_cmd_t {
  uint8_t select;
  uint16_t length;
  uint16_t uram_addr;
  uint8_t id;
};

struct l2_dma_rsp_t {
  uint8_t id;
};

// initialize pkt as a herd init packet with given parameters
hsa_status_t air_packet_herd_init(dispatch_packet_t *pkt, uint16_t herd_id,
                                  uint8_t start_col, uint8_t num_cols,
                                  uint8_t start_row, uint8_t num_rows);
                                  //uint8_t start_row, uint8_t num_rows,
                                  //uint16_t dma0, uint16_t dma1);

hsa_status_t air_packet_device_init(dispatch_packet_t *pkt, uint32_t num_cols);

hsa_status_t air_packet_get_capabilities(dispatch_packet_t *pkt, uint64_t return_address);

hsa_status_t air_packet_hello(dispatch_packet_t *pkt, uint64_t value);

hsa_status_t air_packet_put_stream(dispatch_packet_t *pkt, uint64_t stream, uint64_t value);
hsa_status_t air_packet_get_stream(dispatch_packet_t *pkt, uint64_t stream);

hsa_status_t air_packet_l2_dma(dispatch_packet_t *pkt, uint64_t stream, l2_dma_cmd_t cmd);

hsa_status_t air_packet_cdma_memcpy(dispatch_packet_t *pkt, uint64_t dest,
                                    uint64_t source, uint32_t length);

hsa_status_t air_packet_cdma_configure(dispatch_packet_t *pkt, uint64_t dest,
                                       uint64_t source, uint32_t length);


hsa_status_t air_packet_aie_lock_range(dispatch_packet_t *pkt, uint16_t herd_id,
                                 uint64_t lock_id, uint64_t acq_rel, uint64_t value,
                                 uint8_t start_col, uint8_t num_cols,
                                 uint8_t start_row, uint8_t num_rows);

hsa_status_t air_packet_aie_lock(dispatch_packet_t *pkt, uint16_t herd_id,
                                 uint64_t lock_id, uint64_t acq_rel, uint64_t value,
                                 uint8_t col, uint8_t row);

hsa_status_t air_packet_tile_status(dispatch_packet_t *pkt, uint8_t col, uint8_t row);
hsa_status_t air_packet_dma_status(dispatch_packet_t *pkt, uint8_t col, uint8_t row);
hsa_status_t air_packet_shimdma_status(dispatch_packet_t *pkt, uint8_t col);

hsa_status_t air_packet_nd_memcpy(dispatch_packet_t *pkt, uint16_t herd_id,
                                 uint8_t col, uint8_t direction, uint8_t channel,
                                 uint8_t burst_len, uint8_t memory_space,
                                 uint64_t phys_addr, uint32_t transfer_length1d,
                                 uint32_t transfer_length2d, uint32_t transfer_stride2d,
                                 uint32_t transfer_length3d, uint32_t transfer_stride3d,
                                 uint32_t transfer_length4d, uint32_t transfer_stride4d);

hsa_status_t air_packet_barrier_and(barrier_and_packet_t *pkt, 
                                    uint64_t dep_signal0,
                                    uint64_t dep_signal1,
                                    uint64_t dep_signal2,
                                    uint64_t dep_signal3,
                                    uint64_t dep_signal4);
hsa_status_t air_packet_barrier_or(barrier_or_packet_t *pkt, 
                                    uint64_t dep_signal0,
                                    uint64_t dep_signal1,
                                    uint64_t dep_signal2,
                                    uint64_t dep_signal3,
                                    uint64_t dep_signal4);

// herd descriptors generated by compiler
//

struct air_herd_shim_desc_t {
  int64_t *location_data;
  int64_t *channel_data;
};

struct air_herd_desc_t {
  int64_t name_length;
  char *name;
  air_herd_shim_desc_t *shim_desc;
};

struct air_rt_herd_desc_t {
  queue_t *q;
  air_herd_desc_t *herd_desc;
};

struct air_partition_desc_t {
  int64_t name_length;
  char *name;
  uint64_t herd_length;
  air_herd_desc_t **herd_descs;
};

struct air_rt_partition_desc_t {
  queue_t *q;
  air_partition_desc_t *partition_desc;
};

struct air_module_desc_t {
  uint64_t partition_length;
  air_partition_desc_t **partition_descs;
};

// AIE config functions generated by AIE dialect lowering
struct air_rt_aie_functions_t {
  void (*configure_cores)(aie_libxaie_ctx_t *);
  void (*configure_switchboxes)(aie_libxaie_ctx_t *);
  void (*initialize_locks)(aie_libxaie_ctx_t *);
  void (*configure_dmas)(aie_libxaie_ctx_t *);
  void (*start_cores)(aie_libxaie_ctx_t *);
};

// AIR module shared library helpers
//

typedef size_t air_module_handle_t;

// return 0 on failure, nonzero otherwise
air_module_handle_t air_module_load_from_file(const char* filename, queue_t *q=0);

// return 0 on success, nonzero otherwise
int32_t air_module_unload(air_module_handle_t handle);

air_module_desc_t *air_module_get_desc(air_module_handle_t handle);

air_partition_desc_t *air_partition_get_desc(air_module_handle_t handle, const char *name);

air_herd_desc_t *air_herd_get_desc(air_module_handle_t handle,
                                   air_partition_desc_t *partition,
                                   const char *name);

uint64_t air_partition_load(const char *name);

uint64_t air_herd_load(const char *name);

//typedef uint32_t air_herd_status_t;
//air_herd_status_t air_herd_status(air_herd_desc_t *herd_desc)
//bool air_herd_is_loaded(air_herd_desc_t *herd_desc);

// memory operations
//

void* air_mem_alloc(size_t size);

uint64_t air_mem_get_paddr(void *vaddr);

int air_mem_free(void *vaddr, size_t size);

}

std::string air_get_ddr_bar();
std::string air_get_aie_bar();
std::string air_get_bram_bar();

#endif // AIR_HOST_H
