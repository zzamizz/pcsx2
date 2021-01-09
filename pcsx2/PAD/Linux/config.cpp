/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2020  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <string>

#include "AppConfig.h"
#include "Devices/KeyboardDevice.h"
#include "Global.h"
#include "InputManager.h"
#include "config.h"

// Conversion functions telling yaml_cpp how to cope with this struct.
namespace YAML
{
	template <>
	struct convert<pad_options>
	{
		static Node encode(const pad_options& rhs)
		{
			Node node;
			node["forcefeedback"] = rhs.forcefeedback;
			node["reverse_lx"] = rhs.reverse_lx;
			node["reverse_ly"] = rhs.reverse_ly;
			node["reverse_rx"] = rhs.reverse_rx;
			node["reverse_ry"] = rhs.reverse_ry;
			node["mouse_l"] = rhs.mouse_l;
			node["mouse_r"] = rhs.mouse_r;
			return node;
		}

		static bool decode(const Node& node, pad_options& rhs)
		{
			if (!node.IsMap())
			{
				return false;
			}

			rhs.forcefeedback = node["forcefeedback"].as<bool>();
			rhs.reverse_lx = node["reverse_lx"].as<bool>();
			rhs.reverse_ly = node["reverse_ly"].as<bool>();
			rhs.reverse_rx = node["reverse_rx"].as<bool>();
			rhs.reverse_ry = node["reverse_ry"].as<bool>();
			rhs.mouse_l = node["mouse_l"].as<bool>();
			rhs.mouse_r = node["mouse_r"].as<bool>();
			return true;
		}
	};
} // namespace YAML

void ClearKeyboardValues()
{
	g_conf.keysym_map[0].clear();
	g_conf.keysym_map[1].clear();
}

void DefaultKeyboardValues()
{
	ClearKeyboardValues();
	for (auto& it : keyboard_defaults)
	{
		g_conf.keysym_map[0][it.second] = it.first;
	}
}

void PADSaveConfig()
{
	const std::string iniFile = std::string(GetSettingsFolder().Combine(wxString(L"PAD.yaml")).GetFullPath()); // default path, just in case
	YAML::Node config;

	config["first_time_wizard"] = g_conf.ftw;
	config["log"] = g_conf.log;
	config["options"] = g_conf.options;

	config["mouse_sensibility"] = g_conf.get_sensibility();
	config["ff_intensity"] = g_conf.get_ff_intensity();
	config["uid_0"] = g_conf.get_joy_uid(0);
	config["uid_1"] = g_conf.get_joy_uid(1);
	config["keysym_0"] = g_conf.keysym_map[0];
	config["keysym_1"] = g_conf.keysym_map[1];
	config["sdl2"] = g_conf.sdl2_mapping;

	std::ofstream fout(iniFile);
	fout << config;
}

void PADLoadConfig()
{
	const std::string iniFile = std::string(GetSettingsFolder().Combine(wxString(L"PAD.yaml")).GetFullPath()); // default path, just in case
	YAML::Node config;
	try
	{
		config = YAML::LoadFile(iniFile);
		g_conf.ftw = config["first_time_wizard"].as<bool>();
		g_conf.log = config["log"].as<bool>();
		g_conf.options = config["options"].as<std::array<pad_options, 2>>();
		g_conf.set_sensibility(config["mouse_sensibility"].as<u32>());
		g_conf.set_ff_intensity(config["ff_intensity"].as<u32>());

		g_conf.set_joy_uid(0, config["uid_0"].as<size_t>());
		g_conf.set_joy_uid(1, config["uid_1"].as<size_t>());

		g_conf.keysym_map[0] = config["keysym_0"].as<std::map<u32, u32>>();
		g_conf.keysym_map[1] = config["keysym_1"].as<std::map<u32, u32>>();

		g_conf.sdl2_mapping = config["sdl2"].as<std::vector<std::string>>();
	}
	catch (...)
	{
		std::cout << "Error loading PAD config file." << '\n';
	}
}
