#pragma once

#include <cstddef>
#include <cstdint>

namespace glitchy {

// Portable, allocation-free DSP shared by every logue SDK wrapper.
class GlitchCore {
 public:
  static constexpr uint32_t kRingFrames = 4096U;
  static constexpr uint32_t kChannels = 2U;
  static constexpr uint32_t kMaxPaths = 2U;
  static constexpr uint32_t kBufferFloats =
      kRingFrames * kChannels * kMaxPaths;

  GlitchCore()
      : memory_(nullptr),
        memory_floats_(0U),
        sample_rate_(48000.0f),
        time_target_(0.32f),
        chaos_target_(0.55f),
        time_smoothed_(0.32f),
        chaos_smoothed_(0.55f),
        write_frame_(0U),
        captured_frame_(0U),
        cycle_frame_(0U),
        slice_frames_(1024U),
        written_frames_(0U),
        rng_(0x47544C43U),
        gate_mask_(0xFFU),
        mode_(0U),
        glitch_active_(false) {}

  void init(float *memory, uint32_t memory_floats,
            float sample_rate = 48000.0f) {
    memory_ = memory;
    memory_floats_ = memory_floats;
    sample_rate_ = sample_rate > 1000.0f ? sample_rate : 48000.0f;
    clearMemory();
    reset();
  }

  void reset() {
    write_frame_ = 0U;
    captured_frame_ = 0U;
    cycle_frame_ = 0U;
    slice_frames_ = 1024U;
    written_frames_ = 0U;
    rng_ = 0x47544C43U;
    gate_mask_ = 0xFFU;
    mode_ = 0U;
    glitch_active_ = false;
    time_smoothed_ = time_target_;
    chaos_smoothed_ = chaos_target_;
    clearMemory();
  }

  void setTime(float value) { time_target_ = clamp01(value); }
  void setChaos(float value) { chaos_target_ = clamp01(value); }

  void process(const float *main_in, float *main_out, const float *sub_in,
               float *sub_out, uint32_t frames) {
    if (!main_in || !main_out || !memory_ ||
        memory_floats_ < requiredFloats(1U)) {
      return;
    }

    const bool has_sub =
        sub_in && sub_out && memory_floats_ >= requiredFloats(2U);

    for (uint32_t frame = 0U; frame < frames; ++frame) {
      time_smoothed_ += (time_target_ - time_smoothed_) * 0.0015f;
      chaos_smoothed_ += (chaos_target_ - chaos_smoothed_) * 0.0015f;

      writeInput(0U, write_frame_, main_in + frame * kChannels);
      if (has_sub) {
        writeInput(1U, write_frame_, sub_in + frame * kChannels);
      }

      if (cycle_frame_ == 0U) {
        beginSlice();
      }

      renderPath(0U, main_in + frame * kChannels,
                 main_out + frame * kChannels);
      if (has_sub) {
        renderPath(1U, sub_in + frame * kChannels,
                   sub_out + frame * kChannels);
      } else if (sub_in && sub_out) {
        sub_out[frame * kChannels] = sub_in[frame * kChannels];
        sub_out[frame * kChannels + 1U] =
            sub_in[frame * kChannels + 1U];
      }

      write_frame_ = (write_frame_ + 1U) & (kRingFrames - 1U);
      if (written_frames_ < kRingFrames) {
        ++written_frames_;
      }
      ++cycle_frame_;
      if (cycle_frame_ >= slice_frames_) {
        cycle_frame_ = 0U;
      }
    }
  }

 private:
  static constexpr uint32_t requiredFloats(uint32_t paths) {
    return kRingFrames * kChannels * paths;
  }

  static float clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
  }

  static float clampAudio(float value) {
    return value < -1.2f ? -1.2f : (value > 1.2f ? 1.2f : value);
  }

  void clearMemory() {
    if (!memory_) {
      return;
    }
    const uint32_t count =
        memory_floats_ < kBufferFloats ? memory_floats_ : kBufferFloats;
    for (uint32_t i = 0U; i < count; ++i) {
      memory_[i] = 0.0f;
    }
  }

  uint32_t randomU32() {
    uint32_t value = rng_;
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    rng_ = value;
    return value;
  }

  float randomUnit() {
    return static_cast<float>(randomU32() & 0x00FFFFFFU) /
           16777216.0f;
  }

  void beginSlice() {
    // TIME covers roughly 12-85 ms at 48 kHz, scaled for other sample rates.
    const float shaped_time = time_smoothed_ * time_smoothed_;
    const float min_frames = sample_rate_ * 0.012f;
    const float max_frames = sample_rate_ * 0.085f;
    uint32_t next_slice = static_cast<uint32_t>(
        min_frames + shaped_time * (max_frames - min_frames));
    if (next_slice < 64U) {
      next_slice = 64U;
    }
    if (next_slice >= kRingFrames) {
      next_slice = kRingFrames - 1U;
    }
    slice_frames_ = next_slice;
    captured_frame_ =
        (write_frame_ + kRingFrames - slice_frames_) & (kRingFrames - 1U);

    const float probability = 0.10f + chaos_smoothed_ * 0.88f;
    glitch_active_ = written_frames_ >= slice_frames_ &&
                     randomUnit() < probability && chaos_smoothed_ > 0.002f;
    mode_ = static_cast<uint8_t>(randomU32() & 0x03U);

    gate_mask_ = 0xFFU;
    if (chaos_smoothed_ > 0.55f) {
      const float keep_probability =
          1.0f - (chaos_smoothed_ - 0.55f) * 1.65f;
      gate_mask_ = 0U;
      for (uint32_t gate = 0U; gate < 8U; ++gate) {
        if (randomUnit() < keep_probability) {
          gate_mask_ |= static_cast<uint8_t>(1U << gate);
        }
      }
      // Never silence an entire slice by accident.
      if (gate_mask_ == 0U) {
        gate_mask_ = static_cast<uint8_t>(1U << (randomU32() & 0x07U));
      }
    }
  }

  uint32_t readOffset() const {
    uint32_t offset = cycle_frame_;
    switch (mode_) {
      case 1U:  // reverse
        offset = slice_frames_ - 1U - cycle_frame_;
        break;
      case 2U: {  // tight micro-loop
        uint32_t micro = slice_frames_ >> 2U;
        if (micro < 16U) {
          micro = 16U;
        }
        offset = cycle_frame_ % micro;
        break;
      }
      case 3U:  // half-speed lurch
        offset = cycle_frame_ >> 1U;
        break;
      default:  // straight stutter
        break;
    }

    const float chaos_cubed = chaos_smoothed_ * chaos_smoothed_ *
                              chaos_smoothed_;
    const uint32_t hold = 1U + static_cast<uint32_t>(chaos_cubed * 12.0f);
    return (offset / hold) * hold;
  }

  float edgeFade() const {
    uint32_t fade_frames = slice_frames_ >> 3U;
    if (fade_frames > 64U) {
      fade_frames = 64U;
    }
    if (fade_frames < 8U) {
      fade_frames = 8U;
    }

    uint32_t edge = cycle_frame_;
    const uint32_t remaining = slice_frames_ - 1U - cycle_frame_;
    if (remaining < edge) {
      edge = remaining;
    }
    if (edge >= fade_frames) {
      return 1.0f;
    }
    return static_cast<float>(edge) / static_cast<float>(fade_frames);
  }

  float crush(float sample) const {
    float clipped = sample < -1.0f ? -1.0f : (sample > 1.0f ? 1.0f : sample);
    uint32_t bits = 16U - static_cast<uint32_t>(chaos_smoothed_ * 12.0f);
    if (bits < 4U) {
      bits = 4U;
    }
    const int32_t scale = static_cast<int32_t>(1U << (bits - 1U));
    const float scaled = clipped * static_cast<float>(scale);
    const int32_t rounded =
        static_cast<int32_t>(scaled + (scaled >= 0.0f ? 0.5f : -0.5f));
    return static_cast<float>(rounded) / static_cast<float>(scale);
  }

  void writeInput(uint32_t path, uint32_t frame, const float *input) {
    const uint32_t base =
        path * kRingFrames * kChannels + frame * kChannels;
    memory_[base] = input[0];
    memory_[base + 1U] = input[1];
  }

  void renderPath(uint32_t path, const float *dry, float *output) const {
    if (!glitch_active_ || chaos_target_ <= 0.002f) {
      output[0] = dry[0];
      output[1] = dry[1];
      return;
    }

    const uint32_t offset = readOffset();
    const uint32_t read_frame =
        (captured_frame_ + offset) & (kRingFrames - 1U);
    const uint32_t base =
        path * kRingFrames * kChannels + read_frame * kChannels;

    uint32_t gate = (cycle_frame_ * 8U) / slice_frames_;
    if (gate > 7U) {
      gate = 7U;
    }
    const float gate_gain = (gate_mask_ & (1U << gate)) ? 1.0f : 0.0f;
    const float mix = 0.95f * chaos_smoothed_ * edgeFade();
    const float wet_l = crush(memory_[base]) * gate_gain;
    const float wet_r = crush(memory_[base + 1U]) * gate_gain;
    output[0] = clampAudio(dry[0] + (wet_l - dry[0]) * mix);
    output[1] = clampAudio(dry[1] + (wet_r - dry[1]) * mix);
  }

  float *memory_;
  uint32_t memory_floats_;
  float sample_rate_;
  float time_target_;
  float chaos_target_;
  float time_smoothed_;
  float chaos_smoothed_;
  uint32_t write_frame_;
  uint32_t captured_frame_;
  uint32_t cycle_frame_;
  uint32_t slice_frames_;
  uint32_t written_frames_;
  uint32_t rng_;
  uint8_t gate_mask_;
  uint8_t mode_;
  bool glitch_active_;
};

}  // namespace glitchy
