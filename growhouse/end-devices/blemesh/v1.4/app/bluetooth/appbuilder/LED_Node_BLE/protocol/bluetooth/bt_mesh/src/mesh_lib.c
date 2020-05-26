/***************************************************************************//**
 * @brief Mesh helper library
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>

/* BG stack headers */
#include "bg_types.h"

#if (!defined(MESH_LIB_NATIVE) && !defined(MESH_LIB_HOST)) \
  || (defined(MESH_LIB_NATIVE) && defined(MESH_LIB_HOST))
#error "You must define either MESH_LIB_NATIVE or MESH_LIB_HOST, and not both!"
#endif

/* Select BGAPI flavor */
#if defined(MESH_LIB_HOST)
#include "gecko_bglib.h"
#include "host_gecko.h"
#else /* MESH_LIB_NATIVE */
#include "native_gecko.h"
#endif /* MESH_LIB_NATIVE */

#include "mesh_generic_model_capi_types.h"
#include "mesh_lib.h"
#include "mesh_serdeser.h"

uint32_t mesh_lib_transition_time_to_ms(uint8_t t)
{
  uint32_t res_ms[4] = { 100, 1000, 10000, 600000 };
  uint32_t unit = (t >> 6) & 0x03;
  uint32_t count = t & 0x3f;
  return res_ms[unit] * count;
}

struct reg {
  uint16_t model_id;
  uint16_t elem_index;
  union {
    struct {
      mesh_lib_generic_server_client_request_cb client_request_cb;
      mesh_lib_generic_server_change_cb state_changed_cb;
    } server;
    struct {
      mesh_lib_generic_client_server_response_cb server_response_cb;
    } client;
  };
};

static struct reg *reg = NULL;
static size_t regs = 0;

static void *(*lib_malloc_fn)(size_t) = NULL;
static void (*lib_free_fn)(void *) = NULL;

static struct reg *find_reg(uint16_t model_id,
                            uint16_t elem_index)
{
  size_t r;
  for (r = 0; r < regs; r++) {
    if (reg[r].model_id == model_id && reg[r].elem_index == elem_index) {
      return &reg[r];
    }
  }
  return NULL;
}

static struct reg *find_free(void)
{
  size_t r;
  for (r = 0; r < regs; r++) {
    if (reg[r].model_id == 0x0000 && reg[r].elem_index == 0x0000) {
      return &reg[r];
    }
  }
  return NULL;
}

errorcode_t mesh_lib_init(void *(*malloc_fn)(size_t),
                          void (*free_fn)(void *),
                          size_t generic_models)
{
  lib_malloc_fn = malloc_fn;
  lib_free_fn = free_fn;

  if (generic_models) {
    reg = (lib_malloc_fn)(generic_models * sizeof(struct reg));
    if (!reg) {
      return bg_err_out_of_memory;
    }
    memset(reg, 0, generic_models * sizeof(struct reg));
    regs = generic_models;
  }

  return bg_err_success;
}

void mesh_lib_deinit(void)
{
  if (reg) {
    (lib_free_fn)(reg);
    reg = NULL;
    regs = 0;
  }
}

errorcode_t
mesh_lib_generic_server_register_handler(uint16_t model_id,
                                         uint16_t elem_index,
                                         mesh_lib_generic_server_client_request_cb cb,
                                         mesh_lib_generic_server_change_cb ch)
{
  struct reg *reg = NULL;

  reg = find_reg(model_id, elem_index);
  if (reg) {
    return bg_err_wrong_state; // already exists
  }

  reg = find_free();
  if (!reg) {
    return bg_err_out_of_memory;
  }

  reg->model_id = model_id;
  reg->elem_index = elem_index;
  reg->server.client_request_cb = cb;
  reg->server.state_changed_cb = ch;
  return bg_err_success;
}

errorcode_t
mesh_lib_generic_client_register_handler(uint16_t model_id,
                                         uint16_t elem_index,
                                         mesh_lib_generic_client_server_response_cb cb)
{
  struct reg *reg = NULL;

  reg = find_reg(model_id, elem_index);
  if (reg) {
    return bg_err_wrong_state; // already exists
  }

  reg = find_free();
  if (!reg) {
    return bg_err_out_of_memory;
  }

  reg->model_id = model_id;
  reg->elem_index = elem_index;
  reg->client.server_response_cb = cb;
  return bg_err_success;
}

void mesh_lib_generic_server_event_handler(struct gecko_cmd_packet *evt)
{
  struct gecko_msg_mesh_generic_server_client_request_evt_t *req = NULL;
  struct gecko_msg_mesh_generic_server_state_changed_evt_t *chg = NULL;
  struct mesh_generic_request request;
  struct mesh_generic_state current;
  struct mesh_generic_state target;
  int has_target;
  struct reg *reg;

  if (!evt) {
    return;
  }

  switch (BGLIB_MSG_ID(evt->header)) {
    case gecko_evt_mesh_generic_server_client_request_id:
      req = &(evt->data.evt_mesh_generic_server_client_request);
      reg = find_reg(req->model_id, req->elem_index);
      if (reg) {
        if (mesh_lib_deserialize_request(&request,
                                         req->type,
                                         req->parameters.data,
                                         req->parameters.len) == 0) {
          (reg->server.client_request_cb)(req->model_id,
                                          req->elem_index,
                                          req->client_address,
                                          req->server_address,
                                          req->appkey_index,
                                          &request,
                                          req->transition,
                                          req->delay,
                                          req->flags);
        }
      }
      break;
    case gecko_evt_mesh_generic_server_state_changed_id:
      chg = &(evt->data.evt_mesh_generic_server_state_changed);
      reg = find_reg(chg->model_id, chg->elem_index);
      if (reg) {
        if (mesh_lib_deserialize_state(&current,
                                       &target,
                                       &has_target,
                                       chg->type,
                                       chg->parameters.data,
                                       chg->parameters.len) == 0) {
          (reg->server.state_changed_cb)(chg->model_id,
                                         chg->elem_index,
                                         &current,
                                         has_target ? &target : NULL,
                                         chg->remaining);
        }
      }
      break;
  }
}

void mesh_lib_generic_client_event_handler(struct gecko_cmd_packet *evt)
{
  struct gecko_msg_mesh_generic_client_server_status_evt_t *res = NULL;
  struct mesh_generic_state current;
  struct mesh_generic_state target;
  int has_target;
  struct reg *reg;

  if (!evt) {
    return;
  }

  switch (BGLIB_MSG_ID(evt->header)) {
    case gecko_evt_mesh_generic_client_server_status_id:
      res = &(evt->data.evt_mesh_generic_client_server_status);
      reg = find_reg(res->model_id, res->elem_index);
      if (reg) {
        if (mesh_lib_deserialize_state(&current,
                                       &target,
                                       &has_target,
                                       res->type,
                                       res->parameters.data,
                                       res->parameters.len) == 0) {
          (reg->client.server_response_cb)(res->model_id,
                                           res->elem_index,
                                           res->client_address,
                                           res->server_address,
                                           &current,
                                           has_target ? &target : NULL,
                                           res->remaining,
                                           res->flags);
        }
      }
      break;
  }
}

errorcode_t
mesh_lib_generic_server_response(uint16_t model_id,
                                 uint16_t element_index,
                                 uint16_t client_addr,
                                 uint16_t appkey_index,
                                 const struct mesh_generic_state *current,
                                 const struct mesh_generic_state *target,
                                 uint32_t remaining_ms,
                                 uint8_t response_flags)
{
  uint8_t buf[12];
  size_t len;

  if (mesh_lib_serialize_state(current, target, buf, sizeof(buf), &len) != 0) {
    return bg_err_invalid_param;
  }
  return gecko_cmd_mesh_generic_server_response(model_id,
                                                element_index,
                                                client_addr,
                                                appkey_index,
                                                remaining_ms,
                                                response_flags,
                                                current->kind,
                                                len,
                                                buf)->result;
}

errorcode_t
mesh_lib_generic_server_update(uint16_t model_id,
                               uint16_t element_index,
                               const struct mesh_generic_state *current,
                               const struct mesh_generic_state *target,
                               uint32_t remaining_ms)
{
  uint8_t buf[12];
  size_t len;

  if (mesh_lib_serialize_state(current, target, buf, sizeof(buf), &len) != 0) {
    return bg_err_invalid_param;
  }
  return gecko_cmd_mesh_generic_server_update(model_id,
                                              element_index,
                                              remaining_ms,
                                              current->kind,
                                              len,
                                              buf)->result;
}

errorcode_t
mesh_lib_generic_server_publish(uint16_t model_id,
                                uint16_t element_index,
                                mesh_generic_state_t kind)
{
  return gecko_cmd_mesh_generic_server_publish(model_id,
                                               element_index,
                                               kind)->result;
}

errorcode_t mesh_lib_generic_client_get(uint16_t model_id,
                                        uint16_t element_index,
                                        uint16_t server_addr,
                                        uint16_t appkey_index,
                                        mesh_generic_state_t kind)
{
  return gecko_cmd_mesh_generic_client_get(model_id,
                                           element_index,
                                           server_addr,
                                           appkey_index,
                                           kind)->result;
}

errorcode_t mesh_lib_generic_client_set(uint16_t model_id,
                                        uint16_t element_index,
                                        uint16_t server_addr,
                                        uint16_t appkey_index,
                                        uint8_t transaction_id,
                                        const struct mesh_generic_request *request,
                                        uint32_t transition_ms,
                                        uint16_t delay_ms,
                                        uint8_t flags)
{
  uint8_t buf[10];
  size_t len;

  if (mesh_lib_serialize_request(request, buf, sizeof(buf), &len) != 0) {
    return bg_err_invalid_param;
  }
  return gecko_cmd_mesh_generic_client_set(model_id,
                                           element_index,
                                           server_addr,
                                           appkey_index,
                                           transaction_id,
                                           transition_ms,
                                           delay_ms,
                                           flags,
                                           request->kind,
                                           len,
                                           buf)->result;
}

errorcode_t
mesh_lib_generic_client_publish(uint16_t model_id,
                                uint16_t element_index,
                                uint8_t transaction_id,
                                const struct mesh_generic_request *request,
                                uint32_t transition_ms,
                                uint16_t delay_ms,
                                uint8_t request_flags)
{
  uint8_t buf[10];
  size_t len;

  if (mesh_lib_serialize_request(request, buf, sizeof(buf), &len) != 0) {
    return bg_err_invalid_param;
  }
  return gecko_cmd_mesh_generic_client_publish(model_id,
                                               element_index,
                                               transaction_id,
                                               transition_ms,
                                               delay_ms,
                                               request_flags,
                                               request->kind,
                                               len,
                                               buf)->result;
}
