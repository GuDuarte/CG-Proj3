#include "openglwindow.hpp"

#include <imgui.h>
#include <iostream>
#include <cmath>
#include <random>
#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/fast_trigonometry.hpp>


#include "imfilebrowser.h"

//I just want 2 rnd floats -1<f<1
float OpenGLWindow::thisshouldbeeasier()
{   unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::uniform_real_distribution<double> dist(-1.0f,1.0f);
    std::mt19937_64 rng(seed);
    return dist(rng);
}  

void OpenGLWindow::randomizeStar(glm::vec3 &position, glm::vec3 &rotation) {
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

void OpenGLWindow::handleEvent(SDL_Event& event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallModel.mouseMove(mousePosition);
    m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mousePress(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mouseRelease(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mouseRelease(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? 1.0f : -1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      Directionx = -1;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      Directionx= +1;
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      Directiony = 1;
    if (event.key.keysym.sym == SDLK_DOWN|| event.key.keysym.sym == SDLK_s)
      Directiony = -1;
    if (event.key.keysym.sym == SDLK_LSHIFT )
      Boost = 1;
    if (event.key.keysym.sym == SDLK_n )
      N = 1;    
    if (event.key.keysym.sym == SDLK_t )
      N = 0;  
    if (event.key.keysym.sym == SDLK_SPACE)
      Boost = -1;
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      Directionx = -1;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      Directionx = +1;
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      Directiony = 1;
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      Directiony = -1;
    if (event.key.keysym.sym == SDLK_LSHIFT )
      Boost = 1;  
    if (event.key.keysym.sym == SDLK_n )
      N = 1;       
    if (event.key.keysym.sym == SDLK_t )
      N = 0;  
    if (event.key.keysym.sym == SDLK_SPACE)
      Boost = -1;      
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);

  
  // Create programs
  for (const auto& name : m_shaderNames) {
    //std::cout << name; std::cout << "\n";
    auto path{getAssetsPath() + "shaders/" + name};
    auto program{createProgramFromFile(path + ".vert", path + ".frag")};
    m_programs.push_back(program);
  }

  m_program_star =   createProgramFromFile("./Proj3/assets/o_starfield.vert",
                                    "./Proj3/assets/o_starfield.frag");

  mstar.initializeGL(m_program_star);

  m_programA = createProgramFromFile(getAssetsPath() + "shaders/" + "f-16t.vert",
                                    getAssetsPath() + "shaders/" + "f-16t.frag");

  m_programB = createProgramFromFile(getAssetsPath() + "shaders/" + "f-16.vert",
                                    getAssetsPath() + "shaders/" + "f-16.frag");

  m_program2 = m_programA;

  m_program3 = createProgramFromFile(getAssetsPath() + "shaders/" + "star.vert",
                                    getAssetsPath() + "shaders/" + "star.frag");
  // Load default model
  
  loadModel(getAssetsPath() + "Skull.obj" , 1);
  m_mappingMode = 0;  // "From mesh" option

  loadModel(getAssetsPath() + "f-16.obj" , 2);

  loadModel(getAssetsPath() + "box.obj" , 3);
  
  // Initial trackball spin
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  m_trackBallModel.setVelocity(0.0001f);

}

void OpenGLWindow::loadModel(std::string_view path , int W) {
  if ( W == 1 ){
    m_model.loadDiffuseTexture(getAssetsPath() + "maps/pattern.png");
    m_model.loadNormalTexture(getAssetsPath() + "maps/pattern_normal.png");
    m_model.loadFromFile(path);
    m_model.setupVAO(m_programs.at(m_currentProgramIndex));
    m_trianglesToDraw = m_model.getNumTriangles();

    // Use material properties from the loaded model
    m_Ka = m_model.getKa();
    m_Kd = m_model.getKd();
    m_Ks = m_model.getKs();
    m_shininess = m_model.getShininess();
  }
  if ( W == 2 ){
    m_model2.loadDiffuseTexture(getAssetsPath() + "f-16.jpg");
    m_model2.loadNormalTexture(getAssetsPath() + "f-16.jpg");
    m_model2.loadFromFile(path);
    //std::cout << path << "\n";
    m_model2.setupVAO(m_program2);
    m_trianglesToDraw2 = m_model2.getNumTriangles();

    // Use material properties from the loaded model
    m_Ka2 = m_model2.getKa();
    m_Kd2 = m_model2.getKd();
    m_Ks2 = m_model2.getKs();
    m_shininess2 = m_model2.getShininess();
  }

  if ( W == 3 ){
    m_model3.loadDiffuseTexture(getAssetsPath() + "maps/pattern.png");
    m_model3.loadNormalTexture(getAssetsPath() + "maps/pattern_normal.png");
    m_model3.loadFromFile(path);
    //std::cout << path << "\n";
    m_model3.setupVAO(m_program3);
    m_trianglesToDraw3 = m_model3.getNumTriangles();

    // Use material properties from the loaded model
    m_Ka3 = m_model3.getKa();
    m_Kd3 = m_model3.getKd();
    m_Ks3 = m_model3.getKs();
    m_shininess3 = m_model3.getShininess();
  }



}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  //Program draws skull
  // Use currently selected program
  const auto program{m_programs.at(m_currentProgramIndex)};
  glUseProgram(program);
  
  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(program, "shininess")};
  GLint IaLoc{glGetUniformLocation(program, "Ia")};
  GLint IdLoc{glGetUniformLocation(program, "Id")};
  GLint IsLoc{glGetUniformLocation(program, "Is")};
  GLint KaLoc{glGetUniformLocation(program, "Ka")};
  GLint KdLoc{glGetUniformLocation(program, "Kd")};
  GLint KsLoc{glGetUniformLocation(program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(program, "normalTex")};
  GLint mappingModeLoc{glGetUniformLocation(program, "mappingMode")};
  GLint sscale{glGetUniformLocation(program, "scale")};
  GLint sscalex{glGetUniformLocation(program, "scalex")};
  GLint xloc{glGetUniformLocation(program, "xloc")};
  GLint yloc{glGetUniformLocation(program, "yloc")};
  GLint zloc{glGetUniformLocation(program, "closer")};
  
  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(mappingModeLoc, m_mappingMode);
  
  glUniform1f(sscale, m_scale);
  glUniform1f(sscalex, m_scalex);
  glUniform1f(xloc, m_xrnd);
  glUniform1f(yloc, m_yrnd);
  glUniform1f(zloc, m_speed);
  
  auto lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);

  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);

  auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);

  m_model.render(m_trianglesToDraw);

  glUseProgram(0);
  if(N == 1){
    m_program2 = m_programB;
  }
  if(N == 0){
    m_program2 = m_programA;
  }
  //Program2 draws plane
  glUseProgram(m_program2);
  // Set uniform variables used by every scene object
  m_mappingMode = 3;
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 1);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(mappingModeLoc, m_mappingMode);
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);
  
  //Plane Movement
   
    float deltaTime{static_cast<float>(getDeltaTime())};
    
    m_sidex = m_sidex+((Directionx*0.25)*deltaTime);
    m_sidey = m_sidey+((Directiony*0.25)*deltaTime);
    //m_sidez = m_sidez-(0.25*deltaTime);

    //Speed and Scale the skulls moves towards the camera
    m_speed = m_speed - speed_factor*deltaTime;
    //m_scale = m_scale + 0.0005f; 
    //std::cout << deltaTime << " " << speed_factor << " " << base_delta << "\n";
    // If the skull left [-1,1], restart speed and scale at a random x,y
    if(m_speed < 0){
      m_speed = 100.0f;      
      
      m_xrnd = thisshouldbeeasier();
      m_yrnd = thisshouldbeeasier();
    }
    
    // Angle while moving
    if(Directionx != 0){
      m_angle = glm::wrapAngle(m_angle + glm::radians(10.0f) * deltaTime* Directionx);
    }
    if (Boost == 1 and speed_factor < 10.0*base_delta){
      Boost = 0;
      speed_factor=speed_factor*2;
      //std::cout << m_FOV << "\n";
    }
    if (Boost == -1 and speed_factor > base_delta/2.0){
      Boost = 0;
      speed_factor=speed_factor/2;
      //std::cout << m_FOV << "\n";
    }
    
    // Reset if out of bounds
    if(m_sidex > 0.8 or m_sidex < -0.8){ 
      m_sidex = 0.0;
      m_sidey = 0.0;
      m_angle = 0.0;
    }   
    
    // Reset if out of bounds
    
    if(m_sidey > 0.8 or m_sidey < -0.8){ 
      m_sidex = 0.0;
      m_sidey = 0.0;
      m_angle = 0.0;
    } 
  
  
  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);

  GLint sideLocx{glGetUniformLocation(m_program2, "sidex")};
  glUniform1f(sideLocx, m_sidex);

  GLint sideLocy{glGetUniformLocation(m_program2, "sidey")};
  glUniform1f(sideLocy, m_sidey);
  
  GLint sAngle{glGetUniformLocation(m_program2, "angle")};
  glUniform1f(sAngle, m_angle);

  m_model2.render(m_trianglesToDraw2);

  glUseProgram(0);
  //END PROGRAM2

  //PROGRAM 3 DRAWS STARS
  glUseProgram(m_program3);

  // Set uniform variables used by every scene object
  m_mappingMode = 3;
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 1);
  glUniform1i(normalTexLoc, 0);
  glUniform1i(mappingModeLoc, m_mappingMode);
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);  

  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);

  GLint sideLocz{glGetUniformLocation(m_program3, "sidez")};
  glUniform1f(sideLocz, m_sidez);
  m_model3.render(m_trianglesToDraw3); 

  glUseProgram(0);
  glUseProgram(m_program_star);
  
  GLint viewMatrixLocstar{glGetUniformLocation(m_program_star, "viewMatrix")};
  GLint projMatrixLocstar{glGetUniformLocation(m_program_star, "projMatrix")};
  GLint modelMatrixLocstar{glGetUniformLocation(m_program_star, "modelMatrix")};
  
  glUniformMatrix4fv(viewMatrixLocstar, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLocstar, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniformMatrix4fv(modelMatrixLocstar, 1, GL_FALSE, &m_modelMatrix[0][0]);
  
  mstar.paintGL(deltaTime, m_viewMatrix , m_projMatrix , Boost);  
  if (Boost == 1 and m_FOV < 140){
    Boost = 0;
    m_FOV = m_FOV + 30;
    //std::cout << m_FOV << "\n";
    
  }
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  //mstar.paintUI();
  //abcg::OpenGLWindow::paintUI();
  // This function did too much to be erased. Got worked around instead. Pushed all UIs outside of viewrange.
  // File browser for models
  static ImGui::FileBrowser fileDialogModel;
  fileDialogModel.SetTitle("Load 3D Model");
  fileDialogModel.SetTypeFilters({".obj"});
  fileDialogModel.SetWindowSize(m_viewportWidth * 0.8f,
                                m_viewportHeight * 0.8f);

  // File browser for textures
  static ImGui::FileBrowser fileDialogDiffuseMap;
  fileDialogDiffuseMap.SetTitle("Load Diffuse Map");
  fileDialogDiffuseMap.SetTypeFilters({".jpg", ".png"});
  fileDialogDiffuseMap.SetWindowSize(m_viewportWidth * 0.8f,
                                     m_viewportHeight * 0.8f);

  // File browser for normal maps
  static ImGui::FileBrowser fileDialogNormalMap;
  fileDialogNormalMap.SetTitle("Load Normal Map");
  fileDialogNormalMap.SetTypeFilters({".jpg", ".png"});
  fileDialogNormalMap.SetWindowSize(m_viewportWidth * 0.8f,
                                    m_viewportHeight * 0.8f);

// Only in WebGL
#if defined(__EMSCRIPTEN__)
  fileDialogModel.SetPwd(getAssetsPath());
  fileDialogDiffuseMap.SetPwd(getAssetsPath() + "/maps");
  fileDialogNormalMap.SetPwd(getAssetsPath() + "/maps");
#endif

  // Create main window widget
  {
    auto widgetSize{ImVec2(222, 190)};

    if (!m_model.isUVMapped()) {
      // Add extra space for static text
      widgetSize.y += 26;
    }

    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth + widgetSize.x + 15, 15));
    ImGui::SetNextWindowSize(widgetSize);
    auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration};
    ImGui::Begin("Widget window", nullptr, flags);

    // Menu
    {
      bool loadModel{};
      bool loadDiffMap{};
      bool loadNormalMap{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          ImGui::MenuItem("Load 3D Model...", nullptr, &loadModel);
          ImGui::MenuItem("Load Diffuse Map...", nullptr, &loadDiffMap);
          ImGui::MenuItem("Load Normal Map...", nullptr, &loadNormalMap);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (loadModel) fileDialogModel.Open();
      if (loadDiffMap) fileDialogDiffuseMap.Open();
      if (loadNormalMap) fileDialogNormalMap.Open();
    }

    // Slider will be stretched horizontally
    ImGui::PushItemWidth(widgetSize.x - 16);
    ImGui::SliderInt("", &m_trianglesToDraw, 0, m_model.getNumTriangles(),
                     "%d triangles");
    ImGui::PopItemWidth();

    static bool faceCulling{};
    ImGui::Checkbox("Back-face culling", &faceCulling);

    if (faceCulling) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }

    // CW/CCW combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"CCW", "CW"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Front face",
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

      if (currentIndex == 0) {
        glFrontFace(GL_CCW);
      } else {
        glFrontFace(GL_CW);
      }
    }

    // Projection combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      ImGui::PushItemWidth(120);
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

      auto aspect{static_cast<float>(m_viewportWidth) /
                  static_cast<float>(m_viewportHeight)};
      
        m_projMatrix =
            glm::perspective(glm::radians(30.0f), aspect, 0.01f, 105.0f);

    }

    // Shader combo box
    {
      static std::size_t currentIndex{};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Shader", m_shaderNames.at(currentIndex))) {
        for (auto index : iter::range(m_shaderNames.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(m_shaderNames.at(index), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      // Set up VAO if shader program has changed
      if (static_cast<int>(currentIndex) != m_currentProgramIndex) {
        m_currentProgramIndex = currentIndex;
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
      }
    }

    if (!m_model.isUVMapped()) {
      ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh has no UV coords.");
    }

    // UV mapping box
    {
      std::vector<std::string> comboItems{"Triplanar", "Cylindrical",
                                          "Spherical"};

      if (m_model.isUVMapped()) comboItems.emplace_back("From mesh");

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("UV mapping",
                            comboItems.at(m_mappingMode).c_str())) {
        for (auto index : iter::range(comboItems.size())) {
          const bool isSelected{m_mappingMode == static_cast<int>(index)};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            m_mappingMode = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }

  // Create window for light sources
  if (m_currentProgramIndex < 4) {
    auto widgetSize{ImVec2(222, 244)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth + widgetSize.x +25, 50));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text("Light properties");

    // Slider to control light properties
    ImGui::PushItemWidth(widgetSize.x - 36);
    ImGui::ColorEdit3("Ia", &m_Ia.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Id", &m_Id.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Is", &m_Is.x, ImGuiColorEditFlags_Float);
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("Material properties");

    // Slider to control material properties
    ImGui::PushItemWidth(widgetSize.x - 36);
    ImGui::ColorEdit3("Ka", &m_Ka.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Kd", &m_Kd.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Ks", &m_Ks.x, ImGuiColorEditFlags_Float);
    ImGui::PopItemWidth();

    // Slider to control the specular shininess
    ImGui::PushItemWidth(widgetSize.x - 16);
    ImGui::SliderFloat("", &m_shininess, 0.0f, 500.0f, "shininess: %.1f");
    ImGui::PopItemWidth();

    ImGui::End();
  }

  fileDialogModel.Display();
  if (fileDialogModel.HasSelected()) {
    loadModel(fileDialogModel.GetSelected().string(), 1);
    fileDialogModel.ClearSelected();

    if (m_model.isUVMapped()) {
      // Use mesh texture coordinates if available...
      m_mappingMode = 3;
    } else {
      // ...or triplanar mapping otherwise
      m_mappingMode = 0;
    }
  }

  fileDialogDiffuseMap.Display();
  if (fileDialogDiffuseMap.HasSelected()) {
    m_model.loadDiffuseTexture(fileDialogDiffuseMap.GetSelected().string());
    fileDialogDiffuseMap.ClearSelected();
  }

  fileDialogNormalMap.Display();
  if (fileDialogNormalMap.HasSelected()) {
    m_model.loadNormalTexture(fileDialogNormalMap.GetSelected().string());
    fileDialogNormalMap.ClearSelected();
  }
  
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_trackBallModel.resizeViewport(width, height);
  m_trackBallLight.resizeViewport(width, height);
}

void OpenGLWindow::terminateGL() {
  for (const auto& program : m_programs) {
    glDeleteProgram(program);
  }
  mstar.terminateGL();
}

void OpenGLWindow::update() {
  //m_modelMatrix = m_trackBallModel.getRotation();
  
  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  
  /*
  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  */  
}

