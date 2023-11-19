// SDL_OGL1.cpp : Defines the entry point for the console application.
//

//#define GLEW_STATIC
#include <GL\glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <gl\GLU.h>

bool init();
bool initGL();
void render();
GLuint CreateCube(float, GLuint&);
GLuint CreateShaderProg();
void DrawCube(GLuint id);
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

GLuint gShaderProgID;
GLuint gVAO, gVBO;

void HandleKeyUp(const SDL_KeyboardEvent& key);


int main(int argc, char* args[])
{
	init();

	SDL_Event e;
	//While application is running
	bool quit = false;
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				else {
					HandleKeyUp(e.key);
				}
				break;
			}
		}

		//Render quad
		render();

		//Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	close();

	return 0;
}

void HandleKeyUp(const SDL_KeyboardEvent& key)
{

}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 3.3
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);


		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN*/);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				//Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}

	return success;
}

bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	glewInit();

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		success = false;
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
	}
	glClearColor(0, 1, 0, 1);
	gShaderProgID = CreateShaderProg();
	gVAO = CreateCube(1.0f, gVBO);

	return success;
}

void close()
{
	//delete GL programs, buffers and objects
	glDeleteProgram(gShaderProgID);
	glDeleteVertexArrays(1, &gVAO);
	glDeleteBuffers(1, &gVBO);

	//Delete OGL context
	SDL_GL_DeleteContext(gContext);
	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void render()
{
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	DrawCube(gVAO);
}

GLuint CreateShaderProg()
{
	const GLchar* vertexShaderSource = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{ gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);	}\n";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint  success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //stores GL_TRUE or GL_FALSE in success
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", infoLog);
	}

	const GLchar* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{ FragColor = vec4(0.0f, 0.5f, 1.0f, 1.0f); }\n";
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n", infoLog);
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINK_FAILED: %s\n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLuint CreateCube(float width, GLuint& VBO)
{
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,

		-0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};

	GLuint VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //the data comes from the currently bound GL_ARRAY_BUFFER
	glEnableVertexAttribArray(0); 

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	return VAO;
}

void DrawCube(GLuint vaoID)
{
	glUseProgram(gShaderProgID);
	glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}


