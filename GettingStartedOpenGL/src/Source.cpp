#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "include/Core.h"
#include "include/Shader.h"
#include "include/ShaderProgram.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void ProcessInput(GLFWwindow* window);
void PrintMaximumVertexAttributes();

// settings
constexpr uint16 kScreenWidth = 1280;
constexpr uint16 kScreenHeight = 720;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = kScreenWidth / 2.0;
float lastY = kScreenHeight / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

struct Vertex
{
    glm::vec3 pos_coord;
    glm::vec2 tex_coord;
};

int main()
{
    // Initialization
    // ----------------------------------------------------------------------------------
    glfwInit();  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        kScreenHeight, // Window Width
        kScreenWidth, // Window Height
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
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    // glfwSetScrollCallback(window, ScrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // ----------------------------------------------------------------------------------

    // configure global OpenGl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

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
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f),  glm::vec2(0.0f, 0.0f) }, // bottom left
        Vertex{glm::vec3(-0.5f,  0.5f, 0.0f),  glm::vec2(0.0f, 1.0f) }, // top left
        Vertex{glm::vec3( 0.5f,  0.5f, 0.0f),  glm::vec2(1.0f, 1.0f) }, // top right
        Vertex{glm::vec3( 0.5f, -0.5f, 0.0f),  glm::vec2(1.0f, 0.0f) }  // bottom right
    };

    std::array<Vertex, 36> cube = {
    	Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3( 0.5f, -0.5f, -0.5f),  glm::vec2(1.0f, 0.0f)},
		Vertex{glm::vec3( 0.5f,  0.5f, -0.5f),  glm::vec2(1.0f, 1.0f)},
		Vertex{glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)},

		
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec2(0.0f, 0.0f)},

        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},

        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f, -0.5f),  glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f,  0.5f),  glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},
        
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f, -0.5f),  glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3( 0.5f, -0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)},
        
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f, -0.5f),  glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),  glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec2(0.0f, 1.0f)}
    };

    std::array<uint32_t, 6> indices_of_rectangle =
    {
        0, 1, 2, // first triangle, top left ¨I
        2, 3, 0, // second triangle, bottom right ¨K
    };

    std::array<uint32_t, 8> indices_of_cube =
    {
        0, 1, // left side
    	1, 2, // upper side
        2, 3, // right side
    	3, 0  // lower side
    };

    uint32 myVAO, myVBO, myEBO;
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

    // Uncomment the following to enable frame mode;
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    uint32 texture;
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
    uint8 *data = stbi_load("assets/textures/Kurisu.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture\n";
    }
    stbi_image_free(data);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos_coord));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind objects
    glBindBuffer(GL_ARRAY_BUFFER, 0); // recommended
    glBindVertexArray(0); // optional

    // Set clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Set matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 combo = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glEnable(GL_DEPTH_TEST);

    // render Loop
    while (!glfwWindowShouldClose(window)) // when the window is on, do the followings
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(fov), static_cast<float>(kScreenWidth / kScreenHeight), 0.1f, 100.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        combo = projection * view * model;

        shader.Bind();
        shader.UploadMat4("u_combo_mat", combo);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(myVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    	glBindVertexArray(0);
        shader.Unbind();

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

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void FramebufferSizeCallback(GLFWwindow* window, const int &width, const int &height)
{
    glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void PrintMaximumVertexAttributes()
{
    int numAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributes);
    std::cout << "Maximum number of vertex attributes supported: " << numAttributes << std::endl;
}