
#include "zview/drawables/statusbar.h"

StatusBar::StatusBar() {
  m_statusFieldWidths.at(static_cast<std::underlying_type<StatusField>::type>(
      StatusField::FPS)) = 90;
  m_statusFieldWidths.at(static_cast<std::underlying_type<StatusField>::type>(
      StatusField::OBJECT_END_CURSOR)) = 250;
  m_statusFieldWidths.at(static_cast<std::underlying_type<StatusField>::type>(
      StatusField::MEASURED_DISTANCE)) = 250;
};
void StatusBar::update(StatusField field, const std::string &value) {
  m_statusFieldValues.at(
      static_cast<std::underlying_type<StatusField>::type>(field)) = value;
};

void StatusBar::draw(ImVec2 wh) {
  ImGui::SetNextWindowPos(ImVec2(0, wh.y - STATUSBAR_HEIGHT), 0);
  ImGui::SetNextWindowSize(ImVec2(wh.x, STATUSBAR_HEIGHT), 0);
  ImGui::SetNextWindowBgAlpha(0.35f);  // Transparent background
  ImGui::Begin("Status Bar", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav);
  ImGui::Columns(static_cast<int>(StatusField::size), nullptr, false);
  for (int i{0}; i < static_cast<int>(StatusField::size); ++i) {
    ImGui::SetColumnWidth(i, m_statusFieldWidths.at(i));
    ImGui::TextUnformatted(m_statusFieldValues.at(i).c_str());
    ImGui::NextColumn();
  }
  ImGui::End();
}