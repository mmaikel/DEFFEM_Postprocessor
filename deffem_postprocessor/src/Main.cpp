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

vec3 modelOriginOffset(0.0f);
mat4 textProjection;

Button* playButton = nullptr;
Button* scaleUpButton = nullptr;
Button* scaleDownButton = nullptr;
Button* lastStepButton = nullptr;
Button* firstStepButton = nullptr;

Typer* typer = nullptr;


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

    auto meshGap = 60;
    if (!settings.showMeshPlane)
    {
        meshGap = 0;
    }

    MeshPlane mesh(vec3(0.0f, 0.0f, 0.0f), fvec2(1.0f, 1.0f), meshGap, settings.meshPlaneColor);

    // Compile and setup the shaders
    Shader textShader("./shaders/text.vs", "./shaders/text.fs");
    Shader modelShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader meshShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader heatmapShader("./shaders/heatmap.vs", "./shaders/heatmap.fs");
    Shader buttonShader("./shaders/control.vs", "./shaders/control.fs");


    textProjection = ortho(0.0f, static_cast<GLfloat>(settings.screenResolution.x), 0.0f,
                           static_cast<GLfloat>(settings.screenResolution.y));

    textShader.use();
    textShader.setMat4("projection", textProjection);

    heatmapShader.use();
    heatmapShader.setMat4("projection", textProjection);


    firstStepButton = new Button(fvec2(settings.screenResolution.x - 120,
                                       settings.screenResolution.y - 40),
                                 fvec2(30),
                                 "./resources/previous_ctrl.jpg");

    playButton = new Button(vec2(settings.screenResolution.x - 80,
                                 settings.screenResolution.y - 40),
                            vec2(30), "./resources/play_ctrl.jpg");

    lastStepButton = new Button(vec2(settings.screenResolution.x - 40,
                                     settings.screenResolution.y - 40),
                                vec2(30), "./resources/next_ctrl.jpg");

    scaleUpButton = new Button(vec2(settings.screenResolution.x - 40,
                                    settings.screenResolution.y - 80),
                               vec2(30),
                               "./resources/scale_point_up.jpg");

    scaleDownButton = new Button(vec2(settings.screenResolution.x - 80,
                                      settings.screenResolution.y - 80),
                                 vec2(30),
                                 "./resources/scale_point_down.jpg");

    firstStepButton->setViewport(settings.screenResolution);
    playButton->setViewport(settings.screenResolution);
    lastStepButton->setViewport(settings.screenResolution);
    scaleUpButton->setViewport(settings.screenResolution);
    scaleDownButton->setViewport(settings.screenResolution);


    const milliseconds animationTick(settings.animationTickMillis);

    auto nextAnimationDeadline = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ) + animationTick;


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        // Render background color
        glClearColor(settings.backgroundColor.red, settings.backgroundColor.green, settings.backgroundColor.blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        state->animationTick();

        if (!state->animation.play)
            playButton->changeTexture("./resources/play_ctrl.jpg");


        // DEFFEM Model transformation
        modelShader.use();

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
        const auto centeredModel = translate(scaledModel, modelOriginOffset);
        const auto translatedProjection = translate(projection, fvec3(camera.offset.x, -camera.offset.y, 0.0f));


        // Pass transformation matrices to the shader
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
        firstStepButton->draw(&buttonShader);
        playButton->draw(&buttonShader);
        lastStepButton->draw(&buttonShader);
        scaleUpButton->draw(&buttonShader);
        scaleDownButton->draw(&buttonShader);


        // Display state info 
        state->displayStateInfoText(&textShader, typer);


        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    glfwTerminate();

    return 0;
}

ModelContext* loadModel(const string& filename)
{
    CustomObject* model = nullptr;

    vector<float> vertices;
    vector<unsigned int> indices;
    vector<unsigned int> attributeSizes;
    attributeSizes.push_back(3);
    attributeSizes.push_back(3);

    const auto modelInfo = FileParser::readSections(filename, vertices, indices);

    modelOriginOffset.x = -vertices[0];
    modelOriginOffset.y = -vertices[1];
    modelOriginOffset.z = -vertices[2];


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
                playButton->changeTexture("./resources/stop_ctrl.jpg");
            }
            else
            {
                playButton->changeTexture("./resources/play_ctrl.jpg");
            }
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
}

void buttonsListener(const fvec2 mousePos)
{
    if (playButton->isClicked(mousePos))
    {
        cout << "[EVENT] Play animation" << endl;
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
            playButton->changeTexture("./resources/stop_ctrl.jpg");
        }
        else
        {
            playButton->changeTexture("./resources/play_ctrl.jpg");
        }
    }
    else if (firstStepButton->isClicked(mousePos))
    {
        cout << "[EVENT] First step" << endl;
        state->currentModelIndex = 0;
    }
    else if (lastStepButton->isClicked(mousePos))
    {
        cout << "[EVENT] Last step" << endl;
        state->currentModelIndex = state->deffemModelContexts.size() - 1;
    }
    else if (scaleUpButton->isClicked(mousePos))
    {
        glPointSize(settings.pointSize += 1.0);
        cout << "[EVENT] Point size: " << settings.pointSize << endl;
    }
    else if (scaleDownButton->isClicked(mousePos))
    {
        if (settings.pointSize > 1)
        {
            glPointSize(settings.pointSize -= 1.0);
        }
        cout << "[EVENT] Point size: " << settings.pointSize << endl;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    const auto camera = state->camera;
    const auto mousePos = state->mouse.position;

    if (state->mouse.isPressedWithControl)
    {
        state->camera.offset.x += (xpos - mousePos.x) * 0.0002f;
        state->camera.offset.y += (ypos - mousePos.y) * 0.0002f;
    }
    else if (state->mouse.isPressedWithShift)
    {
        auto sy = (ypos - mousePos.y) * 0.0005f;
        state->modelScale -= sy;
    }
    else if (state->mouse.isPressed)
    {
        state->camera.theta += (xpos - mousePos.x) * 0.01f;
        state->camera.phi += (ypos - mousePos.y) * 0.01f;

        if (camera.phi > 3.139f) state->camera.phi = 3.139f;
        if (camera.phi < 0.01f) state->camera.phi = 0.01f;
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
    const auto screenSize = fvec2(width, height);
    state->screenSize = screenSize;

    glViewport(0, 0, width, height);

    textProjection = ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                           static_cast<GLfloat>(height));


    firstStepButton->setViewport(screenSize);
    playButton->setViewport(screenSize);
    lastStepButton->setViewport(screenSize);

    scaleUpButton->setViewport(screenSize);
    scaleDownButton->setViewport(screenSize);

    firstStepButton->setPosition(vec2(width - 120.0, height - 40.0));
    playButton->setPosition(vec2(width - 80.0, height - 40.0));
    lastStepButton->setPosition(vec2(width - 40.0, height - 40.0));

    scaleUpButton->setPosition(vec2(width - 80.0, height - 80.0));
    scaleDownButton->setPosition(vec2(width - 40.0, height - 80.0));
}


int maxNrOfVertexAttrsSupported()
{
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    return nrAttributes;
}
