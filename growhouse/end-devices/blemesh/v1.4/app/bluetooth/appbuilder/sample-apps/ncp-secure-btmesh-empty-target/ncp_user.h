#ifndef NCP_USER_H
#define NCP_USER_H
#include "ncp_gecko.h"

#define USER_EVENT_ID       255
#define USER_EVENT_DATA_MAX_LEN   73
extern uint8_t user_event_data_len;
extern uint8_t user_event_data[USER_EVENT_DATA_MAX_LEN];

enum user_cmd_codes{
  user_cmd_increase_security
};

//TODO: fix this
PACKSTRUCT(struct user_msg_ncp_increase_security_t{
  uint8_t public_keys[64];
  uint8_t host_iv_to_target[4];
  uint8_t host_iv_to_host[4];
});

PACKSTRUCT(struct user_cmd_packet{
  uint8_t code;
  union {
    struct user_msg_ncp_increase_security_t ncp_increase_security;
  } data;
});

#endif /* NCP_USER_H */
