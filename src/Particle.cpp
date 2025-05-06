#include "Particle.h"
#include "Matrices.h"
#include "util.h"

Particle::Particle(RenderTarget &target, int numPoints,
                   Vector2i mouseClickPosition)
    : m_ttl(TTL), m_numPoints(numPoints),
      m_radiansPerSec(getRandInt(0, 1) * M_PI), m_vx(getRandInt(100, 500)),
      m_vy(getRandInt(100, 500)), m_color1(WHITE), m_color2(getRandColor()),
      m_A(2, numPoints) {

  m_cartesianPlane.setCenter(0, 0);
  m_cartesianPlane.setSize(target.getSize().x, (-1.0) * target.getSize().y);
  m_centerCoordinate =
      target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

  double theta = getRandInt(0, 1) * M_PI / 2;
  const float dTheta = 2 * M_PI / (numPoints - 1);

  for (size_t i = 0; i < numPoints; i++) {
    float r = getRandInt(20, 80);
    float dx = r * cos(theta);
    float dy = r * sin(theta);

    m_A(0, i) = m_centerCoordinate.x + dx;
    m_A(1, i) = m_centerCoordinate.y + dy;
    theta += dTheta;
  }
};


void Particle::rotate(double theta) {
  const Matrices::RotationMatrix rotateMatrix(theta);
  const Vector2f tempCoord = m_centerCoordinate;

  translate(-m_centerCoordinate.x, -m_centerCoordinate.y);
  m_A = rotateMatrix * m_A;
  translate(tempCoord.x, tempCoord.y);
};


void Particle::scale(double c) {
  const Matrices::ScalingMatrix scaleMatrix(c);
  const Vector2f tempCoord = m_centerCoordinate;

  translate(-m_centerCoordinate.x, -m_centerCoordinate.y);
  m_A = scaleMatrix * m_A;
  translate(tempCoord.x, tempCoord.y);
};


void Particle::translate(double xShift, double yShift) {
  const Matrices::TranslationMatrix transMatrix(xShift, yShift, m_numPoints);

  m_A = transMatrix + m_A;
  m_centerCoordinate.x += xShift;
  m_centerCoordinate.y += yShift;
};


bool Particle::almostEqual(double a, double b, double eps) {
  return fabs(a - b) < eps;
}


void Particle::unitTests() {
  int score = 0;

  std::cout << "Testing RotationMatrix constructor...";
  double theta = M_PI / 4.0;
  RotationMatrix r(M_PI / 4);
  if (r.getRows() == 2 && r.getCols() == 2 &&
      almostEqual(r(0, 0), cos(theta)) && almostEqual(r(0, 1), -sin(theta)) &&
      almostEqual(r(1, 0), sin(theta)) && almostEqual(r(1, 1), cos(theta))) {
    std::cout << "Passed.  +1" << std::endl;
    score++;
  } else {
    std::cout << "Failed." << std::endl;
  }

  std::cout << "Testing ScalingMatrix constructor...";
  ScalingMatrix s(1.5);
  if (s.getRows() == 2 && s.getCols() == 2 && almostEqual(s(0, 0), 1.5) &&
      almostEqual(s(0, 1), 0) && almostEqual(s(1, 0), 0) &&
      almostEqual(s(1, 1), 1.5)) {
    std::cout << "Passed.  +1" << std::endl;
    score++;
  } else {
    std::cout << "Failed." << std::endl;
  }

  std::cout << "Testing TranslationMatrix constructor...";
  TranslationMatrix t(5, -5, 3);
  if (t.getRows() == 2 && t.getCols() == 3 && almostEqual(t(0, 0), 5) &&
      almostEqual(t(1, 0), -5) && almostEqual(t(0, 1), 5) &&
      almostEqual(t(1, 1), -5) && almostEqual(t(0, 2), 5) &&
      almostEqual(t(1, 2), -5)) {
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
    if (!almostEqual(m_A(0, j), -initialCoords(1, j)) ||
        !almostEqual(m_A(1, j), initialCoords(0, j))) {
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
    if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0, j)) ||
        !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j))) {
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
    if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) ||
        !almostEqual(m_A(1, j), 5 + initialCoords(1, j))) {
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
