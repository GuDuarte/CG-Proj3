#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>
#include <random>
#include "abcg.hpp"
#include "model.hpp"
#include "trackball.hpp"
#include "star.hpp"


class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  int m_viewportWidth{};
  int m_viewportHeight{};

  
  Model m_model;
  Model m_model2;
  Model m_model3;
  int m_trianglesToDraw{};
  int m_trianglesToDraw2{};
  int m_trianglesToDraw3{};
  
  TrackBall m_trackBallModel;
  TrackBall m_trackBallLight;
  float m_zoom{};

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  //skull uses mesh + normalmapping

  // Shaders
  std::vector<const char*> m_shaderNames{
      "normalmapping", "texture", "blinnphong", "phong",
      "gouraud",       "normal",  "depth"};
  std::vector<GLuint> m_programs;

  GLuint m_program2{};
  GLuint m_programA{};
  GLuint m_programB{};
  GLuint m_program3{};

  int m_currentProgramIndex{};

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
  int m_mappingMode{3};

  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  float m_shininess{};

  Star mstar;

  /*
  glm::vec4 m_lightDir2{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia2{1.0f};
  glm::vec4 m_Id2{1.0f};
  glm::vec4 m_Is2{1.0f};
  */
  glm::vec4 m_Ka2{};
  glm::vec4 m_Kd2{};
  glm::vec4 m_Ks2{};
  float m_shininess2{};
  /*
  glm::vec4 m_lightDir3{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia3{1.0f};
  glm::vec4 m_Id3{1.0f};
  glm::vec4 m_Is3{1.0f};
  */
  glm::vec4 m_Ka3{};
  glm::vec4 m_Kd3{};
  glm::vec4 m_Ks3{};
  float m_shininess3{};

  //Plane controls
  float m_sidex{};
  float m_sidey{};
  float m_sidez{99.0f};
  float m_speed{90.0f};
  float m_scale{0.33f};
  float m_scalex{-0.33f};
  float m_angle{};
  float speed_factor{20.0f};
  float base_delta{20.0f};
  int Directionx;
  int Directiony;
  int Boost;
  int N;
  float m_xrnd{0.33f};
  float m_yrnd{0.33f};


  static const int m_numStars{1000};

  GLuint m_program_star{};

  std::default_random_engine m_randomEngine;
  /*
  std::array<glm::vec3, m_numStars> m_starPositions;
  std::array<glm::vec3, m_numStars> m_starRotations;
  float m_angle_star{};

  glm::mat4 m_viewMatrixstar{1.0f};
  glm::mat4 m_projMatrixstar{1.0f};
  */
  float m_FOV{30.0f};



  void randomizeStar(glm::vec3 &position, glm::vec3 &rotation);
  void loadModel(std::string_view path , int W);
  void update();
  float thisshouldbeeasier();
};

#endif