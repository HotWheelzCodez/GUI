#pragma once /* GUI_HPP */

#include <stdint.h>
#include <string>
#include <vector>

#include "raylib.h"

/* GUI namespace - start 
 * 
 * This namespace will be used to manage all the needed GUI funictionalty,
 * this will only be used in the 2d render stage of the game
 * */
namespace GUI
{
  /* MACROS & VARIABLES */

  #define SEGMENTS 10 // Predefined number of segments for any rounded object
  #define SPACING 1   // Set space in between characters when renderin text

  #define TRANSPARENT { 0, 0, 0, 0 } // Transparent color

  static float fontScale;

  /* STRUCTURES & ENUMS */

  // Used to store the state of the mouse per frame
  typedef struct MouseState
  {
    Vector2 position;
    bool clicked;
    MouseCursor cursor;
  } MouseState;

  // Used to represent text alignment, for other GUI components
  enum TextAlignments : uint8_t
  {
    TEXT_ALIGNMENT_LEFT = 0,
    TEXT_ALIGNMENT_CENTER,
    TEXT_ALIGNMENT_RIGHT,
  };

  // Properties for styling GUI Text component
  typedef struct TextStyle
  {
    Color   backgroundColor;
    Color   textColor;
    Color   backgroundHoverColor;
    Color   textHoverColor;
    Font    font;
    float   fontSize;
    int     textAlignment;
    Vector2 padding;
  } TextStyle;

  // Properties for styling GUI Button component
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

  // Properties for styling GUI Input component
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

  // Properties for styling GUI ScrollArea component
  typedef struct ScrollAreaStyle
  {
    Color backgroundColor;
    Color scrollbarColor;
    Color backgroundHoverColor;
    Color scrollbarHoverColor;
    float roundness;
    float scrollSpeed;
  } ScrollAreaStyle;

  /* CLASSES */

  // Text class, GUI Text component, used for rendering styled text element
  class Text
  {
  public:
    // Empty default constructor
    Text(void) = default; 
    // Normal constructore for initalizing GUI text component
    Text(Rectangle bounds, GUI::TextStyle style, const std::string& text) noexcept; 

    // Updates GUI text component as needed, than renders it
    void UpdateAndRender(GUI::MouseState& mouseState) noexcept;

    // Allows you to change GUI text component properties
    void SetText(const std::string& text) noexcept;
    void SetBounds(Rectangle bounds) noexcept;

  private:
    Rectangle m_Bounds;
    GUI::TextStyle m_Style;
    std::vector<std::string> m_TextLines;
  };

  // Button class, GUI Button component, used for rendering styled button element
  class Button
  {
  public:
    // Empty default constructor
    Button(void) = default;
    // Normal constructore for initalizing GUI button component
    Button(Rectangle bounds, GUI::ButtonStyle style, const std::string& text) noexcept;

    // Updates GUI button component as needed, than renders it
    bool UpdateAndRender(GUI::MouseState& mouseState) noexcept;

    // Allows you to change GUI button component properties
    void SetText(const std::string& text) noexcept;
    void SetBounds(Rectangle bounds) noexcept;

  private:
    Rectangle m_Bounds;
    GUI::ButtonStyle m_Style;
    std::string m_Text;
  };

  // Input class, GUI Input component, used for rendering styled input element
  class Input
  {
  public:
    // Empty default constructor
    Input(void) = default;
    // Normal constructore for initalizing GUI button component
    Input(Rectangle bounds, GUI::InputStyle style, const std::string& placeholderText) noexcept;

    // Updates GUI input component as needed, than renders it
    void UpdateAndRender(GUI::MouseState& mouseState) noexcept;

    // Allows you to change GUI input component properties
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
    // Will draw the input cursor
    void DrawCursor(void) noexcept;
    // Updates input cursor position based off user input
    void UpdateCursorPosition(GUI::MouseState& mouseState) noexcept;
    // Helper string functions
    void ReplaceAll(std::string& str, const char toReplace, const char replaceWith) const noexcept;
    [[nodiscard]] size_t FindLeftOf(const std::string& str, const char toFind, size_t startIndex) const noexcept;
    [[nodiscard]] size_t FindRightOf(const std::string& str, const char toFind, size_t startIndex) const noexcept;
  };

  // ScrollArea class, GUI ScrollArea component, used for rendering styled scroll area element
  class ScrollArea
  {
  public:
    // Empty default constructor
    ScrollArea(void) = default;
    // Normal constructore for initalizing GUI button component
    ScrollArea(Rectangle bounds, GUI::ScrollAreaStyle style) noexcept;

    // Updates GUI scroll area component as needed, than renders it
    void UpdateAndRender(GUI::MouseState& mouseState, float itemHeight, std::vector<std::string> info, void (*renderItem)(Rectangle parentItem, std::string parameter)) noexcept;

    // Helper functions for calculation render offsets
    [[nodiscard]] Vector2 GetOffset(void) noexcept;
    [[nodiscard]] float GetYOffset(void) noexcept;
    [[nodiscard]] float GetXOffset(void) noexcept;

    // Allows you to change GUI scroll area component properties
    void SetBounds(Rectangle bounds) noexcept;

  private:
    Rectangle m_Bounds;
    GUI::ScrollAreaStyle m_Style;
    Vector2 m_Offset;
  };

  /* FUNCTIONS */

  // Calculates the font scale based off the monitor width and target resolution
  [[nodiscard]] float CalculateFontScale(int monitorWidth) noexcept;
} /* GUI namespace - end */
/* GUI_HPP */
