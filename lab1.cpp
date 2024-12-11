#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>
#include <iostream>

// g++ lab1.cpp -o lab1 -lsfml-graphics -lsfml-window -lsfml-system

// Функция для генерации случайной точки на экране
sf::Vector2f generateRandomPosition(const sf::Window& window) {
    auto time_point = std::chrono::high_resolution_clock::now();
    std::mt19937 generator(time_point.time_since_epoch().count());
    float pos = generator() % window.getSize().x;
    
    if (pos > window.getSize().x) { 
        std::cout << "Error: Random Position Exceeds Window Bounds!\n"; 
        exit(-1); 
    }

    return (generator() % 2) ? 
           sf::Vector2f{
               (generator() % 2) ? (window.getSize().x - 300) : 0,
               generator() % (window.getSize().y - 100)
           } :
           sf::Vector2f{
               generator() % (window.getSize().x - 300),
               (generator() % 2) ? (window.getSize().y - 100) : 0
           };
}

// Линейная интерполяция между двумя точками
sf::Vector2f interpolatePosition(const sf::Vector2f& from, const sf::Vector2f& to, float factor) {
    return from * (1 - factor) + to * factor;
}

// Обработка событий окна
void handleWindowEvents(sf::Window& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed ||
            (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            window.close();
        }
    }
}

int main() {
    // Создание окна
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hexagon Animation");

    // Создание шестиугольника
    sf::CircleShape hexagonShape(50, 6);
    hexagonShape.setOutlineThickness(5);
    hexagonShape.setOutlineColor(sf::Color::White);
    hexagonShape.setFillColor(sf::Color::Green); // Меняем цвет на зелёный

    // Начальная и конечная позиции
    sf::Vector2f initialPosition = static_cast<sf::Vector2f>(window.getSize()) / 2.0f;
    sf::Vector2f targetPosition = generateRandomPosition(window);

    float interpolationFactor; // Коэффициент интерполяции
    bool isReturning = true; // Флаг, определяющий направление движения

    sf::Transform transformMatrix; // Матрица преобразования для фигуры

    sf::Clock clock;
    float transitionDuration = 3.0f; // Время для перемещения фигуры

    while (window.isOpen()) {
        std::cout << "\r                \r" << targetPosition.x << ' ' << targetPosition.y << std::flush;
        interpolationFactor = 0;

        while (window.isOpen() && interpolationFactor < 1) {
            handleWindowEvents(window); // Обработка событий окна

            auto elapsedTime = clock.restart();
            interpolationFactor += elapsedTime.asSeconds() / transitionDuration;
            if (interpolationFactor > 1) {
                interpolationFactor = 1;
            }

            // Получаем текущее положение фигуры через интерполяцию
            auto currentPos = interpolatePosition(initialPosition, targetPosition, interpolationFactor);

            // Настройка трансформации
            transformMatrix = sf::Transform::Identity;
            transformMatrix.translate(currentPos);

            // Масштабируем шестиугольник
            if (isReturning) {
                transformMatrix.scale(sf::Vector2f{1 + 2 * interpolationFactor, 1});
            } else {
                transformMatrix.scale(sf::Vector2f{1 + 2 * (1 - interpolationFactor), 1});
            }

            // Очистка окна и отрисовка фигуры
            window.clear();
            window.draw(hexagonShape, transformMatrix);
            window.display();
        }

        // Переключаем начальную и конечную позиции
        interpolationFactor = 0;
        std::swap(initialPosition, targetPosition);
        if (!isReturning) {
            targetPosition = generateRandomPosition(window); // Генерируем новую случайную точку
        }
        isReturning = !isReturning; // Меняем направление движения
    }

    std::cout << std::endl;
}
