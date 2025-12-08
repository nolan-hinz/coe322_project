#pragma once // Guard multiple instances

#include <random>

std::default_random_engine& engine() {
  static std::default_random_engine g{ std::random_device{}() };
  return g;
}
