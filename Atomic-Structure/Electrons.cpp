#include "headers/Electrons.hpp"
#include <glm/gtx/rotate_vector.hpp>

Electron::Electron(float orbitRadius, float orbitSpeed,
    const glm::vec3& orbitalPlaneNormal, const glm::vec3& color)
    : m_orbitRadius(orbitRadius), m_orbitSpeed(orbitSpeed),
    m_orbitalPlaneNormal(glm::normalize(orbitalPlaneNormal)),
    m_color(color), m_currentAngle(0.0f),
    m_sphere(0.03f, 16, 16,  // Small radius, low resolution
        "C:\\Users\\Akhil\\source\\repos\\Atomic-Structure\\Atomic-Structure\\assets\\shaders\\Electrons.vert",
        "C:\\Users\\Akhil\\source\\repos\\Atomic-Structure\\Atomic-Structure\\assets\\shaders\\Electrons.frag") {
}

void Electron::Update(float deltaTime) {
    m_currentAngle += m_orbitSpeed * deltaTime;
    if (m_currentAngle > 360.0f) m_currentAngle -= 360.0f;
}

// In Electron.cpp - Uncomment and fix the Render method
void Electron::Render(const glm::mat4& view, const glm::mat4& projection,
    const glm::vec3& viewPos) {
    // Calculate orbital position
    glm::vec3 basePosition(m_orbitRadius, 0.0f, 0.0f);
    glm::vec3 rotatedPosition = glm::rotate(basePosition,
        glm::radians(m_currentAngle),
        m_orbitalPlaneNormal);

    // Create model matrix
    glm::mat4 model(1.0f);
    model = glm::translate(model, rotatedPosition);

    // Set custom color
    m_sphere.GetShader().use();
    m_sphere.GetShader().setVec3("objectColor", m_color);

    m_sphere.render(view, projection, viewPos, model);
}

//Electron::~Electron() {
//    m_sphere.~Sphere();
//}
