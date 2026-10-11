#pragma once

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace engine {

    enum class RandomAlgorithm {
        Splitmix64,
        Mulberry32,
    };

    /// @brief A random stream with selectable PRNG algorithm.
    template <RandomAlgorithm Algorithm>
    class RandomStream {
        using StateType =
            std::conditional_t<Algorithm == RandomAlgorithm::Splitmix64, uint64_t, uint32_t>;
        using Seed = StateType;

      public:
        RandomStream(Seed seed = 0) : m_seed(seed), m_state(seed) {}

        float nextFloat() {
            if constexpr (Algorithm == RandomAlgorithm::Splitmix64) {
                return (getNumber() >> 40) * (1.0f / 16777216.0f);
            } else if constexpr (Algorithm == RandomAlgorithm::Mulberry32) {
                return (getNumber() >> 8) * (1.0f / 16777216.0f);
            }
            return 0.0f;
        }
        double nextDouble() {
            if constexpr (Algorithm == RandomAlgorithm::Splitmix64) {
                return (getNumber() >> 11) * (1.0 / 9007199254740992.0);
            } else if constexpr (Algorithm == RandomAlgorithm::Mulberry32) {
                return getNumber() / 4294967296.0;
            }
        }
        float range(float min, float max) { return nextFloat() * (max - min) + min; }
        double range(double min, double max) { return nextDouble() * (max - min) + min; }
        /// @brief Uniform int in the half-open range [min, max). Requires min < max; returns min otherwise.
        int range(int min, int max) {
            assert(min < max && "max must be greater than min");
            if (max <= min) return min;

            const uint32_t span = static_cast<uint32_t>(static_cast<int64_t>(max) - min);
            uint64_t m = static_cast<uint64_t>(next32()) * span;
            if (static_cast<uint32_t>(m) < span) {
                const uint32_t threshold = (0u - span) % span;
                while (static_cast<uint32_t>(m) < threshold) {
                    m = static_cast<uint64_t>(next32()) * span;
                }
            }
            return static_cast<int>(static_cast<int64_t>(min) + static_cast<int64_t>(m >> 32));
        }

        void setSeed(Seed seed) {
            m_seed = seed;
            m_state = seed;
        }
        void reset() { m_state = m_seed; }

      private:
        Seed m_seed;
        StateType m_state;

        StateType getNumber() {
            if constexpr (Algorithm == RandomAlgorithm::Splitmix64) {
                m_state += 0x9e3779b97f4a7c15;
                StateType z = m_state;
                z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
                z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
                return z ^ (z >> 31);
            } else if constexpr (Algorithm == RandomAlgorithm::Mulberry32) {
                m_state += 0x6d2b79f5u;
                StateType t = m_state;
                t = (t ^ (t >> 15)) * (t | 1u);
                t = ((t + ((t ^ (t >> 7)) * (t | 61u)))) ^ t;
                return t ^ (t >> 14);
            }
        }

        uint32_t next32() {
            if constexpr (Algorithm == RandomAlgorithm::Splitmix64) {
                return static_cast<uint32_t>(getNumber() >> 32);
            } else {
                return getNumber();
            }
        }
    };
}  // namespace engine