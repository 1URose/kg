#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <opencv2/opencv.hpp>
#include <omp.h>
 
// Структура для векторов и цветов
struct Vec3 {
    double x, y, z;
    Vec3(double x_=0, double y_=0, double z_=0) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
    Vec3 operator*(double d) const { return Vec3(x*d, y*d, z*d); }
    Vec3 operator/(double d) const { return Vec3(x/d, y/d, z/d); }

    Vec3 normalize() const { double mg = std::sqrt(x*x + y*y + z*z); return Vec3(x/mg, y/mg, z/mg); }
    double dot(const Vec3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vec3 cross(const Vec3& v) const { return Vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); }
};

// Оператор умножения для double * Vec3
Vec3 operator*(double d, const Vec3& v) {
    return Vec3(v.x * d, v.y * d, v.z * d);
}

// Структура луча
struct Ray {
    Vec3 origin;
    Vec3 direction;
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d.normalize()) {}
};

// Абстрактный класс объекта сцены
class Object {
public:
    virtual bool intersect(const Ray& ray, double& t) const = 0;
    virtual Vec3 getNormal(const Vec3& point) const = 0;
    virtual Vec3 getColor(const Vec3& point) const = 0;
    virtual bool isReflective() const = 0;
    virtual double getReflectivity() const = 0;
};

// Класс сферы
class Sphere : public Object {
public:
    Vec3 center;
    double radius;
    Vec3 color;
    double reflectivity;

    Sphere(const Vec3& c, double r, const Vec3& col, double refl) : center(c), radius(r), color(col), reflectivity(refl) {}

    bool intersect(const Ray& ray, double& t) const override {
        Vec3 oc = ray.origin - center;
        double b = 2 * oc.dot(ray.direction);
        double c = oc.dot(oc) - radius*radius;
        double discriminant = b*b - 4*c;
        if (discriminant < 0) return false;
        else {
            discriminant = std::sqrt(discriminant);
            double t0 = (-b - discriminant) / 2;
            double t1 = (-b + discriminant) / 2;
            t = (t0 < t1) ? t0 : t1;
            if (t < 0) t = (t0 > t1) ? t0 : t1;
            return t >= 0;
        }
    }

    Vec3 getNormal(const Vec3& point) const override {
        return (point - center).normalize();
    }

    Vec3 getColor(const Vec3& point) const override {
        return color;
    }

    bool isReflective() const override {
        return reflectivity > 0;
    }

    double getReflectivity() const override {
        return reflectivity;
    }

    void setReflectivity(double refl) {
        reflectivity = refl;
    }
};

// Класс плоскости
class Plane : public Object {
public:
    Vec3 point;      // Точка на плоскости
    Vec3 normal;     // Нормаль плоскости
    cv::Mat texture; // Текстура плоскости
    double scale;    // Масштаб текстуры
    bool reflective; // Флаг отражательной способности

    Plane(const Vec3& p, const Vec3& n, const cv::Mat& tex, double s, bool refl = false) :
        point(p), normal(n), texture(tex), scale(s), reflective(refl) {
            normal = normal.normalize();
        }

    bool intersect(const Ray& ray, double& t) const override {
        double denom = normal.dot(ray.direction);
        if (std::abs(denom) > 1e-6) {
            t = (point - ray.origin).dot(normal) / denom;
            return t >= 0;
        }
        return false;
    }

    Vec3 getNormal(const Vec3& point_) const override {
        return normal;
    }

    Vec3 getColor(const Vec3& point_) const override {
        // Вычисление UV-координат
        double u, v;

        if (std::abs(normal.y) > 0.999) { // Горизонтальная плоскость (пол)
            u = point_.x * scale;
            v = point_.z * scale;
        } else if (std::abs(normal.x) > 0.999) { // Вертикальная плоскость по X
            u = point_.z * scale;
            v = point_.y * scale;
        } else if (std::abs(normal.z) > 0.999) { // Вертикальная плоскость по Z (стена)
            u = point_.x * scale;
            v = point_.y * scale;
        } else {
            u = 0;
            v = 0;
        }

        u = u - std::floor(u);
        v = v - std::floor(v);

        int tex_u = static_cast<int>(u * texture.cols);
        int tex_v = static_cast<int>(v * texture.rows);

        // Обработка выхода за границы текстуры
        tex_u = std::clamp(tex_u, 0, texture.cols - 1);
        tex_v = std::clamp(tex_v, 0, texture.rows - 1);

        cv::Vec3b color = texture.at<cv::Vec3b>(tex_v, tex_u);
        return Vec3(color[2]/255.0, color[1]/255.0, color[0]/255.0);
    }

    bool isReflective() const override {
        return reflective;
    }

    double getReflectivity() const override {
        return 0;
    }
};

// Функция трассировки луча
Vec3 trace(const Ray& ray, const std::vector<Object*>& objects, const Vec3& lightPos, int depth, int maxDepth) {
    if (depth > maxDepth) return Vec3(0, 0, 0); // Ограничение глубины рекурсии

    double nearest_t = std::numeric_limits<double>::max();
    const Object* hitObject = nullptr;

    // Поиск ближайшего пересечения
    for (const auto& obj : objects) {
        double t = 0;
        if (obj->intersect(ray, t)) {
            if (t < nearest_t) {
                nearest_t = t;
                hitObject = obj;
            }
        }
    }

    if (hitObject) {
        Vec3 hitPoint = ray.origin + ray.direction * nearest_t;
        Vec3 normal = hitObject->getNormal(hitPoint);
        Vec3 toLight = (lightPos - hitPoint).normalize();

        // Тени
        bool inShadow = false;
        Ray shadowRay(hitPoint + normal * 1e-4, toLight);
        for (const auto& obj : objects) {
            double t = 0;
            if (obj != hitObject && obj->intersect(shadowRay, t)) {
                inShadow = true;
                break;
            }
        }

        // Освещение
        Vec3 color = hitObject->getColor(hitPoint);
        Vec3 ambient = color * 0.1;
        Vec3 diffuse(0, 0, 0);
        Vec3 specular(0, 0, 0);

        if (!inShadow) {
            double diff = std::max(normal.dot(toLight), 0.0);
            diffuse = color * diff;

            // Зеркальное освещение
            Vec3 viewDir = (ray.origin - hitPoint).normalize();
            Vec3 reflectDir = (2 * normal.dot(toLight) * normal - toLight).normalize();
            double spec = std::pow(std::max(viewDir.dot(reflectDir), 0.0), 32);
            specular = Vec3(1, 1, 1) * spec;
        }

        Vec3 localColor = ambient + diffuse + specular;

        // Отражения
        if (hitObject->isReflective()) {
            Vec3 reflectDir = (ray.direction - 2 * ray.direction.dot(normal) * normal).normalize();
            Ray reflectRay(hitPoint + normal * 1e-4, reflectDir);
            Vec3 reflectedColor = trace(reflectRay, objects, lightPos, depth + 1, maxDepth);
            double reflectivity = hitObject->getReflectivity();
            localColor = localColor * (1 - reflectivity) + reflectedColor * reflectivity;
        }

        return localColor;
    }

    return Vec3(0.5, 0.7, 1.0); // Цвет фона (небо)
}

int main() {
    int width = 800;
    int height = 600;
    int maxDepth = 3; // Максимальная глубина рекурсии

    // Позиция камеры
    Vec3 cameraPos(0, 1, 5);

    // Позиция источника света
    Vec3 lightPos(-10, 10, 10);

    // Загрузка текстур
    cv::Mat floorTexture = cv::imread("floor_texture.jpg");
    cv::Mat wallTexture = cv::imread("wall_texture.jpg");

    if (floorTexture.empty() || wallTexture.empty()) {
        std::cerr << "Не удалось загрузить текстуры!" << std::endl;
        return -1;
    }

    // Создание объектов сцены
    std::vector<Object*> objects;

    // Пол
    Plane* floorPlane = new Plane(Vec3(0, 0, 0), Vec3(0, 1, 0), floorTexture, 0.1);
    objects.push_back(floorPlane);

    // Стена
    Plane* wallPlane = new Plane(Vec3(0, 0, -5), Vec3(0, 0, 1), wallTexture, 0.1);
    objects.push_back(wallPlane);

    // Сфера
    double sphereReflectivity = 0.5; // Начальная зеркальность сферы
    Sphere* sphere = new Sphere(Vec3(0, 1, 0), 1, Vec3(1, 1, 1), sphereReflectivity);
    objects.push_back(sphere);

    // Создание окна для отображения
    cv::namedWindow("Ray Tracing", cv::WINDOW_AUTOSIZE);

    // Основной цикл
    bool running = true;
    while (running) {
        // Изображение результата
        cv::Mat image(height, width, CV_8UC3);

        // Трассировка лучей для каждого пикселя
        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // Нормализованные координаты экрана
                double u = (2 * (x + 0.5) / (double)width - 1) * (width / (double)height);
                double v = 1 - 2 * (y + 0.5) / (double)height;

                // Направление луча
                Vec3 dir = Vec3(u, v, -1).normalize();

                Ray ray(cameraPos, dir);
                Vec3 color = trace(ray, objects, lightPos, 0, maxDepth);

                // Коррекция цвета
                color.x = std::min(1.0, std::max(0.0, color.x));
                color.y = std::min(1.0, std::max(0.0, color.y));
                color.z = std::min(1.0, std::max(0.0, color.z));

                // Запись цвета в изображение
                image.at<cv::Vec3b>(y, x) = cv::Vec3b(color.z * 255, color.y * 255, color.x * 255);
            }
        }

        // Отображение изображения
        cv::imshow("Ray Tracing", image);
        std::cout << "Текущая зеркальность сферы: " << sphere->getReflectivity() << std::endl;

        // Обработка событий
        int key = cv::waitKey(1);
        if (key == 27) { // ESC
            running = false;
        } else if (key == 'w' || key == 'W') {
            // Увеличение зеркальности
            double refl = sphere->getReflectivity();
            refl = std::min(1.0, refl + 0.1);
            sphere->setReflectivity(refl);
        } else if (key == 's' || key == 'S') {
            // Уменьшение зеркальности
            double refl = sphere->getReflectivity();
            refl = std::max(0.0, refl - 0.1);
            sphere->setReflectivity(refl);
        } else if (key == ' ') {
            // Сохранение изображения
            cv::imwrite("result.png", image);
            std::cout << "Изображение сохранено в 'result.png'" << std::endl;
        }
    }

    // Освобождение памяти
    for (auto obj : objects) {
        delete obj;
    }

    cv::destroyAllWindows();

    return 0;
}
