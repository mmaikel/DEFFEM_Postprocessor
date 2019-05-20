#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../headers/FileParser.h"
#include "../headers/Shader.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.inl>
#include "../headers/Heatmap.h"
#include "../headers/GridPlane.h"
#include "../headers/Button.h"
#include <algorithm>
#include <chrono>
#include "../headers/ApplicationSettings.h"
#include "../headers/ModelContext.h"
#include "../headers/State.h"
#include "../headers/Rectangle.h"

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

vec3 MODEL_ORIGIN_DEVIATION(0.0f);

Button* REPEAT_ANIMATION_BUTTON = nullptr;
Button* PLAY_ANIMATION_BUTTON = nullptr;
Button* SCALE_POINT_UP_BUTTON = nullptr;
Button* SCALE_POINT_DOWN_BUTTON = nullptr;
Button* LAST_ANIMATION_STEP_BUTTON = nullptr;
Button* FIRST_ANIMATION_STEP_BUTTON = nullptr;
Button* DECREASE_ANIMATION_SPEED_BUTTON = nullptr;
Button* INCREASE_ANIMATION_SPEED_BUTTON = nullptr;
Button* SHOW_GRID_BUTTON = nullptr;


Typer* TYPER = nullptr;
Shader* GLOBAL_TEXT_SHADER = nullptr;
Shader* MODEL_SHADER = nullptr;
Shader* SPH_SHADER = nullptr;
Shader* MES_SHADER = nullptr;
GridPlane* GRID_PLANE;


ApplicationSettings SETTINGS;
State* STATE = nullptr;


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
    SETTINGS = ApplicationSettings(conf);
    STATE = new State(&SETTINGS);


    // Window initialization
    GLFWwindow* window = glfwCreateWindow(SETTINGS.screenResolution.x, SETTINGS.screenResolution.y,
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


    glViewport(0, 0, SETTINGS.screenResolution.x, SETTINGS.screenResolution.y);

    // Set OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(SETTINGS.lineWidth);
    glPointSize(SETTINGS.pointSize);
    glDisable(GL_CULL_FACE);

    // Set callbacks
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetDropCallback(window, fileDropCallback);
    glfwSetKeyCallback(window, keyCallback);

    TYPER = new Typer();
    GRID_PLANE = new GridPlane(fvec2(1.0, 1.0), 100, SETTINGS.gridPlaneColor);

    GRID_PLANE->showGrid = SETTINGS.showGridPlane;

    // Compile and setup the shaders
    Shader textShader("./shaders/text.vs", "./shaders/text.fs");
    Shader gridShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader heatmapShader("./shaders/heatmap.vs", "./shaders/heatmap.fs");
    Shader buttonShader("./shaders/control.vs", "./shaders/control.fs");
    GLOBAL_TEXT_SHADER = new Shader("./shaders/text.vs", "./shaders/text.fs");
    Shader rectShader("./shaders/axis_label.vs", "./shaders/axis_label.fs");

    SPH_SHADER = new Shader("./shaders/model_sph.vs", "./shaders/shader.fs");
    MES_SHADER = new Shader("./shaders/model_mes.vs", "./shaders/shader.fs");


    textShader.use();
    textShader.setMat4("projection", STATE->HUDprojection);

    GLOBAL_TEXT_SHADER->use();
    GLOBAL_TEXT_SHADER->setMat4("projection", STATE->HUDprojection);

    heatmapShader.use();
    heatmapShader.setMat4("projection", STATE->HUDprojection);


    REPEAT_ANIMATION_BUTTON = new Button(fvec2(SETTINGS.screenResolution.x - 160.0f,
                                               SETTINGS.screenResolution.y - 40.0f),
                                         fvec2(30.0f),
                                         "./resources/repeat_off.jpg");

    FIRST_ANIMATION_STEP_BUTTON = new Button(fvec2(SETTINGS.screenResolution.x - 120.0f,
                                                   SETTINGS.screenResolution.y - 40.0f),
                                             fvec2(30.0f),
                                             "./resources/previous_ctrl.jpg");

    PLAY_ANIMATION_BUTTON = new Button(vec2(SETTINGS.screenResolution.x - 80.0f,
                                            SETTINGS.screenResolution.y - 40.0f),
                                       vec2(30.0f), "./resources/play_ctrl.jpg");

    LAST_ANIMATION_STEP_BUTTON = new Button(vec2(SETTINGS.screenResolution.x - 40.0f,
                                                 SETTINGS.screenResolution.y - 40.0f),
                                            vec2(30.0f), "./resources/next_ctrl.jpg");


    SCALE_POINT_DOWN_BUTTON = new Button(vec2(SETTINGS.screenResolution.x - 80.0f,
                                              SETTINGS.screenResolution.y - 80.0f),
                                         vec2(30.0f),
                                         "./resources/scale_point_down.jpg");

    SCALE_POINT_UP_BUTTON = new Button(vec2(SETTINGS.screenResolution.x - 40.0f,
                                            SETTINGS.screenResolution.y - 80.0f),
                                       vec2(30.0f),
                                       "./resources/scale_point_up.jpg");

    DECREASE_ANIMATION_SPEED_BUTTON = new Button(vec2(SETTINGS.screenResolution.x - 80.0f,
                                                      SETTINGS.screenResolution.y - 120.0f),
                                                 vec2(30.0f),
                                                 "./resources/speed_down.jpg");

    INCREASE_ANIMATION_SPEED_BUTTON = new Button(vec2(SETTINGS.screenResolution.x - 40.0f,
                                                      SETTINGS.screenResolution.y - 120.0f),
                                                 vec2(30.0f),
                                                 "./resources/speed_up.jpg");

    SHOW_GRID_BUTTON = new Button(vec2(SETTINGS.screenResolution.x - 25.0f,
                                       5.0f),
                                  vec2(20.0f),
                                  SETTINGS.showGridPlane ? "./resources/grid_shown.jpg" : "./resources/grid_hidden.jpg");


    REPEAT_ANIMATION_BUTTON->setProjection(&STATE->HUDprojection);
    FIRST_ANIMATION_STEP_BUTTON->setProjection(&STATE->HUDprojection);
    PLAY_ANIMATION_BUTTON->setProjection(&STATE->HUDprojection);
    LAST_ANIMATION_STEP_BUTTON->setProjection(&STATE->HUDprojection);
    SCALE_POINT_UP_BUTTON->setProjection(&STATE->HUDprojection);
    SCALE_POINT_DOWN_BUTTON->setProjection(&STATE->HUDprojection);
    DECREASE_ANIMATION_SPEED_BUTTON->setProjection(&STATE->HUDprojection);
    INCREASE_ANIMATION_SPEED_BUTTON->setProjection(&STATE->HUDprojection);
    SHOW_GRID_BUTTON->setProjection(&STATE->HUDprojection);


    auto axisLabelX = deffem::Rectangle(0.0f, 0.0f, 0.0f, 0.0025f, 0.0025f);
    auto axisLabelY = deffem::Rectangle(0.0f, 0.0f, 0.0f, 0.0025f, 0.0025f);
    auto axisLabelZ = deffem::Rectangle(0.0f, 0.0f, 0.0f, 0.0025f, 0.0025f);

    const auto axisLabelX_t = translate(fmat4(1.0f), fvec3(0.105f, 0.0f, 0.0f));
    const auto axisLabelY_t = translate(fmat4(1.0f), fvec3(0.0f, 0.105f, 0.0f));
    const auto axisLabelZ_t = translate(fmat4(1.0f), fvec3(0.0f, 0.0f, 0.105f));


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        // Render background color
        glClearColor(SETTINGS.backgroundColor.red, SETTINGS.backgroundColor.green, SETTINGS.backgroundColor.blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto ticked = STATE->animationTick();

        if (ticked && !STATE->animation.play)
            PLAY_ANIMATION_BUTTON->changeTexture("./resources/play_ctrl.jpg");


        // DEFFEM Model transformation

        const auto camera = STATE->camera;
        auto projection = perspective(radians(camera.fov), STATE->screenSize.x / STATE->screenSize.y, 0.1f, 100.0f);
        const auto eye = fvec3(camera.target.x + camera.radius * sin(camera.phi) * cos(camera.theta),
                               camera.target.y + camera.radius * cos(camera.phi),
                               camera.target.z + camera.radius * sin(camera.phi) * sin(camera.theta)
        );
        const auto view = lookAt(fvec3(eye.x, eye.y, eye.z),
                                 fvec3(camera.target.x, camera.target.y, camera.target.z),
                                 fvec3(0.0f, 1.0f, 0.0f));

        auto scaledModel = scale(fmat4(1.0f), fvec3(STATE->modelScale));
        const auto centeredModel = translate(scaledModel, MODEL_ORIGIN_DEVIATION);
        const auto translatedProjection = translate(projection, fvec3(camera.offset.x, -camera.offset.y, 0.0f));


        // Pass transformation matrices to the shader
        if (MODEL_SHADER)
        {
            MODEL_SHADER->use();
            MODEL_SHADER->setMat4("model", centeredModel);
            MODEL_SHADER->setMat4("projection", translatedProjection);
            MODEL_SHADER->setMat4("view", view);
        }

        // Mesh transformation
        gridShader.use();
        gridShader.setMat4("model", mat4(1.0f));
        gridShader.setMat4("projection", translatedProjection);
        gridShader.setMat4("view", view);


        // Draw mesh    
        GRID_PLANE->draw(&gridShader, SETTINGS.lineWidth);


        // Draw DEFFEM model
        const auto deffemModelCtx = STATE->currentModelContext();
        if (deffemModelCtx)
        {
            deffemModelCtx->model->draw(MODEL_SHADER);
        }


        // Draw heatmap
        if (STATE->heatmap)
        {
            STATE->heatmap->draw(&heatmapShader, &textShader, TYPER);
        }


        // Axis labels
        const auto axisLabelXRotation = transpose(lookAt(fvec3(0.0f), eye, vec3(0.0f, 1.0f, 0.0f)));
        const auto axisLabelYRotation = transpose(lookAt(fvec3(0.0f), eye, vec3(0.0f, 1.0f, 0.0f)));
        const auto axisLabelZRotation = transpose(lookAt(fvec3(0.0f), eye, vec3(0.0f, 1.0f, 0.0f)));

        rectShader.use();
        rectShader.setMat4("projection", translatedProjection);
        rectShader.setMat4("view", view);

        // Draw x axis label
        rectShader.setMat4("model_t", axisLabelX_t);
        rectShader.setMat4("model", axisLabelXRotation);
        rectShader.setVec3("textColor", vec3(1.0f, 0.0f, 0.0f));
        TYPER->bindTexture('X');
        axisLabelX.draw(&rectShader);

        // Draw y axis label
        rectShader.setVec3("textColor", vec3(0.0f, 1.0f, 0.0f));
        rectShader.setMat4("model_t", axisLabelY_t);
        rectShader.setMat4("model", axisLabelYRotation);
        TYPER->bindTexture('Y');
        axisLabelY.draw(&rectShader);

        // Draw z axis label
        rectShader.setVec3("textColor", vec3(0.0f, 0.0f, 1.0f));
        rectShader.setMat4("model_t", axisLabelZ_t);
        rectShader.setMat4("model", axisLabelZRotation);
        TYPER->bindTexture('Z');
        axisLabelZ.draw(&rectShader);


        // Draw buttons
        REPEAT_ANIMATION_BUTTON->draw(&buttonShader);
        FIRST_ANIMATION_STEP_BUTTON->draw(&buttonShader);
        PLAY_ANIMATION_BUTTON->draw(&buttonShader);
        LAST_ANIMATION_STEP_BUTTON->draw(&buttonShader);
        SCALE_POINT_UP_BUTTON->draw(&buttonShader);
        SCALE_POINT_DOWN_BUTTON->draw(&buttonShader);
        DECREASE_ANIMATION_SPEED_BUTTON->draw(&buttonShader);
        INCREASE_ANIMATION_SPEED_BUTTON->draw(&buttonShader);
        SHOW_GRID_BUTTON->draw(&buttonShader);


        // Display state info 
        STATE->displayStateInfoText(GLOBAL_TEXT_SHADER, TYPER);


        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    glfwTerminate();

    delete TYPER;
    delete GLOBAL_TEXT_SHADER;
    delete STATE;
    delete MODEL_SHADER;
    delete MES_SHADER;
    delete SPH_SHADER;
    
    delete REPEAT_ANIMATION_BUTTON;
    delete PLAY_ANIMATION_BUTTON;
    delete SCALE_POINT_UP_BUTTON;
    delete SCALE_POINT_DOWN_BUTTON;
    delete LAST_ANIMATION_STEP_BUTTON;
    delete FIRST_ANIMATION_STEP_BUTTON;
    delete DECREASE_ANIMATION_SPEED_BUTTON;
    delete INCREASE_ANIMATION_SPEED_BUTTON;
    delete SHOW_GRID_BUTTON;

    return 0;
}

ModelContext* loadModel(const string& filename)
{
    CustomObject* model = nullptr;

    vector<GLfloat> vertices;
    vector<GLuint> indices;
    const auto attributeSizes = vector<GLuint>{3, 3};

    const auto modelInfo = FileParser::readSections(filename, vertices, indices);

    MODEL_ORIGIN_DEVIATION.x = -vertices[0];
    MODEL_ORIGIN_DEVIATION.y = -vertices[1];
    MODEL_ORIGIN_DEVIATION.z = -vertices[2];


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

    STATE->clearModels();

    for (const auto& path : v)
    {
        auto model = loadModel(path);
        STATE->deffemModelContexts.push_back(model);
    }

    STATE->refresh();

    const auto ctx = STATE->currentModelContext();
    if (ctx && ctx->info.elementCount == 0)
    {
        MODEL_SHADER = SPH_SHADER;
    }
    else if (ctx)
    {
        MODEL_SHADER = MES_SHADER;
    }

    MODEL_SHADER->use();
    MODEL_SHADER->setVec3("intersection", SETTINGS.modelIntersection);
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
            STATE->animation.stop();
            STATE->previousModel();
        }
    }
    else if (key == GLFW_KEY_RIGHT)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            STATE->animation.stop();
            STATE->nextModel();
        }
    }
    else if (key == GLFW_KEY_SPACE)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            cout << "[EVENT] Play/Stop animation" << endl;
            if (STATE->animation.isFinished)
            {
                STATE->restartAnimation();
            }
            else
            {
                STATE->animation.play = !STATE->animation.play;
            }


            if (STATE->animation.play)
            {
                PLAY_ANIMATION_BUTTON->changeTexture("./resources/stop_ctrl.jpg");
            }
            else
            {
                PLAY_ANIMATION_BUTTON->changeTexture("./resources/play_ctrl.jpg");
            }
        }
    }
    else if (key == GLFW_KEY_R)
    {
        if (action == GLFW_PRESS)
        {
            cout << "[EVENT] Reset settings" << endl;
            SETTINGS.resetAll();
            STATE->modelScale = SETTINGS.modelScale;
            STATE->animation.tickMillis = milliseconds(SETTINGS.animationTickMillis);
            glPointSize(SETTINGS.pointSize);
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
                STATE->mouse.pressedWith(GLFW_MOD_CONTROL);
            }
            else if (mods == GLFW_MOD_SHIFT)
            {
                STATE->mouse.pressedWith(GLFW_MOD_SHIFT);
            }
            else
            {
                STATE->mouse.pressed();
                const auto mousePos = STATE->mouse.position;
                buttonsListener(fvec2(mousePos.x, STATE->screenSize.y - mousePos.y));
            }
        }
        else if (action == GLFW_RELEASE)
        {
            STATE->mouse.released();
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_2)
    {
        if (action == GLFW_PRESS)
        {
            STATE->mouse.pressedWith(GLFW_MOD_CONTROL);
        }
        else if (action == GLFW_RELEASE)
        {
            STATE->mouse.released();
        }
    }
}

void buttonsListener(const fvec2 mousePos)
{
    if (PLAY_ANIMATION_BUTTON->isClicked(mousePos))
    {
        cout << "[EVENT] Play/Stop animation" << endl;
        if (STATE->animation.isFinished)
        {
            STATE->restartAnimation();
        }
        else
        {
            STATE->animation.play = !STATE->animation.play;
        }

        if (STATE->animation.play)
        {
            PLAY_ANIMATION_BUTTON->changeTexture("./resources/stop_ctrl.jpg");
        }
        else
        {
            PLAY_ANIMATION_BUTTON->changeTexture("./resources/play_ctrl.jpg");
        }
    }
    else if (REPEAT_ANIMATION_BUTTON->isClicked(mousePos))
    {
        STATE->animation.repeat = !STATE->animation.repeat;
        cout << "[EVENT] Repeat animation: " << STATE->animation.repeat << endl;
        if (STATE->animation.repeat)
        {
            REPEAT_ANIMATION_BUTTON->changeTexture("./resources/repeat_on.jpg");
        }
        else
        {
            REPEAT_ANIMATION_BUTTON->changeTexture("./resources/repeat_off.jpg");
        }
    }
    else if (FIRST_ANIMATION_STEP_BUTTON->isClicked(mousePos))
    {
        cout << "[EVENT] First step" << endl;
        STATE->changeModel(0);
    }
    else if (LAST_ANIMATION_STEP_BUTTON->isClicked(mousePos))
    {
        cout << "[EVENT] Last step" << endl;
        STATE->changeModel(STATE->deffemModelContexts.size() - 1);
    }
    else if (SCALE_POINT_UP_BUTTON->isClicked(mousePos))
    {
        glPointSize(SETTINGS.pointSize += 1.0);
        cout << "[EVENT] Point size: " << SETTINGS.pointSize << endl;
    }
    else if (SCALE_POINT_DOWN_BUTTON->isClicked(mousePos))
    {
        if (SETTINGS.pointSize > 1)
        {
            glPointSize(SETTINGS.pointSize -= 1.0);
        }
        cout << "[EVENT] Point size: " << SETTINGS.pointSize << endl;
    }
    else if (DECREASE_ANIMATION_SPEED_BUTTON->isClicked(mousePos))
    {
        STATE->animation.tickMillis += milliseconds(5);
        cout << "[EVENT] Animation tick: " << STATE->animation.tickMillis.count() << "ms" << endl;
    }
    else if (INCREASE_ANIMATION_SPEED_BUTTON->isClicked(mousePos) && STATE->animation.tickMillis.count() >= 5)
    {
        STATE->animation.tickMillis -= milliseconds(5);
        cout << "[EVENT] Animation tick: " << STATE->animation.tickMillis.count() << "ms" << endl;
    }
    else if (SHOW_GRID_BUTTON->isClicked(mousePos))
    {
        cout << "[EVENT] Show grid: " << !GRID_PLANE->showGrid << endl;
        GRID_PLANE->showGrid = !GRID_PLANE->showGrid;
        if (GRID_PLANE->showGrid)
        {
            SHOW_GRID_BUTTON->changeTexture("./resources/grid_shown.jpg");
        }
        else
        {
            SHOW_GRID_BUTTON->changeTexture("./resources/grid_hidden.jpg");
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    const auto mousePos = STATE->mouse.position;

    if (STATE->mouse.isPressedWithControl)
    {
        STATE->camera.offset.x += (xpos - mousePos.x) * 0.0002f;
        STATE->camera.offset.y += (ypos - mousePos.y) * 0.0002f;
    }
    else if (STATE->mouse.isPressedWithShift)
    {
        const auto sy = (ypos - mousePos.y) * 0.0005f;
        STATE->modelScale -= sy;
    }
    else if (STATE->mouse.isPressed)
    {
        STATE->camera.theta += (xpos - mousePos.x) * 0.01f;
        STATE->camera.phi += (ypos - mousePos.y) * 0.01f;

        if (STATE->camera.phi > 3.139f) STATE->camera.phi = 3.139f;
        if (STATE->camera.phi < 0.01f) STATE->camera.phi = 0.01f;
    }

    STATE->mouse.position.x = xpos;
    STATE->mouse.position.y = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (STATE->camera.fov >= 1.0f && STATE->camera.fov <= 45.0f)
        STATE->camera.fov -= yoffset;
    if (STATE->camera.fov <= 1.0f)
        STATE->camera.fov = 1.0f;
    if (STATE->camera.fov >= 45.0f)
        STATE->camera.fov = 45.0f;
}


void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    if (width > 0 && height > 0)
    {
        glViewport(0, 0, width, height);

        const auto screenSize = fvec2(width, height);
        STATE->screenSize = screenSize;

        STATE->HUDprojection = ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                                     static_cast<GLfloat>(height));

        if (STATE->heatmap)
        {
            STATE->heatmap->setPosition(fvec2(25, height / 2));
        }

        GLOBAL_TEXT_SHADER->setMat4("projection", STATE->HUDprojection);

        REPEAT_ANIMATION_BUTTON->setPosition(fvec2(width - 160.0f, height - 40.0f));
        FIRST_ANIMATION_STEP_BUTTON->setPosition(fvec2(width - 120.0f, height - 40.0f));
        PLAY_ANIMATION_BUTTON->setPosition(fvec2(width - 80.0f, height - 40.0f));
        LAST_ANIMATION_STEP_BUTTON->setPosition(fvec2(width - 40.0f, height - 40.0f));

        SCALE_POINT_DOWN_BUTTON->setPosition(fvec2(width - 80.0f, height - 80.0f));
        SCALE_POINT_UP_BUTTON->setPosition(fvec2(width - 40.0f, height - 80.0f));

        DECREASE_ANIMATION_SPEED_BUTTON->setPosition(fvec2(width - 80.0f, height - 120.0f));
        INCREASE_ANIMATION_SPEED_BUTTON->setPosition(fvec2(width - 40.0f, height - 120.0f));

        SHOW_GRID_BUTTON->setPosition(fvec2(width - 25.0f, 5.0));
    }
}


int maxNrOfVertexAttrsSupported()
{
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    return nrAttributes;
}
