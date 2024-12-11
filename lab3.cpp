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

// Глобальные переменные для объектов
struct Object {
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation; // В градусах (x, y, z)
};

std::vector<Object> objects = {
    {{-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}, // Куб
    {{2.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},  // Пирамида
    {{0.0f, 0.0f, -3.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}  // Сфера
};

int currentObject = 0; // Индекс текущего объекта

// Функция для обновления объекта
void transformObject(Object& obj) {
    glTranslatef(obj.position.x, obj.position.y, obj.position.z);
    glRotatef(obj.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(obj.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(obj.rotation.z, 0.0f, 0.0f, 1.0f);
    glScalef(obj.scale.x, obj.scale.y, obj.scale.z);
}

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
    sf::Window window(sf::VideoMode(800, 600), "3D Scene with Transformable Objects", sf::Style::Close, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Управление объектами
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
            currentObject = 0; // Куб
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
            currentObject = 1; // Пирамида
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
            currentObject = 2; // Сфера
        }

        Object& obj = objects[currentObject];

        // Перемещение
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)) obj.position.z -= moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) obj.position.z += moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) obj.position.x -= moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) obj.position.x += moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) obj.position.y += moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) obj.position.y -= moveSpeed;

        // Масштабирование
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            obj.scale += glm::vec3(0.1f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            obj.scale -= glm::vec3(0.1f);
        }

        // Вращение
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) obj.rotation.y -= sensitivity;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) obj.rotation.y += sensitivity;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) obj.rotation.x -= sensitivity;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) obj.rotation.x += sensitivity;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(cameraZoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view;
        glMultMatrixf(&mvp[0][0]);

        // Отрисовка объектов
        for (size_t i = 0; i < objects.size(); ++i) {
            glPushMatrix();
            transformObject(objects[i]);
            if (i == 0) drawCube();
            if (i == 1) drawPyramid();
            if (i == 2) drawSphere(1.0f, 20, 20);
            glPopMatrix();
        }

        window.display();
    }

    return 0;
}