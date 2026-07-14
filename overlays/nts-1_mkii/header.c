#include "unit_modfx.h"

// "ksd6" and "GLTC" identify this community build and unit.
const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_modfx,
    .api = UNIT_API_VERSION,
    .dev_id = 0x6B736436U,
    .unit_id = 0x474C5443U,
    .version = 0x00010000U,
    .name = "GLITCHY",
    .num_params = 2,
    .params = {
        {0, 1023, 0, 327, k_unit_param_type_none, 1, 0, 0, {"TIME"}},
        {0, 1023, 0, 563, k_unit_param_type_none, 1, 0, 0, {"CHAOS"}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
    },
};
