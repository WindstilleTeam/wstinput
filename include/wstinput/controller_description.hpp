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

#ifndef HEADER_WINDSTILLE_INPUT_CONTROLLER_DESCRIPTION_HPP
#define HEADER_WINDSTILLE_INPUT_CONTROLLER_DESCRIPTION_HPP

#include <map>
#include <string>

#include "input_event.hpp"

namespace wstinput {

struct InputEventDefinition
{
  InputEventType type = {};
  int            id = {};
  std::string    name = {};
};

class ControllerDescription final
{
public:
  ControllerDescription();
  virtual ~ControllerDescription();

  void add_button(const std::string& name, int id);
  void add_axis(const std::string& name, int id);
  void add_ball(const std::string& name, int id);
  void add_pointer(const std::string& name, int id);

  const InputEventDefinition& get_definition(int id) const;
  const InputEventDefinition& get_definition(const std::string& name) const;

  size_t size() const { return str_to_event.size(); }
  int get_max_id() const;

private:
  std::map<std::string, InputEventDefinition> str_to_event;
  std::map<int,         InputEventDefinition> id_to_event;
};

} // namespace wstinput

#endif

/* EOF */
