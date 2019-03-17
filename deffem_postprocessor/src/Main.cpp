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
#include <algorithm>


// GLFW Callback functions definitions
int max_nr_of_vertex_attrs_supported();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void rotateModel(Shader& shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
ModelContext* loadModel(char const* filename);
void file_drop_callback(GLFWwindow* window, int count, const char** paths);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Settings
float SCR_WIDTH = 1280;
float SCR_HEIGHT = 720;

// Field of view
float fov = 25.0f;

// Last position of the mouse cursor
glm::vec2 mousePos;

double theta = 0.0f, phi = 3.14f / 2;
double radius = 0.25f;

// Point where the camera points to
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.033f, 0.0f);

// Used to center the object on the stage
glm::vec3 modelOriginOffset;

// Projections
glm::mat4 textProjection;

// Objects pointers
std::vector<ModelContext*> deffemModelContexts;
Heatmap* heatmap = nullptr;
Typer* typer = nullptr;

int currentModelIndex;
string currentFilename;


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
    // glEnable(GL_CULL_FACE);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetDropCallback(window, file_drop_callback);
    glfwSetKeyCallback(window, key_callback);

    // Used to display text on the screen
    typer = new Typer();

    MeshPlane mesh(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 60);

    // Compile and setup the shaders
    Shader textShader("./shaders/text.vs", "./shaders/text.fs");
    Shader modelShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader meshShader("./shaders/shader.vs", "./shaders/shader.fs");
    Shader heatmapShader("./shaders/heatmap.vs", "./shaders/heatmap.fs");

    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f,
                                static_cast<GLfloat>(SCR_HEIGHT));
    textShader.use();
    textShader.setMat4("projection", textProjection);

    heatmapShader.use();
    heatmapShader.setMat4("projection", textProjection);


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Render background color
        glClearColor(0.15f, 0.15f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // DEFFEM Model transformation
        modelShader.use();

        auto projection = glm::perspective(glm::radians(fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        auto view = glm::mat4(1.0f);
        glm::vec3 eye;

        eye.x = cameraTarget.x + radius * sin(phi) * cos(theta);
        eye.y = cameraTarget.y + radius * cos(phi);
        eye.z = cameraTarget.z + radius * sin(phi) * sin(theta);
        view = glm::lookAt(glm::vec3(eye.x, eye.y, eye.z),
                           glm::vec3(cameraTarget.x, cameraTarget.y, cameraTarget.z),
                           glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 centered = glm::translate(glm::mat4(1.0f), modelOriginOffset);

        // Pass transformation matrices to the shader
        modelShader.setMat4("model", centered);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        // Mesh transformation
        meshShader.use();

        meshShader.setMat4("model", glm::mat4(1.0f));
        meshShader.setMat4("projection", projection);
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

        // Display model number and filename
        if (!currentFilename.empty())
        {
            typer->renderText(
                textShader,
                "Model: " + std::to_string(currentModelIndex + 1) + " of " + std::to_string(deffemModelContexts.size()) +
                " loaded", 10.0f, 30.0f, 0.3f,
                glm::vec3(1.0f, 1.0f, 1.0f));

            typer->renderText(textShader, "File: " + currentFilename, 10.0f, 10.0f, 0.3f,
                              glm::vec3(0.85f, 0.85f, 0.85f));
        }
        else
        {
            typer->renderText(textShader, "Drag and drop a file here...", SCR_WIDTH / 2 - 100.0, SCR_HEIGHT / 2 - 5.0,
                              0.4f, glm::vec3(0.85f, 0.85f, 0.85f));
        }


        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    deffemModelContexts.clear();
    delete heatmap;

    glfwTerminate();

    return 0;
}

ModelContext* loadModel(const string filename)
{
    deffem::CustomObject* model = NULL;

    vector<float> vertices;
    vector<unsigned int> indices;
    vector<unsigned int> attributeSizes;
    attributeSizes.push_back(3);
    attributeSizes.push_back(3);

    auto modelInfo = FileParser::readSections(filename, vertices, indices);

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
    heatmap = new Heatmap(25.0f, 500.0f, 50.0f, 200.0f, deffemModelContexts[index]->info, typer);

    currentFilename = deffemModelContexts[index]->filename;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Target camera up/down and left/right on the x axis
    if (key == GLFW_KEY_UP)
    {
        cameraTarget.y += 0.001;
    }
    else if (key == GLFW_KEY_DOWN)
    {
        cameraTarget.y -= 0.001;
    }
    else if (key == GLFW_KEY_LEFT)
    {
        cameraTarget.x -= 0.001;
    }
    else if (key == GLFW_KEY_RIGHT)
    {
        cameraTarget.x += 0.001;
    }
    else if (key == GLFW_KEY_LEFT_BRACKET && currentModelIndex > 0)
    {
        if (action == GLFW_PRESS)
        {
            changeModel(currentModelIndex - 1);
        }
        else if (action == GLFW_REPEAT)
        {
            changeModel(currentModelIndex - 1);
        }
    }
    else if (key == GLFW_KEY_RIGHT_BRACKET && currentModelIndex < deffemModelContexts.size() - 1)
    {
        if (action == GLFW_PRESS)
        {
            changeModel(currentModelIndex + 1);
        }
        else if (action == GLFW_REPEAT)
        {
            changeModel(currentModelIndex + 1);
        }
    }
}


void file_drop_callback(GLFWwindow* window, int count, const char** paths)
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
    heatmap = new Heatmap(25.0f, 500.0f, 50.0f, 200.0f, deffemModelContexts[0]->info, typer);
    currentFilename = deffemModelContexts[0]->filename;
}

bool mousePressed = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1)
    {
        if (action == GLFW_PRESS)
        {
            mousePressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mousePressed = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mousePressed)
    {
        theta += (xpos - mousePos.x) * 0.01f;
        phi += (ypos - mousePos.y) * 0.01f;
    }


    if (phi > 3.139f) phi = 3.139f;
    if (phi < 0.01f) phi = 0.01f;


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


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_HEIGHT = width;
    SCR_HEIGHT = height;

    glViewport(0, 0, width, height);

    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                                static_cast<GLfloat>(height));
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_HEIGHT = width;
    SCR_HEIGHT = height;

    glViewport(0, 0, width, height);

    textProjection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f,
                                static_cast<GLfloat>(height));
}


int max_nr_of_vertex_attrs_supported()
{
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    return nrAttributes;
}
