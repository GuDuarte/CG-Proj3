#ifndef SMODEL_HPP_
#define SMODEL_HPP_

#include "abcg.hpp"

struct sVertex {
  glm::vec3 position{};

  bool operator==(const sVertex& other) const noexcept {
    return position == other.position;
  }
};

class sModel {
 public:
  sModel() = default;
  virtual ~sModel();

  sModel(const sModel&) = delete;
  sModel(sModel&&) = default;
  sModel& operator=(const sModel&) = delete;
  sModel& operator=(sModel&&) = default;

  void loadFromFile(std::string_view path, bool standardize = true);
  void render(int snumTriangles = -1) const;
  void setupVAO(GLuint sprogram);

  [[nodiscard]] int getsNumTriangles() const {
    return static_cast<int>(m_sindices.size()) / 3;
  }

 private:
  GLuint m_sVAO{};
  GLuint m_sVBO{};
  GLuint m_sEBO{};

  std::vector<sVertex> m_svertices;
  std::vector<GLuint> m_sindices;

  void createBuffers();
  void standardize();
};

#endif