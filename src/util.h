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

    float const LIGHTNESS = .4f;
    float const CHROMA = 2.f;
    float const sample_degrees = (360.0f * rainbow_percent) / 100.0f;

    auto [l, c, start_hue] = clrspc::Rgb(start_color.r, start_color.g, start_color.b)
                                 .to_lab()
                                 .to_lch_ab()
                                 .get_values();

    start_hue -= 20.f; // offset to match perceived color

    std::vector<sf::Color> colors;
    colors.reserve(sample_count);

    for (int i = 0; i < sample_count; ++i) {

        float const hue
            = clrspc::normalize_degrees(start_hue + (sample_degrees * i) / (sample_count - 1));

        clrspc::Lch_Ab const lch_ab(LIGHTNESS, CHROMA, hue);

        auto const [r, g, b] = lch_ab.to_lab().to_rgb().get_values();

        colors.push_back({ r, g, b });
    }

    return colors;
}

std::vector<sf::Color> const RAINBOW_GRADIENT = {
    { 255, 124, 136 },
    { 255, 125, 131 },
    { 255, 126, 127 },
    { 255, 127, 122 },
    { 255, 129, 117 },
    { 255, 130, 112 },
    { 255, 132, 108 },
    { 255, 134, 103 },
    { 255, 135, 99 },
    { 255, 137, 95 },
    { 255, 139, 91 },
    { 252, 141, 87 },
    { 249, 143, 83 },
    { 246, 145, 80 },
    { 243, 147, 76 },
    { 240, 149, 73 },
    { 236, 151, 70 },
    { 233, 153, 67 },
    { 229, 155, 64 },
    { 225, 157, 62 },
    { 221, 159, 60 },
    { 217, 161, 58 },
    { 212, 163, 56 },
    { 208, 165, 55 },
    { 203, 167, 54 },
    { 199, 168, 54 },
    { 194, 170, 54 },
    { 189, 172, 54 },
    { 184, 173, 55 },
    { 179, 175, 56 },
    { 174, 176, 57 },
    { 169, 178, 59 },
    { 164, 179, 61 },
    { 158, 180, 63 },
    { 153, 182, 66 },
    { 147, 183, 69 },
    { 141, 184, 72 },
    { 135, 185, 76 },
    { 129, 186, 79 },
    { 123, 187, 83 },
    { 116, 188, 87 },
    { 109, 189, 91 },
    { 102, 190, 96 },
    { 94, 191, 100 },
    { 86, 192, 105 },
    { 77, 192, 109 },
    { 67, 193, 114 },
    { 55, 194, 119 },
    { 39, 194, 124 },
    { 12, 195, 129 },
    { 0, 195, 134 },
    { 0, 195, 140 },
    { 0, 196, 145 },
    { 0, 196, 150 },
    { 0, 197, 156 },
    { 0, 197, 161 },
    { 0, 197, 166 },
    { 0, 197, 172 },
    { 0, 197, 177 },
    { 0, 197, 183 },
    { 0, 197, 188 },
    { 0, 197, 193 },
    { 0, 197, 198 },
    { 0, 197, 204 },
    { 0, 197, 209 },
    { 0, 197, 214 },
    { 0, 197, 219 },
    { 0, 196, 223 },
    { 0, 196, 228 },
    { 0, 196, 233 },
    { 0, 195, 237 },
    { 0, 195, 241 },
    { 0, 194, 245 },
    { 0, 194, 249 },
    { 0, 193, 253 },
    { 0, 192, 255 },
    { 0, 191, 255 },
    { 0, 191, 255 },
    { 0, 190, 255 },
    { 0, 189, 255 },
    { 0, 188, 255 },
    { 0, 187, 255 },
    { 0, 185, 255 },
    { 0, 184, 255 },
    { 0, 183, 255 },
    { 0, 181, 255 },
    { 0, 180, 255 },
    { 0, 178, 255 },
    { 25, 177, 255 },
    { 56, 175, 255 },
    { 76, 174, 255 },
    { 91, 172, 255 },
    { 105, 170, 255 },
    { 117, 168, 255 },
    { 128, 166, 255 },
    { 138, 164, 255 },
    { 148, 162, 255 },
    { 157, 160, 255 },
    { 165, 158, 255 },
    { 174, 156, 255 },
    { 181, 154, 255 },
    { 189, 151, 255 },
    { 196, 149, 255 },
    { 202, 147, 252 },
    { 208, 145, 248 },
    { 214, 143, 245 },
    { 220, 141, 241 },
    { 225, 139, 236 },
    { 230, 137, 232 },
    { 235, 135, 228 },
    { 240, 133, 223 },
    { 244, 131, 218 },
    { 248, 129, 213 },
    { 251, 128, 209 },
    { 254, 126, 204 },
    { 255, 125, 199 },
    { 255, 124, 193 },
    { 255, 123, 188 },
    { 255, 122, 183 },
    { 255, 122, 178 },
    { 255, 121, 173 },
    { 255, 121, 167 },
    { 255, 121, 162 },
    { 255, 121, 157 },
    { 255, 122, 152 },
    { 255, 122, 147 },
    { 255, 123, 141 },
    { 255, 124, 136 },
};
