#pragma once

#include "glitch_core.hpp"
#include "processor.h"
#include "unit_modfx.h"

class Modfx : public Processor {
 public:
  enum ParameterId { TIME = 0U, CHAOS, NUM_PARAMS };

  uint32_t getBufferSize() const override final {
    return glitchy::GlitchCore::kBufferFloats;
  }

  void init(float *allocated_buffer) override final {
    buffer_ = allocated_buffer;
    glitch_.init(buffer_, glitchy::GlitchCore::kBufferFloats, getSampleRate());
    glitch_.setTime(327.0f / 1023.0f);
    glitch_.setChaos(563.0f / 1023.0f);
  }

  void teardown() override final { buffer_ = nullptr; }

  void reset() override final { glitch_.reset(); }

  void process(const float *__restrict in, float *__restrict out,
               uint32_t frames) override final {
    glitch_.process(in, out, nullptr, nullptr, frames);
  }

  void setParameter(uint8_t index, int32_t value) override final {
    if (value < 0) {
      value = 0;
    } else if (value > 1023) {
      value = 1023;
    }
    const float normalized = static_cast<float>(value) / 1023.0f;
    if (index == TIME) {
      glitch_.setTime(normalized);
    } else if (index == CHAOS) {
      glitch_.setChaos(normalized);
    }
  }

 private:
  float *buffer_ = nullptr;
  glitchy::GlitchCore glitch_;
};
