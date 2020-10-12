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

#include <iostream>
#include <sstream>

#include <prio/reader.hpp>

#include "input_manager.hpp"
#ifdef HAVE_CWIID
#  include "wiimote.hpp"
#endif

using namespace prio;

namespace wstinput {

const int dead_zone = 0;

InputManagerSDL::InputManagerSDL(const ControllerDescription& controller_description)
  : m_controller_description(controller_description),
    m_controller(controller_description.get_max_id() + 1),
    m_joystick_button_bindings(),
    m_joystick_button_axis_bindings(),
    m_joystick_axis_bindings(),
    m_joystick_axis_button_bindings(),
    m_keyboard_button_bindings(),
    m_keyboard_axis_bindings(),
    m_mouse_button_bindings(),
    m_wiimote_button_bindings(),
    m_wiimote_axis_bindings(),
    m_joysticks(),
    m_keyidmapping()
{
  for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
    const char* key_name = SDL_GetScancodeName(static_cast<SDL_Scancode>(i));
    m_keyidmapping[key_name] = static_cast<SDL_Scancode>(i);
    // FIXME: Make the keynames somewhere user visible so that users can use them
    std::cout << "'" << key_name << "'" << std::endl;
  }

#ifdef HAVE_CWIID
  // FIXME: doesn't really belong here
  Wiimote::init();

  if (wiimote && config.get<bool>("wiimote").get())
    wiimote->connect();

#endif // HAVE_CWIID
}

InputManagerSDL::~InputManagerSDL()
{
#ifdef HAVE_CWIID
  Wiimote::deinit();
#endif
}

std::string
InputManagerSDL::keyid_to_string(SDL_Scancode id) const
{
  return SDL_GetKeyName(id);
}

SDL_Scancode
InputManagerSDL::string_to_keyid(const std::string& str) const
{
  auto const it = m_keyidmapping.find(str);
  if (it == m_keyidmapping.end())
  {
    std::ostringstream msg;
    msg << "key lookup failure for '" << str << "'";
    throw std::runtime_error(msg.str());
  }
  else
  {
    return it->second;
  }
}

void
InputManagerSDL::ensure_open_joystick(int device)
{
  if (device >= int(m_joysticks.size()))
    m_joysticks.resize(device + 1, nullptr);

  if (!m_joysticks[device])
  {
    if (SDL_Joystick* joystick = SDL_JoystickOpen(device))
    {
      m_joysticks[device] = joystick;
    }
    else
    {
      std::cout << "InputManagerSDL: Couldn't open joystick device " << device << std::endl;
    }
  }

}

void
InputManagerSDL::load(std::filesystem::path const& filename)
{
  ReaderDocument doc = ReaderDocument::from_file(filename);

  std::cout << "InputManager: " << filename << std::endl;

  if (doc.get_name() != "windstille-controller") {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille-controller file";
    throw std::runtime_error(msg.str());
  }

  parse_config(doc.get_mapping());
}

void
InputManagerSDL::parse_config(ReaderMapping const& reader)
{
  for (auto const& key : reader.get_keys())
  {
    if (key.ends_with("-button"))
    {
      ReaderObject button_obj;
      reader.read(key, button_obj);
      ReaderMapping const& button_map = button_obj.get_mapping();

      if (button_obj.get_name() == "joystick-button") {
        int device = 0;
        int button = 0;

        button_map.read("device", device);
        button_map.read("button", button);

        bind_joystick_button(m_controller_description.get_definition(key).id,
                             device, button);
      } else if (button_obj.get_name() == "joystick-axis-button") {
        int  device;
        int  axis;
        bool up;

        button_map.read("device", device);
        button_map.read("axis", axis);
        button_map.read("up", up);

        bind_joystick_axis_button(m_controller_description.get_definition(key).id,
                                  device, axis, up);
      } else if (button_obj.get_name() == "wiimote-button") {
        int device = 0;
        int button = 0;

        button_map.read("device", device);
        button_map.read("button", button);

        bind_wiimote_button(m_controller_description.get_definition(key).id,
                            device, button);
      } else if (button_obj.get_name() == "keyboard-button") {
        std::string key_text;
        button_map.read("key", key_text);

        bind_keyboard_button(m_controller_description.get_definition(key).id,
                             string_to_keyid(key_text));
      } else {
        std::cout << "InputManagerSDL: Unknown tag: " << button_obj.get_name() << std::endl;
      }
    }
    else if (key.ends_with("-axis"))
    {
      ReaderObject axis_obj;
      reader.read(key, axis_obj);
      ReaderMapping const& axis_map = axis_obj.get_mapping();

        if (axis_obj.get_name() == "joystick-axis")
        {
          int  device = 0;
          int  axis   = 0;
          bool invert = false;

          axis_map.read("device", device);
          axis_map.read("axis",   axis);
          axis_map.read("invert", invert);

          bind_joystick_axis(m_controller_description.get_definition(key).id,
                             device, axis, invert);
        }
        else if (axis_obj.get_name() == "keyboard-axis")
        {
          std::string minus;
          std::string plus;

          axis_map.read("minus", minus);
          axis_map.read("plus",  plus);

          bind_keyboard_axis(m_controller_description.get_definition(key).id,
                             string_to_keyid(minus), string_to_keyid(plus));
        }
        else if (axis_obj.get_name() == "wiimote-axis")
        {
          int  device = 0;
          int  axis   = 0;

          axis_map.read("device", device);
          axis_map.read("axis",   axis);

          bind_wiimote_axis(m_controller_description.get_definition(key).id,
                            device, axis);
        }
        else
        {
          std::cout << "InputManagerSDL: Unknown tag: " << axis_obj.get_name() << std::endl;
        }
    }
  }
}

void
InputManagerSDL::on_key_event(const SDL_KeyboardEvent& event)
{
  // Dynamic bindings
  for (std::vector<KeyboardButtonBinding>::const_iterator i = m_keyboard_button_bindings.begin();
       i != m_keyboard_button_bindings.end();
       ++i)
  {
    if (event.keysym.scancode == i->key)
    {
      add_button_event(i->event, event.state);
    }
  }

  const Uint8* keystate = SDL_GetKeyboardState(nullptr);

  for (std::vector<KeyboardAxisBinding>::const_iterator i = m_keyboard_axis_bindings.begin();
       i != m_keyboard_axis_bindings.end();
       ++i)
  {
    if (event.keysym.scancode == i->minus)
    {
      if (event.state)
        add_axis_event(i->event, -1.0f);
      else if (!keystate[i->plus])
        add_axis_event(i->event, 0.0f);
    }
    else if (event.keysym.scancode == i->plus)
    {
      if (event.state)
      {
        add_axis_event(i->event, 1.0f);
      }
      else if (!keystate[i->minus])
      {
        add_axis_event(i->event, 0.0f);
      }
    }
  }
}

void
InputManagerSDL::on_mouse_button_event(const SDL_MouseButtonEvent& button)
{
  for (std::vector<MouseButtonBinding>::const_iterator i = m_mouse_button_bindings.begin();
       i != m_mouse_button_bindings.end();
       ++i)
  {
    if (button.button == i->button)
    {
      add_button_event(i->event, button.state);
    }
  }
}

void
InputManagerSDL::on_joy_button_event(const SDL_JoyButtonEvent& button)
{
  for (std::vector<JoystickButtonBinding>::const_iterator i = m_joystick_button_bindings.begin();
       i != m_joystick_button_bindings.end();
       ++i)
  {
    if (button.which  == i->device &&
        button.button == i->button)
    {
      add_button_event(i->event, button.state);
    }
  }

  for (std::vector<JoystickButtonAxisBinding>::const_iterator i = m_joystick_button_axis_bindings.begin();
       i != m_joystick_button_axis_bindings.end();
       ++i)
  {
    if (button.which  == i->device)
    {
      if (button.button == i->minus)
      {
        add_axis_event(i->event, button.state ? -1.0f : 0.0f);
      }
      else if (button.button == i->plus)
      {
        add_axis_event(i->event, button.state ?  1.0f : 0.0f);
      }
    }
  }
}

void
InputManagerSDL::on_joy_axis_event(const SDL_JoyAxisEvent& event)
{
  for (std::vector<JoystickAxisBinding>::const_iterator i = m_joystick_axis_bindings.begin();
       i != m_joystick_axis_bindings.end();
       ++i)
  {
    if (event.which  == i->device &&
        event.axis   == i->axis)
    {
      if (abs(event.value) > dead_zone)
      {
        add_axis_event(i->event, static_cast<float>(event.value) / (i->invert ? -32768.0f : 32768.0f));
      }
      else
      {
        add_axis_event(i->event, 0);
      }
    }
  }

  for(std::vector<JoystickAxisButtonBinding>::const_iterator i = m_joystick_axis_button_bindings.begin();
      i != m_joystick_axis_button_bindings.end();
      ++i)
  {
    if (event.which == i->device &&
        event.axis  == i->axis)
    {
      if (i->up)
      { // signal button press when axis is up
        if (event.value < -dead_zone)
          add_button_event(i->event, true);
        else
          add_button_event(i->event, false);
      }
      else
      { // signal button press when axis is down
        if (event.value > dead_zone)
          add_button_event(i->event, true);
        else
          add_button_event(i->event, false);
      }
    }
  }
}

void
InputManagerSDL::on_event(const SDL_Event& event)
{
  switch(event.type)
  {
    case SDL_TEXTINPUT:
#if 0
      if ((event.key.keysym.unicode > 0 && event.key.keysym.unicode < 128)
          && (isgraph(event.key.keysym.unicode) || event.key.keysym.unicode == ' '))
      {
        add_keyboard_event(0, KeyboardEvent::LETTER, event.key.keysym.unicode);
      }
      else
      {
        add_keyboard_event(0, KeyboardEvent::SPECIAL, event.key.keysym.sym);
      }
#endif
      break;

    case SDL_KEYUP:
    case SDL_KEYDOWN:
      on_key_event(event.key);
      break;

    case SDL_MOUSEMOTION:
      // event.motion
      // FIXME: Hardcodes 0,1 values are not a good idea, need to bind the stuff like the rest
      if ((false)) std::cout << "mouse: " << event.motion.xrel << " " << event.motion.yrel << std::endl;
      add_ball_event(0, static_cast<float>(event.motion.xrel));
      add_ball_event(1, static_cast<float>(event.motion.yrel));
      break;

    case SDL_MOUSEBUTTONDOWN:
      on_mouse_button_event(event.button);
      break;

    case SDL_MOUSEBUTTONUP:
      on_mouse_button_event(event.button);
      break;

    case SDL_JOYAXISMOTION:
      on_joy_axis_event(event.jaxis);
      break;

    case SDL_JOYBALLMOTION:
      // event.jball
      break;

    case SDL_JOYHATMOTION:
      // event.jhat
      break;

    case SDL_JOYBUTTONUP:
    case SDL_JOYBUTTONDOWN:
      on_joy_button_event(event.jbutton);
      break;

    default:
      std::cout << "InputManagerSDL: unknown event" << std::endl;
      break;
  }
}

void
InputManagerSDL::update(float /*delta*/)
{
#ifdef HAVE_CWIID
  if (wiimote && wiimote->is_connected())
  {
    // Check for new events from the Wiimote
    std::vector<WiimoteEvent> events = wiimote->pop_events();
    for(std::vector<WiimoteEvent>::iterator i = events.begin(); i != events.end(); ++i)
    {
      WiimoteEvent& event = *i;
      if (event.type == WiimoteEvent::WIIMOTE_BUTTON_EVENT)
      {
        //std::cout << "WiimoteButton: " << event.button.button << " " << event.button.down << std::endl;

        for (std::vector<WiimoteButtonBinding>::const_iterator j = m_wiimote_button_bindings.begin();
             j != m_wiimote_button_bindings.end();
             ++j)
        {
          if (event.button.device == j->device &&
              event.button.button == j->button)
          {
            add_button_event(j->event, event.button.down);
          }
        }
      }
      else if (event.type == WiimoteEvent::WIIMOTE_AXIS_EVENT)
      {
        //std::cout << "WiimoteAxis: " << event.axis.axis << " " << event.axis.pos << std::endl;

        for (std::vector<WiimoteAxisBinding>::const_iterator j = m_wiimote_axis_bindings.begin();
             j != m_wiimote_axis_bindings.end();
             ++j)
        {
          if (event.axis.device == j->device &&
              event.axis.axis == j->axis)
          {
            add_axis_event(j->event, event.axis.pos);
          }
        }
      }
      else if (event.type == WiimoteEvent::WIIMOTE_ACC_EVENT)
      {
        if (event.acc.accelerometer == 0)
        {
          if (0)
            printf("%d - %6.3f %6.3f %6.3f\n",
                   event.acc.accelerometer,
                   event.acc.x,
                   event.acc.y,
                   event.acc.z);

          float roll = atanf(static_cast<float>(event.acc.x / event.acc.z));
          if (event.acc.z <= 0.0) {
            roll += math::pi * ((event.acc.x > 0.0f) ? 1.0f : -1.0f);
          }
          roll *= -1;

          float pitch = atanf(event.acc.y / event.acc.z * cosf(roll));

          add_axis_event(X2_AXIS, math::mid(-1.0f, -float(pitch / M_PI), 1.0f));
          add_axis_event(Y2_AXIS, math::mid(-1.0f, -float(roll  / M_PI), 1.0f));

          std::cout << fmt::format("{:6.3f} {:6.3f}", pitch, roll) << std::endl;
        }
      }
      else
      {
        assert(!"Never reached");
      }
    }
  }
#endif
}

void
InputManagerSDL::bind_mouse_button(int event, int device, int button)
{
  MouseButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  m_mouse_button_bindings.push_back(binding);
}

void
InputManagerSDL::bind_joystick_hat_axis(int /*event*/, int /*device*/, int /*axis*/)
{
  std::cerr << "implement me\n";
}

void
InputManagerSDL::bind_joystick_button_axis(int event, int device, int minus, int plus)
{
  ensure_open_joystick(device);

  JoystickButtonAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.minus  = minus;
  binding.plus   = plus;

  m_joystick_button_axis_bindings.push_back(binding);
}

void
InputManagerSDL::bind_joystick_axis(int event, int device, int axis, bool invert)
{
  ensure_open_joystick(device);

  JoystickAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;
  binding.invert = invert;

  m_joystick_axis_bindings.push_back(binding);
}

void
InputManagerSDL::bind_joystick_button(int event, int device, int button)
{
  ensure_open_joystick(device);

  JoystickButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  m_joystick_button_bindings.push_back(binding);
}

void
InputManagerSDL::bind_joystick_axis_button(int event, int device, int axis, bool up)
{
  ensure_open_joystick(device);

  JoystickAxisButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;
  binding.up   = up;

  m_joystick_axis_button_bindings.push_back(binding);
}

void
InputManagerSDL::bind_keyboard_button(int event, SDL_Scancode key)
{
  KeyboardButtonBinding binding;

  binding.event = event;
  binding.key   = key;

  m_keyboard_button_bindings.push_back(binding);
}

void
InputManagerSDL::bind_keyboard_axis(int event, SDL_Scancode minus, SDL_Scancode plus)
{
  KeyboardAxisBinding binding;

  binding.event = event;
  binding.minus = minus;
  binding.plus  = plus;

  m_keyboard_axis_bindings.push_back(binding);
}

void
InputManagerSDL::bind_wiimote_button(int event, int device, int button)
{
  WiimoteButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  m_wiimote_button_bindings.push_back(binding);
}

void
InputManagerSDL::bind_wiimote_axis(int event, int device, int axis)
{
  WiimoteAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;

  m_wiimote_axis_bindings.push_back(binding);
}

void
InputManagerSDL::clear_bindings()
{
  m_joystick_button_bindings.clear();
  m_joystick_axis_bindings.clear();
  m_joystick_button_axis_bindings.clear();
  m_joystick_axis_button_bindings.clear();

  m_keyboard_button_bindings.clear();
  m_keyboard_axis_bindings.clear();

  m_mouse_button_bindings.clear();

  m_wiimote_button_bindings.clear();
  m_wiimote_axis_bindings.clear();
}

void
InputManagerSDL::add_ball_event(int name, float pos)
{
  InputEvent event;

  event.type = BALL_EVENT;
  event.axis.name = name;
  event.axis.pos  = pos;

  m_controller.add_event(event);
  m_controller.set_ball_state(name, pos);
}

void
InputManagerSDL::add_button_event(int name, bool down)
{
  InputEvent event;

  event.type = BUTTON_EVENT;
  event.button.name = name;
  event.button.down = down;

  m_controller.add_event(event);
  m_controller.set_button_state(name, down);
}

void
InputManagerSDL::add_keyboard_event(int , KeyboardEvent::KeyType key_type, int code)
{
  InputEvent event;

  event.type = KEYBOARD_EVENT;
  event.keyboard.key_type = key_type;
  event.keyboard.code     = code;

  m_controller.add_event(event);
}

void
InputManagerSDL::add_axis_event(int name, float pos)
{
#if 0
  // FIXME: reimplement this in some generic fashion

  // Convert analog axis events into digital menu movements
  // FIXME: add key repeat
  float click_threshold = 0.5f;
  float release_threshold = 0.3f;

  // FIXME: need state info
  float old_pos = m_controller.get_axis_state(name);
  if (name == X_AXIS)
  {
    if (m_controller.get_button_state(MENU_LEFT_BUTTON) == 0 &&
        pos < -click_threshold && old_pos > -click_threshold)
    {
      add_button_event(MENU_LEFT_BUTTON, 1);
    }
    else if (m_controller.get_button_state(MENU_LEFT_BUTTON) == 1 &&
             old_pos < -release_threshold && pos > -release_threshold)
    {
      add_button_event(MENU_LEFT_BUTTON, 0);
    }

    else if (m_controller.get_button_state(MENU_RIGHT_BUTTON) == 0 &&
             pos > click_threshold && old_pos < click_threshold)
    {
      add_button_event(MENU_RIGHT_BUTTON, 1);
    }
    else  if (m_controller.get_button_state(MENU_RIGHT_BUTTON) == 1 &&
              old_pos > release_threshold && pos < release_threshold)
    {
      add_button_event(MENU_RIGHT_BUTTON, 0);
    }
  }
  else if (name == Y_AXIS)
  {
    if (m_controller.get_button_state(MENU_UP_BUTTON) == 0 &&
        pos < -click_threshold && old_pos > -click_threshold)
    {
      add_button_event(MENU_UP_BUTTON, 1);
    }
    else if (m_controller.get_button_state(MENU_UP_BUTTON) == 1 &&
             old_pos < -release_threshold && pos > -release_threshold)
    {
      add_button_event(MENU_UP_BUTTON, 0);
    }

    else  if (m_controller.get_button_state(MENU_DOWN_BUTTON) == 0 &&
              pos > click_threshold && old_pos < click_threshold)
    {
      add_button_event(MENU_DOWN_BUTTON, 1);
    }
    else  if (m_controller.get_button_state(MENU_DOWN_BUTTON) == 1 &&
              old_pos > release_threshold && pos < release_threshold)
    {
      add_button_event(MENU_DOWN_BUTTON, 0);
    }
  }
#endif

  InputEvent event;

  event.type = AXIS_EVENT;
  event.axis.name = name;
  event.axis.pos  = pos;

  m_controller.add_event(event);
  m_controller.set_axis_state(name, pos);
}

void
InputManagerSDL::clear()
{
  m_controller.clear();
}

} // namespace wstinput

/* EOF */