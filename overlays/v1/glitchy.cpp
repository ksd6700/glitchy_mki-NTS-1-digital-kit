#include "glitch_core.hpp"
#include "usermodfx.h"

namespace {

float g_audio_memory[glitchy::GlitchCore::kBufferFloats] __sdram;
glitchy::GlitchCore g_glitch;

float parameterToFloat(int32_t value) {
  if (value < 0) {
    value = 0;
  } else if (value > 1023) {
    value = 1023;
  }
  return static_cast<float>(value) / 1023.0f;
}

}  // namespace

void MODFX_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  g_glitch.init(g_audio_memory, glitchy::GlitchCore::kBufferFloats, 48000.0f);
  g_glitch.setTime(0.32f);
  g_glitch.setChaos(0.55f);
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                   const float *sub_xn, float *sub_yn, uint32_t frames) {
  g_glitch.process(main_xn, main_yn, sub_xn, sub_yn, frames);
}

void MODFX_SUSPEND(void) {}

void MODFX_RESUME(void) { g_glitch.reset(); }

void MODFX_PARAM(uint8_t index, int32_t value) {
  const float normalized = parameterToFloat(value);
  if (index == k_user_modfx_param_time) {
    g_glitch.setTime(normalized);
  } else if (index == k_user_modfx_param_depth) {
    g_glitch.setChaos(normalized);
  }
}
