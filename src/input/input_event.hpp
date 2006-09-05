/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_INPUT_EVENT_HXX
#define HEADER_INPUT_EVENT_HXX

#include <vector>

enum InputEventType { BUTTON_EVENT, AXIS_EVENT, BALL_EVENT, KEYBOARD_EVENT };

/** Used for textual input */
struct KeyboardEvent
{
  enum KeyType { LETTER, SPECIAL } key_type;
  int code;
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
    struct ButtonEvent   button;
    struct AxisEvent     axis;
    struct KeyboardEvent keyboard;
    struct BallEvent     ball;
  };
};

typedef std::vector<InputEvent> InputEventLst;

#endif

/* EOF */
