#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../headers/FileParser.h"
#include "../headers/Shader.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.inl>
#include "../headers/Heatmap.h"
#include "../headers/MeshPlane.h"
#include "../headers/Button.h"
#include <algorithm>
#include <chrono>
#include "../headers/ApplicationSettings.h"
#include "../headers/ModelContext.h"
#include "../headers/State.h"

using namespace std::chrono;
using namespace std;
using namespace deffem;
using namespace glm;


// GLFW Callback functions definitions
int maxNrOfVertexAttrsSupported();
void windowSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void rotateModel(Shader& shader);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void fileDropCallback(GLFWwindow* window, int count, const char** paths);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
ModelContext* loadModel(char const* filename);
void buttonsListener(fvec2 mousePos);

vec3 modelOriginDeviation(0.0f);

Button* repeatAnimationButton = nullptr;
Button* playAnimationButton = nullptr;
Button* scalePointUpButton = nullptr;
Button* scalePointDownButton = nullptr;
Button* lastAnimationStepButton = nullptr;
Button* firstAnimationStepButton = nullptr;
Button* decreaseAnimationSpeedButton = nullptr;
Button* increaseAnimationSpeedButton = nullptr;


Typer* typer = nullptr;
Shader* globalTextShader;


ApplicationSettings settings;
State* state = nullptr;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    auto conf = FileParser::readConfig("./resources/application.conf");
    settings = ApplicationSettings(conf);
    state = new State(&settings);


    // Window initialization
    GLFWwindow* window = glfwCreateWindow(settings.screenResolution.x, settings.screenResolution.y,
                                          "DEFFEM Postprocessing", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glViewport(0, 0, settings.screenResolution.x, settings.screenResolution.y);

    // Set OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(settings.lineWidth);
    glPointSize(settings.pointSize);
    // glEnable(GL_CULL_FACE);

    // Set callbacks
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetDropCallback(window, fileDropCallback);
    glfwSetKeyCallback(window, keyCallback);

    typer = new Typer();


    MeshPlane mesh(vec3(0.0f, 0.0f, 0.0f), fvec2(1.0f, 1.0f), settings.showMeshPlane ? 60 : 0, settings.meshPlaneColor);

    // Compile and setup the shaders
    Shader textShader("./shaders/text.vs", "./shaders/text.fs");
    Shader modelShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader meshShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader heatmapShader("./shaders/heatmap.vs", "./shaders/heatmap.fs");
    Shader buttonShader("./shaders/control.vs", "./shaders/control.fs");
    globalTextShader = new Shader("./shaders/text.vs", "./shaders/text.fs");


    textShader.use();
    textShader.setMat4("projection", state->HUDprojection);

    globalTextShader->use();
    globalTextShader->setMat4("projection", state->HUDprojection);

    heatmapShader.use();
    heatmapShader.setMat4("projection", state->HUDprojection);


    repeatAnimationButton = new Button(fvec2(settings.screenResolution.x - 160,
                                             settings.screenResolution.y - 40),
                                       fvec2(30),
                                       "./resources/repeat_off.jpg");

    firstAnimationStepButton = new Button(fvec2(settings.screenResolution.x - 120,
                                                settings.screenResolution.y - 40),
                                          fvec2(30),
                                          "./resources/previous_ctrl.jpg");

    playAnimationButton = new Button(vec2(settings.screenResolution.x - 80,
                                          settings.screenResolution.y - 40),
                                     vec2(30), "./resources/play_ctrl.jpg");

    lastAnimationStepButton = new Button(vec2(settings.screenResolution.x - 40,
                                              settings.screenResolution.y - 40),
                                         vec2(30), "./resources/next_ctrl.jpg");


    scalePointDownButton = new Button(vec2(settings.screenResolution.x - 80,
                                           settings.screenResolution.y - 80),
                                      vec2(30),
                                      "./resources/scale_point_down.jpg");

    scalePointUpButton = new Button(vec2(settings.screenResolution.x - 40,
                                         settings.screenResolution.y - 80),
                                    vec2(30),
                                    "./resources/scale_point_up.jpg");

    decreaseAnimationSpeedButton = new Button(vec2(settings.screenResolution.x - 80,
                                                   settings.screenResolution.y - 120),
                                              vec2(30),
                                              "./resources/speed_down.jpg");

    increaseAnimationSpeedButton = new Button(vec2(settings.screenResolution.x - 40,
                                                   settings.screenResolution.y - 120),
                                              vec2(30),
                                              "./resources/speed_up.jpg");


    repeatAnimationButton->setProjection(&state->HUDprojection);
    firstAnimationStepButton->setProjection(&state->HUDprojection);
    playAnimationButton->setProjection(&state->HUDprojection);
    lastAnimationStepButton->setProjection(&state->HUDprojection);
    scalePointUpButton->setProjection(&state->HUDprojection);
    scalePointDownButton->setProjection(&state->HUDprojection);
    decreaseAnimationSpeedButton->setProjection(&state->HUDprojection);
    increaseAnimationSpeedButton->setProjection(&state->HUDprojection);


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        // Render background color
        glClearColor(settings.backgroundColor.red, settings.backgroundColor.green, settings.backgroundColor.blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto ticked = state->animationTick();

        if (ticked && !state->animation.play)
            playAnimationButton->changeTexture("./resources/play_ctrl.jpg");


        // DEFFEM Model transformation

        const auto camera = state->camera;
        auto projection = perspective(radians(camera.fov), state->screenSize.x / state->screenSize.y, 0.1f, 100.0f);
        const auto eye = vec3(camera.target.x + camera.radius * sin(camera.phi) * cos(camera.theta),
                              camera.target.y + camera.radius * cos(camera.phi),
                              camera.target.z + camera.radius * sin(camera.phi) * sin(camera.theta)
        );
        const auto view = lookAt(fvec3(eye.x, eye.y, eye.z),
                                 fvec3(camera.target.x, camera.target.y, camera.target.z),
                                 fvec3(0.0f, 1.0f, 0.0f));

        auto scaledModel = scale(fmat4(1.0f), fvec3(state->modelScale));
        const auto centeredModel = translate(scaledModel, modelOriginDeviation);
        const auto translatedProjection = translate(projection, fvec3(camera.offset.x, -camera.offset.y, 0.0f));


        // Pass transformation matrices to the shader
        modelShader.use();
        modelShader.setMat4("model", centeredModel);
        modelShader.setMat4("projection", translatedProjection);
        modelShader.setMat4("view", view);


        // Mesh transformation
        meshShader.use();
        meshShader.setMat4("model", mat4(1.0f));
        meshShader.setMat4("projection", translatedProjection);
        meshShader.setMat4("view", view);


        // Draw DEFFEM model
        const auto deffemModelCtx = state->currentModelContext();
        if (deffemModelCtx)
        {
            deffemModelCtx->model->draw(&modelShader);
        }


        // Draw mesh    
        mesh.draw(&meshShader);


        // Draw heatmap
        if (state->heatmap)
        {
            state->heatmap->draw(&heatmapShader, &textShader, typer);
        }


        // Draw buttons
        repeatAnimationButton->draw(&buttonShader);
        firstAnimationStepButton->draw(&buttonShader);
        playAnimationButton->draw(&buttonShader);
        lastAnimationStepButton->draw(&buttonShader);
        scalePointUpButton->draw(&buttonShader);
        scalePointDownButton->draw(&buttonShader);
        decreaseAnimationSpeedButton->draw(&buttonShader);
        increaseAnimationSpeedButton->draw(&buttonShader);


        // Display state info 
        state->displayStateInfoText(globalTextShader, typer);


        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    glfwTerminate();
    delete typer;
    delete globalTextShader;

    return 0;
}

ModelContext* loadModel(const string& filename)
{
    CustomObject* model = nullptr;

    vector<GLfloat> vertices;
    vector<GLuint> indices;
    const auto attributeSizes = vector<GLuint>{3, 3};

    const auto modelInfo = FileParser::readSections(filename, vertices, indices);

    modelOriginDeviation.x = -vertices[0];
    modelOriginDeviation.y = -vertices[1];
    modelOriginDeviation.z = -vertices[2];


    if (indices.empty())
    {
        model = new CustomObject(vertices, attributeSizes, 6);
    }
    else
    {
        model = new CustomObject(vertices, indices, attributeSizes, 6);
    }

    cout << "[INFO]\tFile \"" << filename << "\" was read successfully" << endl;
    cout << "[INFO]\tNode count: " << modelInfo.nodeCount << endl;
    cout << "[INFO]\tElement count: " << modelInfo.elementCount << endl;
    cout << "[INFO]\tNumber of vector components: " << vertices.size() << endl;
    cout << "[INFO]\tNumber of indices: " << indices.size() << endl;
    cout << "[INFO]\tMinimum value: " << modelInfo.minMaxValue.min << endl;
    cout << "[INFO]\tMaximum value: " << modelInfo.minMaxValue.max << endl;


    return new ModelContext(model, modelInfo, filename);
}

void fileDropCallback(GLFWwindow* window, const int count, const char** paths)
{
    std::vector<std::string> v(paths, paths + count);
    std::sort(v.begin(), v.end());

    state->clearModels();

    for (const auto& path : v)
    {
        auto model = loadModel(path);
        state->deffemModelContexts.push_back(model);
    }

    state->refresh();
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Target camera up/down and left/right on the x axis
    if (key == GLFW_KEY_LEFT)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            state->animation.stop();
            state->previousModel();
        }
    }
    else if (key == GLFW_KEY_RIGHT)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            state->animation.stop();
            state->nextModel();
        }
    }
    else if (key == GLFW_KEY_SPACE)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            cout << "[EVENT] Play/Stop animation" << endl;
            if (state->animation.isFinished)
            {
                state->restartAnimation();
            }
            else
            {
                state->animation.play = !state->animation.play;
            }


            if (state->animation.play)
            {
                playAnimationButton->changeTexture("./resources/stop_ctrl.jpg");
            }
            else
            {
                playAnimationButton->changeTexture("./resources/play_ctrl.jpg");
            }
        }
    }
    else if (key == GLFW_KEY_R)
    {
        if (action == GLFW_PRESS)
        {
            cout << "[EVENT] Reset settings" << endl;
            settings.resetAll();
            state->modelScale = settings.initialModelScale;
            state->animation.tickMillis = milliseconds(settings.animationTickMillis);
            glPointSize(settings.pointSize);
        }
    }
}


void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1)
    {
        if (action == GLFW_PRESS)
        {
            if (mods == GLFW_MOD_CONTROL)
            {
                state->mouse.pressedWith(GLFW_MOD_CONTROL);
            }
            else if (mods == GLFW_MOD_SHIFT)
            {
                state->mouse.pressedWith(GLFW_MOD_SHIFT);
            }
            else
            {
                state->mouse.pressed();
                const auto mousePos = state->mouse.position;
                buttonsListener(fvec2(mousePos.x, state->screenSize.y - mousePos.y));
            }
        }
        else if (action == GLFW_RELEASE)
        {
            state->mouse.released();
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_2)
    {
        if (action == GLFW_PRESS)
        {
            state->mouse.pressedWith(GLFW_MOD_CONTROL);
        }
        else if (action == GLFW_RELEASE)
        {
            state->mouse.released();
        }
    }
}

void buttonsListener(const fvec2 mousePos)
{
    if (playAnimationButton->isClicked(mousePos))
    {
        cout << "[EVENT] Play/Stop animation" << endl;
        if (state->animation.isFinished)
        {
            state->restartAnimation();
        }
        else
        {
            state->animation.play = !state->animation.play;
        }

        if (state->animation.play)
        {
            playAnimationButton->changeTexture("./resources/stop_ctrl.jpg");
        }
        else
        {
            playAnimationButton->changeTexture("./resources/play_ctrl.jpg");
        }
    }
    else if (repeatAnimationButton->isClicked(mousePos))
    {
        state->animation.repeat = !state->animation.repeat;
        cout << "[EVENT] Repeat animation: " << state->animation.repeat << endl;
        if (state->animation.repeat)
        {
            repeatAnimationButton->changeTexture("./resources/repeat_on.jpg");
        } else
        {
            repeatAnimationButton->changeTexture("./resources/repeat_off.jpg");
        }
    }
    else if (firstAnimationStepButton->isClicked(mousePos))
    {
        cout << "[EVENT] First step" << endl;
        state->changeModel(0);
    }
    else if (lastAnimationStepButton->isClicked(mousePos))
    {
        cout << "[EVENT] Last step" << endl;
        state->changeModel(state->deffemModelContexts.size() - 1);
    }
    else if (scalePointUpButton->isClicked(mousePos))
    {
        glPointSize(settings.pointSize += 1.0);
        cout << "[EVENT] Point size: " << settings.pointSize << endl;
    }
    else if (scalePointDownButton->isClicked(mousePos))
    {
        if (settings.pointSize > 1)
        {
            glPointSize(settings.pointSize -= 1.0);
        }
        cout << "[EVENT] Point size: " << settings.pointSize << endl;
    }
    else if (decreaseAnimationSpeedButton->isClicked(mousePos))
    {
        state->animation.tickMillis += milliseconds(5);
        cout << "[EVENT] Animation tick: " << state->animation.tickMillis.count() << "ms" << endl;
    }
    else if (increaseAnimationSpeedButton->isClicked(mousePos) && state->animation.tickMillis.count() >= 5)
    {
        state->animation.tickMillis -= milliseconds(5);
        cout << "[EVENT] Animation tick: " << state->animation.tickMillis.count() << "ms" << endl;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    const auto mousePos = state->mouse.position;

    if (state->mouse.isPressedWithControl)
    {
        state->camera.offset.x += (xpos - mousePos.x) * 0.0002f;
        state->camera.offset.y += (ypos - mousePos.y) * 0.0002f;
    }
    else if (state->mouse.isPressedWithShift)
    {
        const auto sy = (ypos - mousePos.y) * 0.0005f;
        state->modelScale -= sy;
    }
    else if (state->mouse.isPressed)
    {
        state->camera.theta += (xpos - mousePos.x) * 0.01f;
        state->camera.phi += (ypos - mousePos.y) * 0.01f;

        if (state->camera.phi > 3.139f) state->camera.phi = 3.139f;
        if (state->camera.phi < 0.01f) state->camera.phi = 0.01f;
    }

    state->mouse.position.x = xpos;
    state->mouse.position.y = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (state->camera.fov >= 1.0f && state->camera.fov <= 45.0f)
        state->camera.fov -= yoffset;
    if (state->camera.fov <= 1.0f)
        state->camera.fov = 1.0f;
    if (state->camera.fov >= 45.0f)
        state->camera.fov = 45.0f;
}


void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    const auto screenSize = fvec2(width, height);
    state->screenSize = screenSize;

    state->HUDprojection = ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                                 static_cast<GLfloat>(height));

    if (state->heatmap)
    {
        state->heatmap->setPosition(fvec2(25, height / 2));
    }

    globalTextShader->setMat4("projection", state->HUDprojection);

    repeatAnimationButton->setPosition(fvec2(width - 160.0f, height - 40.0f));
    firstAnimationStepButton->setPosition(fvec2(width - 120.0f, height - 40.0f));
    playAnimationButton->setPosition(fvec2(width - 80.0f, height - 40.0f));
    lastAnimationStepButton->setPosition(fvec2(width - 40.0f, height - 40.0f));

    scalePointDownButton->setPosition(fvec2(width - 80.0f, height - 80.0f));
    scalePointUpButton->setPosition(fvec2(width - 40.0f, height - 80.0f));

    decreaseAnimationSpeedButton->setPosition(fvec2(width - 80.0f, height - 120.0f));
    increaseAnimationSpeedButton->setPosition(fvec2(width - 40.0f, height - 120.0f));
}


int maxNrOfVertexAttrsSupported()
{
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    return nrAttributes;
}
