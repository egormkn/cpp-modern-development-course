#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace Svg {

struct Point {
  double x = 0.0;
  double y = 0.0;
};

struct Rgb {
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
};

using Color = std::variant<std::monostate, std::string, Rgb>;

const Color NoneColor;

std::ostream& operator<<(std::ostream& stream, const Color& color);

class Object {
 public:
  virtual void Render(std::ostream& out) const = 0;
  virtual ~Object() = default;
};

template <typename ObjectType>
class ObjectProperties {
 public:
  ObjectType& SetFillColor(const Color& value);

  ObjectType& SetStrokeColor(const Color& value);

  ObjectType& SetStrokeWidth(double value);

  ObjectType& SetStrokeLineCap(const std::string& value);

  ObjectType& SetStrokeLineJoin(const std::string& value);

 protected:
  Color fill = NoneColor;
  Color stroke = NoneColor;
  double stroke_width = 1.0;
  std::optional<std::string> stroke_linecap;
  std::optional<std::string> stroke_linejoin;
};

class Circle : public Object, public ObjectProperties<Circle> {
 public:
  Circle& SetCenter(Point value);

  Circle& SetRadius(double value);

  void Render(std::ostream& out) const override;

 private:
  Point center;
  double radius = 1.0;
};

class Polyline : public Object, public ObjectProperties<Polyline> {
 public:
  Polyline& AddPoint(Point value);

  void Render(std::ostream& out) const override;

 private:
  std::vector<Point> points;
};

class Text : public Object, public ObjectProperties<Text> {
 public:
  Text& SetPoint(Point value);

  Text& SetOffset(Point value);

  Text& SetFontSize(uint32_t value);

  Text& SetFontFamily(const std::string& value);

  Text& SetData(const std::string& data);

  void Render(std::ostream& out) const override;

 private:
  std::string data;
  Point point;
  Point offset;
  uint32_t font_size;
  std::optional<std::string> font_family;
};

using ObjectPtr = std::unique_ptr<Object>;

class Document {
 public:
  template <typename ObjectType>
  void Add(ObjectType object) {
    objects.push_back(std::make_unique<ObjectType>(std::move(object)));
  }

  void Render(std::ostream& out) const;

 private:
  std::vector<ObjectPtr> objects;
};

}