#include "../../include/gui.hpp"

// Base constructor to initalize private fields
GUI::ScrollArea::ScrollArea(Rectangle bounds, GUI::ScrollAreaStyle style) noexcept
  : m_Bounds(std::move(bounds)), m_Style(std::move(style)), m_Offset({ 0, 0 })
{ }

Vector2 GUI::ScrollArea::GetOffset(void) noexcept
{
  return m_Offset;
}

float GUI::ScrollArea::GetYOffset(void) noexcept
{
  return m_Offset.y;
}

float GUI::ScrollArea::GetXOffset(void) noexcept
{
  return m_Offset.x;
}

void GUI::ScrollArea::SetBounds(Rectangle bounds) noexcept
{
  m_Bounds = bounds;
}

void GUI::ScrollArea::UpdateAndRender(GUI::MouseState& mouseState, float itemHeight, std::vector<std::string> info, void (*renderItem)(Rectangle parentBounds, std::string parameters)) noexcept
{
  // Get mouse scroll input and update offset based off of that
  Vector2 scroll = GetMouseWheelMoveV(); 
  m_Offset.x += scroll.x * m_Style.scrollSpeed;
  m_Offset.y += scroll.y * m_Style.scrollSpeed;

  // Clamp the y offset so that it does not exceeded the render bounds
  float maxScrollY = std::max(0.0f, info.size()*itemHeight-m_Bounds.height);
  if (m_Offset.y < 0.0f) 
    m_Offset.y = 0.0f;
  else if (m_Offset.y > maxScrollY)
    m_Offset.y = maxScrollY;

  // Set base properties
  Color backgroundColor = m_Style.backgroundColor;

  // If mouse is hovered, update background
  if (CheckCollisionPointRec(mouseState.position, m_Bounds))
    backgroundColor = m_Style.backgroundHoverColor;

  // Render background and begin the scissor mode, to only render in the scroll area
  DrawRectangleRounded(m_Bounds, m_Style.roundness, SEGMENTS, backgroundColor);
  BeginScissorMode(m_Bounds.x+5, m_Bounds.y+5, m_Bounds.width-10, m_Bounds.height-10); 

  // Calculate new bounds based off scroll offset
  Rectangle offsetBounds = m_Bounds;
  offsetBounds.y -= m_Offset.y;

  // Iterate over the given parameters for each item to be rendered
  float yOffset = 0.0f;
  for (std::string parameters : info)
  {
    // If item is within in the render bounds, render that item with the parameter
    if (offsetBounds.y+itemHeight > m_Bounds.y && offsetBounds.y < m_Bounds.y+m_Bounds.height) 
      renderItem(offsetBounds, parameters);

    offsetBounds.y += itemHeight + 5;
  }

  EndScissorMode();
}
