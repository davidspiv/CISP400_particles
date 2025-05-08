#pragma once
#include "../lib/Color_Space.h"
#include "config.h"
#include <SFML/Graphics.hpp>
#include <random>

inline int getRandInt(int const min, int const max)
{
    return (std::rand() % (max - min + 1)) + min;
}

inline int getRandOddInt(int const min, int const max)
{
    int const randInt = getRandInt(min, max);
    return randInt % 2 ? randInt : randInt - 1;
}

// Mersenne Twister engine
inline double getRandDouble(double min, double max)
{
    static thread_local std::mt19937 generator(std::random_device {}());
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator);
}

inline sf::Uint8 to_u8(float x)
{
    return static_cast<sf::Uint8>(std::round(std::clamp(x, 0.f, 255.f)));
}

inline sf::Color getRandColor()
{
    return sf::Color(getRandInt(0, 255), getRandInt(0, 255), getRandInt(0, 255));
}

/**
 * @brief Generates a smooth rainbow of colors starting from a specified color.
 *
 * @param sample_count The number of colors to generate. Must be at least 2.
 * @param start_color The color where the rainbow starts. Defaults to red.
 * @param rainbow_percent The portion of the color wheel to cover, as a
 * percentage (100 = full rainbow, 50 = half rainbow, etc.). Defaults to 100.
 *
 * @return std::vector<sf::Color> A list of colors smoothly transitioning across
 * the specified portion of the rainbow.
 */
inline std::vector<sf::Color> get_rainbow_colors(
    int sample_count, sf::Color start_color = sf::Color::Red, float rainbow_percent = 100.f)
{
    if (sample_count < 2) {
        throw std::domain_error("sample count must be >= 2 for correct interpolation.");
    }

    float const LIGHTNESS = .7f;
    float const CHROMA = .18f;
    float const sample_degrees = (360.0f * rainbow_percent) / 100.0f;

    float start_hue = clrspc::Rgb(start_color.r, start_color.g, start_color.b)
                          .to_ok_lab()
                          .to_ok_lch_ab()
                          .get_values()[2];

    start_hue -= 20.f; // offset to match perceived color

    std::vector<sf::Color> colors;
    colors.reserve(sample_count);

    for (int i = 0; i < sample_count; ++i) {

        float const hue
            = clrspc::normalize_degrees(start_hue + (sample_degrees * i) / (sample_count - 1));

        clrspc::Ok_Lch_Ab const ok_lch_ab(LIGHTNESS, CHROMA, hue);

        auto const [r, g, b] = ok_lch_ab.to_ok_lab().to_rgb().get_values();

        colors.push_back({ to_u8(r), to_u8(g), to_u8(b) });
    }

    return colors;
}
