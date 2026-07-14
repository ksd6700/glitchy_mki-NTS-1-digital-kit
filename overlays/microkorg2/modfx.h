#pragma once

#include <cstddef>
#include <cstdint>

#include "glitch_core.hpp"
#include "runtime.h"
#include "unit_modfx.h"

class ModFx {
 public:
  enum ParameterId { kTime = 0U, kChaos, kNumParams };

  ModFx() : allocated_buffer_(nullptr), tempo_(120.0f) {
    params_[kTime] = 327;
    params_[kChaos] = 563;
  }

  int8_t Init(const unit_runtime_desc_t *desc) {
    if (!desc) {
      return k_unit_err_undef;
    }
    if (desc->target != unit_header.target) {
      return k_unit_err_target;
    }
    if (!UNIT_API_IS_COMPAT(desc->api)) {
      return k_unit_err_api_version;
    }
    if (desc->samplerate != 48000) {
      return k_unit_err_samplerate;
    }
    if (desc->input_channels != 2 || desc->output_channels != 2) {
      return k_unit_err_geometry;
    }
    if (!desc->hooks.sdram_alloc) {
      return k_unit_err_memory;
    }

    allocated_buffer_ = static_cast<float *>(
        desc->hooks.sdram_alloc(glitchy::GlitchCore::kBufferFloats *
                                sizeof(float)));
    if (!allocated_buffer_) {
      return k_unit_err_memory;
    }

    runtime_desc_ = *desc;
    glitch_.init(allocated_buffer_, glitchy::GlitchCore::kBufferFloats,
                 48000.0f);
    glitch_.setTime(static_cast<float>(params_[kTime]) / 1023.0f);
    glitch_.setChaos(static_cast<float>(params_[kChaos]) / 1023.0f);
    return k_unit_err_none;
  }

  void Teardown() { allocated_buffer_ = nullptr; }
  void Reset() { glitch_.reset(); }
  void Resume() {}
  void Suspend() {}

  void Process(const float *in, float *out, std::size_t frames) {
    glitch_.process(in, out, nullptr, nullptr, static_cast<uint32_t>(frames));
  }

  void setParameter(uint8_t index, int32_t value) {
    if (index >= kNumParams) {
      return;
    }
    if (value < 0) {
      value = 0;
    } else if (value > 1023) {
      value = 1023;
    }
    params_[index] = value;
    const float normalized = static_cast<float>(value) / 1023.0f;
    if (index == kTime) {
      glitch_.setTime(normalized);
    } else if (index == kChaos) {
      glitch_.setChaos(normalized);
    }
  }

  int32_t getParameterValue(uint8_t index) const {
    return index < kNumParams ? params_[index] : 0;
  }

  const char *getParameterStrValue(uint8_t index, int32_t value) const {
    (void)index;
    (void)value;
    return nullptr;
  }

  void setTempo(uint32_t tempo) {
    tempo_ = (tempo >> 16) +
             (tempo & 0xFFFFU) / static_cast<float>(0x10000U);
  }

 private:
  unit_runtime_desc_t runtime_desc_;
  int32_t params_[kNumParams];
  float *allocated_buffer_;
  float tempo_;
  glitchy::GlitchCore glitch_;
};
