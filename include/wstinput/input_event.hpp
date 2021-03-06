// Windstille Input Library
// Copyright (C) 2005-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_WINDSTILLE_INPUT_INPUT_EVENT_HPP
#define HEADER_WINDSTILLE_INPUT_INPUT_EVENT_HPP

#include <array>
#include <vector>

#include <SDL.h>

namespace wstinput {

enum InputEventType
{
  BUTTON_EVENT,
  AXIS_EVENT,
  BALL_EVENT,
  POINTER_EVENT,
  TEXT_EVENT,
  TEXT_EDIT_EVENT,
  KEYBOARD_EVENT
};

/** Used for textual input */
struct TextEvent
{
  std::array<char, 32> text;
};

struct TextEditEvent
{
  std::array<char, 32> text;
  int start;
  int length;
};

/** Raw keyboard events, only send when text input is active */
struct KeyboardEvent
{
  SDL_KeyboardEvent key;
};

struct ButtonEvent
{
  int name;

  /** true if down, false if up */
  bool down;

  bool is_down() const { return down; }
  bool is_up()   const { return !down; }
};

struct BallEvent
{
  int   name;
  float pos;
  float get_pos() const { return pos; }
};

struct PointerEvent
{
  int   name;
  float pos;
  float get_pos() const { return pos; }
};

struct AxisEvent
{
  int name;

  /** Pos can be in range from [-1.0, 1.0], some axis will only use [0,1.0] */
  float pos;

  float get_pos() const { return pos; }
};

struct InputEvent
{
  InputEventType type;

  union
  {
    struct ButtonEvent button;
    struct AxisEvent axis;
    struct TextEvent text;
    struct TextEditEvent text_edit;
    struct KeyboardEvent keyboard;
    struct BallEvent ball;
  };
};

using InputEventLst = std::vector<InputEvent>;

} // namespace wstinput

#endif

/* EOF */
