#include "../../include/gui.hpp"

// Base constructor to initialize private fields, and store each line seperatly, to allow for proper formating later 
GUI::Text::Text(Rectangle bounds, GUI::TextStyle style, const std::string& text) noexcept
  : m_Bounds(std::move(bounds)), m_Style(std::move(style))
{
  std::string line;
  for (const char& c : text)
  {
    // If new line char, then append current line to the lines vector
    if (c == '\n') 
    {
      m_TextLines.push_back(line);
      line.clear(); // Clear the current line, then skip to next char
      continue;
    }
    line += c; 
  }
  m_TextLines.push_back(line); // Append the final line
}

void GUI::Text::SetBounds(Rectangle bounds) noexcept
{
  m_Bounds = bounds;
}

void GUI::Text::UpdateAndRender(GUI::MouseState& mouseState) noexcept
{
  // Calculate font size from font scale, set base properties before user input
  float fontSize = m_Style.fontSize * GUI::fontScale;
  Color textColor = m_Style.textColor;
  Color backgroundColor = m_Style.backgroundColor;

  // Check if mouse hover
  if (CheckCollisionPointRec(mouseState.position, m_Bounds))
  {
    // Update text properties
    textColor = m_Style.textHoverColor;
    backgroundColor = m_Style.backgroundHoverColor;
  }

  // Draw background
  DrawRectangleRec(m_Bounds, backgroundColor);
  // Draw all the text lines
  int yPos = m_Bounds.y;
  for (const std::string& line : m_TextLines)
  {
    // Draw line based off text alignment
    switch (m_Style.textAlignment)
    {
      case TEXT_ALIGNMENT_LEFT:
        DrawTextEx(m_Style.font, line.c_str(), { m_Bounds.x+m_Style.padding.x, (float)yPos+m_Style.padding.y }, fontSize, SPACING, textColor);
        break;
      case TEXT_ALIGNMENT_CENTER:
        DrawTextEx(m_Style.font, line.c_str(), { m_Bounds.x+(m_Bounds.width/2)-(MeasureTextEx(m_Style.font, line.c_str(), fontSize, SPACING).x/2)+m_Style.padding.x, (float)yPos+m_Style.padding.y }, fontSize, SPACING, textColor);
        break;
      case TEXT_ALIGNMENT_RIGHT:
        DrawTextEx(m_Style.font, line.c_str(), { m_Bounds.x+m_Bounds.width-5-MeasureTextEx(m_Style.font, line.c_str(), fontSize, SPACING).x+m_Style.padding.x, (float)yPos+m_Style.padding.y }, fontSize, SPACING, textColor);
        break;
    }
    yPos += fontSize;
  }
}
