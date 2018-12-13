#ifndef GGP_KEYBINDS_H
#define GGP_KEYBINDS_H

#include <Windows.h>
#include <map>
#include <vector>
#include <string>

//TODO: Make this json data.  It would be SO MUCH EASIER

/*
	This file maps actions to associated keys
*/
static std::map<std::string, std::vector<int>> create_binds() {
	std::map<std::string, std::vector<int>> m;

	//Forward action
	m["move_forward"].push_back('W');
	m["move_forward"].push_back(VK_UP);

	//Back action
	m["move_back"].push_back('S');
	m["move_back"].push_back(VK_DOWN);

	//Left action
	m["move_left"].push_back('A');
	m["move_left"].push_back(VK_LEFT);

	//Right action
	m["move_right"].push_back('D');
	m["move_right"].push_back(VK_RIGHT);

	//Up action
	m["move_up"].push_back('E');
	m["move_up"].push_back(VK_SPACE);

	//Down action
	m["move_down"].push_back('Q');

	//Toggle mouse lock
	m["toggle_mouse_lock"].push_back('X');

	//Scene switching
	m["ship_scene"].push_back('1');
	m["pbr_demo_scene"].push_back('2');

	return m;
};

static std::map<std::string, std::vector<int>> keybinds = create_binds();

#endif //GGP_KEYBINDS_H