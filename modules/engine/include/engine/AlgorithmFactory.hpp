// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IAlgorithm.hpp> // for core::IAlgorithm
#include <memory>              // for std::unique_ptr

namespace engine {

enum class AlgorithmType {
    SM2 ///< SM2 algorithm
};

/**
 * @brief The function creates a specific implementation of IAlgorithm (study algorithm) and returns a pointer to it.
 * @type type Type of algorithm
 * @return `std::unique_ptr<core::IAlgorithm>`
 */
std::unique_ptr<core::IAlgorithm> create_study_algorithm(const AlgorithmType type);

} // namespace engine
