#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

// Задаю константы
const float PI = 3.14159265359f;
const int NUM_VERTICES = 6; // Число вершин
const float RADIUS = 100.0f; // Радиус многоугольника
const float SPEED = 0.01f; // Скорость анимации

// Линейная интерполяция
float interpolate(float start, float end, float t) {
    return start + t * (end - start);
}

// Функция для отрисовки шестиугольника
void drawPolygon(float dynamicRadius, float rotation) {
    glPushMatrix();
    glRotatef(rotation, 0, 0, 1); // Вращение

    glBegin(GL_POLYGON);
    for (int i = 0; i < NUM_VERTICES; ++i) {
        float angle = 2 * PI * i / NUM_VERTICES;
        float x = cos(angle) * dynamicRadius;
        float y = sin(angle) * dynamicRadius;
        glVertex2f(x, y);
    }
    glEnd();

    glPopMatrix();
}

int main() {
    // Создаем окно
    sf::RenderWindow window(sf::VideoMode(800, 600), "Интерполяция", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);

    // Настраиваем
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0); // Настройка 2D-координат
    glMatrixMode(GL_MODELVIEW);

    float t = 0.0f; // Время для интерполяции
    float time = 0.0f; // Время для циклической анимации
    float rotation = 0.0f; // Угол поворота

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Очистка экрана
        glClear(GL_COLOR_BUFFER_BIT);

        // Перемещение шестиугольника с интерполяцией
        float x = interpolate(100.0f, 600.0f, t);
        float y = 300.0f;

        // Обновляем параметры для анимации
        t += SPEED;
        if (t > 1.0f) t = 0.0f;

        float dynamicRadius = RADIUS + 20.0f * sin(time);
        rotation += 1.0f; // Плавное вращение
        time += SPEED;

        // Устанавливаем матрицу трансформации
        glPushMatrix();
        glTranslatef(x, y, 0); // Перемещаем фигуру

        // Рисуем шестиугольник
        drawPolygon(dynamicRadius, rotation);

        glPopMatrix();

        // Обновляем окно
        window.display();
    }

    return 0;
}
