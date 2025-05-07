#include <GL/glut.h>  // for windowing and input functionality
#include <cmath>      
#include <vector>     
#include <ctime>      // Time functions for random seed
#include <iostream>

//======================================================================================
// INITIAL CAMERA PARAMETERS
//======================================================================================
float camX = 0, camY = 0, camZ = 3;   // Camera position in 3D space
float lookX = 0, lookY = 0, lookZ = 0; // Point the camera is looking at
float angleX = 0, angleY = 0;          // Camera rotation angles for mouse control

int atomicNumber = 1; // Default to hydrogen

//======================================================================================
// LIGHTING PARAMETERS
//======================================================================================
bool lightingEnabled = true;  // Toggle for lighting effects

// Light position in 3D space
// The fourth parameter (1.0f) indicates a positional light (0.0f would be directional)
GLfloat lightPos[] = {2.0f, 5.0f, 2.0f, 1.0f}; 

//======================================================================================
// ATOMIC NUCLEUS CONFIGURATION
//======================================================================================
// Pre-defined positions for nucleus particles (protons/neutrons)
// Each vector represents a 3D coordinate {x, y, z}
const std::vector<std::vector<float>> nuclearPositions = {
    {0.0f, 0.0f, 0.0f},     // Center
    {0.12f, 0.12f, 0.12f},  // Upper octant 1
    {-0.12f, 0.12f, 0.12f}, // Upper octant 2
    {0.12f, -0.12f, 0.12f}, // Lower octant 1
    {-0.12f, -0.12f, 0.12f},// Lower octant 2
    {0.12f, 0.12f, -0.12f}, // Upper octant 3
    {-0.12f, 0.12f, -0.12f},// Upper octant 4
    {0.12f, -0.12f, -0.12f},// Lower octant 3
    {-0.12f, -0.12f, -0.12f},// Lower octant 4
    {0.16f, 0.0f, 0.0f},    // Along x-axis
    {-0.16f, 0.0f, 0.0f},   // Along x-axis
    {0.0f, 0.16f, 0.0f},    // Along y-axis
    {0.0f, -0.16f, 0.0f},   // Along y-axis
    {0.0f, 0.0f, 0.16f},    // Along z-axis
    {0.0f, 0.0f, -0.16f}    // Along z-axis
};

//======================================================================================
// ELECTRON DATA STRUCTURE AND CONFIGURATION
//======================================================================================
// Structure to hold properties of each electron
struct Electron {
    float radius;    // Distance from nucleus (orbital radius)
    float angle;     // Current position in the orbital (in degrees)
    float speed;     // Rotation speed in degrees per frame
    float normal[3]; // Vector perpendicular to orbital plane (defines orientation)
    float color[3];  // RGB color values for the electron
};

// Array to store all electrons
std::vector<Electron> electrons;

//======================================================================================
// ELECTRON INITIALIZATION FUNCTION
//======================================================================================
void initElectrons(int atomicNumber) {
    // Seed the random number generator with current time
    srand(time(NULL));
    
    // Base radius for electron orbits - will be scaled for each shell
    const float baseRadius = 0.8f;
    
    // rgb for the shells
    const float shellColors[7][3] = {
        {0.2f, 0.4f, 1.0f}, // Shell 1: Blue
        {0.2f, 0.8f, 0.8f}, // Shell 2: Cyan
        {0.2f, 0.8f, 0.2f}, // Shell 3: Green
        {0.8f, 0.8f, 0.2f}, // Shell 4: Yellow
        {1.0f, 0.6f, 0.2f}, // Shell 5: Orange
        {1.0f, 0.2f, 0.2f}, // Shell 6: Red
        {0.8f, 0.2f, 0.8f}  // Shell 7: Magenta
    };
    
    int remaining = atomicNumber;  // Electrons left to distribute
    int shell = 1;                 // Current shell number
    
    // Clearing any existing electrons
    electrons.clear();
    
    while (remaining > 0 && shell <= 7) {  // Limit to 7 shells for visualization
        // Calculate maximum electrons for current shell using 2n^2 formula
        int max_e = 2 * shell * shell;
        
        // Determine how many electrons go into this shell
        int electronsInShell = std::min(max_e, remaining);
        
        // color for this shell
        int colorIndex = (shell - 1) % 7;
        float r = shellColors[colorIndex][0];  
        float g = shellColors[colorIndex][1]; 
        float b = shellColors[colorIndex][2];  
        
        // Creating electrons for this shell
        for(int i = 0; i < electronsInShell; i++) {
            Electron e;
            
            // Set radius based on shell number (grows quadratically)
            e.radius = baseRadius + (shell * shell * 0.4f);
            
            // Set orbital speed (electrons in outer shells move slower)
            e.speed = 45.0f / shell;
            
            // Calculate positioning angles for 3D distribution
            float angleStep = 180.0f / electronsInShell;
            float phi = i * angleStep;  // Vertical angle
            
            // Theta creates variation in horizontal angle based on shell number
            float theta = (i % shell) * (180.0f / shell) + (shell * 20.0f);
            
            // Initial angle in orbital path
            e.angle = (360.0f / electronsInShell) * i;
            
            // Calculate normal vector for orbital plane orientation
            e.normal[0] = sin(phi * M_PI/180) * cos(theta * M_PI/180);
            e.normal[1] = sin(phi * M_PI/180) * sin(theta * M_PI/180);
            e.normal[2] = cos(phi * M_PI/180);
            
            // Normalize the vector to ensure unit length
            float length = sqrt(e.normal[0]*e.normal[0] +
                               e.normal[1]*e.normal[1] +
                               e.normal[2]*e.normal[2]);
            e.normal[0] /= length;
            e.normal[1] /= length;
            e.normal[2] /= length;
            
            // Assign color from the palette
            e.color[0] = r;
            e.color[1] = g;
            e.color[2] = b;
            
            // Add the electron to our collection
            electrons.push_back(e);
        }
        
        // Update counters for next shell
        remaining -= electronsInShell;
        shell++;
    }
}

//======================================================================================
// FUNCTION TO DRAW A COLORED SPHERE (USED FOR NUCLEUS PARTICLES)
//======================================================================================
void drawSphere(float r, float g, float b) {
    // Set material properties for lighting calculations
    GLfloat matAmbient[] = {r*0.3, g*0.3, b*0.3, 1.0};  // Ambient light reflection (darker)
    GLfloat matDiffuse[] = {r, g, b, 1.0};              // Diffuse light reflection (full color)
    
    // Apply material properties to the front face of polygons
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    
    // Draw a sphere with radius 0.1, 16 longitudinal and 16 latitudinal subdivisions
    glutSolidSphere(0.1, 16, 16);
}

//======================================================================================
// FUNCTION TO DRAW AN ELECTRON
//======================================================================================
void drawElectron(const Electron &e) {
    // Save current transformation matrix
    glPushMatrix();
    
    // Calculate rotation angle to align with orbital plane
    // acos(e.normal[2]) gives angle between normal and z-axis in radians
    float rotAngle = acos(e.normal[2]) * 180.0/M_PI; // Convert to degrees
    
    // Rotate to align with orbital plane
    // The rotation axis (-e.normal[1], e.normal[0], 0) is perpendicular to both
    // the z-axis and the orbital normal, creating the correct orbital plane
    glRotatef(rotAngle, -e.normal[1], e.normal[0], 0.0f);
    
    // Position electron in its orbit
    glRotatef(e.angle, 0, 0, 1);           // Rotate to current angle around z-axis
    glTranslatef(e.radius, 0, 0);          // Move outward to orbital radius
    
    // Set electron material properties
    GLfloat matAmbient[] = {e.color[0]*0.3, e.color[1]*0.3, e.color[2]*0.3, 1.0};
    GLfloat matDiffuse[] = {e.color[0], e.color[1], e.color[2], 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    
    // Draw the electron as a small sphere
    glutSolidSphere(0.03, 8, 8); 
    
    // Restore previous transformation matrix
    glPopMatrix();
}

//======================================================================================
// FUNCTION TO DRAW ELECTRON ORBITAL PATH
//======================================================================================
void drawOrbit(const Electron &e) {
    // Save current transformation matrix
    glPushMatrix();
    
    // Rotate to orbital plane (same logic as in drawElectron)
    float rotAngle = acos(e.normal[2]) * 180.0/M_PI;
    glRotatef(rotAngle, -e.normal[1], e.normal[0], 0.0f);
    
    // Disable lighting for the orbit path (we want it to be unaffected by lighting)
    glDisable(GL_LIGHTING);
    
    // Set orbit color as a dimmer version of electron color
    glColor3f(e.color[0]*0.2f, e.color[1]*0.2f, e.color[2]*0.2f);
    
    // Draw orbit as a circle (line loop)
    glBegin(GL_LINE_LOOP);
    for(int i=0; i<60; i++) {  // 60 segments for smooth circle
        float angle = i * 2.0f * M_PI / 60;  // Angle in radians
        float x = e.radius * cos(angle);     // x-coordinate on circle
        float y = e.radius * sin(angle);     // y-coordinate on circle
        glVertex3f(x, y, 0.0f);              // Add point to line loop
    }
    glEnd();
    
    // Re-enable lighting if it was on
    if(lightingEnabled) glEnable(GL_LIGHTING);
    
    // Restore previous transformation matrix
    glPopMatrix();
}

//======================================================================================
// MAIN DISPLAY FUNCTION - CALLED TO RENDER EACH FRAME
//======================================================================================
void display() {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Work with modelview matrix (for 3D transformations)
    glMatrixMode(GL_MODELVIEW);
    
    // Reset modelview matrix to identity
    glLoadIdentity();
    
    // Set camera position and orientation
    gluLookAt(camX, camY, camZ,     // Eye position
              lookX, lookY, lookZ,  // Look-at point (target)
              0.0f, 1.0f, 0.0f);    // Up vector (world's "up" direction)

    // Draw nucleus particles
    glColor3f(0.8f, 0.3f, 0.2f);  // Reddish color for nucleus
    for(const auto &pos : nuclearPositions) {
        glPushMatrix();                            // Save transformation state
        glTranslatef(pos[0], pos[1], pos[2]);      // Move to particle position
        drawSphere(0.8f, 0.3f, 0.2f);              // Draw particle as colored sphere
        glPopMatrix();                             // Restore transformation state
    }
    
    // Draw electrons and their orbitals
    for(auto &e : electrons) {
        drawElectron(e);      // Draw the electron itself
        drawOrbit(e);         // Draw its orbital path
        
        // Update electron position for next frame (animation)
        e.angle += e.speed * 0.016f;  // 0.016 seconds = ~60 FPS
        if(e.angle > 360) e.angle -= 360;  // Keep angle in 0-360 range
    }
    
    // Swap back and front buffers (double buffering)
    glutSwapBuffers();
}

//======================================================================================
// WINDOW RESIZE HANDLER
//======================================================================================
void reshape(int w, int h) {
    // Update viewport to cover entire window
    glViewport(0, 0, w, h);
    
    // Switch to projection matrix to update perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set perspective projection with:
    // 45° field of view, aspect ratio based on window dimensions,
    // near clipping plane at 0.1, far clipping plane at 100.0
    gluPerspective(45.0, (float)w/h, 0.1, 100.0);
}

//======================================================================================
// KEYBOARD INPUT HANDLER
//======================================================================================
void keyboard(unsigned char key, int x, int y) {
    float moveSpeed = 0.1f;  // Movement speed per keypress
    
    // Calculate forward direction vector based on camera orientation
    float forwardX = lookX - camX;
    float forwardY = lookY - camY;
    float forwardZ = lookZ - camZ;
    
    // Normalize the forward vector (make it unit length)
    float len = sqrt(forwardX*forwardX + forwardY*forwardY + forwardZ*forwardZ);
    forwardX /= len; forwardY /= len; forwardZ /= len;
    
    // Calculate right direction vector (perpendicular to forward and world up)
    float rightX = forwardZ;  // Cross product of forward and up vectors
    float rightY = 0;         // simplified since up is (0,1,0)
    float rightZ = -forwardX;
    
    // Track mouse capture state
    static bool mouseCaptured = false;
    
    // Handle different key presses
    switch(key) {
        case 27: // ESC key - exit program
            exit(0);
            break;
        case 'w': // Move forward
            // Move both camera and look-at point in forward direction
            camX += forwardX * moveSpeed;
            camY += forwardY * moveSpeed;
            camZ += forwardZ * moveSpeed;
            lookX += forwardX * moveSpeed;
            lookY += forwardY * moveSpeed;
            lookZ += forwardZ * moveSpeed;
            break;
        case 's': // Move backward
            // Move both camera and look-at point in backward direction
            camX -= forwardX * moveSpeed;
            camY -= forwardY * moveSpeed;
            camZ -= forwardZ * moveSpeed;
            lookX -= forwardX * moveSpeed;
            lookY -= forwardY * moveSpeed;
            lookZ -= forwardZ * moveSpeed;
            break;
        case 'a': // Strafe left
            // Move both camera and look-at point to the left
            camX -= rightX * moveSpeed;
            camY -= rightY * moveSpeed;
            camZ -= rightZ * moveSpeed;
            lookX -= rightX * moveSpeed;
            lookY -= rightY * moveSpeed;
            lookZ -= rightZ * moveSpeed;
            break;
        case 'd': // Strafe right
            // Move both camera and look-at point to the right
            camX += rightX * moveSpeed;
            camY += rightY * moveSpeed;
            camZ += rightZ * moveSpeed;
            lookX += rightX * moveSpeed;
            lookY += rightY * moveSpeed;
            lookZ += rightZ * moveSpeed;
            break;
        case ' ': // Move up (space bar)
            camY += moveSpeed;
            lookY += moveSpeed;
            break;
        case 'c': // Move down
            camY -= moveSpeed;
            lookY -= moveSpeed;
            break;
        case 'l': // Toggle lighting
            lightingEnabled = !lightingEnabled;
            if(lightingEnabled) glEnable(GL_LIGHTING);
            else glDisable(GL_LIGHTING);
            break;
        case 'm': // Toggle mouse capture mode
            mouseCaptured = !mouseCaptured;
            if (mouseCaptured) {
                glutSetCursor(GLUT_CURSOR_NONE); // Hide cursor
            } else {
                glutSetCursor(GLUT_CURSOR_INHERIT); // Show cursor
            }
            break;
    }
    
    // Request a redraw with updated camera position
    glutPostRedisplay();
}

//======================================================================================
// MOUSE MOVEMENT HANDLER
//======================================================================================
void mouseMotion(int x, int y) {
    // Variables to track previous mouse position and state
    static int lastX = -1, lastY = -1;
    static bool firstMouse = true;  // Flag for first mouse movement
    static bool mouseCaptured = false;
    
    // Get window dimensions
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    
    // Skip the first mouse event to initialize last position
    if(firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
        return;
    }
    
    // Toggle mouse capture when Shift key is held
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
        if (!mouseCaptured) {
            mouseCaptured = true;
            glutSetCursor(GLUT_CURSOR_NONE);  // Hide cursor
        }
    } else {
        if (mouseCaptured) {
            mouseCaptured = false;
            glutSetCursor(GLUT_CURSOR_INHERIT);  // Show normal cursor
        }
    }
    
    // Calculate mouse movement since last position
    int deltaX = x - lastX;
    int deltaY = y - lastY;
    
    // Update camera angles based on mouse movement
    // Lower sensitivity makes camera control less jerky
    float sensitivity = 0.2f;
    angleY += deltaX * sensitivity;  // Horizontal rotation (yaw)
    angleX += deltaY * sensitivity;  // Vertical rotation (pitch)
    
    // Clamp vertical angle to prevent camera flipping
    if(angleX > 89.0f) angleX = 89.0f;
    if(angleX < -89.0f) angleX = -89.0f;
    
    // Calculate new look-at point based on angles and distance from camera
    float distance = sqrt(pow(lookX-camX, 2) + pow(lookY-camY, 2) + pow(lookZ-camZ, 2));
    
    // Convert spherical coordinates to Cartesian
    lookX = camX + cos(angleY * M_PI/180) * cos(angleX * M_PI/180) * distance;
    lookY = camY + sin(angleX * M_PI/180) * distance;
    lookZ = camZ + sin(angleY * M_PI/180) * cos(angleX * M_PI/180) * distance;
    
    // If in capture mode, reset cursor to center of window
    if (mouseCaptured) {
        lastX = centerX;
        lastY = centerY;
        glutWarpPointer(centerX, centerY);  // Move cursor to center
    } else {
        lastX = x;
        lastY = y;
    }
    
    // Request a redraw with updated camera view
    glutPostRedisplay();
}

//======================================================================================
// MOUSE WHEEL HANDLER
//======================================================================================
void mouseWheel(int button, int dir, int x, int y) {
    float zoomSpeed = 0.1f;  // Zoom speed factor
    
    // Calculate forward direction for zooming
    float forwardX = lookX - camX;
    float forwardY = lookY - camY;
    float forwardZ = lookZ - camZ;
    
    // Normalize the direction vector
    float len = sqrt(forwardX*forwardX + forwardY*forwardY + forwardZ*forwardZ);
    forwardX /= len; forwardY /= len; forwardZ /= len;
    
    if (dir > 0) { // Zoom in (wheel scrolled up)
        camX += forwardX * zoomSpeed;
        camY += forwardY * zoomSpeed;
        camZ += forwardZ * zoomSpeed;
    } else { // Zoom out (wheel scrolled down)
        camX -= forwardX * zoomSpeed;
        camY -= forwardY * zoomSpeed;
        camZ -= forwardZ * zoomSpeed;
    }
    
    // Request a redraw with updated camera position
    glutPostRedisplay();
}

//======================================================================================
// MOUSE BUTTON HANDLER
//======================================================================================
void mouseButton(int button, int state, int x, int y) {
    static bool mouseCaptured = false;
    
    if (button == 3 || button == 4) { 
        // Handle mouse wheel (3=up, 4=down on many systems)
        mouseWheel(button, button == 3 ? 1 : -1, x, y);
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Toggle mouse capture on left click
        mouseCaptured = !mouseCaptured;
        if (mouseCaptured) {
            glutSetCursor(GLUT_CURSOR_NONE);  // Hide cursor
            
            // Center mouse cursor
            int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
            int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
            glutWarpPointer(windowWidth/2, windowHeight/2);
        } else {
            glutSetCursor(GLUT_CURSOR_INHERIT);  // Show normal cursor
        }
    }
}

//======================================================================================
// OPENGL INITIALIZATION
//======================================================================================
void initGL() {
    // Set background color (dark grayish)
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    
    // Enable depth testing (handles object occlusion)
    glEnable(GL_DEPTH_TEST);
    
    // Set up lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  // Enable the first light source
    
    // Set light source position
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // Set light color (white)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, new GLfloat[4]{1,1,1,1});
    
    // Enable smooth shading (gradual color transition across polygons)
    glShadeModel(GL_SMOOTH);
}

//======================================================================================
// MAIN FUNCTION
//======================================================================================
int main(int argc, char** argv) {
    // Get atomic number from user
    do {
        std::cout << "Enter atomic number (1-118): ";
        std::cin >> atomicNumber;
        if(std::cin.fail() || atomicNumber < 1 || atomicNumber > 118) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number between 1 and 118.\n";
        }
        else break;
    } while(true);

    glutInit(&argc, argv);
    // Set up display mode:
    // - GLUT_DOUBLE: double buffering for smooth animation
    // - GLUT_RGB: color mode
    // - GLUT_DEPTH: enable depth buffer for 3D rendering
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1920, 1080);
    glutCreateWindow("Atomic Structure Visualizer");

    initGL();
    initElectrons(atomicNumber);

    glutDisplayFunc(display);           // Frame rendering
    glutReshapeFunc(reshape);           // Window resize handling
    glutKeyboardFunc(keyboard);         // Keyboard input
    glutMotionFunc(mouseMotion);        // Mouse movement with button pressed
    glutPassiveMotionFunc(mouseMotion); // Mouse movement without button press

    // Set up continuous animation (call display function whenever possible)
    glutIdleFunc([](){ glutPostRedisplay(); });

    glutMainLoop();
    return 0;
}