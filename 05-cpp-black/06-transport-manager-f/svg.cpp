#include "svg.h"

using namespace std;

namespace Svg {

std::ostream& operator<<(std::ostream& stream, const Color& color);

class Object {
 public:
  virtual void Render(std::ostream& out) const;
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

 private:
  Point center;
  double radius = 1.0;
};

class Polyline : public Object, public ObjectProperties<Polyline> {
 public:
  Polyline& AddPoint(Point value);

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

 private:
  std::string data;
  Point point;
  Point offset;
  uint32_t font_size;
  std::optional<std::string> font_family;
};

using ObjectPtr = std::unique_ptr<Object>;

class Document : public Object {
 public:
  template <typename ObjectType>
  void Add(ObjectType object) {
    objects.push_back(std::make_unique<ObjectType>(std::move(object)));
  }

  void Render(std::ostream& out) const override;

 private:
  std::vector<ObjectPtr> objects;
};

}