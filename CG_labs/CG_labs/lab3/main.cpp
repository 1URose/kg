#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <vector>
#include <cmath> // Для функций tan и M_PI

// Структура для хранения трансформаций объекта
struct Transform {
    sf::Vector3f position; // Позиция объекта
    sf::Vector3f rotation; // Углы поворота объекта
    sf::Vector3f scale;    // Масштаб объекта

    Transform() : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1) {}
};

// Базовый класс для объектов сцены
class SceneObject {
public:
    Transform transform;

    virtual void draw() = 0; // Виртуальная функция отрисовки
};

// Класс куба
class Cube : public SceneObject {
public:
    void draw() override {
        glPushMatrix();
        // Применение трансформаций
        glTranslatef(transform.position.x, transform.position.y, transform.position.z);
        glRotatef(transform.rotation.x, 1, 0, 0);
        glRotatef(transform.rotation.y, 0, 1, 0);
        glRotatef(transform.rotation.z, 0, 0, 1);
        glScalef(transform.scale.x, transform.scale.y, transform.scale.z);

        // Отрисовка куба
        glBegin(GL_QUADS);

        glColor3f(1, 0, 0); // Красный
        glVertex3f(-1, -1, -1);
        glVertex3f(1, -1, -1);
        glVertex3f(1, 1, -1);
        glVertex3f(-1, 1, -1);

        glColor3f(0, 1, 0); // Зеленый
        glVertex3f(-1, -1, 1);
        glVertex3f(1, -1, 1);
        glVertex3f(1, 1, 1);
        glVertex3f(-1, 1, 1);

        glColor3f(0, 0, 1); // Синий
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, -1, 1);
        glVertex3f(-1, 1, 1);
        glVertex3f(-1, 1, -1);

        glColor3f(1, 1, 0); // Желтый
        glVertex3f(1, -1, -1);
        glVertex3f(1, -1, 1);
        glVertex3f(1, 1, 1);
        glVertex3f(1, 1, -1);

        glColor3f(1, 0, 1); // Фиолетовый
        glVertex3f(-1, -1, -1);
        glVertex3f(1, -1, -1);
        glVertex3f(1, -1, 1);
        glVertex3f(-1, -1, 1);

        glColor3f(0, 1, 1); // Голубой
        glVertex3f(-1, 1, -1);
        glVertex3f(1, 1, -1);
        glVertex3f(1, 1, 1);
        glVertex3f(-1, 1, 1);

        glEnd();

        glPopMatrix();
    }
};

// Класс пирамиды
class Pyramid : public SceneObject {
public:
    void draw() override {
        glPushMatrix();
        // Применение трансформаций
        glTranslatef(transform.position.x, transform.position.y, transform.position.z);
        glRotatef(transform.rotation.x, 1, 0, 0);
        glRotatef(transform.rotation.y, 0, 1, 0);
        glRotatef(transform.rotation.z, 0, 0, 1);
        glScalef(transform.scale.x, transform.scale.y, transform.scale.z);

        // Отрисовка пирамиды
        glBegin(GL_TRIANGLES);

        glColor3f(1, 0, 0); // Красная грань
        glVertex3f(0, 1, 0);
        glVertex3f(-1, -1, 1);
        glVertex3f(1, -1, 1);

        glColor3f(0, 1, 0); // Зеленая грань
        glVertex3f(0, 1, 0);
        glVertex3f(1, -1, 1);
        glVertex3f(1, -1, -1);

        glColor3f(0, 0, 1); // Синяя грань
        glVertex3f(0, 1, 0);
        glVertex3f(1, -1, -1);
        glVertex3f(-1, -1, -1);

        glColor3f(1, 1, 0); // Желтая грань
        glVertex3f(0, 1, 0);
        glVertex3f(-1, -1, -1);
        glVertex3f(-1, -1, 1);

        glEnd();

        glBegin(GL_QUADS); // Основание пирамиды
        glColor3f(0, 1, 1);
        glVertex3f(-1, -1, 1);
        glVertex3f(1, -1, 1);
        glVertex3f(1, -1, -1);
        glVertex3f(-1, -1, -1);
        glEnd();

        glPopMatrix();
    }
};

int main() {
    // Создание окна с настройками OpenGL
    sf::Window window(sf::VideoMode(800, 600), "Лабораторная работа 3: Камера и трансформации", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);

    // Инициализация OpenGL
    glEnable(GL_DEPTH_TEST); // Включение буфера глубины

    // Настройка перспективы с использованием glFrustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = 800.0f / 600.0f;
    float fovY = 45.0f;
    float zNear = 0.1f;
    float zFar = 100.0f;
    float fH = tan(fovY / 360.0f * M_PI) * zNear;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);

    // Создание объектов сцены
    Cube cube;
    cube.transform.position = sf::Vector3f(-2, 0, 0);

    Pyramid pyramid;
    pyramid.transform.position = sf::Vector3f(2, 0, 0);

    std::vector<SceneObject*> objects = { &cube, &pyramid };

    // Параметры камеры
    sf::Vector3f cameraPosition(0, 0, 10);
    sf::Vector2f cameraRotation(0, 0);

    // Индекс текущего объекта
    int currentObjectIndex = 0;

    while (window.isOpen()) {
        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            // Выход по клавише Esc
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        // Управление камерой
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cameraPosition.z -= 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cameraPosition.z += 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cameraPosition.x -= 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cameraPosition.x += 0.1f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            cameraRotation.x -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            cameraRotation.x += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            cameraRotation.y -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            cameraRotation.y += 1.0f;

        // Переключение между объектами
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
            currentObjectIndex = 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
            currentObjectIndex = 1;

        // Управление текущим объектом
        SceneObject* currentObject = objects[currentObjectIndex];

        // Перемещение
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
            currentObject->transform.position.y += 0.05f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
            currentObject->transform.position.y -= 0.05f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
            currentObject->transform.position.x -= 0.05f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
            currentObject->transform.position.x += 0.05f;

        // Поворот
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
            currentObject->transform.rotation.y -= 2.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            currentObject->transform.rotation.y += 2.0f;

        // Масштабирование
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            currentObject->transform.scale *= 1.01f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
            currentObject->transform.scale *= 0.99f;

        // Очистка буферов
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Установка камеры
        glLoadIdentity();
        glRotatef(cameraRotation.x, 1, 0, 0);
        glRotatef(cameraRotation.y, 0, 1, 0);
        glTranslatef(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z);

        // Отрисовка объектов сцены
        for (auto obj : objects) {
            obj->draw();
        }

        // Отображение кадра
        window.display();
    }

    return 0;
}
