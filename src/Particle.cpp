#include "Particle.h"
#include "Matrices.h"
#include "util.h"

Particle::Particle(RenderTarget& target, int numPoints,
    Vector2i mouseClickPosition)
    : m_ttl(TTL)
    , m_numPoints(numPoints)
    , m_radiansPerSec(getRandInt(0, 1) * M_PI)
    , m_vx(rand() % 2 ? getRandInt(100, 500) : getRandInt(100, 500) * -1)
    , m_vy(getRandInt(100, 500))
    , m_color1(WHITE)
    , m_color2(getRandColor())
    , m_A(2, numPoints)
{

    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(target.getSize().x, (-1.0) * target.getSize().y);
    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    double theta = getRandDouble(0, 1) * M_PI / 2;
    double const dTheta = 2 * M_PI / (numPoints - 1);

    for (int j = 0; j < numPoints; j++) {
        double r = getRandInt(20, 80);
        double dx = r * std::cos(theta);
        double dy = r * std::sin(theta);

        m_A(0, j) = m_centerCoordinate.x + dx;
        m_A(1, j) = m_centerCoordinate.y + dy;
        theta += dTheta;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const
{
    sf::VertexArray lines(sf::TriangleFan, m_numPoints + 1);
    sf::Vector2f center(m_centerCoordinate);

    lines[0].position = sf::Vector2f(target.mapCoordsToPixel(center, m_cartesianPlane));
    lines[0].color = m_color1;

    for (int j = 1; j <= m_numPoints; j++) {
        sf::Vector2f worldPos(m_A(0, j - 1), m_A(1, j - 1));
        sf::Vector2i pixelPos = target.mapCoordsToPixel(worldPos, m_cartesianPlane);
        lines[j].position = static_cast<sf::Vector2f>(pixelPos);
        lines[j].color = m_color2;
    }

    target.draw(lines);
}

void Particle::update(float dt)
{
    m_ttl -= dt;
    m_vy -= G * dt;

    float const dx = m_vx * dt;
    float const dy = m_vy * dt;

    rotate(dt * m_radiansPerSec);
    scale(SCALE);
    translate(dx, dy);
}

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

bool Particle::almostEqual(double a, double b, double eps)
{
    return fabs(a - b) < eps;
}

void Particle::unitTests()
{
    int score = 0;

    std::cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta)) && almostEqual(r(0, 1), -sin(theta)) && almostEqual(r(1, 0), sin(theta)) && almostEqual(r(1, 1), cos(theta))) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Testing ScalingMatrix constructor..." << std::flush;
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2 && almostEqual(s(0, 0), 1.5) && almostEqual(s(0, 1), 0) && almostEqual(s(1, 0), 0) && almostEqual(s(1, 1), 1.5)) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Testing TranslationMatrix constructor..." << std::flush;
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3 && almostEqual(t(0, 0), 5) && almostEqual(t(1, 0), -5) && almostEqual(t(0, 1), 5) && almostEqual(t(1, 1), -5) && almostEqual(t(0, 2), 5) && almostEqual(t(1, 2), -5)) {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    } else {
        std::cout << "Failed." << std::endl;
    }

    std::cout << "Testing Particles..." << std::endl;
    std::cout << "Testing Particle mapping to Cartesian origin..." << std::endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0) {
        std::cout << "Failed.  Expected (0,0).  Received: (" << m_centerCoordinate.x
                  << "," << m_centerCoordinate.y << ")" << std::endl;
    } else {
        std::cout << "Passed.  +1" << std::endl;
        score++;
    }

    std::cout << "Applying one rotation of 90 degrees about the origin..."
              << std::endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++) {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1, j), initialCoords(0, j))) {
            std::cout << "Failed mapping: ";
            std::cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j)
                      << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")"
                      << std::endl;
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
    for (int j = 0; j < initialCoords.getCols(); j++) {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j)) || !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j))) {
            std::cout << "Failed mapping: ";
            std::cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j)
                      << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")"
                      << std::endl;
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
    for (int j = 0; j < initialCoords.getCols(); j++) {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !almostEqual(m_A(1, j), 5 + initialCoords(1, j))) {
            std::cout << "Failed mapping: ";
            std::cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j)
                      << ") ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")"
                      << std::endl;
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
