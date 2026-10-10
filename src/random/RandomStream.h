#pragma once

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
        int range(int min, int max) {
            return min + getNumber() % static_cast<StateType>(max - min);
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
    };
}  // namespace engine