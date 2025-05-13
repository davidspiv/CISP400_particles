#pragma once
#include "Matrices.h"
#include <SFML/Graphics.hpp>

#define _USE_MATH_DEFINES // for vs

using namespace Matrices;
using namespace sf;

class Particle : public Drawable {
public:
    static float constexpr G = 1000;  // Gravity
    static float constexpr TTL = 2.0; // Time To Live
    static float constexpr SCALE = 0.99;

    Particle(RenderTarget& target, sf::Color color, Vector2i mouseClickPosition);
    void update(RenderTarget& target, float dt);
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    float getTTL() { return m_ttl; }

    // Functions for unit testing
    bool almostEqual(double a, double b, double eps = 0.0001);
    void unitTests();

private:
    float m_ttl;
    int m_numPoints;
    Vector2f m_centerCoordinate;
    float m_radiansPerSec;
    float m_vx;
    float m_vy;
    Color m_color1;
    Color m_color2;
    Matrix m_A;
    sf::VertexArray m_shape;

    /// rotate Particle by theta radians counter-clockwise
    /// construct a RotationMatrix R, left multiply it to m_A
    void rotate(double theta);

    /// Scale the size of the Particle by factor c
    /// construct a ScalingMatrix S, left multiply it to m_A
    void scale(double c);

    /// shift the Particle by (xShift, yShift) coordinates
    /// construct a TranslationMatrix T, add it to m_A
    void translate(double xShift, double yShift);
};
