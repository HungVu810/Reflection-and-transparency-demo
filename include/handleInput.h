#ifndef HANDLEINPUT_H
#define HANDLEINPUT_H
#include "camera.h"
#include <GLFW/glfw3.h>

namespace handleFunc{
	extern void keystroke(GLFWwindow *window, int key, int scancode, int actions, int mods);
	extern void cursorPos(GLFWwindow *window, double xpos, double ypos);
};

#endif // HANDLEINPUT_H
