#include "star.hpp"
#include "openglwindow.hpp"

#include <imgui.h>
#include <iostream>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Star::initializeGL(GLuint m_program) {
  glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  glEnable(GL_DEPTH_TEST);

  // Create program
  m_program_star = m_program;

  // Load model
  m_model_star.loadFromFile("./Proj3/assets/o_box.obj");

  m_model_star.setupVAO(m_program_star);

  // Camera at (0,0,0) and looking towards the negative z
  m_viewMatrixstar =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  // Setup stars
  for (const auto index : iter::range(m_numStars)) {
    auto &positionstar{m_starPositions.at(index)};
    auto &rotationstar{m_starRotations.at(index)};

    randomizeStar(positionstar, rotationstar);
  }
}

void Star::randomizeStar(glm::vec3 &position, glm::vec3 &rotation) {
  // Get random position
  // x and y coordinates in the range [-20, 20]
  // z coordinates in the range [-100, 0]
  std::uniform_real_distribution<float> distPosXY(-20.0f, 20.0f);
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);

  position = glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                       distPosZ(m_randomEngine));

  //  Get random rotation axis
  std::uniform_real_distribution<float> distRotAxis(-1.0f, 1.0f);

  rotation = glm::normalize(glm::vec3(distRotAxis(m_randomEngine),
                                      distRotAxis(m_randomEngine),
                                      distRotAxis(m_randomEngine)));
}

void Star::paintGL(float deltatime ,	glm::mat4 view , 	glm::mat4 proj , int Boost) {
  update(deltatime);
  /*
  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  */
  glUseProgram(m_program_star);
  
  // Get location of uniform variables (could be precomputed)
  GLint viewMatrixLocstar{glGetUniformLocation(m_program_star, "viewMatrix")};
  GLint projMatrixLocstar{glGetUniformLocation(m_program_star, "projMatrix")};
  GLint modelMatrixLocstar{glGetUniformLocation(m_program_star, "modelMatrix")};
  GLint colorLoc{glGetUniformLocation(m_program_star, "color")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLocstar, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projMatrixLocstar, 1, GL_FALSE, &proj[0][0]);
  glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);  // White
  if (Boost == 1 and m_FOV < 140){
    Boost = 0;
    m_FOV = m_FOV + 30;
    //std::cout << m_FOV << "\n";
    
  }
  // Render each star
  for (const auto index : iter::range(m_numStars)) {
    auto &positionstar{m_starPositions.at(index)};
    auto &rotationstar{m_starRotations.at(index)};

    // Compute model matrix of the current star
    glm::mat4 modelMatrixstar{1.0f};
    modelMatrixstar = glm::translate(modelMatrixstar, positionstar);
    modelMatrixstar = glm::scale(modelMatrixstar, glm::vec3(0.2f));
    modelMatrixstar = glm::rotate(modelMatrixstar, m_angle_star, rotationstar);

    // Set uniform variable
    glUniformMatrix4fv(modelMatrixLocstar, 1, GL_FALSE, &modelMatrixstar[0][0]);

    m_model_star.render();
  }

  glUseProgram(0);
}

void Star::paintUI() {
  //abcg::OpenGLWindow::paintUI();

  {
    auto widgetSize{ImVec2(218, 62)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    {
      ImGui::PushItemWidth(120);
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto index : iter::range(comboItems.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      ImGui::PushItemWidth(170);
      auto aspect{static_cast<float>(m_viewportWidth) /
                  static_cast<float>(m_viewportHeight)};
     
        m_projMatrixstar =
            glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 105.0f);

        ImGui::SliderFloat("FOV", &m_FOV, 5.0f, 179.0f, "%.0f degrees");
      
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }
}


void Star::terminateGL() { glDeleteProgram(m_program_star); }

void Star::update(float deltaTime) {
  // Animate angle by 90 degrees per second
  
  m_angle_star = glm::wrapAngle(m_angle_star + glm::radians(90.0f) * deltaTime);

  // Update stars
  for (const auto index : iter::range(m_numStars)) {
    auto &positionstar{m_starPositions.at(index)};
    auto &rotationstar{m_starRotations.at(index)};

    // The star position in z increases 10 units per second
    positionstar.z += deltaTime * 10.0f;

    // If this star is now behind the camera, select a new random position and
    // orientation, and move it back to -100
    if (positionstar.z > 0.1f) {
      randomizeStar(positionstar, rotationstar);
      positionstar.z = -100.0f;  // Back to -100
    }
  }
}