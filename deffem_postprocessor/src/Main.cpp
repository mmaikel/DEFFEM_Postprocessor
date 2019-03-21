#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "FileParser.cpp"
#include "../headers/Shader.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.inl>
#include "Heatmap.cpp"
#include "MeshPlane.cpp"
#include "ModelContext.cpp"
#include "../headers/Button.h"
#include <algorithm>
#include <chrono>
#include "../headers/stb_image.h"


using namespace std::chrono;


// GLFW Callback functions definitions
int max_nr_of_vertex_attrs_supported();
// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void rotateModel(Shader& shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
ModelContext* loadModel(char const* filename);
void file_drop_callback(GLFWwindow* window, int count, const char** paths);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

bool nextModel();
bool previousModel();


// Settings
float SCR_WIDTH;
float SCR_HEIGHT;

// Field of view
float fov = 25.0f;

// Last position of the mouse cursor
glm::vec2 mousePos(0.0f);

// Projection translation
double alpha = 0.0f, beta = 0.0f;

// Camera position 
double theta = 0.0f;
double phi = 1.57f;
double radius = 0.25f;

// Point where the camera points to
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.033f, 0.0f);

// Used to center the object on the stage
glm::vec3 modelOriginOffset(0.0f);

// Projections
glm::mat4 textProjection;

// Objects pointers
std::vector<ModelContext*> deffemModelContexts;
Heatmap* heatmap = nullptr;
Typer* typer = nullptr;

int currentModelIndex;
string currentFilename;

Button* playButton = nullptr;
Button* scaleUpButton = nullptr;
Button* scaleDownButton = nullptr;
Button* lastStepButton = nullptr;
Button* firstStepButton = nullptr;


bool playAnimation = false;

auto modelScale = 0.1f;

auto pointSize = 2.0f;

bool lightTheme = true;

glm::vec3 textColor;


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

    modelScale = stof(conf["model_scale"]);
    std::stringstream ss(conf["resolution"]);

    string w, h;
    std::getline(ss, w, 'x');
    std::getline(ss, h);

    SCR_WIDTH = stof(w);
    SCR_HEIGHT = stof(h);

    // Window initialization
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DEFFEM Postprocessing", nullptr, nullptr);
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


    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


    // Set OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(0.5);
    glPointSize(pointSize);
    // glEnable(GL_CULL_FACE);

    // Set callbacks
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetDropCallback(window, file_drop_callback);
    glfwSetKeyCallback(window, key_callback);

    auto bgColor = Color::fromString(conf["bg-color"]);
    lightTheme = (0.3 * bgColor.red + 0.59 * bgColor.green + 0.11 * bgColor.blue) >= 0.5;
    glm::vec3 meshColor;

    if(lightTheme)
    {
        textColor = glm::vec3(0.0, 0.0, 0.0);
        meshColor = glm::vec3(0.8, 0.8, 0.8);
    } else
    {
        textColor = glm::vec3(1.0, 1.0, 1.0);
        meshColor = glm::vec3(0.1, 0.1, 0.1);
    }

    // Used to display text on the screen
    typer = new Typer();

    int gap = 60;
    if (conf["show_mesh"] == "false")
    {
        gap = 0;
    }
    MeshPlane mesh(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, gap, meshColor);

    // Compile and setup the shaders
    Shader textShader("./shaders/text.vs", "./shaders/text.fs");
    Shader modelShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader meshShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader heatmapShader("./shaders/heatmap.vs", "./shaders/heatmap.fs");
    Shader buttonShader("./shaders/control.vs", "./shaders/control.fs");


    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f,
                                static_cast<GLfloat>(SCR_HEIGHT));


    textShader.use();
    textShader.setMat4("projection", textProjection);

    heatmapShader.use();
    heatmapShader.setMat4("projection", textProjection);


    firstStepButton = new Button(glm::vec2(SCR_WIDTH - 120, SCR_HEIGHT - 40), glm::vec2(30), "./resources/button.jpg");
    firstStepButton->setViewport(SCR_WIDTH, SCR_HEIGHT);

    playButton = new Button(glm::vec2(SCR_WIDTH - 80, SCR_HEIGHT - 40), glm::vec2(30), "./resources/button.jpg");
    playButton->setViewport(SCR_WIDTH, SCR_HEIGHT);

    lastStepButton = new Button(glm::vec2(SCR_WIDTH - 40, SCR_HEIGHT - 40), glm::vec2(30), "./resources/button.jpg");
    lastStepButton->setViewport(SCR_WIDTH, SCR_HEIGHT);

    scaleUpButton = new Button(glm::vec2(SCR_WIDTH - 40, SCR_HEIGHT - 80), glm::vec2(30), "./resources/button.jpg");
    scaleUpButton->setViewport(SCR_WIDTH, SCR_HEIGHT);

    scaleDownButton = new Button(glm::vec2(SCR_WIDTH - 80, SCR_HEIGHT - 80), glm::vec2(30), "./resources/button.jpg");
    scaleDownButton->setViewport(SCR_WIDTH, SCR_HEIGHT);


    const milliseconds animationTick(50);

    milliseconds nextAnimationDeadline = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ) + animationTick;


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        // Render background color
        // glClearColor(0.15f, 0.15f, 0.17f, 1.0f);
        glClearColor(bgColor.red, bgColor.green, bgColor.blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (playAnimation)
        {
            auto currentTimeMillis = duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
            );

            if (currentTimeMillis >= nextAnimationDeadline)
            {
                playAnimation = nextModel();
                nextAnimationDeadline = currentTimeMillis + animationTick;
            }
        }


        // DEFFEM Model transformation
        modelShader.use();

        auto projection = glm::perspective(glm::radians(fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        auto view = glm::mat4(1.0f);

        glm::vec3 eye = glm::vec3(cameraTarget.x + radius * sin(phi) * cos(theta),
                                  cameraTarget.y + radius * cos(phi),
                                  cameraTarget.z + radius * sin(phi) * sin(theta)
        );

        view = glm::lookAt(glm::vec3(eye.x, eye.y, eye.z),
                           glm::vec3(cameraTarget.x, cameraTarget.y, cameraTarget.z),
                           glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 scaledModel = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale));
        glm::mat4 centeredModel = glm::translate(scaledModel, modelOriginOffset);

        glm::mat4 translatedProjection = glm::translate(projection, glm::vec3(-alpha, beta, 0.0f));


        // Pass transformation matrices to the shader
        modelShader.setMat4("model", centeredModel);
        modelShader.setMat4("projection", translatedProjection);
        modelShader.setMat4("view", view);


        // Mesh transformation
        meshShader.use();


        meshShader.setMat4("model", glm::mat4(1.0f));
        meshShader.setMat4("projection", translatedProjection);
        meshShader.setMat4("view", view);


        // Draw DEFFEM model
        if (!deffemModelContexts.empty())
        {
            auto currentModel = deffemModelContexts[currentModelIndex];
            if (currentModel && currentModel->model)
            {
                currentModel->model->draw(&modelShader);
            }
        }

        // Draw mesh    
        mesh.draw(&meshShader);


        // Draw heatmap
        if (heatmap)
        {
            heatmap->draw(&heatmapShader, &textShader);
        }

        firstStepButton->draw(&buttonShader);
        playButton->draw(&buttonShader);
        lastStepButton->draw(&buttonShader);

        scaleUpButton->draw(&buttonShader);
        scaleDownButton->draw(&buttonShader);


        // Display model number and filename
        if (!currentFilename.empty())
        {

            typer->renderText(textShader, "Scale: " + std::to_string(modelScale), 10.0f, 60.0f, 0.3f,
                textColor);

            typer->renderText(
                textShader,
                "Step: " + std::to_string(currentModelIndex + 1) + " of " + std::to_string(deffemModelContexts.size()) +
                " loaded", 10.0f, 30.0f, 0.3f,
                textColor);

            typer->renderText(textShader, "File: " + currentFilename, 10.0f, 10.0f, 0.3f,
                textColor);
        }
        else
        {
            typer->renderText(textShader, "Drag and drop files here...", SCR_WIDTH / 2 - 100.0, SCR_HEIGHT / 2 - 5.0,
                              0.4f, textColor);
        }


        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    for (auto model : deffemModelContexts)
    {
        delete model;
    }
    deffemModelContexts.clear();
    delete heatmap;
    delete typer;

    glfwTerminate();

    return 0;
}

ModelContext* loadModel(const string& filename)
{
    deffem::CustomObject* model = nullptr;

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
        model = new deffem::CustomObject(vertices, attributeSizes, 6);
    }
    else
    {
        model = new deffem::CustomObject(vertices, indices, attributeSizes, 6);
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

void file_drop_callback(GLFWwindow* window, const int count, const char** paths)
{
    // Sort file paths
    std::vector<std::string> v(paths, paths + count);
    std::sort(v.begin(), v.end());

    // Remove old models
    if (!deffemModelContexts.empty())
    {
        for (auto ctx : deffemModelContexts)
        {
            delete ctx;
        }

        deffemModelContexts.clear();
    }

    // Load models from files
    for (const auto& path : v)
    {
        auto model = loadModel(path);
        deffemModelContexts.push_back(model);
    }

    currentModelIndex = 0;
    heatmap = new Heatmap(25.0f, SCR_HEIGHT / 2, 50.0f, 200.0f, deffemModelContexts[0]->info, typer, textColor);
    currentFilename = deffemModelContexts[0]->filename;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void changeModel(int index)
{
    currentModelIndex = index;
    delete heatmap;
    heatmap = new Heatmap(25.0f, SCR_HEIGHT / 2, 50.0f, 200.0f, deffemModelContexts[index]->info, typer, textColor);

    currentFilename = deffemModelContexts[index]->filename;
}

bool mousePressed = false;
bool mouseWithControlPressed = false;
bool mouseWithShiftPressed = false;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Target camera up/down and left/right on the x axis
    if (key == GLFW_KEY_LEFT)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            playAnimation = false;
            previousModel();
        }
    }
    else if (key == GLFW_KEY_RIGHT && currentModelIndex < deffemModelContexts.size() - 1)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            playAnimation = false;
            nextModel();
        }
    }
    else if (key == GLFW_KEY_SPACE)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            cout << "[EVENT] Play/Stop animation" << endl;
            if (currentModelIndex == deffemModelContexts.size() - 1)
            {
                changeModel(0);
                playAnimation = true;
            } else
            {
                playAnimation = !playAnimation;
            }
        }
    }
}

bool nextModel()
{
    if (!deffemModelContexts.empty() && currentModelIndex < deffemModelContexts.size() - 1)
    {
        changeModel(currentModelIndex + 1);

        return true;
    }
    return false;
}

bool previousModel()
{
    if (currentModelIndex > 0)
    {
        changeModel(currentModelIndex - 1);

        return true;
    }

    return false;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1)
    {
        if (mods == GLFW_MOD_CONTROL && !mousePressed && !mouseWithShiftPressed)
        {
            if (action == GLFW_PRESS)
            {
                mouseWithControlPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                mouseWithControlPressed = false;
            }
        }
        else if (mods == GLFW_MOD_SHIFT && !mousePressed && !mouseWithControlPressed)
        {
            if (action == GLFW_PRESS)
            {
                mouseWithShiftPressed = true;
            }
            else if (action == GLFW_RELEASE)
            {
                mouseWithShiftPressed = false;
            }
        }
        else if (action == GLFW_PRESS)
        {
            mousePressed = true;

            if (playButton->isClicked(glm::vec2(mousePos.x, SCR_HEIGHT - mousePos.y)))
            {
                cout << "[EVENT] Play animation" << endl;
                playAnimation = !playAnimation;
            }

            else if (firstStepButton->isClicked(glm::vec2(mousePos.x, SCR_HEIGHT - mousePos.y)))
            {
                cout << "[EVENT] First step" << endl;
                currentModelIndex = 0;
            }

            else if (lastStepButton->isClicked(glm::vec2(mousePos.x, SCR_HEIGHT - mousePos.y)))
            {
                cout << "[EVENT] Last step" << endl;
                currentModelIndex = deffemModelContexts.size() - 1;
            }

            else if (scaleUpButton->isClicked(glm::vec2(mousePos.x, SCR_HEIGHT - mousePos.y)))
            {
                glPointSize(pointSize += 1.0);
                cout << "[EVENT] Point size: " << pointSize << endl;
            }

            else if (scaleDownButton->isClicked(glm::vec2(mousePos.x, SCR_HEIGHT - mousePos.y)))
            {
                if (pointSize > 1)
                {
                    glPointSize(pointSize -= 1.0);
                }
                cout << "[EVENT] Point size: " << pointSize << endl;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            mousePressed = false;
            mouseWithControlPressed = false;
            mouseWithShiftPressed = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mouseWithControlPressed)
    {
        alpha += (xpos - mousePos.x) * 0.0005f;
        beta += (ypos - mousePos.y) * 0.0005f;
    }
    else if (mouseWithShiftPressed)
    {
        auto sy = (ypos - mousePos.y) * 0.0005f;

        modelScale -= sy;
    }
    else if (mousePressed)
    {
        theta += (xpos - mousePos.x) * 0.01f;
        phi += (ypos - mousePos.y) * 0.01f;

        if (phi > 3.139f) phi = 3.139f;
        if (phi < 0.01f) phi = 0.01f;
    }

    mousePos.x = xpos;
    mousePos.y = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}


void window_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_HEIGHT = width;
    SCR_HEIGHT = height;

    glViewport(0, 0, width, height);

    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                                static_cast<GLfloat>(height));


    firstStepButton->setViewport(width, height);
    playButton->setViewport(width, height);
    lastStepButton->setViewport(width, height);

    scaleUpButton->setViewport(width, height);
    scaleDownButton->setViewport(width, height);

    firstStepButton->setPosition(glm::vec2(width - 120.0, height - 40.0));
    playButton->setPosition(glm::vec2(width - 80.0, height - 40.0));
    lastStepButton->setPosition(glm::vec2(width - 40.0, height - 40.0));

    scaleUpButton->setPosition(glm::vec2(width - 80.0, height - 80.0));
    scaleDownButton->setPosition(glm::vec2(width - 40.0, height - 80.0));
}


int max_nr_of_vertex_attrs_supported()
{
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    return nrAttributes;
}
