#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "Sphere.hpp"
#include <glm/glm.hpp>

class Electron {
public:
    Electron(float orbitRadius, float orbitSpeed,
        const glm::vec3& orbitalPlaneNormal = glm::vec3(0.0f, 1.0f, 0.0f),
        const glm::vec3& color = glm::vec3(0.0f, 0.5f, 1.0f));

    void Update(float deltaTime);
    void Render(const glm::mat4& view, const glm::mat4& projection,
        const glm::vec3& viewPos);
    //~Electron();

private:
    Sphere m_sphere;
    float m_orbitRadius;
    float m_orbitSpeed;
    float m_currentAngle;
    glm::vec3 m_orbitalPlaneNormal;
    glm::vec3 m_color;
};