#ifndef APP_PROTOCOL_LIB_H
#define APP_PROTOCOL_LIB_H

#if 0
#elif APP_MODULE_PROTOCOL_USE_JSON
#include "cJSON.h"
#include "app_json_xfer.h"
#include "app_json_xfer_mix.h"
#elif APP_MODULE_PROTOCOL_USE_NANOPB
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "app_nanopb_set.pb.h"
#include "app_nanopb_xfer.h"
#include "app_nanopb_xfer_mix.h"
#else
#error "app protocol is unknown"
#endif

#endif
