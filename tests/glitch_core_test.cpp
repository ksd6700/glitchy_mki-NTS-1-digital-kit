#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "glitch_core.hpp"

namespace {

constexpr uint32_t kFrames = 48000U;

std::vector<float> makeInput() {
  std::vector<float> input(kFrames * 2U);
  for (uint32_t frame = 0U; frame < kFrames; ++frame) {
    const float saw = static_cast<float>(frame % 211U) / 105.0f - 1.0f;
    input[frame * 2U] = saw * 0.7f;
    input[frame * 2U + 1U] = -saw * 0.6f;
  }
  return input;
}

bool allFiniteAndBounded(const std::vector<float> &signal) {
  for (float sample : signal) {
    if (!std::isfinite(sample) || std::fabs(sample) > 1.2001f) {
      return false;
    }
  }
  return true;
}

}  // namespace

int main() {
  const std::vector<float> input = makeInput();
  std::vector<float> output(input.size(), 0.0f);
  std::vector<float> repeat(input.size(), 0.0f);
  std::vector<float> memory(glitchy::GlitchCore::kBufferFloats, 0.0f);

  glitchy::GlitchCore core;
  core.init(memory.data(), static_cast<uint32_t>(memory.size()));
  core.setTime(0.35f);
  core.setChaos(0.0f);
  core.process(input.data(), output.data(), nullptr, nullptr, kFrames);
  if (output != input) {
    std::cerr << "CHAOS=0 must be bit-exact dry pass-through\n";
    return 1;
  }

  core.setChaos(1.0f);
  core.reset();
  core.process(input.data(), output.data(), nullptr, nullptr, kFrames);
  if (!allFiniteAndBounded(output)) {
    std::cerr << "Glitch output exceeded its finite safety bound\n";
    return 1;
  }

  uint32_t changed = 0U;
  for (std::size_t i = 0U; i < input.size(); ++i) {
    if (std::fabs(input[i] - output[i]) > 1.0e-5f) {
      ++changed;
    }
  }
  if (changed < input.size() / 10U) {
    std::cerr << "High CHAOS did not produce enough audible change\n";
    return 1;
  }

  core.reset();
  core.process(input.data(), repeat.data(), nullptr, nullptr, kFrames);
  if (repeat != output) {
    std::cerr << "Reset render should remain deterministic\n";
    return 1;
  }

  std::cout << "glitch_core_test: PASS (" << changed << " changed samples)\n";
  return 0;
}
