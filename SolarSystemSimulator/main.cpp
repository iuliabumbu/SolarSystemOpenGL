#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>
#include "SkyBox.hpp"
#include <random>

// window
gps::Window myWindow;

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
GLfloat yaw = 260.0f;
GLfloat pitch = 0.0f;
const GLfloat SENSITIVTY = 0.01f;
float fov = 45.0f;

bool wireframe = false;
bool smooth = true;
bool points = false;


// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 modelSun;
glm::mat4 modelMercury;
glm::mat4 modelVenus;
glm::mat4 modelEarth;
glm::mat4 modelMars;
glm::mat4 modelJupiter;
glm::mat4 modelSaturn;
glm::mat4 modelUranus;
glm::mat4 modelNeptune;
glm::mat4 modelMoon;    //Earth
glm::mat4 modelPhobos;  //Mars
glm::mat4 modelDeimos;  //Mars
glm::mat4 modelEuropa; //Jupiter
glm::mat4 modelIo; //Jupiter
glm::mat4 modelGanymede; //Jupiter
glm::mat4 modelCallisto; //Jupiter
glm::mat4 modelTitan; //Saturn
glm::mat4 modelTitania; //Uranus
glm::mat4 modelTriton; //Neptune
glm::mat4 modelStars;
glm::mat4 modelSpaceship;
glm::mat4 modelSpaceship2;
glm::mat4 modelSatelit;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightPos;
glm::vec3 viewPos;
glm::vec3 lightColor;
glm::mat4 lightRotation;
GLfloat lightAngle;

// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightPosLoc;
GLuint lightColorLoc;
GLuint viewPosLoc;
GLuint modelStarLoc;

//shadow
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

GLuint shadowMapFBO;
GLuint depthMapTexture;

glm::mat4 lightView;
const GLfloat near_plane = 0.1f, far_plane = 100.0f;
glm::mat4 lightProjection;
glm::mat4 lightSpaceTrMatrix;

gps::Shader depthMapShader;
gps::Shader lightShader;

bool showDepthMap;

// camera
gps::Camera myCamera(
    glm::vec3(45.0f, 5.0f, 250.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.5f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
GLfloat angle;

gps::Model3D sun;
gps::Model3D mercury;
gps::Model3D venus;
gps::Model3D earth;
gps::Model3D mars;
gps::Model3D jupiter;
gps::Model3D saturn;
gps::Model3D uranus;
gps::Model3D neptune;
gps::Model3D moon;
gps::Model3D phobos;
gps::Model3D deimos;
gps::Model3D europa;
gps::Model3D io;
gps::Model3D ganymede;
gps::Model3D callisto;
gps::Model3D titan;
gps::Model3D titania;
gps::Model3D triton;
gps::Model3D lightCube;
gps::Model3D spaceship;
gps::Model3D spaceship2;
gps::Model3D satelit;

// shaders
gps::Shader myBasicShader;
gps::Shader star;

//skybox
std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;


bool move = false;
bool moons = false;
bool ship = false;
bool spaceObj = false;
bool cameraAnim = false;
bool dir = true;
GLfloat speed = 5.0f;
GLfloat radius, x, y;
GLuint i = 0;
float coordX, coordY;
GLfloat scale = 0.1f;
const double PI = 3.141592653589793238463;

struct WindowDimensions dim;
bool lightPoint = false;
bool shadow;
float lightSourcePoint = 0.0f;
float shadowEnable;



GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    dim = { width, height };
    myWindow.setWindowDimensions(dim);
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 600.0f);
    myBasicShader.useShaderProgram();
    //   glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    lightShader.useShaderProgram();
    // glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    star.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        move = !move;

    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        moons = !moons;

    if (key == GLFW_KEY_N && action == GLFW_PRESS)
        spaceObj = !spaceObj;

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        cameraAnim = !cameraAnim;


    if (key == GLFW_KEY_P && action == GLFW_PRESS) {

        points = !points;

        if (points) {
            std::cout << "Points mode on \n";
        }
        else {
            std::cout << "Solid mode on \n";
        }
    }

    if (key == GLFW_KEY_I && action == GLFW_PRESS) {

        wireframe = !wireframe;

        if (wireframe) {
            std::cout << "Wireframe mode on \n";
        }
        else {
            std::cout << "Wireframe mode off\n";
        }
    }


    if (key == GLFW_KEY_O && action == GLFW_PRESS) {

        smooth = !smooth;

        if (smooth) {
            std::cout << "Smooth mode on \n";
        }
        else {
            std::cout << "Smooth mode off \n";
        }
    }


    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {

        lightPoint = !lightPoint;
        if (lightPoint) {
            lightSourcePoint = 0.0f;
            std::cout << "Point light! \n";
        }
        else {
            lightSourcePoint = 1.0f;
            std::cout << "Directional light! \n";
        }

    }

    if (key == GLFW_KEY_U && action == GLFW_PRESS) {

        shadow = !shadow;
        if (shadow && lightSourcePoint == 1.0) {
            shadowEnable = 0.0f;
            std::cout << "Shadow on! \n";
        }
        else {
            shadowEnable = 1.0f;
            std::cout << "Shadow off! \n";
        }

    }

    if (key == GLFW_KEY_Z) {
        speed = speed + 0.1f;
        std::cout << "Speed increased : " << speed << std::endl;
    }
    else if (key == GLFW_KEY_X) {
        speed = speed - 0.1f;
        std::cout << "Speed decreased : " << speed << std::endl;
    }


    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
  /*  if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xOffset = xpos - lastX;
    GLfloat yOffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    yaw += xOffset;
    pitch += yOffset;

    myCamera.rotate(yaw, pitch);

    //update view matrix
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    star.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));*/
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << " Cursor Position at " << xpos << " : " << ypos << "\n";

        /*   glm::mat4 transform = glm::inverse(glm::mat3(projection * view));

           //normalized device space
           float x = (2.0f * ((float)xpos)) / (myWindow.getWindowDimensions().width - 1.0f);
           float y = (2.0f * ((float)ypos)) / (myWindow.getWindowDimensions().height - 1.0f);

           // homogenous space
           glm::vec4 screenPos = glm::vec4(x, -y, -1.0f, 1.0f);

           // Eye Space
           glm::vec4 worldPos = transform * screenPos;

           std::cout << " World Position at " << worldPos.x << " : " << worldPos.y << "\n";

           coordX = worldPos.x ;
           coordY = worldPos.y;*/


        coordX = xpos / 10;
        coordY = ypos / 10;

        std::cout << " Coords Position at " << coordX << " : " << coordY << "\n";
        ship = !ship;
    }
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;

    myBasicShader.useShaderProgram();
    projection = glm::perspective(glm::radians(fov),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 600.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    star.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 0.5f;
    }

    if (pressedKeys[GLFW_KEY_L]) {
        lightAngle += 0.5f;
    }

    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    }
    if (pressedKeys[GLFW_KEY_UP]) {
        pitch += 1;
        myCamera.rotate(pitch, yaw);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    }
    if (pressedKeys[GLFW_KEY_DOWN]) {
        pitch -= 1;
        myCamera.rotate(pitch, yaw);

        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    }
    if (pressedKeys[GLFW_KEY_RIGHT]) {
        yaw += 1;
        myCamera.rotate(pitch, yaw);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    }
    if (pressedKeys[GLFW_KEY_LEFT]) {
        yaw -= 1;
        myCamera.rotate(pitch, yaw);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    }

}

void initOpenGLWindow() {
    myWindow.Create(1200, 600, "OpenGL Project Core");

}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetMouseButtonCallback(myWindow.getWindow(), mouse_button_callback);
    glfwSetScrollCallback(myWindow.getWindow(), scroll_callback);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    sun.LoadModel("models/sun2/sun.obj");
    mercury.LoadModel("models/mercury/mercury.obj");
    venus.LoadModel("models/venus/venus.obj");
    earth.LoadModel("models/earth/Earth.obj");
    mars.LoadModel("models/mars/Mars.obj");
    jupiter.LoadModel("models/jupiter/jupiter.obj");
    saturn.LoadModel("models/saturn/saturn1.obj");
    uranus.LoadModel("models/uranus2/uranus.obj");
    neptune.LoadModel("models/neptune2/neptune.obj");
    lightCube.LoadModel("models/cube/cube.obj");
    moon.LoadModel("models/moon/moon.obj");
    deimos.LoadModel("models/deimos/deimos.obj");
    phobos.LoadModel("models/phobos/phobos.obj");
    europa.LoadModel("models/europa/europa.obj");
    io.LoadModel("models/io/io.obj");
    ganymede.LoadModel("models/ganymede/ganymede.obj");
    callisto.LoadModel("models/callisto/callisto.obj");
    titan.LoadModel("models/titan/titan.obj");
    titania.LoadModel("models/titania/titania.obj");
    triton.LoadModel("models/triton/triton.obj");
    spaceship.LoadModel("models/spaceship/untitled.obj");
    spaceship2.LoadModel("models/spaceship2/neghvar.obj");
    satelit.LoadModel("models/satelit/10477_Satellite_v1_L3.obj");
}

void initShaders() {
    myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    myBasicShader.useShaderProgram();
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    depthMapShader.useShaderProgram();
    star.loadShader("shaders/stars.vert", "shaders/stars.frag");
    star.useShaderProgram();
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "shadowEnable"), shadowEnable);

    // create model matrix 
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    // model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // create projection matrix
    projection = glm::perspective(glm::radians(fov),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 600.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 0.0f, 190.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));


    lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
    lightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");

    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    //pt soare:
    star.useShaderProgram();

    // send projection matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // send view matrix to shader
    glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


}
void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix

    lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightProjection = glm::ortho(-200.0f, 200.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

void cameraAnimation() {
    if (cameraAnim) {

        if (yaw < 280 && dir == true) {
            yaw += 0.1f;
        }
        if (yaw >= 280) {
            dir = false;
        }
        if (dir == false) {
            yaw -= 0.1f;

            if (yaw < 260) {
                cameraAnim = false;
                dir = true;
            }
        }
        if (dir) {
            fov -= 0.1f;
        }
        else {
            fov += 0.1f;
        }

        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 45.0f)
            fov = 45.0f;

        myCamera.rotate(pitch, yaw);

        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        projection = glm::perspective(glm::radians(fov),
            (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
            0.1f, 600.0f);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        star.useShaderProgram();
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    }
}

void renderSpaceship(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "shadowEnable"), shadowEnable);

    modelSpaceship = glm::mat4(1.0f);
    modelSpaceship = glm::translate(modelSpaceship, glm::vec3(coordX, coordY, 40.0f));
    modelSpaceship = glm::scale(modelSpaceship, glm::vec3(2.0f, 2.0f, 2.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSpaceship));

    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelSpaceship));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    spaceship.Draw(shader);

}


void renderSun(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    modelSun = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));

    glUniformMatrix4fv(glGetUniformLocation(star.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSun));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelSun));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    sun.Draw(shader);
}

void renderSatelit(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelSatelit = glm::translate(model, glm::vec3(25.0f, 5.0f, 0.0f));
    modelSatelit = glm::scale(modelSatelit, glm::vec3(0.001f, 0.001f, 0.001f));
    modelSatelit = glm::rotate(modelSatelit, angle, glm::vec3(0.0f, 1.0f, 0.0f));


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSatelit));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelSatelit));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    satelit.Draw(shader);

}


void renderMercury(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    if (move) {
        angle = 0.0474f * i * speed;
        radius = 60.0f * scale;
        x = radius * sin(PI * 2 * angle / 360);
        y = radius * cos(PI * 2 * angle / 360);
        modelMercury = glm::translate(modelSun, glm::vec3(x, 0.0f, y));
    }
    else {
        modelMercury = glm::translate(modelSun, glm::vec3(60.0f * scale, 0.0f, 0.0f));
    }


    modelMercury = glm::scale(modelMercury, glm::vec3(0.12f, 0.12f, 0.12f));
    angle = 0.00043f * i;
    modelMercury = glm::rotate(modelMercury, angle, glm::vec3(0.0f, 0.1f, 0.0f));


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMercury));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelMercury));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    mercury.Draw(shader);
}
void renderVenus(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    if (move) {
        angle = 0.035f * i * speed;
        radius = 85.0f * scale;
        x = radius * sin(PI * 2 * angle / 360);
        y = radius * cos(PI * 2 * angle / 360);
        modelVenus = glm::translate(modelSun, glm::vec3(x, 0.0f, y));
    }
    else {
        modelVenus = glm::translate(modelSun, glm::vec3(85.0f * scale, 0.0f, 0.0f));
    }
    modelVenus = glm::scale(modelVenus, glm::vec3(0.2f, 0.2f, 0.2f));
    angle = -0.00104f * i;
    modelVenus = glm::rotate(modelVenus, angle, glm::vec3(0.0f, 0.1f, 0.0f));


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelVenus));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelVenus));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    venus.Draw(shader);
}

void renderEarth(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    if (move) {
        angle = 0.0298f * i * speed;
        radius = 110.0f * scale;
        x = radius * sin(PI * 2 * angle / 360);
        y = radius * cos(PI * 2 * angle / 360);
        modelEarth = glm::translate(modelSun, glm::vec3(x, 0.0f, y));
    }
    else {
        modelEarth = glm::translate(modelSun, glm::vec3(110.0f * scale, 0.0f, 0.0f));
    }

    modelEarth = glm::scale(modelEarth, glm::vec3(0.2f, 0.2f, 0.2f));
    angle = 0.00112f * i;
    modelEarth = glm::rotate(modelEarth, angle, glm::vec3(0.0f, 0.1f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelEarth));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelEarth));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    earth.Draw(shader);
}

void renderMoon(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelMoon = glm::translate(modelEarth, glm::vec3(50.0f * scale, 0.0f, 0.0f));
    modelMoon = glm::scale(modelMoon, glm::vec3(0.15f, 0.15f, 0.15f));


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMoon));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelMoon));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    moon.Draw(shader);
}

void renderSpaceship2(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "shadowEnable"), shadowEnable);

    modelSpaceship2 = glm::translate(modelEarth, glm::vec3(80.0f * scale, 20.0f, 0.0f));
    modelSpaceship2 = glm::scale(modelSpaceship2, glm::vec3(0.8f, 0.8f, 0.8f));
    modelSpaceship2 = glm::rotate(modelSpaceship2, angle + 5, glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSpaceship2));

    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelSpaceship2));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    spaceship2.Draw(shader);

}

void renderMars(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    if (move) {
        angle = 0.0241f * i * speed;
        radius = 130.0f * scale;
        x = radius * sin(PI * 2 * angle / 360);
        y = radius * cos(PI * 2 * angle / 360);
        modelMars = glm::translate(modelSun, glm::vec3(x, 0.0f, y));
    }
    else {
        modelMars = glm::translate(modelSun, glm::vec3(130.0f * scale, 0.0f, 0.0f));
    }
    modelMars = glm::scale(modelMars, glm::vec3(0.12f, 0.12f, 0.12f));
    angle = 0.0005f * i;
    modelMars = glm::rotate(modelMars, angle, glm::vec3(0.0f, 0.1f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMars));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelMars));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    mars.Draw(shader);
}
void renderDeimos(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelDeimos = glm::translate(modelMars, glm::vec3(50.0f * scale, 0.0f, 0.0f));
    modelDeimos = glm::scale(modelDeimos, glm::vec3(0.2f, 0.2f, 0.2f));


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelDeimos));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelDeimos));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    deimos.Draw(shader);
}

void renderPhobos(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelPhobos = glm::translate(modelMars, glm::vec3(65.0f * scale, 0.0f, 0.0f));
    modelPhobos = glm::scale(modelPhobos, glm::vec3(0.2f, 0.2f, 0.2f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPhobos));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelPhobos));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    phobos.Draw(shader);
}

void renderJupiter(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    if (move) {
        angle = 0.0131f * i * speed;
        radius = 175.0f * scale;
        x = radius * sin(PI * 2 * angle / 360);
        y = radius * cos(PI * 2 * angle / 360);
        modelJupiter = glm::translate(modelSun, glm::vec3(x, 0.0f, y));
    }
    else {
        modelJupiter = glm::translate(modelSun, glm::vec3(175.0f * scale, 0.0f, 0.0f));
    }

    modelJupiter = glm::scale(modelJupiter, glm::vec3(0.6f, 0.6f, 0.6f));
    angle = 0.00595f * i;
    modelJupiter = glm::rotate(modelJupiter, angle, glm::vec3(0.0f, 0.1f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelJupiter));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelJupiter));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    jupiter.Draw(shader);
}

void renderEuropa(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelEuropa = glm::translate(modelJupiter, glm::vec3(30.0f * scale, 0.0f, 0.0f));
    modelEuropa = glm::scale(modelEuropa, glm::vec3(0.15f, 0.15f, 0.15f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelEuropa));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelEuropa));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    europa.Draw(shader);
}
void renderIo(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelIo = glm::translate(modelJupiter, glm::vec3(40.0f * scale, 0.0f, 0.0f));
    modelIo = glm::scale(modelIo, glm::vec3(0.1f, 0.1f, 0.1f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelIo));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelIo));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    io.Draw(shader);
}

void renderGanymede(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelGanymede = glm::translate(modelJupiter, glm::vec3(50.0f * scale, 0.0f, 0.0f));
    modelGanymede = glm::scale(modelGanymede, glm::vec3(0.17f, 0.17f, 0.17f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGanymede));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelGanymede));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    ganymede.Draw(shader);
}
void renderCallisto(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelCallisto = glm::translate(modelJupiter, glm::vec3(60.0f * scale, 0.0f, 0.0f));
    modelCallisto = glm::scale(modelCallisto, glm::vec3(0.18f, 0.18f, 0.18f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCallisto));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelCallisto));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    callisto.Draw(shader);
}

void renderSaturn(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    if (move) {
        angle = 0.0097f * i * speed;
        radius = 240.0f * scale;
        x = radius * sin(PI * 2 * angle / 360);
        y = radius * cos(PI * 2 * angle / 360);
        modelSaturn = glm::translate(modelSun, glm::vec3(x, 0.0f, y));
    }
    else {
        modelSaturn = glm::translate(modelSun, glm::vec3(240.0f * scale, 0.0f, 0.0f));
    }

    modelSaturn = glm::scale(modelSaturn, glm::vec3(0.5f, 0.5f, 0.5f));
    angle = 0.00355f * i;
    modelSaturn = glm::rotate(modelSaturn, angle, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSaturn));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelSaturn));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    saturn.Draw(shader);
}

void renderTitan(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelTitan = glm::translate(modelSaturn, glm::vec3(25.0f * scale, 0.0f, 0.0f));
    modelTitan = glm::scale(modelTitan, glm::vec3(0.1f, 0.1f, 0.1f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTitan));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelTitan));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    titan.Draw(shader);
}
void renderUranus(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    model = glm::mat4(1);
    if (move) {
        angle = 0.0068f * i * speed;
        radius = 275.0f * scale;
        x = radius * sin(PI * 2 * angle / 360);
        y = radius * cos(PI * 2 * angle / 360);
        modelUranus = glm::translate(modelSun, glm::vec3(x, 0.0f, y));
    }
    else {
        modelUranus = glm::translate(modelSun, glm::vec3(275.0f * scale, 0.0f, 0.0f));
    }

    modelUranus = glm::scale(modelUranus, glm::vec3(0.4f, 0.4f, 0.4f));
    angle = 0.00213f * i;
    modelUranus = glm::rotate(modelUranus, angle, glm::vec3(0.0f, 0.1f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelUranus));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelUranus));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    uranus.Draw(shader);
}

void renderTitania(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelTitania = glm::translate(modelUranus, glm::vec3(30.0f * scale, 0.0f, 0.0f));
    modelTitania = glm::scale(modelTitania, glm::vec3(0.2f, 0.2f, 0.2f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTitania));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelTitania));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    titania.Draw(shader);
}

void renderNeptune(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    if (move) {
        angle = 0.0054f * i * speed;
        radius = 300.0f * scale;
        x = radius * sin(PI * 2 * angle / 360);
        y = radius * cos(PI * 2 * angle / 360);
        modelNeptune = glm::translate(modelSun, glm::vec3(x, 0.0f, y));
    }
    else {
        modelNeptune = glm::translate(modelSun, glm::vec3(300.0f * scale, 0.0f, 0.0f));
    }

    modelNeptune = glm::scale(modelNeptune, glm::vec3(0.4f, 0.4f, 0.4f));
    angle = 0.00235f * i;
    modelNeptune = glm::rotate(modelNeptune, angle, glm::vec3(0.0f, 0.1f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelNeptune));


    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelNeptune));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    neptune.Draw(shader);
}
void renderTriton(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "lightSourcePoint"), lightSourcePoint);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "shadowEnable"), shadowEnable);

    modelTriton = glm::translate(modelNeptune, glm::vec3(30.0f * scale, 0.0f, 0.0f));
    modelTriton = glm::scale(modelTriton, glm::vec3(0.1f, 0.1f, 0.1f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTriton));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelTriton));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    triton.Draw(shader);
}


void drawObjects(bool depthPass) {

    i++;

    //render the scene
    renderSun(star, depthPass);
    renderMercury(myBasicShader, depthPass);
    renderVenus(myBasicShader, depthPass);
    renderEarth(myBasicShader, depthPass);
    renderMars(myBasicShader, depthPass);
    renderJupiter(myBasicShader, depthPass);
    glDisable(GL_CULL_FACE); // cull face
    renderSaturn(myBasicShader, depthPass);
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    renderUranus(myBasicShader, depthPass);
    renderNeptune(myBasicShader, depthPass);

    if (moons) {
        renderMoon(myBasicShader, depthPass);
        renderDeimos(myBasicShader, depthPass);
        renderPhobos(myBasicShader, depthPass);
        renderEuropa(myBasicShader, depthPass);
        renderIo(myBasicShader, depthPass);
        renderGanymede(myBasicShader, depthPass);
        renderCallisto(myBasicShader, depthPass);
        renderTitan(myBasicShader, depthPass);
        renderTitania(myBasicShader, depthPass);
        renderTriton(myBasicShader, depthPass);

    }
    if (ship) {
        renderSpaceship(myBasicShader, depthPass);
    }

    if (spaceObj) {
        renderSpaceship2(myBasicShader, depthPass);
        renderSatelit(myBasicShader, depthPass);
    }





}

void renderScene() {

    //render the scene to the depth buffer
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    drawObjects(true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // final scene rendering pass (with shadows)

    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();

    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //bind the shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));

    drawObjects(false);

    //draw a white cube around the light

    lightShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // model = glm::mat4(1.0f);
    model = lightRotation;
    model = glm::translate(model, 1.0f * lightDir);
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    if (!lightPoint) {
        lightCube.Draw(lightShader);
    }

    mySkyBox.Draw(skyboxShader, view, projection);

}



void initSkyBox() {
    faces.push_back("skybox2/bkg1_left.png");
    faces.push_back("skybox2/bkg1_right.png");
    faces.push_back("skybox2/bkg1_top.png");
    faces.push_back("skybox2/bkg1_bot.png");
    faces.push_back("skybox2/bkg1_back.png");
    faces.push_back("skybox2/bkg1_front.png");

    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));



}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);

    //close GL context and any other GLFW resources
    glfwTerminate();
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    // initFBO();
    setWindowCallbacks();

    //initializam skyboxul
    initSkyBox();

    glCheckError();

    std::cout << "La rularea programului apasati tasta c pentru a porni animatia de vizualizare si implicit scena\n";

    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();


        if (wireframe) {
            //Turn on points mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          
        }
        else {
            if (points) {
                //Turn on points mode
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

        }


        if (smooth) {
            //Turn on smooth mode
            glShadeModel(GL_SMOOTH);
        }
        else {
            //Turn off smooth mode
            glShadeModel(GL_FLAT);
        }

        cameraAnimation();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        // glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
