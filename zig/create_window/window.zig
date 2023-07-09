const print = @import("std").debug.print;
const glfw = @cImport(@cInclude("GLFW/glfw3.h"));

const default_width: u32 = 800;
const default_height: u32 = 600;
const opengl_major_version: u32 = 3;
const opengl_minor_version: u32 = 3;

const Window = struct {
	var m_window : *glfw.GLFWwindow = undefined;
	fn create(title: [*]const u8,width: c_int,height: c_int) void {
		if(glfw.glfwInit() == 0){
			print("Failed to initialized GLFW\n",.{});
			glfw.glfwTerminate();
			return;
		}
		
		glfw.glfwWindowHint(glfw.GLFW_CONTEXT_VERSION_MAJOR,opengl_major_version);
		glfw.glfwWindowHint(glfw.GLFW_CONTEXT_VERSION_MINOR,opengl_minor_version);
		glfw.glfwWindowHint(glfw.GLFW_OPENGL_PROFILE,glfw.GLFW_OPENGL_CORE_PROFILE);
		
		if(glfw.glfwCreateWindow(width,height,title,null,null))|window|{
			m_window = window;
		}else{
			print("Failed to create GLFW window\n",.{});
			glfw.glfwTerminate();
			return;
		}
		
		//create opengl contex
		glfw.glfwMakeContextCurrent(m_window);
	}
	
	fn run() void {
		while (glfw.glfwWindowShouldClose(m_window) != 1){
		
			//close the window, when press the escape key
			if(glfw.glfwGetKey(m_window, glfw.GLFW_KEY_ESCAPE) == glfw.GLFW_PRESS){
       		   glfw.glfwSetWindowShouldClose(m_window, 1);
       		}
		
			//set background color
			glfw.glClearColor(0.20,0.26,0.30,1.0);
			glfw.glClear(glfw.GL_COLOR_BUFFER_BIT);
			
			//swap front buffer and back buffer
			glfw.glfwSwapBuffers(m_window);
			
			//poll all events
        	glfw.glfwPollEvents();
		}
	}
	fn destroy() void {
		 glfw.glfwTerminate();
	}
};
pub fn main() void {
	Window.create("Learn OpenGL (ZigLang Edition)",default_width,default_height);
	Window.run();
	Window.destroy();
}
