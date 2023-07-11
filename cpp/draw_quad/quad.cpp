#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

namespace GlobalSettings
{
	const size_t default_width = 800;
	const size_t default_height = 600;
	const size_t opengl_major_version = 3;
	const size_t opengl_minor_version = 3;
}

const char * vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec3 vposition;\n"
    "layout (location = 1) in vec4 vcolor;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(vposition.x, vposition.y, vposition.z, 1.0);\n"
    "   color = vcolor;\n"
    "}\0";
const char * fragment_shader_source = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec4 color;\n"
	"void main()\n"
	"{\n"
	"   FragColor = color;\n"
	"}\n\0";

enum ShaderType
{
	VERTEX = GL_VERTEX_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER,
	UNKNOWN
};
class ShaderCompiler
{
public:
	static unsigned int compile_shader(const char * source,ShaderType type)
	{
		unsigned int shader_id = glCreateShader(type);
		glShaderSource(shader_id, 1, &source, NULL);
		glCompileShader(shader_id);
		
		int success;
		char infoLog[512];
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
			switch(type)
			{
				case ShaderType::VERTEX:
					std::cout << "Vertex Shader : compiling fail\n" << infoLog << '\n';
				break;
				case ShaderType::FRAGMENT:
					std::cout << "Vertex Shader : compiling fail\n" << infoLog << '\n';
				break;
			}
		}
		return shader_id;
	}
	static unsigned int link_shaders(unsigned int vertex_id,unsigned int fragment_id)
	{
		unsigned int shader_program = glCreateProgram();
		glAttachShader(shader_program, vertex_id);
		glAttachShader(shader_program, fragment_id);
		glLinkProgram(shader_program);
		
		int success;
		char infoLog[512];
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
			std::cout << "Program Linker : linking fail\n" << infoLog << '\n';
		}
		
		glDeleteShader(vertex_id);
		glDeleteShader(fragment_id);
		
		return shader_program;
	}
};

class Vertex
{
public:
	Vertex(glm::vec3 position,glm::vec4 color) : position{position},color{color} {}
	glm::vec3 position;
	glm::vec4 color;
	static const size_t position_offset = 0 * sizeof(float);
	static const size_t color_offset = 3 * sizeof(float);
	static const size_t size = 7 * sizeof(float); 
};

class Vertices
{
public:
	void add_vertex(const Vertex & vertex)
	{
		m_vertices.push_back(vertex);
	}
	//copy all vertex from vertices to 'vram'
	void bind()
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * Vertex::size, m_vertices.data(),GL_STATIC_DRAW);

		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,Vertex::size,(void*)Vertex::position_offset);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,Vertex::size,(void*)Vertex::color_offset);
		glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		m_vertices.clear();
	}
	void draw()
	{
		glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
	}
private:
	std::vector<Vertex> m_vertices;
	unsigned int m_vao;
	unsigned int m_vbo;
};
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
		
		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD\n";
			exit(-1);
		}
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
		init();
		while (!glfwWindowShouldClose(m_window))
		{
			//close the window, when press the escape key
			if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
       		   glfwSetWindowShouldClose(m_window, true);
		
			//set background color
			glClearColor(0.20f,0.26f,0.30f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			update();
			
			//swap front buffer and back buffer
			glfwSwapBuffers(m_window);
			
			//poll all events
        	glfwPollEvents();
		}
	}
private:
	GLFWwindow * m_window;
	
	ShaderCompiler m_shader;
	unsigned int m_program_shader;
	
	Vertices m_vertices;
	
	void init()
	{
		unsigned int vertex_shader = m_shader.compile_shader(vertex_shader_source,ShaderType::VERTEX);
		unsigned int fragment_shader = m_shader.compile_shader(fragment_shader_source,ShaderType::FRAGMENT);
		
		m_program_shader = m_shader.link_shaders(vertex_shader,fragment_shader);
		
		m_vertices.add_vertex(Vertex({-0.5f, -0.5f, +0.0f},{0.0f,0.0f,1.0f,1.0f})); //bottom left
		m_vertices.add_vertex(Vertex({+0.5f, -0.5f, +0.0f},{0.0f,0.0f,1.0f,1.0f})); //bottom right
		m_vertices.add_vertex(Vertex({+0.5f, +0.5f, +0.0f},{0.0f,0.0f,1.0f,1.0f})); //bottom top
		
		m_vertices.add_vertex(Vertex({+0.5f, +0.5f, +0.0f},{0.0f,0.0f,0.5f,1.0f})); //top right
		m_vertices.add_vertex(Vertex({-0.5f, +0.5f, +0.0f},{0.0f,0.0f,0.5f,1.0f})); //top left
		m_vertices.add_vertex(Vertex({-0.5f, -0.5f, +0.0f},{0.0f,0.0f,0.5f,1.0f})); //top bottom
		m_vertices.bind();
	}
	
	void update()
	{
		glUseProgram(m_program_shader);
        
        m_vertices.draw();
	}
	
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
