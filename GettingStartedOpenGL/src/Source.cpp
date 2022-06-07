#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "include/Core.h"
#include "include/Shader.h"
#include "include/ShaderProgram.h"

void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 tex_coord;
};

int main()
{
    glfwInit();  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        1280, // Window Width
        720, // Window Height
        "LearnOpenGL", // Window Title
        nullptr, // Monitor, use glfwPrimaryMontior() for full-screen
        nullptr  // Share, use this for resource sharing (typically not needed)
    );
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback);
    

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    ShaderProgram shader;
	shader.CompileAndLink("assets/shaders/basic.vs", "assets/shaders/basic.fs");


    std::array<Vertex, 3> triangle =
    {
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f)},  // left
        Vertex{glm::vec3( 0.0f,  0.5f, 0.0f)},  // top
        Vertex{glm::vec3( 0.5f, -0.5f, 0.0f)}   // right
    };

    std::array<Vertex, 4> rectangle =
    {
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 0.0f) }, // bottom left
        Vertex{glm::vec3(-0.5f,  0.5f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 1.0f) }, // top left
        Vertex{glm::vec3( 0.5f,  0.5f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 1.0f) }, // top right
        Vertex{glm::vec3( 0.5f, -0.5f, 0.0f),  glm::vec3(1.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 0.0f) }  // bottom right
    };

    std::array<uint32_t, 6> indices_of_rectangle =
    {
        0, 1, 2, // first triangle, top left
        2, 3, 0, // second triangle, bottom right
    };

    std::array<uint32_t, 8> indices_of_outline =
    {
        0, 1, // left side
    	1, 2, // upper side
        2, 3, // right side
    	3, 0  // lower side
    };

    uint32_t myVAO, myVBO, myEBO;
    glGenVertexArrays(1, &myVAO);
    glGenBuffers(1, &myVBO);
    glGenBuffers(1, &myEBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attribute(s).
    glBindVertexArray(myVAO);

    // bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, myVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle.data(), GL_STATIC_DRAW);

    // bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_of_rectangle), indices_of_rectangle.data(), GL_STATIC_DRAW);


    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set to flip the y-axis so that the image isn't upside-down
	stbi_set_flip_vertically_on_load(true);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("assets/textures/Kurisu.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

    // unbind objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);  // recommended
    glBindVertexArray(0);  // optional

    // render Loop
    while (!glfwWindowShouldClose(window)) // when the window is on, do the followings
    {
        ProcessInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        shader.Bind();
        glBindVertexArray(myVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &myVAO);
    glDeleteBuffers(1, &myVBO);
    glDeleteBuffers(1, &myEBO);
    glDeleteProgram(shader.programId);


    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void PrintMaximumVertexAttributes()
{
    int numAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
    std::cout << "Maximum number of vertex attributes supported: " << numAttributes << std::endl;
}