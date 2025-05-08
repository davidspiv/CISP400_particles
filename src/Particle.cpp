#include "Particle.h"
#include "Matrices.h"
#include "config.h"
#include "util.h"

Particle::Particle(RenderTarget& target, sf::Color color, Vector2i mouseClickPosition)
    : m_ttl(TTL)
    , m_numPoints(getRandOddInt(10, 33))
    , m_radiansPerSec(getRandInt(0, 1) * M_PI)
    , m_vx(getRandInt(-500, 500))
    , m_vy(getRandInt(100, 500))
    , m_color1(sf::Color(255l, 255l, 255l))
    , m_color2(color)
    , m_A(2, m_numPoints)
{

    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(target.getSize().x, (-1.0) * target.getSize().y);
    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    double const dTheta = 2 * M_PI / (m_numPoints - 1);
    double theta = getRandDouble(0, 1) * M_PI / 2;

    for (int j = 0; j < m_numPoints; j++) {
        double r = (j % 2) ? 20.f : 40.f;
        double dx = r * std::cos(theta);
        double dy = r * std::sin(theta);

        m_A(0, j) = m_centerCoordinate.x + dx;
        m_A(1, j) = m_centerCoordinate.y + dy;
        theta += dTheta;
    }

    // Initialize vertexArray ONCE per lifetime
    m_shape = sf::VertexArray(sf::TriangleFan, m_numPoints + 1);
    m_shape[0].color = m_color1;
    for (size_t i = 1; i < m_shape.getVertexCount(); i++) {
        m_shape[i].color = m_color2;
    }
}

void Particle::update(RenderTarget& target, float dt)
{
    auto decayToBlack = [](Color& color, int rate = 1) {
        color.r = (color.r > rate) ? color.r - rate : 0;
        color.g = (color.g > rate) ? color.g - rate : 0;
        color.b = (color.b > rate) ? color.b - rate : 0;
    };

    m_ttl -= dt;
    m_vy -= G * dt;

    float const dx = m_vx * dt;
    float const dy = m_vy * dt;

    rotate(dt * m_radiansPerSec);
    scale(SCALE);
    translate(dx, dy);

    decayToBlack(m_color1);
    decayToBlack(m_color2);

    m_shape[0].position
        = sf::Vector2f(target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane));
    m_shape[0].color = m_color1;

    for (int j = 1; j <= m_numPoints; j++) {
        sf::Vector2f worldPos(m_A(0, j - 1), m_A(1, j - 1));
        sf::Vector2i screenPos = target.mapCoordsToPixel(worldPos, m_cartesianPlane);
        m_shape[j].position = static_cast<sf::Vector2f>(screenPos);
        m_shape[j].color = m_color2;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const { target.draw(m_shape); }

void Particle::rotate(double theta)
{
    Matrices::RotationMatrix const rotateMatrix(theta);
    Vector2f const tempCoord = m_centerCoordinate;

    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);
    m_A = rotateMatrix * m_A;
    translate(tempCoord.x, tempCoord.y);
}

void Particle::scale(double c)
{
    Matrices::ScalingMatrix const scaleMatrix(c);
    Vector2f const tempCoord = m_centerCoordinate;

    translate(-m_centerCoordinate.x, -m_centerCoordinate.y);
    m_A = scaleMatrix * m_A;
    translate(tempCoord.x, tempCoord.y);
}

void Particle::translate(double xShift, double yShift)
{
    Matrices::TranslationMatrix const transMatrix(xShift, yShift, m_numPoints);

    m_A = transMatrix + m_A;
    m_centerCoordinate.x += xShift;
    m_centerCoordinate.y += yShift;
}

bool Particle::almostEqual(double a, double b, double eps) { return fabs(a - b) < eps; }

void Particle::unitTests()
{
    int score = 0;

    std::cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.rows() == 2 && r.cols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta)) && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta))) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Testing ScalingMatrix constructor..." << std::flush;
    ScalingMatrix s(1.5);
    if (s.rows() == 2 && s.cols() == 2 && almostEqual(s(0, 0), 1.5) && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0) && almostEqual(s(1, 1), 1.5)) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Testing TranslationMatrix constructor..." << std::flush;
    TranslationMatrix t(5, -5, 3);
    if (t.rows() == 2 && t.cols() == 3 && almostEqual(t(0, 0), 5) && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5) && almostEqual(t(1, 1), -5) && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5)) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Testing Particles..." << std::endl;
    std::cout << "Testing Particle mapping to Cartesian origin..." << std::endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0) {
        std::cout << "Failed.  Expected (0,0).  Received: (" << m_centerCoordinate.x << ","
                  << m_centerCoordinate.y << ")" << std::endl;
    } else {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    }

    std::cout << "Applying one rotation of 90 degrees about the origin..." << std::endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.cols(); j++) {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j))
            || !almostEqual(m_A(1, j), initialCoords(0, j))) {
            std::cout << "Failed mapping: ";
            std::cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> ("
                      << m_A(0, j) << ", " << m_A(1, j) << ")" << std::endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Applying a scale of 0.5..." << std::endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.cols(); j++) {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j))
            || !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j))) {
            std::cout << "Failed mapping: ";
            std::cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> ("
                      << m_A(0, j) << ", " << m_A(1, j) << ")" << std::endl;
            scalePassed = false;
        }
    }
    if (scalePassed) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Applying a translation of (10, 5)..." << std::endl;
    initialCoords = m_A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.cols(); j++) {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j))
            || !almostEqual(m_A(1, j), 5 + initialCoords(1, j))) {
            std::cout << "Failed mapping: ";
            std::cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ") ==> ("
                      << m_A(0, j) << ", " << m_A(1, j) << ")" << std::endl;
            translatePassed = false;
        }
    }
    if (translatePassed) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Score: " << score << " / 7" << std::endl;
}
