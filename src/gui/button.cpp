#include "../../include/gui.hpp"

// Base constructor, initializing private fields
GUI::Button::Button(Rectangle bounds, GUI::ButtonStyle style, const std::string& text) noexcept
  : m_Bounds(bounds), m_Style(style), m_Text(text)
{ }

void GUI::Button::SetText(const std::string& text) noexcept
{
  m_Text = text;
}

void GUI::Button::SetBounds(Rectangle bounds) noexcept
{
  m_Bounds = bounds;
}

bool GUI::Button::UpdateAndRender(GUI::MouseState& mouseState) noexcept
{
  // Store a temp bound (newBounds) incase of hover scale property, calculate new font size from font scale
  Rectangle newBounds = m_Bounds;
  float fontSize = m_Style.fontSize * GUI::fontScale;

  bool clicked = false;

  // Properties to change based of input
  Color backgroundColor = m_Style.baseBackgroundColor;
  Color outlineColor = m_Style.baseOutlineColor;
  Color textColor = m_Style.baseTextColor;

  // Mouse is hovering over the button
  if (CheckCollisionPointRec(mouseState.position, m_Bounds))
  {
    // Update to hover properties
    backgroundColor = m_Style.hoverBackgroundColor;
    outlineColor = m_Style.hoverOutlineColor;
    textColor = m_Style.hoverTextColor;

    // Calculate scale based off hover scale property
    float scaleX = ((m_Bounds.width*m_Style.hoverScale)-m_Bounds.width)/2;
    float scaleY = ((m_Bounds.height*m_Style.hoverScale)-m_Bounds.height)/2;

    // Update temp bounds (newBounds) based off calculated scale
    newBounds.x -= scaleX/2;
    newBounds.y -= scaleY/2;
    newBounds.width += scaleX;
    newBounds.height += scaleY;

    // Upade mouse cursor state, since mouse hovering
    mouseState.cursor = MOUSE_CURSOR_POINTING_HAND;

    // Check if button pressed, if mouse not clicked previously this frame
    if (!mouseState.clicked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      // Update mouse state to reflect user input
      clicked = true;
      mouseState.clicked = true;
    }
  }

  // If button outline fill, draw up to the outline width, else draw outline lines
  if (m_Style.outlineFill)
    DrawRectangleRounded({ newBounds.x-m_Style.outlineDistance, newBounds.y-m_Style.outlineDistance, newBounds.width+m_Style.outlineDistance*2, newBounds.height+m_Style.outlineDistance*2 }, m_Style.roundness, SEGMENTS, outlineColor);
  else
    DrawRectangleRoundedLinesEx({ newBounds.x-m_Style.outlineDistance, newBounds.y-m_Style.outlineDistance, newBounds.width+m_Style.outlineDistance*2, newBounds.height+m_Style.outlineDistance*2 }, m_Style.roundness, SEGMENTS, m_Style.outlineThickness, outlineColor);

  // Draw the base rectangle, button background
  DrawRectangleRounded(newBounds, m_Style.roundness, SEGMENTS, backgroundColor);

  // Draw text based off the text alignment
  switch (m_Style.textAlignment)
  {
    case TEXT_ALIGNMENT_LEFT:
      DrawTextEx(m_Style.font, m_Text.c_str(), { m_Bounds.x+5, m_Bounds.y+(m_Bounds.height/2)-((float)m_Style.fontSize/2) }, fontSize, SPACING, textColor);
      break;
    case TEXT_ALIGNMENT_CENTER:
      DrawTextEx(m_Style.font, m_Text.c_str(), { m_Bounds.x+(m_Bounds.width/2)-(MeasureTextEx(m_Style.font, m_Text.c_str(), fontSize, SPACING).x/2), m_Bounds.y+(m_Bounds.height/2)-((float)fontSize/2) }, fontSize, SPACING, textColor);
      break;
    case TEXT_ALIGNMENT_RIGHT:
      DrawTextEx(m_Style.font, m_Text.c_str(), { m_Bounds.x+m_Bounds.width-5-MeasureTextEx(m_Style.font, m_Text.c_str(), fontSize, SPACING).x, m_Bounds.y+(m_Bounds.height/2)-((float)fontSize/2) }, fontSize, SPACING, textColor);
      break;
  }

  // Return if button clicked
  return clicked;
}
