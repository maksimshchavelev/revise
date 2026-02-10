// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/AlgorithmFactory.hpp" // for header
#include "SM2Algorithm.hpp"            // for SM2

namespace engine {

std::unique_ptr<core::IAlgorithm> create_study_algorithm(const AlgorithmType type) {
    return std::unique_ptr<SM2Algorithm>();
}

} // namespace engine
