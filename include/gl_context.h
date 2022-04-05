#ifndef GLCONTEXT_H
#define GLCONTEXT_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stack>
#include<string>

// forward declaration
class gl_object;

// Manage Opengl contexts and windows associate to those contexts. Contains
// a static stack of gl_object that track which gl_object have been created.
// ONLY PUSH INHERITED CLASS OF GL_OBJECT INTO THE STACK for the correct
// (overwrite version of) destructor call when poping the stacking during
// destruction of the context.
class gl_context{
    public:
        static std::stack<gl_object> st_glObject;

        gl_context(unsigned int width, unsigned int height, std::string wname);

        ~gl_context() noexcept;

        GLFWwindow* getWindow();

		// change the window title name
		void assignWindowTitle(const std::string &n);

        unsigned width() const;

        unsigned height() const;

    private:

        GLFWwindow *window;

        unsigned win_width, win_height;
};

#endif // GLCONTEXT_H
