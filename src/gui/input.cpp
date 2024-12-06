#include "../../include/gui.hpp"
#include <iostream>

GUI::Input::Input(Rectangle bounds, GUI::InputStyle style, const std::string& m_PlaceholderText) noexcept
  : m_Bounds(bounds), m_Style(style), m_PlaceholderText(m_PlaceholderText), m_CursorPosition(0), m_InputText(""), m_Selected(false), m_HighlightText("")
{ }

void GUI::Input::SetPlaceholderText(const std::string& placeholderText) noexcept
{
  m_PlaceholderText = placeholderText;
}

void GUI::Input::SetSelected(bool selected) noexcept
{
  m_Selected = selected;
}

size_t GUI::Input::FindLeftOf(const std::string& str, const char toFind, size_t startIndex) const noexcept
{
  // Can't iterate behind the string lol
  if (!startIndex)
    return 0;

  // Iterate from start to the begining till we find the char, return index
  for (size_t i = startIndex-1; i > 0; i--)
  {
    if (str[i] == toFind)
      return i;
  }

  // Return left most, since it could not be found
  return 0;
}

size_t GUI::Input::FindRightOf(const std::string& str, const char toFind, size_t startIndex) const noexcept
{
  // Iterate from the start to end, till we find the char, return index
  for (size_t i = startIndex+1; i < str.length(); i++)
  {
    if (str[i] == toFind)
      return i;
  }

  // Return right most, since it could not be found
  return str.length();
}

// Replace all occurances of a char in the str, with another
void GUI::Input::ReplaceAll(std::string& str, const char toReplace, const char replaceWith) const noexcept
{
  for (char& c : str)
  {
    if (c == toReplace)
      c = replaceWith;
  }
}

void GUI::Input::DrawCursor(void) noexcept
{
  // Get updated font size from font scale
  float fontSize = m_Style.fontSize * GUI::fontScale;
  // Get cursor x position based off the cursor position
  int x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), fontSize, SPACING).x+m_XOffset;
  // Ensure the cursor position does not exceed the width of the input box
  if (x >= m_Bounds.width-10) 
  {
    size_t textWidth = MeasureTextEx(m_Style.font, m_InputText.c_str(), fontSize, SPACING).x;
    m_XOffset = m_Bounds.width-textWidth-15;
    x = m_Bounds.width-10;
  }
  else if (x < 0) // Ensure it does not exceeded the minimum bounds of the input box either
  {
    m_XOffset = 0;
    x = 0;
  }

  // Draw the cursor itself
  DrawRectangle(m_Bounds.x+5+x, m_Bounds.y+3, 2, m_Bounds.height-6, WHITE);
}

void GUI::Input::UpdateCursorPosition(GUI::MouseState& mouseState) noexcept
{
  // Get font size based off the font scale
  float fontSize = m_Style.fontSize * GUI::fontScale;
  // Check to see if input has been selected, if mouse not previouly clicked this frame
  if (!mouseState.clicked && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouseState.position, m_Bounds))
  {
    int x = (mouseState.position.x)-m_Bounds.x-m_XOffset; // Get the x position of the cursor relative to the input box
    float scaleFactor = fontSize/(float)m_Style.font.baseSize; // Get the font scale factor
    int textWidth = 0; // Stores the text width
    int tempWidth = 0; // Stores the temp width without style property calculations
    int byteCount = 0; // Stores the number of bytes iterated over
    for (size_t i = 0; i < m_InputText.length();) // Iterate through each char (length of the string)
    {
      byteCount++;

      // Get the next letter
      int next;
      const char letter = GetCodepointNext(m_InputText.c_str()+i, &next); // Get letter codepoint
      int index = GetGlyphIndex(m_Style.font, letter); // Get the glyph index of that letter

      i += next; // Increment our counter based off the letter value

      if (m_Style.font.glyphs[index].advanceX)
        tempWidth += m_Style.font.glyphs[index].advanceX; // Add the bare width of the glyph
      else
        tempWidth += m_Style.font.recs[index].width + m_Style.font.glyphs[index].offsetX; // Increment the temp width based off the offset

      // Calculate the real text width based off additonal properties
      textWidth = (tempWidth*scaleFactor+(float)((byteCount+10)*SPACING));

      // Update cursor position once we have reached the x position of our mouse cursor
      if (textWidth >= x)
      {
        m_CursorPosition = i;
        break;
      }
    }
  }
}

void GUI::Input::UpdateAndRender(GUI::MouseState& mouseState) noexcept
{
  // Base properties before user input, calculate font size from font scale
  Color backgroundColor = m_Style.baseBackgroundColor;
  Color outlineColor = m_Style.baseOutlineColor;
  Color textColor = m_Style.baseTextColor;
  float fontSize = m_Style.fontSize * GUI::fontScale;

  // Mouse is hovering
  if (CheckCollisionPointRec(mouseState.position, m_Bounds))
  {
    // Update input properties
    backgroundColor = m_Style.hoverBackgroundColor;
    outlineColor = m_Style.hoverOutlineColor;
    textColor = m_Style.hoverTextColor;

    // Update mouse cursor
    mouseState.cursor = MOUSE_CURSOR_IBEAM;

    // Select input if mouse has not previouly been clicked this frame
    if (!mouseState.clicked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      m_Selected = true;
      mouseState.clicked = true;
    }
  }

  // If selected update input properties
  if (m_Selected)
  {
    backgroundColor = m_Style.selectedBackgroundColor;
    outlineColor = m_Style.selectedOutlineColor;
    textColor = m_Style.selectedTextColor;
  }

  // If outline fill, draw up to outline, else draw outline lines
  if (m_Style.outlineFill)
    DrawRectangleRounded({ m_Bounds.x-m_Style.outlineDistance, m_Bounds.y-m_Style.outlineDistance, m_Bounds.width+m_Style.outlineDistance*2, m_Bounds.height+m_Style.outlineDistance*2 }, m_Style.roundness, SEGMENTS, outlineColor);
  else
    DrawRectangleRoundedLinesEx({ m_Bounds.x-m_Style.outlineDistance, m_Bounds.y-m_Style.outlineDistance, m_Bounds.width+m_Style.outlineDistance*2, m_Bounds.height+m_Style.outlineDistance*2 }, m_Style.roundness, SEGMENTS, m_Style.outlineThickness, outlineColor);

  // Draw background of input
  DrawRectangleRounded(m_Bounds, m_Style.roundness, SEGMENTS, backgroundColor);

  // Draw placeholder text if there is no other text to draw, and not selected
  if (!m_InputText.length() && !m_Selected)
  {
    DrawTextEx(m_Style.font, m_PlaceholderText.c_str(), { m_Bounds.x+5, m_Bounds.y+(m_Bounds.height/2)-(fontSize/2) }, fontSize, SPACING, textColor);
    return;
  }

  int key = GetKeyPressed(); // Get the input key

  // Check backspace repeat
  if (IsKeyDown(KEY_BACKSPACE))
  {
    if (m_TimeWaited >= 0.5f)
    {
      if (m_KeyWaited >= 0.05f && m_CursorPosition) 
      {
        m_CursorPosition--;
        if (m_CursorPosition == m_InputText.length())
        {
          m_InputText.pop_back();
        }
        else
        {
          m_InputText.erase(m_InputText.begin()+m_CursorPosition);
        }
        m_KeyWaited = 0;
      }
      m_KeyWaited += GetFrameTime();
    }
    m_TimeWaited += GetFrameTime();
  }
  else if (IsKeyDown(KEY_RIGHT)) // Check right key repeat
  {
    if (m_TimeWaited >= 0.5f) 
    {
      if (m_KeyWaited >= 0.05f && m_CursorPosition < m_InputText.length())
      {
        m_CursorPosition++;  
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        {
          if (m_CursorPosition < m_HighlightStart)
          {
            m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), fontSize, SPACING).x+m_XOffset;
            m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition).c_str(), fontSize, SPACING).x;
            m_HighlightText = m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition);
          }
          else
          {
            m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_HighlightStart).c_str(), fontSize, SPACING).x+m_XOffset;
            m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart).c_str(), fontSize, SPACING).x;
            m_HighlightText = m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart);
          }
        }
        else
        {
          m_HighlightText = "";
          m_HighlightBounds = { 0 };
        }
        m_KeyWaited = 0;
      }
      m_KeyWaited += GetFrameTime();
    }
    m_TimeWaited += GetFrameTime();
  }
  else if (IsKeyDown(KEY_LEFT)) // Check left key repeat
  {
    if (m_TimeWaited >= 0.5f) 
    {
      if (m_KeyWaited >= 0.05f && m_CursorPosition)
      {
        m_CursorPosition--;  
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
        {
          if (m_CursorPosition > m_HighlightStart)
          {
            m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart).c_str(), fontSize, SPACING).x;
            m_HighlightText = m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart); 
          }
          else
          {
            m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), fontSize, SPACING).x+m_XOffset;
            m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition).c_str(), fontSize, SPACING).x;
            m_HighlightText = m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition);
          }
        }
        else
        {
          m_HighlightText = "";
          m_HighlightBounds = { 0 };
        }
        m_KeyWaited = 0;
      }
      m_KeyWaited += GetFrameTime();
    }
    m_TimeWaited += GetFrameTime();
  }
  else
  {
    m_TimeWaited = 0;
  }

  // Update the stored texted based off the input key
  switch (key)
  {
    case KEY_BACKSPACE:
      if (!m_CursorPosition)
        break;

      m_CursorPosition--;
      if (m_CursorPosition == m_InputText.length())
        m_InputText.pop_back();
      else
        m_InputText.erase(m_InputText.begin()+m_CursorPosition);
      break;
    case KEY_LEFT_SHIFT:
      m_HighlightStart = m_CursorPosition;
      m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), fontSize, SPACING).x;
      break;
    case KEY_RIGHT_SHIFT:
      m_HighlightStart = m_CursorPosition;
      m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), fontSize, SPACING).x;
      break;
    case KEY_LEFT_CONTROL:
      break;
    case KEY_LEFT_SUPER:
      break;
    case KEY_LEFT: // If key left move cursor left
      if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER))
        m_CursorPosition = FindLeftOf(m_InputText, ' ', m_CursorPosition);  
      else if (m_CursorPosition)
        m_CursorPosition--;
      if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
      {
        if (m_CursorPosition > m_HighlightStart)
        {
          m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart).c_str(), fontSize, SPACING).x;
          m_HighlightText = m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart); 
          break;
        }

        m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), fontSize, SPACING).x+m_XOffset;
        m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition).c_str(), fontSize, SPACING).x;
        m_HighlightText = m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition);
      }
      else
      {
        m_HighlightText = "";
        m_HighlightBounds = { 0 };
      }
      break;
    case KEY_RIGHT: // If key right move cursor right
    {
      if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER))
        m_CursorPosition = FindRightOf(m_InputText, ' ', m_CursorPosition);
      else if (m_CursorPosition < m_InputText.length())
        m_CursorPosition++;
      if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
      {
        if (m_CursorPosition < m_HighlightStart)
        {
          m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), fontSize, SPACING).x;
          m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition).c_str(), fontSize, SPACING).x;
          m_HighlightText = m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition);
          break;
        }   

        m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_HighlightStart).c_str(), fontSize, SPACING).x+m_XOffset;
        m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart).c_str(), fontSize, SPACING).x;
        m_HighlightText = m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart);
      }
      else
      {
        m_HighlightText = "";
        m_HighlightBounds = { 0 };
      }
      break;
    }
    default: // Proccess normal keys
      if (IsKeyDown(KEY_LEFT_SHIFT)) 
      {
        if (key == KEY_LEFT_SHIFT) // Handle upercase keys
          break;

        if (key >= 'A' && key <= 'Z')
        {
          if (m_CursorPosition == m_InputText.length())
          {
            m_InputText += (char)key;
            m_CursorPosition = m_InputText.length();
          }
          else
          {
            std::string toInput;
            toInput += (char)key;
            m_InputText.insert(m_CursorPosition, toInput);
            m_CursorPosition++;
          }
        }
        else if (key) // Handle special keys
        {
          std::string toInput;
          switch (key)
          {
            case '0':
              toInput += ')';
              break;
            case '1':
              toInput += '!';
              break;
            case '2':
              toInput += '@';
              break;
            case '3':
              toInput += '#';
              break;
            case '4':
              toInput += '$';
              break;
            case '5':
              toInput += '%';
              break;
            case '6':
              toInput += '^';
              break;
            case '7':
              toInput += '&';
              break;
            case '8':
              toInput += '*';
              break;
            case '9':
              toInput += '(';
              break;
            case '-':
              toInput += '_';
              break;
            case '=':
              toInput += '+';
              break;
            case '[':
              toInput += '{';
              break;
            case ']':
              toInput += '}';
              break;
            case '\\':
              toInput += '|';
              break;
            case '`':
              toInput += '~';
              break;
            case ';':
              toInput += ':';
              break;
            case '\'':
              toInput += '"';
              break;
            case ',':
              toInput += '<';
              break;
            case '.':
              toInput += '>';
              break;
            case '/':
              toInput += '?';
              break;
          }

          if (m_CursorPosition == m_InputText.length())
          {
            m_InputText += toInput;
            m_CursorPosition = m_InputText.length();
          }
          else
          {
            m_InputText.insert(m_CursorPosition, toInput);
            m_CursorPosition++;
          }
        }
      }
      else
      {
        // Handle copy/paste and other shortcuts as well as lowercase keys
        if (key >= 'A' && key <= 'Z')
        {
          if (key == 'C' && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER)))
          {
            SetClipboardText(m_HighlightText.c_str());
          }
          else if (key == 'V' && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER)))
          {
            if (m_CursorPosition == m_InputText.length())
            {
              std::string clipboard = GetClipboardText();
              ReplaceAll(clipboard, '\n', ' ');
              m_InputText += clipboard;
              m_CursorPosition += clipboard.length();
            }
            else
            {
              std::string clipboard = GetClipboardText();
              m_InputText.insert(m_CursorPosition, clipboard);
              m_CursorPosition += clipboard.length(); 
            }
          }
          else if (m_CursorPosition == m_InputText.length())
          {
            m_InputText += (char)(key+32);
            m_CursorPosition = m_InputText.length();
          }
          else
          {
            std::string toInput;
            toInput += (char)(key+32);
            m_InputText.insert(m_CursorPosition, toInput);
            m_CursorPosition++;
          }
        }
        else if (key)
        {
          if (key == KEY_LEFT_CONTROL || key == KEY_LEFT_SUPER)
            break;

          if (m_CursorPosition == m_InputText.length())
          {
            m_InputText += (char)key;
            m_CursorPosition = m_InputText.length();
          }
          else
          {
            std::string toInput;
            toInput += (char)key;
            m_InputText.insert(m_CursorPosition, toInput);
            m_CursorPosition++;
          }
        }
      }
  }

  // Draw highlight background
  if (m_HighlightText.length())
  {
    if (m_Bounds.x+m_HighlightBounds.x+m_HighlightBounds.y > m_Bounds.x+m_Bounds.width)
      m_HighlightBounds.y -= (m_Bounds.x+m_HighlightBounds.x+m_HighlightBounds.y)-(m_Bounds.x+m_Bounds.width)+10;
    DrawRectangle(m_Bounds.x+m_HighlightBounds.x+5, m_Bounds.y+3, m_HighlightBounds.y+5, m_Bounds.height-6, m_Style.highlightColor);
  }

  // Draw cursor after updating it
  if (m_Selected)
  {
    UpdateCursorPosition(mouseState);
    DrawCursor();
  }

  // Draw wrapped text
  BeginScissorMode(m_Bounds.x+5, m_Bounds.y+(m_Bounds.height/2)-(5*GUI::fontScale), m_Bounds.width-5, m_Bounds.height);
  DrawTextEx(m_Style.font, m_InputText.c_str(), { m_Bounds.x+5+m_XOffset, m_Bounds.y+(m_Bounds.height/2)-(fontSize/2) }, fontSize, SPACING, textColor);
  EndScissorMode();
}
