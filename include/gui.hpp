#pragma once

#include <stdint.h>
#include <string>

#include "raylib.h"

namespace GUI
{
  #define SEGMENTS 10
  #define SPACING 1

  typedef struct MouseState
  {
    Vector2 position;
    bool clicked;
    MouseCursor cursor;
  } MouseState;

  enum TextAlignments : uint8_t
  {
    TEXT_ALIGNMENT_LEFT = 0,
    TEXT_ALIGNMENT_CENTER,
    TEXT_ALIGNMENT_RIGHT,
  };

  typedef struct ButtonStyle
  {
    Color baseBackgroundColor;
    Color baseTextColor;
    Color baseOutlineColor;
    Color hoverBackgroundColor;
    Color hoverTextColor;
    Color hoverOutlineColor;
    Font  font;
    float fontSize;
    int   textAlignment;
    float roundness;
    float outlineThickness;
    float outlineDistance;
    bool  outlineFill;
    float hoverScale;
  } ButtonStyle;

  typedef struct InputStyle
  {
    Color baseBackgroundColor;
    Color baseOutlineColor;
    Color baseTextColor;
    Color basePlaceholderColor;
    Color hoverBackgroundColor;
    Color hoverOutlineColor;
    Color hoverTextColor;
    Color hoverPlaceholderColor;
    Color selectedBackgroundColor;
    Color selectedOutlineColor;
    Color selectedTextColor;
    Color highlightColor;
    Font  font;
    float fontSize;
    float roundness;
    float outlineThickness;
    float outlineDistance;
    bool  outlineFill;
  } InputStyle;

  class Button
  {
  public:
    Button(void);
    Button(Rectangle bounds, GUI::ButtonStyle style, const std::string& text);

    bool UpdateAndRender(GUI::MouseState& mouseState);

  private:
    Rectangle m_Bounds;
    GUI::ButtonStyle m_Style;
    std::string m_Text;
  };

  class Input
  {
  public:
    Input(void);
    Input(Rectangle bounds, GUI::InputStyle style, const std::string& placeholderText);

    void UpdateAndRender(GUI::MouseState& mouseState) noexcept;
    void SetPlaceholderText(const std::string& placeholderText) noexcept;
    void SetSelected(bool selected) noexcept;

  private:
    Rectangle m_Bounds;
    GUI::InputStyle m_Style;
    std::string m_PlaceholderText;
    std::string m_InputText;
    std::string m_HighlightText;
    Vector2 m_HighlightBounds;
    size_t m_HighlightStart;
    size_t m_CursorPosition;
    int m_XOffset;
    bool m_Selected;
    float m_TimeWaited;
    float m_KeyWaited;

  private:
    void DrawCursor(void) noexcept;
    void UpdateCursorPosition(GUI::MouseState& mouseState) noexcept;
    void ReplaceAll(std::string& str, const char toReplace, const char replaceWith) noexcept;
    size_t FindLeftOf(const std::string& str, const char toFind, size_t startIndex) noexcept;
    size_t FindRightOf(const std::string& str, const char toFind, size_t startIndex) noexcept;
  };
}
