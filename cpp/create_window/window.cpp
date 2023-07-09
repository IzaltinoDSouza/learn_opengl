#include <GLFW/glfw3.h>
#include <iostream>

namespace GlobalSettings
{
	const size_t default_width = 800;
	const size_t default_height = 600;
	const size_t opengl_major_version = 3;
	const size_t opengl_minor_version = 3;
}

class Window
{
public:
	void create(const char * title,size_t width,size_t height)
	{
		if(!glfwInit()){
			std::cout << "Failed to create GLFW\n";
			exit(-1);
		}
		
		//configure opengl version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,GlobalSettings::opengl_major_version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,GlobalSettings::opengl_minor_version);
		glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
		
		m_window = glfwCreateWindow(width,height,title,NULL,NULL);
		if(!m_window)
		{
			std::cout << "Failed to create GLFW window\n";
			glfwTerminate();
			exit(-1);
		}
		
		//create opengl contex
		glfwMakeContextCurrent(m_window);
		
		glfwSetFramebufferSizeCallback(m_window, resize);
	}
	void destroy()
	{
		 glfwTerminate();
	}
	void vsync_enable(bool enable = true)
	{
		if(m_window)
			glfwSwapInterval(enable);
	}
	void run()
	{
		while (!glfwWindowShouldClose(m_window))
		{
			//close the window, when press the escape key
			if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
       		   glfwSetWindowShouldClose(m_window, true);
		
			//set background color
			glClearColor(0.20f,0.26f,0.30f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			//swap front buffer and back buffer
			glfwSwapBuffers(m_window);
			
			//poll all events
        	glfwPollEvents();
		}
	}
private:
	GLFWwindow * m_window;
	
	static void resize(GLFWwindow * window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
};
int main()
{
	Window window;
	window.create("Learn OpengGL",GlobalSettings::default_width,GlobalSettings::default_height);
	window.vsync_enable();
	window.run();
	window.destroy();
}
