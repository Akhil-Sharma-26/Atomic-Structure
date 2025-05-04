#version 330 core
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

void main()
{
    // Simple diffuse lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 color = vec3(0.7, 0.2, 0.2) * diff;  // Reddish color with lighting
    
    FragColor = vec4(color, 1.0);
}