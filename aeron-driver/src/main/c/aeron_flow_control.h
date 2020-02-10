/*
 * Copyright 2014-2020 Real Logic Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AERON_FLOW_CONTROL_H
#define AERON_FLOW_CONTROL_H

#include "aeron_socket.h"
#include "aeron_driver_common.h"
#include "aeronmd.h"

#define AERON_MAX_FLOW_CONTROL_STRATEGY_RECEIVER_TIMEOUT_NS (2 * 1000 * 1000 * 1000L)
#define AERON_MAX_FLOW_CONTROL_STRAEGY_NAME "max"
#define AERON_MIN_FLOW_CONTROL_STRAEGY_NAME "min"
#define AERON_TAGGED_FLOW_CONTROL_STRAEGY_NAME "tagged"

typedef int64_t (*aeron_flow_control_strategy_on_idle_func_t)(
    void *state,
    int64_t now_ns,
    int64_t snd_lmt,
    int64_t snd_pos,
    bool is_end_of_stream);

typedef int64_t (*aeron_flow_control_strategy_on_sm_func_t)(
    void *state,
    const uint8_t *sm,
    size_t length,
    struct sockaddr_storage *recv_addr,
    int64_t snd_lmt,
    int32_t initial_term_id,
    size_t position_bits_to_shift,
    int64_t now_ns);

typedef int (*aeron_flow_control_strategy_fini_func_t)(aeron_flow_control_strategy_t *strategy);

typedef struct aeron_flow_control_strategy_stct
{
    aeron_flow_control_strategy_on_sm_func_t on_status_message;
    aeron_flow_control_strategy_on_idle_func_t on_idle;
    aeron_flow_control_strategy_fini_func_t fini;
    void *state;
}
aeron_flow_control_strategy_t;

typedef struct aeron_flow_control_tagged_options_stct
{
    size_t strategy_name_length;
    const char *strategy_name;
    bool has_receiver_tag;
    int64_t receiver_tag;
    uint64_t timeout_ns;
}
aeron_flow_control_tagged_options_t;

aeron_flow_control_strategy_supplier_func_t aeron_flow_control_strategy_supplier_load(const char *strategy_name);

int aeron_max_multicast_flow_control_strategy_supplier(
    aeron_flow_control_strategy_t **strategy,
    aeron_driver_context_t *context,
    aeron_udp_channel_t *channel,
    int32_t stream_id,
    int64_t registration_id,
    int32_t initial_term_id,
    size_t term_length);

int aeron_unicast_flow_control_strategy_supplier(
    aeron_flow_control_strategy_t **strategy,
    aeron_driver_context_t *context,
    aeron_udp_channel_t *channel,
    int32_t stream_id,
    int64_t registration_id,
    int32_t initial_term_id,
    size_t term_length);

int aeron_min_flow_control_strategy_supplier(
    aeron_flow_control_strategy_t **strategy,
    aeron_driver_context_t *context,
    aeron_udp_channel_t *channel,
    int32_t stream_id,
    int64_t registration_id,
    int32_t initial_term_id,
    size_t term_buffer_capacity);

int aeron_tagged_flow_control_strategy_supplier(
    aeron_flow_control_strategy_t **strategy,
    aeron_driver_context_t *context,
    aeron_udp_channel_t *channel,
    int32_t stream_id,
    int64_t registration_id,
    int32_t initial_term_id,
    size_t term_buffer_capacity);

int aeron_default_multicast_flow_control_strategy_supplier(
    aeron_flow_control_strategy_t **strategy,
    aeron_driver_context_t *context,
    aeron_udp_channel_t *channel,
    int32_t stream_id,
    int64_t registration_id,
    int32_t initial_term_id,
    size_t term_length);

typedef struct aeron_flow_control_strategy_supplier_func_table_entry_stct
{
    const char *name;
    aeron_flow_control_strategy_supplier_func_t supplier_func;
}
aeron_flow_control_strategy_supplier_func_table_entry_t;

int aeron_flow_control_parse_tagged_options(
    const size_t options_length,
    const char *options,
    aeron_flow_control_tagged_options_t *flow_control_options);

#endif //AERON_FLOW_CONTROL_H
