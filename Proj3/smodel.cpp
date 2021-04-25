#include "smodel.hpp"

#include <fmt/core.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <filesystem>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Custom specialization of std::hash injected in namespace std
namespace std {
template <>
struct hash<sVertex> {
  size_t operator()(sVertex const& svertex) const noexcept {
    std::size_t h1{std::hash<glm::vec3>()(svertex.position)};
    return h1;
  }
};
}  // namespace std

sModel::~sModel() {
  glDeleteBuffers(1, &m_sEBO);
  glDeleteBuffers(1, &m_sVBO);
  glDeleteVertexArrays(1, &m_sVAO);
}

void sModel::createBuffers() {
  // Delete previous buffers
  glDeleteBuffers(1, &m_sEBO);
  glDeleteBuffers(1, &m_sVBO);

  // VBO
  glGenBuffers(1, &m_sVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_sVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_svertices[0]) * m_svertices.size(),
               m_svertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  glGenBuffers(1, &m_sEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_sindices[0]) * m_sindices.size(),
               m_sindices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void sModel::loadFromFile(std::string_view path, bool standardize) {
  auto basePath{std::filesystem::path{path}.parent_path().string() + "/"};

  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = basePath;  // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_svertices.clear();
  m_sindices.clear();

  // A key:value map with key=svertex and value=index
  std::unordered_map<sVertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to svertex
      tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // svertex coordinates
      std::size_t startIndex{static_cast<size_t>(3 * index.vertex_index)};
      float vx{attrib.vertices.at(startIndex + 0)};
      float vy{attrib.vertices.at(startIndex + 1)};
      float vz{attrib.vertices.at(startIndex + 2)};

      sVertex svertex{};
      svertex.position = {vx, vy, vz};

      // If hash doesn't contain this svertex
      if (hash.count(svertex) == 0) {
        // Add this index (size of m_vertices)
        hash[svertex] = m_svertices.size();
        // Add this svertex
        m_svertices.push_back(svertex);
      }

      m_sindices.push_back(hash[svertex]);
    }
  }

  if (standardize) {
    this->standardize();
  }

  createBuffers();
}

void sModel::render(int numTriangles) const {
  glBindVertexArray(m_sVAO);

  GLsizei numIndices = (numTriangles < 0) ? m_sindices.size() : numTriangles * 3;

  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

void sModel::setupVAO(GLuint program) {
  // Release previous VAO
  glDeleteVertexArrays(1, &m_sVAO);

  // Create VAO
  glGenVertexArrays(1, &m_sVAO);
  glBindVertexArray(m_sVAO);

  // Bind EBO and VBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sEBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_sVBO);

  // Bind vertex attributes
  GLint positionAttribute = glGetAttribLocation(program, "inPosition");
  if (positionAttribute >= 0) {
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                          sizeof(sVertex), nullptr);
  }

  // End of binding
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void sModel::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& svertex : m_svertices) {
    max.x = std::max(max.x, svertex.position.x);
    max.y = std::max(max.y, svertex.position.y);
    max.z = std::max(max.z, svertex.position.z);
    min.x = std::min(min.x, svertex.position.x);
    min.y = std::min(min.y, svertex.position.y);
    min.z = std::min(min.z, svertex.position.z);
  }

  // Center and scale
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& svertex : m_svertices) {
    svertex.position = (svertex.position - center) * scaling;
  }
}
