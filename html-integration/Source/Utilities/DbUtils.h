#pragma once
#include <cmath>

// Utilitaires sans dependance JUCE (testables avec un compilateur C++20 seul).
namespace bm::util
{
inline constexpr float kSilenceDb = -144.0f;

/** dB -> gain lineaire. Les valeurs non finies donnent 1.0 (neutre). */
inline float dbToGain(float db) noexcept
{
    return std::isfinite(db) ? std::pow(10.0f, db * 0.05f) : 1.0f;
}

/** gain lineaire -> dB, borne inferieure kSilenceDb. Gains <= 0 ou non finis -> kSilenceDb. */
inline float gainToDb(float g) noexcept
{
    return (std::isfinite(g) && g > 0.0f) ? std::fmax(20.0f * std::log10(g), kSilenceDb) : kSilenceDb;
}

/** Remplace NaN/Inf par 0 : protection de sortie. */
inline float sanitize(float x) noexcept { return std::isfinite(x) ? x : 0.0f; }
}
