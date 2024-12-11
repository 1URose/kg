#include <GL/gl.h>
#include <GL/glu.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// g++ lab2.cpp -o lab2 -lsfml-graphics -lsfml-window -lsfml-system  -lGL -lglut


// Глобальные переменные для камеры
glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f); // Позиция камеры
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);  // Направление камеры
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);      // Вектор "вверх"
float yaw = -90.0f;  // Угол поворота камеры (влево/вправо)
float pitch = 0.0f;  // Угол наклона камеры (вверх/вниз)
float moveSpeed = 0.1f; // Скорость перемещения
float sensitivity = 0.5f; // Чувствительность клавиатуры
float cameraZoom = 45.0f; // Угол обзора

// Обновление вектора направления камеры
void updateCameraDirection() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Функция для рисования куба
void drawCube() {
    glBegin(GL_QUADS);
    
    // Передняя грань
    glColor3f(1.0f, 0.0f, 0.0f); // Красный
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);

    // Задняя грань
    glColor3f(0.0f, 1.0f, 0.0f); // Зеленый
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);

    // Левые грани
    glColor3f(0.0f, 0.0f, 1.0f); // Синий
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);

    // Правые грани
    glColor3f(1.0f, 1.0f, 0.0f); // Желтый
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);

    // Верхняя грань
    glColor3f(1.0f, 0.0f, 1.0f); // Пурпурный
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);

    // Нижняя грань
    glColor3f(0.0f, 1.0f, 1.0f); // Голубой
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);

    glEnd();
}

// Функция для рисования пирамиды
void drawPyramid() {
    glBegin(GL_TRIANGLES);

    // Передняя грань
    glColor3f(1.0f, 0.0f, 0.0f); // Красный
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Правая грань
    glColor3f(0.0f, 1.0f, 0.0f); // Зеленый
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Задняя грань
    glColor3f(0.0f, 0.0f, 1.0f); // Синий
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);

    // Левая грань
    glColor3f(1.0f, 1.0f, 0.0f); // Желтый
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    // Нижняя грань (квадрат)
    glColor3f(0.5f, 0.5f, 0.5f); // Серый
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);

    glEnd();
}

// Функция для рисования сферы
void drawSphere(float radius, int slices, int stacks) {
    glColor3f(0.0f, 0.0f, 1.0f);
    for (int i = 0; i <= stacks; ++i) {
        float lat0 = M_PI * (-0.5f + float(i) / stacks);
        float lat1 = M_PI * (-0.5f + float(i + 1) / stacks);
        float z0 = radius * sin(lat0);
        float z1 = radius * sin(lat1);
        float r0 = radius * cos(lat0);
        float r1 = radius * cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2 * M_PI * float(j) / slices;
            float x0 = r0 * cos(lng);
            float y0 = r0 * sin(lng);
            float x1 = r1 * cos(lng);
            float y1 = r1 * sin(lng);

            glVertex3f(x0, y0, z0);
            glVertex3f(x1, y1, z1);
        }
        glEnd();
    }
}

int main() {
    // Создание окна SFML
    sf::Window window(sf::VideoMode(800, 600), "3D Scene with Free Camera", sf::Style::Close, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    // Инициализация OpenGL
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Главный цикл
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Управление камерой с клавиатуры
        glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            cameraPosition += moveSpeed * cameraFront;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            cameraPosition -= moveSpeed * cameraFront;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            cameraPosition -= moveSpeed * right;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            cameraPosition += moveSpeed * right;
        }

        // Управление углом обзора
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            pitch += sensitivity;
            if (pitch > 89.0f) pitch = 89.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            pitch -= sensitivity;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            yaw -= sensitivity;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            yaw += sensitivity;
        }

        // Обновляем направление камеры
        updateCameraDirection();

        // Очищаем экран
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Устанавливаем камеру
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(cameraZoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view;
        glMultMatrixf(&mvp[0][0]);

        // Рисуем объекты
        glPushMatrix();
        glTranslatef(-2.0f, 0.0f, 0.0f);
        drawCube();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(2.0f, 0.0f, 0.0f);
        drawPyramid();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -3.0f);
        drawSphere(1.0f, 20, 20);
        glPopMatrix();

        // Обновляем экран
        window.display();
    }

    return 0;
}