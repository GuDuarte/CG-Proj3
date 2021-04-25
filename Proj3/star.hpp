#ifndef STAR_HPP_
#define STAR_HPP_

#include <random>

#include "abcg.hpp"
#include "smodel.hpp"

class Star {
 public:
  void initializeGL(GLuint m_program);
  void paintGL(float deltatime, 	glm::mat4 view , 	glm::mat4 proj , int Boost );
  void paintUI();
  void terminateGL();

 private:
  static const int m_numStars{1000};

  GLuint m_program_star{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

  sModel m_model_star;

  std::array<glm::vec3, m_numStars> m_starPositions;
  std::array<glm::vec3, m_numStars> m_starRotations;
  float m_angle_star{};

  glm::mat4 m_viewMatrixstar{1.0f};
  glm::mat4 m_projMatrixstar{1.0f};
  float m_FOV{30.0f};

  void randomizeStar(glm::vec3 &position, glm::vec3 &rotation);
  void update(float deltatime);
};

#endif