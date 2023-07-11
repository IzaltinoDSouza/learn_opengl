#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
    "layout (location = 2) in vec2 vtexture;\n"
    "out vec4 color;\n"
    "out vec2 tex_coord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(vposition.x, vposition.y, vposition.z, 1.0);\n"
    "   color = vcolor;\n"
    "   tex_coord = vec2(vtexture.x,vtexture.y);\n"
    "}\0";
const char * fragment_shader_source = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec4 color;\n"
	"in vec2 tex_coord;\n"
	"uniform sampler2D texture1;"
	"void main()\n"
	"{\n"
	"   FragColor = texture(texture1, tex_coord);\n"
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
					std::cout << "Fragment Shader : compiling fail\n" << infoLog << '\n';
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
class Texture2d
{
public:
	void load(const char * filename)
	{
		int channels;
		int width;
		int height;
		unsigned char * pixels_data = stbi_load(filename, &width, &height, &channels, 0);
		
		if(!pixels_data)
		{
			exit(-1);
			return;
		}
		
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels_data);
        glGenerateMipmap(GL_TEXTURE_2D);
		
		stbi_image_free(pixels_data);
	}
	void bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}
private:
	 unsigned int m_texture;
};
class Vertex
{
public:
	Vertex(glm::vec3 position,glm::vec4 color,glm::vec2 texture)
	: position{position},
	  color{color},
	  texture{texture}
	{
	
	}
	
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texture;
	static const size_t position_offset = 0 * sizeof(float);
	static const size_t color_offset = 3 * sizeof(float);
	static const size_t texture_offset = 7 * sizeof(float);
	static const size_t size = 9 * sizeof(float); 
};

class Vertices
{
public:
	void add_vertex(const Vertex & vertex,const unsigned int & index)
	{
		m_vertices.push_back(vertex);
		m_indices.push_back(index);
	}
	//copy all vertex from vertices to 'vram'
	void bind()
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ebo);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * Vertex::size, m_vertices.data(),GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
					 m_indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,Vertex::size,(void*)Vertex::position_offset);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,Vertex::size,(void*)Vertex::color_offset);
		glEnableVertexAttribArray(1);
		
		glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,Vertex::size,(void*)Vertex::texture_offset);
		glEnableVertexAttribArray(2);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		m_vertices.clear();
		m_indices.clear();
	}
	void draw()
	{
		glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
private:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ebo;
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
	Texture2d m_texture;
	
	void init()
	{
		unsigned int vertex_shader = m_shader.compile_shader(vertex_shader_source,ShaderType::VERTEX);
		unsigned int fragment_shader = m_shader.compile_shader(fragment_shader_source,ShaderType::FRAGMENT);
		
		m_program_shader = m_shader.link_shaders(vertex_shader,fragment_shader);
		
		//bottom left
		m_vertices.add_vertex(Vertex({-0.5f, -0.5f, +0.0f},
									 {0.0f,0.0f,1.0f,1.0f},
									 {0.0f,0.0f}),
									 0);
		
		//bottom right
		m_vertices.add_vertex(Vertex({+0.5f, -0.5f, +0.0f},
									 {0.0f,0.0f,1.0f,1.0f},
									 {1.0f,0.0f}),
									 1);
		//bottom top
		m_vertices.add_vertex(Vertex({+0.5f, +0.5f, +0.0f},
									 {0.0f,0.0f,1.0f,1.0f},
									 {1.0f,1.0f}),
									 2);
		
		//top right
		m_vertices.add_vertex(Vertex({+0.5f, +0.5f, +0.0f},
									 {0.0f,0.0f,0.5f,1.0f},
									 {1.0f,1.0f}),
									 2);
		
		//top left
		m_vertices.add_vertex(Vertex({-0.5f, +0.5f, +0.0f},
									 {0.0f,0.0f,0.5f,1.0f},
									 {0.0f,1.0f}),
									 4);
		
		//top bottom
		m_vertices.add_vertex(Vertex({-0.5f, -0.5f, +0.0f},
									 {0.0f,0.0f,0.5f,1.0f},
									 {0.0f,0.0f}),
									 0);
		m_vertices.bind();
		
		m_texture.load("texture.jpg");
	}
	
	void update()
	{
		m_texture.bind();
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
