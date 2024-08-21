#pragma once

#include <imgui.h>

#include <array>
#include <string>
class StatusBar {
 public:
  enum class StatusField : std::size_t {
    FPS,
    OBJECT_END_CURSOR,
    MEASURED_DISTANCE,
    size
  };

 private:
  static constexpr float STATUSBAR_HEIGHT = 20.0f;
  std::array<float, static_cast<std::underlying_type<StatusField>::type>(
                        StatusField::size)>
      m_statusFieldWidths{};
  std::array<std::string, static_cast<std::underlying_type<StatusField>::type>(
                              StatusField::size)>
      m_statusFieldValues{};

 public:
  StatusBar();
  void update(StatusField field, const std::string &value);
  void draw(ImVec2 wh);
};