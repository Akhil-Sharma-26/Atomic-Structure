#pragma comment(lib, "opengl32.lib") 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include "headers/Ground.hpp"
#include "headers/camera.hpp"
#include "headers/Inputs.hpp"
#include "headers/sphere.hpp"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// position of protons and neutrons
const std::vector<glm::vec3> nuclearPositions = {
    glm::vec3(0.2f,  0.2f,  0.2f),  // Front-top-right
    glm::vec3(-0.2f,  0.2f,  0.2f),  // Front-top-left
    glm::vec3(0.2f, -0.2f,  0.2f),  // Front-bottom-right
    glm::vec3(-0.2f, -0.2f,  0.2f),  // Front-bottom-left
    glm::vec3(0.2f,  0.2f, -0.2f),  // Back-top-right
    glm::vec3(-0.2f,  0.2f, -0.2f),  // Back-top-left
    glm::vec3(0.2f, -0.2f, -0.2f),  // Back-bottom-right
    glm::vec3(-0.2f, -0.2f, -0.2f)   // Back-bottom-left
};


int main()
{
    // debug memory leaks
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // glfw initialization and configuration
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // CORE_PROFILE -> preprocessor macro that defines the OpenGL core profile, which is used to specify that an OpenGL context should be created with the core profile, excluding deprecated features
    // PROFILE -> A preprocessor macro that defines the OpenGL profile setting for GLFW, indicating the desired OpenGL profile version to be used in an application


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Creating GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RaceGL", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // making glfw context for the opened window
    glfwMakeContextCurrent(window);
    Input::WindowData data{}; // instance of WindowData struct of the input class
    Input input;
    data.input = &input;
    data.camera = &camera;
    glfwSetWindowUserPointer(window, &data); // Setting pointers: to allow callback functions to retrieve pointers in order to access the associated Input::WindowData to input(keyboards) and camera objects

    // The following callbacks are using getWindowUserPointer function to get the pointers I set above.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, Input::mouse_callback);
    glfwSetScrollCallback(window, Input::scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // disabling the cursor ie CAPTURING THE MOUSE

    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // using glad instead of glew
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;

    // flip loaded textures on y-axis (stb_image)
    //stbi_set_flip_vertically_on_load(true);

    // OpenGL state
    glEnable(GL_DEPTH_TEST); // using depth

    // Making a ground
    //Ground grnd(50, 50);

    // Making a Sphere
    Sphere sphere(1.0f, 128, 128,
        "C:\\Users\\Akhil\\source\\repos\\Atomic-Structure\\Atomic-Structure\\assets\\shaders\\Sphere.vert",
        "C:\\Users\\Akhil\\source\\repos\\Atomic-Structure\\Atomic-Structure\\assets\\shaders\\Sphere.frag");
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        

        // process input
        Input::keyboardInput(window, camera, deltaTime);

        // clear buffers
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "OpenGL Error before glClear: " << err << std::endl;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "OpenGL Error after glClear: " << err << std::endl;
        }

        // 1. Render the 3D scene
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        //ourShader.setMat4("projection", projection);
        //ourShader.setMat4("view", view
        //grnd.render(view, projection, camera.Position);
        for (const auto& it : nuclearPositions) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, it);
            sphere.render(view, projection, camera.Position, model);
        }


        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "OpenGL error during ground rendering: " << error << std::endl;
        }
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    // Cleanup ImGui and GLFW resources
    glfwTerminate();
    return 0;
}

// GLFW: callback whenever the window size changes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

