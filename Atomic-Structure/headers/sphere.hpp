#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "headers/shaders.hpp"

class Sphere {
public:
    Sphere(float radius = 1.0f, int sectors = 32, int stacks = 32,
        const char* vertPath = "C:\\Users\\Akhil\\source\\repos\\Atomic-Structure\\Atomic-Structure\\assets\\shaders\\Sphere.vert",
        const char* fragPath = "C:\\Users\\Akhil\\source\\repos\\Atomic-Structure\\Atomic-Structure\\assets\\shaders\\Sphere.frag");
    ~Sphere();

    void render(const glm::mat4& view, const glm::mat4& projection,
        const glm::vec3& viewPos, const glm::mat4& model = glm::mat4(1.0f));
    Shader& GetShader() { return *shader; }
    Sphere(const Sphere&) = delete;            // Disable copy
    Sphere& operator=(const Sphere&) = delete; // Disable assignment

    Sphere(Sphere&& other) noexcept;           // Enable move
    Sphere& operator=(Sphere&& other) noexcept;



private:
    void createSphere(float radius, int sectors, int stacks);
    void setupBuffers();

    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
    std::unique_ptr<Shader> shader;
    bool initialized;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};