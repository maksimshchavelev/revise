// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/AlgorithmFactory.hpp" // for header
#include "SM2Algorithm.hpp"            // for SM2

namespace engine {

std::unique_ptr<core::IAlgorithm> create_study_algorithm(AlgorithmType type, const core::AlgorithmSettings& settings) {
    return std::make_unique<SM2Algorithm>(settings);
}

} // namespace engine
