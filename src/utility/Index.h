#pragma once


namespace engine {

    /// @brief Calculates the index of 2D array to 1D array
    /// @note The nested loops should go in the reverse order of the parameters, i.e. (outer) y then (inner) x
    inline constexpr size_t index2D(size_t x, size_t y, size_t width) {
        return y * width + x;
    }

    /// @brief Calculates the index of 3D array to 1D array
    /// @note The nested loops should go in the reverse order of the parameters, i.e. (outer) z then (inner) y then (inner) x
    inline constexpr size_t index3D(size_t x, size_t y, size_t z, size_t width, size_t height) {
        return z * width * height + y * width + x;
    }

    /// @brief Calculates the index of 4D array to 1D array
    /// @note The nested loops should go in the reverse order of the parameters, i.e. (outer) w then (inner) z then (inner) y then (inner) x
    inline constexpr size_t index4D(
        size_t x, size_t y, size_t z, size_t w, size_t width, size_t height, size_t depth
    ) {
        return w * width * height * depth + z * width * height + y * width + x;
    }
}  // namespace engine