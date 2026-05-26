#define GLFW_DLL
#define GLEW_DLL
#include "glew-2.1.0/include/GL/glew.h"
#include "glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include <iostream>
#include "Shader.h"
#include "Model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Параметры камеры
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 512.0f;
float lastY = 384.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastTime = 0.0f;

/*glm::mat4 OX1_Pos(1.0f);
glm::mat4 OX2_Pos(1.0f);
glm::mat4 OX3_Pos(1.0f);*/

float angle1;
float angle2;
float pos3Y;

const float ROT_SPEED = 60.0f;
const float MOV_SPEED = 2.0f;

// Пивот оси вращения звена 1
const float PX1 = -0.7f;
const float PY1 = 2.0f;
const float PZ1 = 0.0f;

glm::mat4 mat0 = glm::mat4(1.0f);
glm::mat4 mat1 = glm::mat4(1.0f);
glm::mat4 mat2 = glm::mat4(1.0f);
glm::mat4 mat3 = glm::mat4(1.0f);

void recalcMatrices()
{
    mat0 = glm::mat4(1.0f);

    // Звено 1: поворот вокруг Y со смещённой осью 
    glm::mat4 rot1 = glm::mat4(1.0f);
    rot1 = glm::translate(rot1, glm::vec3(PX1, PY1, PZ1));
    rot1 = glm::rotate(rot1, glm::radians(angle1), glm::vec3(0.0f, 1.0f, 0.0f));
    rot1 = glm::translate(rot1, glm::vec3(-PX1, -PY1, -PZ1));
    mat1 = mat0 * rot1;

    glm::mat4 rot2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle2), glm::vec3(0.0f, 1.0f, 0.0f));
    mat2 = mat1 * rot2;

    mat3 = glm::translate(mat2, glm::vec3(0.0f, pos3Y, 0.0f));
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    bool changed = false;

    // Звено 1: Q/E — поворот вокруг Y
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { angle1 += ROT_SPEED * deltaTime; changed = true; }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { angle1 -= ROT_SPEED * deltaTime; changed = true; }

    // Звено 2: R/F — поворот вокруг Y
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { angle2 += ROT_SPEED * deltaTime; changed = true; }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) { angle2 -= ROT_SPEED * deltaTime; changed = true; }

    // Звено 3: V/B — смещение вверх/вниз
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) { pos3Y += MOV_SPEED * deltaTime; changed = true; }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) { pos3Y -= MOV_SPEED * deltaTime; changed = true; }

    if (changed) recalcMatrices();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.03f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}


int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* Okno = glfwCreateWindow(800, 600, "Okno", NULL, NULL);
    if (!Okno) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(Okno);
    glewInit();
    glEnable(GL_DEPTH_TEST);

    Shader labShader("vertex.glsl", "fragment.glsl");

    // Загрузка модели
    Model ourModel("model.obj");

    glfwSetInputMode(Okno, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(Okno, mouse_callback);

    // Позиция источника света (немного сбоку и сверху от модели)
    glm::vec3 lightPos(5.0f, 5.0f, 10.0f);

    while (!glfwWindowShouldClose(Okno)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        processInput(Okno);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        labShader.activate();

        // Матрицы трансформации
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        // Раскомментируйте, если модель слишком большая:
        // model = glm::scale(model, glm::vec3(0.2f));

        // Нормальная матрица — корректирует нормали при неоднородном масштабировании
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

        labShader.setMat4("projection", projection);
        labShader.setMat4("view", view);
        labShader.setMat4("model", model);
        labShader.setMat4("transform", glm::mat4(1.0f));
        labShader.setMat3("normalMatrix", normalMatrix);

        // Позиция камеры для расчёта бликов
        labShader.setVec3("viewPos", cameraPosition);

        // --- Параметры источника света ---
        labShader.setVec3("light.position", lightPos);
        // Фоновый (ambient) — слабая интенсивность
        labShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // Диффузный (diffuse) — основной цвет освещения
        labShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        // Зеркальный (specular) — полная интенсивность
        labShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // --- Свойства материала модели ---
        // Цвет поверхности — тёплый медно-оранжевый
        labShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        labShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        labShader.setVec3("material.specular", 0.5f, 0.5f, 0.50f);
        labShader.setFloat("material.shininess", 32.0f);

        glm::mat4 globalScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));

        for (unsigned int i = 0; i < ourModel.meshes.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            switch (i)
            {
            case 0: model = globalScale * mat0; break; // неподвижно
            case 1: model = globalScale * mat1; break; // поворот по Y 
            case 2: model = globalScale * mat2; break; // поворот по Y + зависит от звена 1
            case 3: model = globalScale * mat3; break; // смещение по Y + зависит от звена 2
            default: model = globalScale;        break;
            }

            labShader.setMat4("model", model);
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            labShader.setMat3("normalMatrix", normalMatrix);

            ourModel.meshes[i].Draw();
        }
        glfwSwapBuffers(Okno);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}