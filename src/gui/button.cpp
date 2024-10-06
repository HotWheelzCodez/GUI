#include "../../include/gui.hpp"

GUI::Button::Button(Rectangle bounds, GUI::ButtonStyle style, const std::string& text)
  : m_Bounds(bounds), m_Style(style), m_Text(text)
{ }

GUI::Button::Button(void) { }

bool GUI::Button::UpdateAndRender(GUI::MouseState& mouseState)
{
  Rectangle newBounds = m_Bounds;

  bool clicked = false;

  Color backgroundColor = m_Style.baseBackgroundColor;
  Color outlineColor = m_Style.baseOutlineColor;
  Color textColor = m_Style.baseTextColor;

  if (CheckCollisionPointRec(mouseState.position, m_Bounds))
  {
    backgroundColor = m_Style.hoverBackgroundColor;
    outlineColor = m_Style.hoverOutlineColor;
    textColor = m_Style.hoverTextColor;

    float scaleX = ((m_Bounds.width*m_Style.hoverScale)-m_Bounds.width)/2;
    float scaleY = ((m_Bounds.height*m_Style.hoverScale)-m_Bounds.height)/2;

    newBounds.x -= scaleX/2;
    newBounds.y -= scaleY/2;
    newBounds.width += scaleX;
    newBounds.height += scaleY;

    mouseState.cursor = MOUSE_CURSOR_POINTING_HAND;

    if (!mouseState.clicked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      clicked = true;
      mouseState.clicked = true;
    }
  }

  if (m_Style.outlineFill)
    DrawRectangleRounded({ newBounds.x-m_Style.outlineDistance, newBounds.y-m_Style.outlineDistance, newBounds.width+m_Style.outlineDistance*2, newBounds.height+m_Style.outlineDistance*2 }, m_Style.roundness, SEGMENTS, outlineColor);
  else
    DrawRectangleRoundedLines({ newBounds.x-m_Style.outlineDistance, newBounds.y-m_Style.outlineDistance, newBounds.width+m_Style.outlineDistance*2, newBounds.height+m_Style.outlineDistance*2 }, m_Style.roundness, SEGMENTS, m_Style.outlineThickness, outlineColor);

  DrawRectangleRounded(newBounds, m_Style.roundness, SEGMENTS, backgroundColor);

  switch (m_Style.textAlignment)
  {
    case TEXT_ALIGNMENT_LEFT:
      DrawTextEx(m_Style.font, m_Text.c_str(), { m_Bounds.x+5, m_Bounds.y+(m_Bounds.height/2)-((float)m_Style.fontSize/2) }, m_Style.fontSize, SPACING, textColor);
      break;
    case TEXT_ALIGNMENT_CENTER:
      DrawTextEx(m_Style.font, m_Text.c_str(), { m_Bounds.x+(m_Bounds.width/2)-(MeasureTextEx(m_Style.font, m_Text.c_str(), m_Style.fontSize, SPACING).x/2), m_Bounds.y+(m_Bounds.height/2)-((float)m_Style.fontSize/2) }, m_Style.fontSize, SPACING, textColor);
      break;
    case TEXT_ALIGNMENT_RIGHT:
      DrawTextEx(m_Style.font, m_Text.c_str(), { m_Bounds.x+m_Bounds.width-5-MeasureTextEx(m_Style.font, m_Text.c_str(), m_Style.fontSize, SPACING).x, m_Bounds.y+(m_Bounds.height/2)-((float)m_Style.fontSize/2) }, m_Style.fontSize, SPACING, textColor);
      break;
  }

  return clicked;
}
