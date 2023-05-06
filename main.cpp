#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>


#include "shader_s.h"
#include "camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Константы
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Камера
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

// Тайминги
float deltaTime = 0.0f;	// время между текущим кадром и последним кадром
float lastFrame = 0.0f;

int main()
{
    // glfw: инициализация и конфигурирование
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // раскомментируйте эту строку, если используете macOS
#endif

    // glfw: создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Сообщаем GLFW, чтобы он захватил наш курсор
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Конфигурирование глобального состояния OpenGL
    glEnable(GL_DEPTH_TEST);

    // Компилирование нашей шейдерной программы
    Shader ourShader("../7.4.camera.vs", "../7.4.camera.fs");

    // Указание вершин (и буфера(ов)) и настройка вершинных атрибутов


    struct float2{
        float x;
        float y;
    };

    struct float3{
        float x;
        float y;
        float z;
        float3 operator-(const float3& a){
            return {
                    this->x-a.x,
                    this->y-a.y,
                    this->z-a.z,
            };
        }
    };

    struct Vertex{
        float3 pos;
        float2 tex;
        float3 normal;
        float3 color;
    };

    auto genNormals = [](float3 a, float3 b) -> float3{
        glm::vec3 result = glm::normalize(glm::cross(glm::vec3(a.x, a.y, a.z), glm::vec3(b.x, b.y, b.z)));
        return {
            result.x, result.y, result.z
        };
    };

    auto addNormals = [](Vertex *vertices, int size){
        for(int i = 0; i < size; i+=3){
            float3 v = float3{vertices[i+1].pos.x - vertices[i].pos.x, vertices[i+1].pos.y - vertices[i].pos.y, vertices[i+1].pos.z - vertices[i].pos.z};
            float3 w = float3{vertices[i+2].pos.x - vertices[i].pos.x, vertices[i+2].pos.y - vertices[i].pos.y, vertices[i+2].pos.z - vertices[i].pos.z};
            float3 normal;
            normal.x = v.y*w.z - v.z * w.y;
            normal.y = v.z*w.x - v.x * w.z;
            normal.z = v.x*w.y - v.y * w.x;
            float length = sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2));
            normal.x = normal.x/length;
            normal.y = normal.y/length;
            normal.z = normal.z/length;
            pow(666, 666);
            vertices[i].normal = normal;
            vertices[i+1].normal = normal;
            vertices[i+2].normal = normal;
        }
    };

    Vertex vertices1[] = {
            {{0.0f, 0.0f, 2.5f}, {}, {}, {1.0f, 1.f, 0.f}},
            {{-1.0f, 1.0f, 0.0f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{-0.8f, -0.8f, 0.0f}, {}, {}, {1.0f, 0.f, 0.f}},

            {{0.0f, 0.0f, 2.5f}, {}, {}, {1.0f, 1.f, 0.f}},
            {{0.8f, 0.8f, 0.0f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{-1.0f, 1.0f, 0.0f}, {}, {}, {1.0f, 0.f, 0.f}},

            {{0.0f, 0.0f, 2.5f}, {}, {}, {1.0f, 1.f, 0.f}},
            {{1.0f, -1.0f, 0.0f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{0.8f, 0.8f, 0.0f}, {}, {}, {1.0f, 0.f, 0.f}},

            {{0.0f, 0.0f, 2.5f}, {}, {}, {1.0f, 1.f, 0.f}},
            {{-0.8f, -0.8f, 0.0f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{1.0f, -1.0f, 0.0f}, {}, {}, {1.0f, 0.f, 0.f}},


            {{0.0f, 0.0f, -2.5f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{-0.8f, -0.8f, 0.0f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{-1.0f, 1.0f, 0.0f}, {}, {}, {0.0f, 0.f, 1.f}},

            {{0.0f, 0.0f, -2.5f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{-1.0f, 1.0f, 0.0f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{0.8f, 0.8f, 0.0f}, {}, {}, {0.0f, 0.f, 1.f}},

            {{0.0f, 0.0f, -2.5f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{0.8f, 0.8f, 0.0f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{1.0f, -1.0f, 0.0f}, {}, {}, {0.0f, 0.f, 1.f}},

            {{0.0f, 0.0f, -2.5f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{1.0f, -1.0f, 0.0f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{-0.8f, -0.8f, 0.0f}, {}, {}, {0.0f, 0.f, 1.f}},

    };

    Vertex vertices2[] = {
            {{-1.25f, -1.25f, 1.25f}, {}, {}, {1.0f, 1.f, 0.f}},
            {{1.25f, -1.25f, 1.25f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{-1.25f, 1.25f, 1.25f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{-1.25f, 1.25f, 1.25f}, {}, {}, {1.0f, 1.f, 0.f}},
            {{1.25f, -1.25f, 1.25f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{1.25f, 1.25f, 1.25f}, {}, {}, {1.0f, 0.f, 0.f}},

            {{-1.25f, 1.25f, -1.25f}, {}, {}, {1.0f, 1.f, 0.f}},
            {{1.25f, -1.25f, -1.25f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{-1.25f, -1.25f, -1.25f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{-1.25f, 1.25f, -1.25f}, {}, {}, {1.0f, 1.f, 0.f}},
            {{1.25f, 1.25f, -1.25f}, {}, {}, {1.0f, 0.f, 0.f}},
            {{1.25f, -1.25f, -1.25f}, {}, {}, {1.0f, 0.f, 0.f}},

            {{1.25f, 1.25f, 1.25f}, {}, {}, {0.0f, 0.f, 1.f}},
            {{1.25f, -1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{1.25f, -1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{1.25f, 1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{1.25f, -1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{1.25f, 1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},

            {{1.25f, 1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{1.25f, 1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, 1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{1.25f, 1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, 1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, 1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},

            {{-1.25f, 1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, 1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, -1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, 1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, -1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, -1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},

            {{1.25f, -1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, -1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{1.25f, -1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{1.25f, -1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, -1.25f, 1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},
            {{-1.25f, -1.25f, -1.25f}, {}, {}, {-1.25f, 0.f, 1.f}},

    };


    addNormals(vertices1, 24);
    addNormals(vertices2, 36);


//    float vertices[] = {
//            // координаты        // текстурные координаты
//            0.0f, 0.0f, 2.0f,  1.0f, 1.0f, 0.f, 0.f, 0.f,
//            -0.8f, -0.8f, 0.0f,  1.0f, 0.0f, 0.f, 0.f, 0.f,
//            -1.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.f, 0.f, 0.f,
//
//            0.0f, 0.0f, 2.0f,  1.0f, 1.0f, 0.f, 0.f, 0.f,
//            -1.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.f, 0.f, 0.f,
//            0.8f, 0.8f, 0.0f,  1.0f, 0.0f, 0.f, 0.f, 0.f,
//
//            0.0f, 0.0f, 2.0f,  1.0f, 1.0f, 0.f, 0.f, 0.f,
//            0.8f, 0.8f, 0.0f,  1.0f, 0.0f, 0.f, 0.f, 0.f,
//            1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.f, 0.f, 0.f,
//
//            0.0f, 0.0f, 2.0f,  1.0f, 1.0f, 0.f, 0.f, 0.f,
//            1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.f, 0.f, 0.f,
//            -0.8f, -0.8f, 0.0f,  1.0f, 0.0f, 0.f, 0.f, 0.f,
//
//
//
//    };


    // Мировые координаты наших кубиков



//    float indeces[] = {
//
//    };
    unsigned int VBO1, VAO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);

    glBindVertexArray(VAO1);


    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    // Координатные атрибуты
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Нормалі
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float )));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float )));
    glEnableVertexAttribArray(3);


    unsigned int VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);


    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    // Координатные атрибуты
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Нормалі
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float )));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float )));
    glEnableVertexAttribArray(3);

    glEnable(GL_CULL_FACE);

    // Цикл рендеринга
    while (!glfwWindowShouldClose(window))
    {
        // Логическая часть работы со временем для каждого кадра
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Обработка ввода
        processInput(window);

        // Рендеринг
        glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очищаем буфер цвета и буфер глубины


        // Активируем шейдер
        ourShader.use();

        // Передаем шейдеру матрицу проекции (поскольку проекционная матрица редко меняется, то нет необходимости делать это для каждого кадра)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // Создаем преобразование камеры/вида
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        //рендерим прикол
        ourShader.setVec3("cameraViewDirection", camera.Front);


        ourShader.setVec3("cameraPosition", camera.Position);


        ourShader.setVec3("lightPosition", glm::vec3(-3.f, 3.f, 5.f));




        ourShader.setFloat("frame", currentFrame);

        // Рендерим трикутник
        glBindVertexArray(VAO1);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.5f,  0.0f, -4.5f));
        model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f,  0.f, 0.f));
        model = glm::rotate(model, glm::radians(currentFrame*13), glm::vec3(0.f,  0.f, 1.f));
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 24);

        //рендерим куб
        glBindVertexArray(VAO2);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.5f,  0.0f, -4.5f));
        model = glm::rotate(model, glm::radians(45.f), glm::vec3(1.f,  0.f, 0.f));
        model = glm::rotate(model, glm::radians(currentFrame*13), glm::vec3(0.f,  0.f, 1.f));
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        // glfw: обмен содержимым front- и back- буферов. Отслеживание событий ввода/вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Опционально: освобождаем все ресурсы, как только они выполнили свое предназначение
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);

    // glfw: завершение, освобождение всех выделенных ранее GLFW-реcурсов
    glfwTerminate();
    return 0;
}

// Обработка всех событий ввода: запрос GLFW о нажатии/отпускании кнопки мыши в данном кадре и соответствующая обработка данных событий
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: всякий раз, когда изменяются размеры окна (пользователем или операционной системой), вызывается данная callback-функция
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Убеждаемся, что окно просмотра соответствует новым размерам окна.
    // Обратите внимание, ширина и высота будут значительно больше, чем указано, на Retina-дисплеях
    glViewport(0, 0, width, height);
}

// glfw: всякий раз, когда перемещается мышь, вызывается данная callback-функция
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // перевернуто, так как y-координаты идут снизу вверх

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: всякий раз, когда прокручивается колесико мыши, вызывается данная callback-функция
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}