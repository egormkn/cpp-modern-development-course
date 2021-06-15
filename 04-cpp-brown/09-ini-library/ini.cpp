#include "ini.h"

using namespace std;

namespace Ini {

Section& Document::AddSection(std::string name) {
  auto [it, success] = sections.insert({move(name), {}});
  return it->second;
}

const Section& Document::GetSection(const std::string& name) const {
  return sections.at(name);
}

size_t Document::SectionCount() const { return sections.size(); }

Document Load(std::istream& input) {  // TODO
  Document document;
  Ini::Section* section = nullptr;
  for (char c; input >> ws >> c;) {
    if (c == '[') {
      string section_name;
      getline(input, section_name, ']');
      section = &document.AddSection(section_name);
    } else {
      input.putback(c);
      string key, value;
      getline(input, key, '=');
      getline(input, value);
      section->emplace(move(key), move(value));
    }
  }
  return document;
}
}  // namespace Ini
