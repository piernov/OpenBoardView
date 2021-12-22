#include "KeyBindings.h"

#include <algorithm>
#include <iostream>

#include "SDL.h"
#include "imgui/imgui.h"

#include <toml.hpp>

KeyBindings::KeyBindings() {
	reset();
}

bool KeyBindings::isPressed(const std::string &name) const {
	auto kb = keybindings.find(name);
	if (kb == keybindings.end()) {
		return false;
	}
	return std::any_of(kb->second.begin(), kb->second.end(), [](const KeyBinding &kb){return kb.isPressed();});
}

void KeyBindings::reset() {
	keybindings["Quit"] = {KeyBinding(SDLK_q, {keyModifiers.fromName("Ctrl")}), KeyBinding(SDLK_c, {keyModifiers.fromName("Ctrl")})};
	keybindings["Open"] = {KeyBinding(SDLK_o, {keyModifiers.fromName("Ctrl")})};
	keybindings["CloseDialog"] = {KeyBinding(SDLK_ESCAPE)};
	keybindings["Validate"] = {KeyBinding(SDLK_RETURN, {keyModifiers.fromName("Shift")})};
	keybindings["Accept"] = {KeyBinding(SDLK_RETURN)};

	keybindings["Flip"] = {KeyBinding(SDLK_SPACE)};
	keybindings["Mirror"] = {KeyBinding(SDLK_m)};
	keybindings["RotateCW"] = {KeyBinding(SDLK_r), KeyBinding(SDLK_PERIOD), KeyBinding(SDLK_KP_PERIOD)};
	keybindings["RotateCCW"] = {KeyBinding(SDLK_COMMA), KeyBinding(SDLK_KP_0)};
	keybindings["ZoomIn"] = {KeyBinding(SDLK_EQUALS), KeyBinding(SDLK_KP_PLUS)};
	keybindings["ZoomOut"] = {KeyBinding(SDLK_MINUS), KeyBinding(SDLK_KP_MINUS)};

	keybindings["PanDown"] = {KeyBinding(SDLK_s), KeyBinding(SDLK_KP_2)};
	keybindings["PanUp"] = {KeyBinding(SDLK_w), KeyBinding(SDLK_KP_8)};
	keybindings["PanLeft"] = {KeyBinding(SDLK_a), KeyBinding(SDLK_KP_4)};
	keybindings["PanRight"] = {KeyBinding(SDLK_d), KeyBinding(SDLK_KP_6)};
	keybindings["Center"] = {KeyBinding(SDLK_x), KeyBinding(SDLK_KP_5)};


	keybindings["InfoPanel"] = {KeyBinding(SDLK_i)};
	keybindings["NetList"] = {KeyBinding(SDLK_l)};
	keybindings["PartList"] = {KeyBinding(SDLK_k)};
	keybindings["TogglePins"] = {KeyBinding(SDLK_p)};
	keybindings["Search"] = {KeyBinding(SDLK_f, {keyModifiers.fromName("Ctrl")}), KeyBinding(SDLK_SLASH)};
	keybindings["Clear"] = {KeyBinding(SDLK_ESCAPE)};


	saveToFile("miaou");
	readFromFile("miaou");
}

void KeyBindings::readFromFile(const std::string &filename) {

	const std::string fname("sample.toml");
	const auto data = toml::parse(fname);

	std::unordered_map<std::string, std::vector<KeyBinding>> newkeybindings;

	auto miaou = toml::get<std::unordered_map<std::string, std::vector<std::vector<std::string>>>>(data);

	for (auto &m : miaou) {
		std::cout << "m" << m.first << std::endl;
		for (auto &n : m.second) {
			for (auto &o : n) {
				auto k = SDL_GetKeyFromName(o.c_str());
				std::cout << "o" << o << "k" << SDL_GetKeyName(k) << std::endl;
			}
		}
	}
/*
	std::transform(data.begin(), data.end(), std::inserter(newkeybindings, newkeybindings.begin()), [](const std::pair<std::string, toml::value> entry){
		std::cout << "miaou " << entry.first << std::endl;
		std::vector<KeyBinding> array;
		std::transform(entry.second.begin(), entry.second.end(), std::back_inserter(array), [](const toml::value &value){
			std::cout << "nyan " << value << std::endl;
		});
		return std::pair<std::string, std::vector<KeyBinding>>{entry.first, {}};
	});*/

	//SDL_GetKeyFromName(const char *name);
}

void KeyBindings::saveToFile(const std::string &filename) const {
	/*for (auto &[name, bindings] : keybindings) {
		std::cout << "\"" << name << "\" = ";
		for (auto & binding : bindings) {
			std::cout << "\"" << SDL_GetKeyName(binding.getKeycode()) << "\", ";
		}
		std::cout << std::endl;
	}*/

	//const auto data = toml::table{{"foo", 42}, {"bar", "baz"}};
	//std::cout << data << std::endl;



	auto data2 = toml::table{};

	std::transform(keybindings.begin(), keybindings.end(), std::inserter(data2, data2.begin()), [](const std::pair<std::string, std::vector<KeyBinding>> &kbs){
		auto array = toml::array{};
		std::transform(kbs.second.begin(), kbs.second.end(), std::back_inserter(array), [](const KeyBinding &kb){
			auto keys = toml::array{SDL_GetKeyName(kb.getKeycode())};
			auto kbmodifiers = kb.getModifiers();
			std::transform(kbmodifiers.begin(), kbmodifiers.end(), std::back_inserter(keys), [](const KeyModifier &keyModifier) {
				return keyModifier.name;
			});
			return keys;
		});
		return std::pair<std::string, toml::array>{kbs.first, array};
	});

/*	for (auto &[name, bindings] : keybindings) {
		auto kbarray = toml::array{};

		for (auto & binding : bindings) {
			auto btable = toml::table{};
			btable["keycode"] = SDL_GetKeyName(binding.getKeycode());
			auto marray = toml::array{};
			for (auto &modifier : binding.getModifiers()) {
				marray.push_back("M");
			}
			btable["modifiers"] = marray;
			kbarray.push_back(btable);
		}

		data2[name] = kbarray;
	}
*/

	//const auto data2 = toml::table{keybindings};
	std::cout <<  std::setw(80) << toml::value(data2) << std::endl;
}
