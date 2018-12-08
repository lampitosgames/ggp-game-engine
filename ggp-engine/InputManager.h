#ifndef GGP_INPUT_MANAGER_H
#define GGP_INPUT_MANAGER_H

#include <Windows.h>
#include <map>
#include "Keybinds.h"
#include "InputEvent.h"
class InputListener;
class GameObject;

typedef UINT InputListenerID;

class InputManager {
	//Singleton pointer
	static InputManager* instance;

	//Unique member id given to each inputListener
	InputListenerID ilCount = 0;
	//Map of all input listeners
	std::map<InputListenerID, InputListener*> inputListenerUIDMap;

	//Is the mouse locked to the window?
	bool mouseLocked;

	//Two maps for key state
	std::map<int, bool> prevKeyState;
	std::map<int, bool> keyState;
	//Store mouse state
	int prevMousePos[2] = {0, 0};
	int mousePos[2] = {0, 0};
	float totalScrollDelta = 0.0f;
public:
	//Static Singleton get/release for the single input manager instance
	static InputManager* GetInstance();
	static void ReleaseInstance();

	// We don't want anything making copies of this class so delete these operators
	InputManager(InputManager const&) = delete;
	void operator=(InputManager const&) = delete;

	//Main update function
	void Update();

	//Create (and return the UID of) a new input listener
	InputListenerID AddInputListener(InputListener* _inputListener);
	//Get an input listener given its UID
	InputListener* GetInputListener(InputListenerID _uniqueID);
	//Remove the reference to an input listener from the InputManager
	void RemoveInputListener(InputListener* _inputListener);

	/*
		Public functions to check different action states
	*/
	//Check against the input state to see if an action key is currently pressed
	//This is designed to be used outside of the context of an input event
	bool ActionPressed(std::string _action);

	//Functions to evaluate a specific event
	bool ActionPressed(std::string _action, InputEvent _event);
	bool ActionReleased(std::string _action, InputEvent _event);
	bool ActionHeld(std::string _action, InputEvent _event);

	/*
		Get/Set functions
	*/
	//Get the previous mouse position. Returns array of 2 ints
	int* GetPrevMousePos();
	//Get the current mouse position. Returns array of 2 ints
	int* GetMousePos();

	//mouseLocked get/set
	bool GetMouseLocked();
	void SetMouseLocked(bool _val);

	/*
		Mouse handling functions
	*/
	void _OnMouseUp(WPARAM _buttonState, int _x, int _y);
	void _OnMouseDown(WPARAM _buttonState, int _x, int _y);
	void _OnMouseMove(int _prevX, int _prevY, int _x, int _y);
	void _OnMouseWheel(float _delta);
private:
	InputManager();
	~InputManager();
	void Release();

	//Dispatch an InputEvent to all listeners
	void Dispatch(InputEvent _event);
};

#endif //GGP_INPUT_MANAGER_H