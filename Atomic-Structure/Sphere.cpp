#include "headers/sphere.hpp"    
#include <iostream>
#include <glm/ext/scalar_constants.hpp>

Sphere::Sphere(float radius, int sectors, int stacks, const char* vertPath, const char* fragPath)
    : initialized(false) {
    createSphere(radius, sectors, stacks);
    setupBuffers();

    try {
        shader = std::make_unique<Shader>(vertPath, fragPath);
        initialized = true;
        std::cout << "Sphere shader compilation successful" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Failed to create sphere shader: " << e.what() << std::endl;
    }
}

void Sphere::createSphere(float radius, int sectors, int stacks) {
    const float PI = glm::pi<float>();
    vertices.clear();
    indices.clear();

    // Vertex generation
    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = PI / 2 - i * (PI / stacks);
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * PI / sectors;

            // Position
            vertices.push_back(xy * cosf(sectorAngle)); // x
            vertices.push_back(xy * sinf(sectorAngle)); // y
            vertices.push_back(z);                      // z

            // Normal
            glm::vec3 normal = glm::normalize(glm::vec3(
                vertices[vertices.size() - 3],
                vertices[vertices.size() - 2],
                vertices[vertices.size() - 1]
            ));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    // Index generation
    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    indexCount = indices.size();
}

void Sphere::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Sphere::render(const glm::mat4& view, const glm::mat4& projection,
    const glm::vec3& viewPos, const glm::mat4& model) {

    if (!initialized) {
        std::cout << "Warning: Attempting to render uninitialized sphere" << std::endl;
        return;
    }

    shader->use();

    // Matrices
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Lighting
    shader->setVec3("lightPos", glm::vec3(2.0f, 5.0f, 2.0f));
    shader->setVec3("viewPos", viewPos);
    shader->setVec3("objectColor", glm::vec3(0.8f, 0.3f, 0.2f));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL error during sphere rendering: " << error << std::endl;
    }
}

Sphere::~Sphere() {
    if (shader) {
        shader = nullptr;
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    VAO = VBO = EBO = 0;
}

Sphere::Sphere(Sphere&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
    indexCount(other.indexCount),
    shader(std::move(other.shader)),
    initialized(other.initialized) {
    other.VAO = other.VBO = other.EBO = 0;
    other.initialized = false;
}

Sphere& Sphere::operator=(Sphere&& other) noexcept {
    if (this != &other) {
        this->~Sphere();
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        indexCount = other.indexCount;
        shader = std::move(other.shader);
        initialized = other.initialized;

        other.VAO = other.VBO = other.EBO = 0;
        other.initialized = false;
    }
    return *this;
}