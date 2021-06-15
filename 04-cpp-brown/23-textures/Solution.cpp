#include "Common.h"

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`

class AbstractShape : public IShape {
 public:
  void SetPosition(Point p) override { position = move(p); }

  Point GetPosition() const override { return position; }

  void SetSize(Size s) override { size = move(s); }

  Size GetSize() const override { return size; }

  void SetTexture(shared_ptr<ITexture> t) override { texture = move(t); }

  ITexture* GetTexture() const override { return texture.get(); }

  // Рисует фигуру на указанном изображении
  void Draw(Image& image) const override {
    for (int dy = 0; dy < size.height; ++dy) {
      int y = position.y + dy;
      if (y >= static_cast<int>(image.size())) continue;
      for (int dx = 0; dx < size.width; ++dx) {
        int x = position.x + dx;
        if (x >= static_cast<int>(image[0].size()) || !IsPointInShape({x, y})) {
          continue;
        }
        if (texture && dx < texture->GetSize().width &&
            dy < texture->GetSize().height) {
          image[y][x] = texture->GetImage()[dy][dx];
        } else {
          image[y][x] = '.';
        }
      }
    }
  }

 protected:
  AbstractShape() = default;

  AbstractShape(Size size, Point position, shared_ptr<ITexture> texture)
      : size(move(size)), position(move(position)), texture(move(texture)) {}

  virtual bool IsPointInShape(Point p) const = 0;

  Size size;
  Point position;
  shared_ptr<ITexture> texture;
};

class Rectangle : public AbstractShape {
 public:
  Rectangle() = default;
  
  Rectangle(Size size, Point position, shared_ptr<ITexture> texture)
      : AbstractShape(move(size), move(position), move(texture)) {}

  // Возвращает точную копию фигуры.
  // Если фигура содержит текстуру, то созданная копия содержит ту же самую
  // текстуру. Фигура и её копия совместно владеют этой текстурой.
  unique_ptr<IShape> Clone() const override {
    return make_unique<Rectangle>(size, position, texture);
  }

 protected:
  bool IsPointInShape(Point p) const override {
    return position.y <= p.y && p.y < position.y + size.height &&
           position.x <= p.x && p.x < position.x + size.width;
  }
};

class Ellipse : public AbstractShape {
 public:
  Ellipse() = default;

  Ellipse(Size size, Point position, shared_ptr<ITexture> texture)
      : AbstractShape(move(size), move(position), move(texture)) {}

  // Возвращает точную копию фигуры.
  // Если фигура содержит текстуру, то созданная копия содержит ту же самую
  // текстуру. Фигура и её копия совместно владеют этой текстурой.
  unique_ptr<IShape> Clone() const override {
    return make_unique<Ellipse>(size, position, texture);
  }

 protected:
  bool IsPointInShape(Point p) const override {
    return IsPointInEllipse({p.x - position.x, p.y - position.y}, size);
  }
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  switch (shape_type) {
    case ShapeType::Ellipse:
      return make_unique<Ellipse>();
    case ShapeType::Rectangle:
      return make_unique<Rectangle>();
  }
}
