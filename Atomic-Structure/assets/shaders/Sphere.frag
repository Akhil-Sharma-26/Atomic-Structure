#version 330 core
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Add some randomness to color
    vec3 baseColor = vec3(0.8, 0.3, 0.2);
    vec3 variedColor = baseColor * (0.9 + 0.1 * sin(FragPos.x * 10.0));
    
    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * variedColor;
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * variedColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = specularStrength * spec * variedColor;
    
    // Final color with some depth-based darkening
    float depthFactor = 1.0 - smoothstep(0.0, 5.0, length(FragPos));
    vec3 result = (ambient + diffuse + specular) * depthFactor;
    
    FragColor = vec4(result, 1.0);
}