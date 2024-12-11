#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/glu.h>
#include <cmath>

// Углы вращения камеры и дистанция до сцены
float cameraAngleX = 0.0f, cameraAngleY = 0.0f, cameraDistance = 5.0f;

// Функция для отрисовки куба
void drawCube() {
    glBegin(GL_QUADS); // Начало отрисовки полигонов (грани куба)

    // Задняя грань (красная)
    glColor3f(1, 0, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, -1, -1);

    // Передняя грань (зеленая)
    glColor3f(0, 1, 0);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, -1, 1);

    // Левая грань (синяя)
    glColor3f(0, 0, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, 1, -1);

    // Правая грань (желтая)
    glColor3f(1, 1, 0);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);

    // Нижняя грань (фиолетовая)
    glColor3f(1, 0, 1);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);

    // Верхняя грань (циановая)
    glColor3f(0, 1, 1);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);

    glEnd(); // Завершение отрисовки
}

// Функция для отрисовки пирамиды
void drawPyramid() {
    glBegin(GL_TRIANGLES); // Начало отрисовки треугольников

    // Каждая грань пирамиды окрашивается в разные цвета
    glColor3f(1, 0, 0); // Грань 1 (красная)
    glVertex3f(0, 1, 0); // Верхняя вершина
    glVertex3f(-1, -1, 1); // Левая нижняя вершина
    glVertex3f(1, -1, 1); // Правая нижняя вершина

    glColor3f(0, 1, 0); // Грань 2 (зеленая)
    glVertex3f(0, 1, 0);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);

    glColor3f(0, 0, 1); // Грань 3 (синяя)
    glVertex3f(0, 1, 0);
    glVertex3f(1, -1, -1);
    glVertex3f(-1, -1, -1);

    glColor3f(1, 1, 0); // Грань 4 (желтая)
    glVertex3f(0, 1, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);

    glEnd(); // Завершение отрисовки треугольников

    glBegin(GL_QUADS); // Основание пирамиды
    glColor3f(0, 1, 1); // Основание (циановое)
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);
    glVertex3f(-1, -1, -1);
    glEnd();
}

// Функция для отрисовки сферы
void drawSphere() {
    GLUquadric *quad = gluNewQuadric(); // Создание квадрики для сферы
    glColor3f(0.5, 0.5, 0.5); // Серый цвет сферы
    gluSphere(quad, 1.0, 32, 32); // Радиус = 1.0, 32 сегмента по широте и долготе
    gluDeleteQuadric(quad); // Удаление квадрики
}

// Установка камеры для просмотра сцены
void setupCamera() {
    glLoadIdentity(); // Сброс матрицы
    glTranslatef(0, 0, -cameraDistance); // Перемещение камеры назад
    glRotatef(cameraAngleX, 1, 0, 0); // Вращение камеры по оси X
    glRotatef(cameraAngleY, 0, 1, 0); // Вращение камеры по оси Y
}

int main() {
    // Создание окна с настройками для OpenGL
    sf::Window window(sf::VideoMode(800, 600), "lab2", sf::Style::Default, sf::ContextSettings(32));
    window.setFramerateLimit(60); // Ограничение кадров в секунду

    glEnable(GL_DEPTH_TEST); // Включение теста глубины для 3D-отрисовки
    glMatrixMode(GL_PROJECTION); // Установка режима проекционной матрицы
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0); // Перспективная проекция

    glMatrixMode(GL_MODELVIEW); // Установка режима модельно-видовой матрицы

    // Главный цикл приложения
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) // Обработка закрытия окна
                window.close();

            if (event.type == sf::Event::KeyPressed) { // Обработка нажатий клавиш
                if (event.key.code == sf::Keyboard::W) cameraAngleX -= 5; // Вращение камеры вверх
                if (event.key.code == sf::Keyboard::S) cameraAngleX += 5; // Вращение камеры вниз
                if (event.key.code == sf::Keyboard::A) cameraAngleY -= 5; // Вращение камеры влево
                if (event.key.code == sf::Keyboard::D) cameraAngleY += 5; // Вращение камеры вправо
                if (event.key.code == sf::Keyboard::Up) cameraDistance -= 0.5; // Приближение камеры
                if (event.key.code == sf::Keyboard::Down) cameraDistance += 0.5; // Удаление камеры
            }
        }

        // Очистка экрана и буфера глубины
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setupCamera(); // Установка камеры

        // Отрисовка куба
        glPushMatrix();
        glTranslatef(-2.0, 0.0, 0.0); // Перемещение куба влево
        drawCube();
        glPopMatrix();

        // Отрисовка пирамиды
        glPushMatrix();
        glTranslatef(2.0, 0.0, 0.0); // Перемещение пирамиды вправо
        drawPyramid();
        glPopMatrix();

        // Отрисовка сферы
        glPushMatrix();
        glTranslatef(0.0, 0.0, -2.0); // Перемещение сферы назад
        drawSphere();
        glPopMatrix();

        window.display(); // Отображение содержимого окна
    }

    return 0;
}
