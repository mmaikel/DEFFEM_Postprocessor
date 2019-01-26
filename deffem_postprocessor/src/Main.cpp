#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "FileParser.cpp"
#include "../headers/Shader.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.inl>
#include "Heatmap.cpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
int max_nr_of_vertex_attrs_supported();
void rotateModel(Shader& shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const float SCR_WIDTH = 800;
const float SCR_HEIGHT = 600;

float fov = 45.0f;

double oldX, oldY;
double theta = 0.0f, phi = 3.14f / 2;
double radius = 0.2f;

double targetX = 0.005f;
double targetY = 0.038f;
double targetZ = 0.005f;



GLuint VAO1, VBO1, VAO2, VBO2, VAO3, VBO3, EBO3;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif


    // Window initialization
    // ---------------------
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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Set OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_CULL_FACE);
   


    // Compile and setup the shader
    Shader textShader("./shaders/text.vs", "./shaders/text.fs");
    
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));
    
    Typer typer;
    

    // Read results from a file
    // ------------------------
    const auto filename = "resources/temperature 00807.k";
    float min, max;
    vector<float> vertices;
    vector<unsigned int> indices;
    vector<unsigned int> attribsSizes;
    attribsSizes.push_back(3);
    attribsSizes.push_back(3);
   
    FileParser::readSections(filename, vertices, indices, min, max);
    
    Shader modelShader("./shaders/shader.vs", "./shaders/shader.fs");
    deffem::CustomObject deffemModel(vertices, indices, attribsSizes, 6);

  
    float axis[] = {
        0.0f, -0.1f, 0.0f,
        0.0f, 0.1f, 0.0f,
    
        -0.1f, 0.0f, 0.0f,
        0.1f, 0.0f, 0.0f,
    
        0.0f, 0.0f, -0.1f,
        0.0f, 0.0f, 0.1f,
    };


    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    Shader heatmapShader("./shaders/heatmap.vs", "./shaders/heatmap.fs"); 

    glm::mat4 heatmapProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
    heatmapShader.use();
    glUniformMatrix4fv(glGetUniformLocation(heatmapShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(heatmapProjection));

    Heatmap heatmap(25.0f, 350.0f, 50.0f, 200.0f, min, max);


    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // activate shader
        modelShader.use();
        
        // create transformations
        glm::mat4 model;
        glm::mat4 projection = glm::perspective(glm::radians(fov), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first 
        
        double eyeX = targetX + radius * sin(phi) * cos(theta);
        double eyeY = targetY + radius * cos(phi);
        double eyeZ = targetZ + radius * sin(phi) * sin(theta);
        view = glm::lookAt(glm::vec3(eyeX, eyeY, eyeZ),
            glm::vec3(targetX, targetY, targetZ),
            glm::vec3(0.0f, 1.0f, 0.0f));
        
        // pass transformation matrices to the shader
        modelShader.setMat4("model", model);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        
        // draw model
        deffemModel.draw(modelShader);
        
        // draw axis
        glBindVertexArray(VAO1);
        glDrawArrays(GL_LINES, 0, 9);


        // draw heatmap
        heatmap.draw(heatmapShader, textShader);

        
        typer.renderText(textShader, "DEFFEM POSTPROCESSING", 25.0f, 25.0f, 0.7f, glm::vec3(0.85f, 0.85f, 0.85f));
        typer.renderText(textShader, "(C) Mikolaj Stepniewski", 630.0f, 570.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));


        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // // optional: de-allocate all resources once they've outlived their purpose:
    // // ------------------------------------------------------------------------


    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteVertexArrays(1, &VAO3);
    glDeleteBuffers(1, &VBO3);
    glDeleteBuffers(1, &EBO3);


    glfwTerminate();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
        theta += (xpos - oldX) * 0.01f;
        phi += (ypos - oldY) * 0.01f;
    }


    if (phi > 3.139f) phi = 3.139f;
    if (phi < 0.01f) phi = 0.01f;


    oldX = xpos;
    oldY = ypos;
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
    glViewport(0, 0, width, height);
}


int max_nr_of_vertex_attrs_supported()
{
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    return nrAttributes;
}


