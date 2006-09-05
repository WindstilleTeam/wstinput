/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_INPUT_CONFIGURATOR_HPP
#define HEADER_INPUT_CONFIGURATOR_HPP

#include "screen.hpp"
#include "text_area.hpp"

struct ConfigureItem
{
  enum Mode { CONFIGURE_AXIS, CONFIGURE_BUTTON };
  Mode mode;
  int  event_id;
};

/** */
class InputConfigurator : public Screen
{
private:
  std::vector<ConfigureItem> items;
  bool wait_for_plus;
  SDL_Event minus;
  std::ostringstream out;
  TextArea area;
  
public:
  InputConfigurator();
  ~InputConfigurator();
  
  void draw();
  void update(float delta, const Controller& controller);
  void handle_event(const SDL_Event& event);
  void add_configure_item(ConfigureItem::Mode mode, int event_id);
  void next_item();
  void print_item();
  
private:
  InputConfigurator(const InputConfigurator&);
  InputConfigurator& operator=(const InputConfigurator&);
};

#endif

/* EOF */
