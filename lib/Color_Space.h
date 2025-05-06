#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES // for VS

namespace clrspc {

// forward declarations
class Lch_Ab;
class Lab;
class Rgb;

template<typename channel_t> class Color {
protected:
    std::array<channel_t, 3> m_values;

public:
    Color(channel_t x, channel_t y, channel_t z) { m_values = { x, y, z }; }

    [[nodiscard]] std::array<channel_t, 3> get_values() const { return m_values; }

    virtual void print() const
    {
        std::cout << "[GEN]" << "\nC1: " << m_values[0] << "\nC2: " << m_values[1]
                  << "\nC3: " << m_values[2] << "\n\n";
    }

    [[nodiscard]] bool operator==(Color const& other) const
    {
        auto [x, y, z] = m_values;
        float error = 1;
        auto [other_x, other_y, other_z] = other.get_values();
        return (std::abs(x - other_x) < error) && (std::abs(y - other_y) < error)
            && (std::abs(z - other_z) < error);
    }

    [[nodiscard]] bool operator!=(Color const& other) const { return !(*this == other); }
};

class Lab : public Color<float> {
public:
    Lab(float l, float a, float b);

    float l() const { return m_values[0]; }
    float a() const { return m_values[1]; }
    float b() const { return m_values[2]; }

    [[nodiscard]] Lch_Ab to_lch_ab() const;
    [[nodiscard]] Rgb to_rgb() const;

    void print() const override;
};

class Lch_Ab : public Color<float> {
public:
    Lch_Ab(float l, float c, float h);

    [[nodiscard]] Lab to_lab() const;

    void print() const override;
};

class Rgb : public Color<uint8_t> {
public:
    Rgb(float r, float g, float b);

    float r() const { return m_values[0]; }
    float g() const { return m_values[1]; }
    float b() const { return m_values[2]; }

    [[nodiscard]] Lab to_lab() const;

    void print() const override;
};

// =========== Utils ==========

inline float to_radians(float const degrees) { return degrees * (M_PI / 180.f); }

inline float to_degrees(float const radians) { return radians * (180.0 / M_PI); }

inline float remove_gamma(float c)
{
    if (c <= 0) {
        return c;
    }

    return (c <= 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

inline float apply_gamma(float const c)
{
    if (c <= 0) {
        return c;
    }

    return (c <= 0.0031308f) ? (c * 12.92f) : 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}

inline double normalize_degrees(double x) { return x - std::floor(x / 360.0f) * 360.0f; }

inline std::array<float, 3> to_polar_color_space(std::array<float, 3> const& cartesian_color_space)
{
    auto const [l, a, b] = cartesian_color_space; // LchAb equivalents: a=u and b=v
    float const c = std::sqrt(a * a + b * b);
    float const h_component = to_degrees(std::atan2(b, a));
    float const h = (h_component >= 0) ? h_component : h_component + 360.0;

    return { l, c, h };
}

inline std::array<float, 3> from_polar_color_space(std::array<float, 3> const& polar_color_space)
{
    auto [l, c, h_deg] = polar_color_space;
    float const h_rad = to_radians(h_deg);

    float const a = c * std::cos(h_rad);
    float const b = c * std::sin(h_rad);

    return { l, a, b };
}

// =========== okLAB Space ==========

inline Lab::Lab(float l, float a, float b)
    : Color(l, a, b)
{
}

inline Lch_Ab Lab::to_lch_ab() const
{
    auto const [l, c, h] = to_polar_color_space(m_values);

    return Lch_Ab(l, c, h);
}

inline Rgb Lab::to_rgb() const
{
    auto [L, a, b] = m_values;

    float l_ = L + 0.3963377774f * a + 0.2158037573f * b;
    float m_ = L - 0.1055613458f * a - 0.0638541728f * b;
    float s_ = L - 0.0894841775f * a - 1.2914855480f * b;

    float l = l_ * l_ * l_;
    float m = m_ * m_ * m_;
    float s = s_ * s_ * s_;

    return {
        +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
        -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
        -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
    };
}

inline void Lab::print() const
{
    std::cout << "[Lab]" << "\nL: " << m_values[0] << "\na: " << m_values[1]
              << "\nb: " << m_values[2] << "\n\n";
}

// =========== okLCH Space ==========

inline Lch_Ab::Lch_Ab(float l, float c, float h)
    : Color(l, c, h)
{
}

inline Lab Lch_Ab::to_lab() const
{
    auto const [l, a, b] = from_polar_color_space(m_values);

    return Lab(l, a, b);
}

inline void Lch_Ab::print() const
{
    std::cout << "[LCHab]" << "\nL: " << m_values[0] << "\nc: " << m_values[1]
              << "\nh: " << m_values[2] << "\n\n";
}

// ========== sRGB Space ==========

inline Rgb::Rgb(float r, float g, float b)
    : Color { static_cast<uint8_t>(std::clamp(std::lround(r), 0l, 255l)),
        static_cast<uint8_t>(std::clamp(std::lround(g), 0l, 255l)),
        static_cast<uint8_t>(std::clamp(std::lround(b), 0l, 255l)) }
{

    auto warn_if_clamped = [](float val, char const* name) {
        if (val < -0.001f || val > 255.001f) {
            std::cout << "Warning: " << name << " channel clamped: " << val << '\n';
        }
    };

    warn_if_clamped(r, "R");
    warn_if_clamped(g, "G");
    warn_if_clamped(b, "B");
}

inline Lab Rgb::to_lab() const
{
    auto [r, g, b] = m_values;

    float l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
    float m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
    float s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    float l_ = cbrtf(l);
    float m_ = cbrtf(m);
    float s_ = cbrtf(s);

    return Lab(0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
        1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
        0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_);
}

inline void Rgb::print() const
{
    std::cout << "[Rgb]" << "\nr: " << m_values[0] << "\ng: " << m_values[1]
              << "\nb: " << m_values[2] << std::endl;
}

} // namespace clrspc
