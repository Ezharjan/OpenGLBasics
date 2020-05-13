#include<GLEW/glew.h>
#include <GLFW/glfw3.h>
#include<math.h>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
using namespace std;

//by Alexander

struct ShaderProgrameSource{
	string VertexShader;
	string FragmentShader;
};

static ShaderProgrameSource ParseShader(const string filePath){

	enum ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ifstream stream(filePath);
	ShaderType type = ShaderType :: NONE;
	string line;
	stringstream ss[2];
	while (getline(stream,line))
	{
		if (line.find("#shader") != string :: npos)
		{
			if (line.find("vertex") != string ::npos)
			{
				// vertex mode
				type = ShaderType::VERTEX;

			}
			else
			{
				// fragment mode
				type = ShaderType::FRAGMENT;

			}
		}
		else
		{
			ss[(int)type] << line << '\n'; // Add line to the stream string
		}
	}
	return {ss[0].str(), ss[1].str()};
} 

static int CompilerShader(unsigned int type, const string& source){
	unsigned int id = glCreateShader(type);
	const char* src =  source.c_str();// &source[0]

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//Erro handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length *sizeof(char));
		glGetShaderInfoLog(id, GL_INFO_LOG_LENGTH, &length,message);
		cout << "Falied to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment") << "shader" << endl;
		cout << message <<endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const string& vertexShader, const string& fragmentShder){
	unsigned int programe = glCreateProgram(); // GLuint programe = glCreateProgram();
	unsigned int vShader = CompilerShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fShader = CompilerShader(GL_FRAGMENT_SHADER, fragmentShder);

	glAttachShader(programe,vShader);
	glAttachShader(programe,fShader);
	glLinkProgram(programe);
	glValidateProgram(programe);

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return programe;	   	
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;



    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error " << std::endl;
	}

	
			float position[6] = {
				0,    0.5f,
				0.5f ,-0.5f,
				-0.5f, -0.5f
			};
			unsigned int buffer;
			glGenBuffers(1, &buffer);// Generate
			glBindBuffer(GL_ARRAY_BUFFER, buffer ); // Select buffer
			glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW); // Set the buffer data into the buffer
			glEnableVertexAttribArray(0); // Enable the attribute of the array frome the selected buffer's 0 index
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);
			
			ShaderProgrameSource source = ParseShader("myFirstShader.shader");
			unsigned int shader = CreateShader(source.VertexShader, source.FragmentShader);
			glUseProgram(shader);
			glBindBuffer(GL_ARRAY_BUFFER, 0);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0 , 3);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
	glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}