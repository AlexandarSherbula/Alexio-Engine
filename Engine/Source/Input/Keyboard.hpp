#pragma once
#include "InputDevices.hpp"

#include <unordered_map>

namespace nle
{
	enum KeyCode
	{
		NONE,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		UP, DOWN, LEFT, RIGHT,
		SPACE, TAB, L_SHIFT, R_SHIFT, L_CTRL, R_CTRL, L_ALT, R_ALT, INS, DEL, HOME, END, PGUP, PGDN,
		BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,
		NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
		NP_MUL, NP_DIV, NP_PLUS, NP_MINUS, NP_DECIMAL, PERIOD,
		EQUALS, COMMA, MINUS,
		SEMICOLON, SLASH, BACKTICK, LBRACKET, BACKSLASH, RBRACKET, APOSTROPHE, OEM_8,
		CAPS_LOCK, ENUM_END
	};


	class Keyboard : public InputDevice
	{
	public:
		static std::unordered_map<size_t, int8_t> MapKeys;
	public:
		Keyboard();
		~Keyboard();

		static void SetCodes();
	};
}