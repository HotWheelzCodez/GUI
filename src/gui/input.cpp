#include "../../include/gui.hpp"

GUI::Input::Input(void) = default;

GUI::Input::Input(Rectangle bounds, GUI::InputStyle style, const std::string& m_PlaceholderText)
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

size_t GUI::Input::FindLeftOf(const std::string& str, const char toFind, size_t startIndex) noexcept
{
  if (!startIndex)
    return 0;

  for (size_t i = startIndex-1; i > 0; i--)
  {
    if (str[i] == toFind)
      return i;
  }

  return 0;
}

size_t GUI::Input::FindRightOf(const std::string& str, const char toFind, size_t startIndex) noexcept
{
  for (size_t i = startIndex+1; i < str.length(); i++)
  {
    if (str[i] == toFind)
      return i;
  }

  return str.length();
}

void GUI::Input::ReplaceAll(std::string& str, const char toReplace, const char replaceWith) noexcept
{
  for (char& c : str)
  {
    if (c == toReplace)
      c = replaceWith;
  }
}

void GUI::Input::DrawCursor(void) noexcept
{
  int x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x+m_XOffset;
  if (x >= m_Bounds.width-10) 
  {
    size_t textWidth = MeasureTextEx(m_Style.font, m_InputText.c_str(), m_Style.fontSize, SPACING).x;
    m_XOffset = m_Bounds.width-textWidth-15;
    x = m_Bounds.width-10;
  }
  else if (x < 0)
  {
    m_XOffset = 0;
    x = 0;
  }

  DrawRectangle(m_Bounds.x+5+x, m_Bounds.y+3, 2, m_Bounds.height-6, WHITE);
}

void GUI::Input::UpdateCursorPosition(GUI::MouseState& mouseState) noexcept
{
  if (!mouseState.clicked && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouseState.position, m_Bounds))
  {
    int x = (mouseState.position.x)-m_Bounds.x-m_XOffset;
    float scaleFactor = m_Style.fontSize/(float)m_Style.font.baseSize;
    int textWidth = 0;
    int tempWidth = 0;
    int byteCount = 0;
    for (size_t i = 0; i < TextLength(m_InputText.c_str());)
    {
      byteCount++;

      int next;
      const char letter = GetCodepointNext(m_InputText.c_str()+i, &next);
      int index = GetGlyphIndex(m_Style.font, letter);

      i += next;

      if (m_Style.font.glyphs[index].advanceX)
        tempWidth += m_Style.font.glyphs[index].advanceX;
      else
        tempWidth += m_Style.font.recs[index].width + m_Style.font.glyphs[index].offsetX;

      textWidth = (tempWidth*scaleFactor+(float)((byteCount+10)*SPACING));

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
  Color backgroundColor = m_Style.baseBackgroundColor;
  Color outlineColor = m_Style.baseOutlineColor;
  Color textColor = m_Style.baseTextColor;

  if (CheckCollisionPointRec(mouseState.position, m_Bounds))
  {
    backgroundColor = m_Style.hoverBackgroundColor;
    outlineColor = m_Style.hoverOutlineColor;
    textColor = m_Style.hoverTextColor;

    mouseState.cursor = MOUSE_CURSOR_IBEAM;

    if (!mouseState.clicked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      m_Selected = true;
      mouseState.clicked = true;
    }
  }

  if (m_Selected)
  {
    backgroundColor = m_Style.selectedBackgroundColor;
    outlineColor = m_Style.selectedOutlineColor;
    textColor = m_Style.selectedTextColor;
  }

  if (m_Style.outlineFill)
    DrawRectangleRounded({ m_Bounds.x-m_Style.outlineDistance, m_Bounds.y-m_Style.outlineDistance, m_Bounds.width+m_Style.outlineDistance*2, m_Bounds.height+m_Style.outlineDistance*2 }, m_Style.roundness, SEGMENTS, outlineColor);
  else
    DrawRectangleRoundedLines({ m_Bounds.x-m_Style.outlineDistance, m_Bounds.y-m_Style.outlineDistance, m_Bounds.width+m_Style.outlineDistance*2, m_Bounds.height+m_Style.outlineDistance*2 }, m_Style.roundness, SEGMENTS, m_Style.outlineThickness, outlineColor);

  DrawRectangleRounded(m_Bounds, m_Style.roundness, SEGMENTS, backgroundColor);

  if (!m_InputText.length() && !m_Selected)
  {
    DrawTextEx(m_Style.font, m_PlaceholderText.c_str(), { m_Bounds.x+5, m_Bounds.y+(m_Bounds.height/2)-(m_Style.fontSize/2) }, m_Style.fontSize, SPACING, textColor);
    return;
  }

  int key = GetKeyPressed();

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
  else if (IsKeyDown(KEY_RIGHT))
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
            m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x+m_XOffset;
            m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x;
            m_HighlightText = m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition);
          }
          else
          {
            m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_HighlightStart).c_str(), m_Style.fontSize, SPACING).x+m_XOffset;
            m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart).c_str(), m_Style.fontSize, SPACING).x;
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
  else if (IsKeyDown(KEY_LEFT))
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
            m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart).c_str(), m_Style.fontSize, SPACING).x;
            m_HighlightText = m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart); 
          }
          else
          {
            m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x+m_XOffset;
            m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x;
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
      m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x;
      break;
    case KEY_RIGHT_SHIFT:
      m_HighlightStart = m_CursorPosition;
      m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x;
      break;
    case KEY_LEFT_CONTROL:
      break;
    case KEY_LEFT_SUPER:
      break;
    case KEY_LEFT:
      if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER))
        m_CursorPosition = FindLeftOf(m_InputText, ' ', m_CursorPosition);  
      else if (m_CursorPosition)
        m_CursorPosition--;
      if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
      {
        if (m_CursorPosition > m_HighlightStart)
        {
          m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart).c_str(), m_Style.fontSize, SPACING).x;
          m_HighlightText = m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart); 
          break;
        }

        m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x+m_XOffset;
        m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x;
        m_HighlightText = m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition);
      }
      else
      {
        m_HighlightText = "";
        m_HighlightBounds = { 0 };
      }
      break;
    case KEY_RIGHT:
    {
      if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER))
        m_CursorPosition = FindRightOf(m_InputText, ' ', m_CursorPosition);
      else if (m_CursorPosition < m_InputText.length())
        m_CursorPosition++;
      if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
      {
        if (m_CursorPosition < m_HighlightStart)
        {
          m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x;
          m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition).c_str(), m_Style.fontSize, SPACING).x;
          m_HighlightText = m_InputText.substr(m_CursorPosition, m_HighlightStart-m_CursorPosition);
          break;
        }   

        m_HighlightBounds.x = MeasureTextEx(m_Style.font, m_InputText.substr(0, m_HighlightStart).c_str(), m_Style.fontSize, SPACING).x+m_XOffset;
        m_HighlightBounds.y = MeasureTextEx(m_Style.font, m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart).c_str(), m_Style.fontSize, SPACING).x;
        m_HighlightText = m_InputText.substr(m_HighlightStart, m_CursorPosition-m_HighlightStart);
      }
      else
      {
        m_HighlightText = "";
        m_HighlightBounds = { 0 };
      }
      break;
    }
    default:
      if (IsKeyDown(KEY_LEFT_SHIFT))
      {
        if (key == KEY_LEFT_SHIFT)
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
        else if (key)
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

  if (m_HighlightText.length())
  {
    if (m_Bounds.x+m_HighlightBounds.x+m_HighlightBounds.y > m_Bounds.x+m_Bounds.width)
      m_HighlightBounds.y -= (m_Bounds.x+m_HighlightBounds.x+m_HighlightBounds.y)-(m_Bounds.x+m_Bounds.width)+10;
    DrawRectangle(m_Bounds.x+m_HighlightBounds.x+5, m_Bounds.y+3, m_HighlightBounds.y+5, m_Bounds.height-6, m_Style.highlightColor);
  }

  if (m_Selected)
  {
    UpdateCursorPosition(mouseState);
    DrawCursor();
  }

  BeginScissorMode(m_Bounds.x+5, m_Bounds.y+(m_Bounds.height/2)-5, m_Bounds.width-5, m_Bounds.height);
  DrawTextEx(m_Style.font, m_InputText.c_str(), { m_Bounds.x+5+m_XOffset, m_Bounds.y+(m_Bounds.height/2)-(m_Style.fontSize/2) }, m_Style.fontSize, SPACING, textColor);
  EndScissorMode();
}
