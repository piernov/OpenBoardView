#ifndef _KEYMODIFIERS_H_
#define _KEYMODIFIERS_H_

#include <array>
#include <unordered_map>
#include <algorithm>

#include "imgui/imgui.h"

struct KeyModifier {
	std::string name;
	bool ImGuiIO::* value;

	bool isPressed() const {
		auto io = ImGui::GetIO();
		return io.*value;
	}
};

class KeyModifiers {
private:
	std::array<KeyModifier, 4> keyModifiers{{
		{"Ctrl", &ImGuiIO::KeyCtrl},
		{"Shift", &ImGuiIO::KeyShift},
		{"Alt", &ImGuiIO::KeyAlt},
		{"Super", &ImGuiIO::KeySuper}
	}};

	std::unordered_map<std::string, KeyModifier> nameToKeyModifier;

public:
	KeyModifiers() {
		std::transform(keyModifiers.begin(), keyModifiers.end(), std::inserter(nameToKeyModifier, nameToKeyModifier.begin()), [](const KeyModifier &keyModifier){
			return std::pair<std::string, KeyModifier>{keyModifier.name, keyModifier};
		});
	}

	KeyModifier fromName(const std::string &name) {
		return nameToKeyModifier.at(name);
	}

};

#endif
