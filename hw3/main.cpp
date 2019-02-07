// Shader Test for OpenGL 3.3
// Based on:
// http://www.opengl.org/wiki/Tutorial:_OpenGL_3.1_The_First_Triangle_%28C%2B%2B/Win%29

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h> 
#include <vector>
using namespace std;

bool drawing = false;

GLfloat vertices[] = { -1.0f,-1.0f,
1.0f,-1.0f,
1.0f,1.0f,
-1.0f,1.0f };

GLuint brushTexture;

vector<float> brushPath;


// A vertex array object for the quad
unsigned int vertexArrayObjID;
// A vertex buffer object
unsigned int vertexBufferObjID;

// A Shader program
unsigned int programID;

// loadFile - loads text file into char* 
// allocates memory - so need to delete after use
// size of file returned in fSize
char* loadFile(char *fname, GLint &fSize)
{
	ifstream::pos_type size;
	char * memblock;
	string text;

	// file read based on example in cplusplus.com tutorial
	ifstream file(fname, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		fSize = (GLuint)size;
		memblock = new char[size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();
		cout << "file " << fname << " loaded" << endl;
		text.assign(memblock);
	}
	else
	{
		cout << "Unable to open file " << fname << endl;
		exit(1);
	}
	return memblock;
}

void printProgramInfoLog(GLint program)
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);

	// should additionally check for OpenGL errors here

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetProgramInfoLog(program, infoLogLen, &charsWritten, infoLog);
		cout << "InfoLog:" << endl << infoLog << endl;
		delete[] infoLog;
	}

	// should additionally check for OpenGL errors here
}



// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile 
void printShaderInfoLog(GLint shader)
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

	// should additionally check for OpenGL errors here

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
		cout << "InfoLog:" << endl << infoLog << endl;
		delete[] infoLog;
	}

	// should additionally check for OpenGL errors here
}


void init(void)
{
	// Would load objects from file here - but using globals in this example	

	// Allocate Vertex Array Objects (get a handle or ID)
	glGenVertexArrays(1, &vertexArrayObjID);
	// Setup first Vertex Array Object
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Object (get a handle or ID)
	glGenBuffers(1, &vertexBufferObjID);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	// Set the model data into the VBO.
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	// Define the layout of the vertex data.
	glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Reset 
	glBindVertexArray(0);
}

void display(void)
{
	if (brushPath.size() <= 0)
		return;
	// Set the shader program
	glUseProgram(programID);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	// Draw the brush strokes
	// Bind the Vertex Array, which sets up the vertex attribute pointers.
	glBindVertexArray(vertexArrayObjID);
	// Update the brush data.
	// Note that we do not need to bind the VertexAttribPointer, since we are 
	// not changing the association.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, brushPath.size() * sizeof(GLfloat), &brushPath[0]);
	// Draw the Brush stroke.
	glDrawArrays(GL_POINTS, 0, brushPath.size() / 2);
	brushPath.clear();

	// Be nice and reset things back to the default.
	glBindVertexArray(0);
	glUseProgram(0);

	glFinish();
}

unsigned int createShader(const char* vertexShader, const char* fragmentShader, const char* geometryShader = 0 )
//void initShaders(void)
{
	GLuint f, v;

	char *vs, *fs;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	// load shaders & get length of each
	GLint vlen;
	GLint flen;
	char vsname[100], fsname[100];
	strcpy_s(vsname, vertexShader);
	strcpy_s(fsname, fragmentShader);
	vs = loadFile(vsname, vlen);
	fs = loadFile(fsname, flen);

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv, &vlen);
	glShaderSource(f, 1, &ff, &flen);

	GLint compiled;

	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		cout << "Vertex shader not compiled." << endl;
		printShaderInfoLog(v);
	}

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		cout << "Fragment shader not compiled." << endl;
		printShaderInfoLog(f);
	}

	GLuint g;
	char *gs;
	if (geometryShader != 0)
	{
		g = glCreateShader(GL_GEOMETRY_SHADER);

		// load shaders & get length of each
		GLint glen;
		char gsname[100];
		strcpy_s(gsname, geometryShader);
		gs = loadFile(gsname, glen);

		const char * gg = gs;

		glShaderSource(g, 1, &gg, &glen);

		glCompileShader(g);
		glGetShaderiv(g, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			cout << "geometry shader not compiled." << endl;
			printShaderInfoLog(g);
		}
	}

	programID = glCreateProgram();

	glBindAttribLocation(programID, 0, "in_Position");

	glAttachShader(programID, v);
	glAttachShader(programID, f);

	if (geometryShader != 0)
	{
		glAttachShader(programID, g);

	}

	glLinkProgram(programID);
	//glUseProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &compiled);
	if (!compiled)
	{
		cout << "program not linked." << endl;
		printProgramInfoLog(programID);
	}

	glUseProgram(0);

	delete[] vs; // dont forget to free allocated memory
	delete[] fs; // we allocated this in the loadFile function...
	if (geometryShader != 0)
	{
		delete[] gs;
	}
	return 1;
}
void reshape(int w, int h)
{

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	// get window_width and window_height and their types are int.
	// glutGet(GLUT_WINDOW_WIDTH) 
	// glutGet(GLUT_WINDOW_HEIGHT)
}

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)

	{
		drawing = true;
	}
	else
		drawing = false;
}
void MouseMotion(int x, int y)
{
	if (drawing == true) {

		float px = (x*2.0 / glutGet(GLUT_WINDOW_WIDTH)) - 1;
		float py = 1 - (y*2.0 / glutGet(GLUT_WINDOW_HEIGHT));
		brushPath.push_back(px);
		brushPath.push_back(py);

	}
	glutPostRedisplay();
}



int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Simple 2D Painter by Ming Yi Su_hw3");
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		cout << "glewInit failed, aborting." << endl;
		exit(1);
	}
	cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "OpenGL version " << glGetString(GL_VERSION) << " supported" << endl;
	// New hw 3 Create the brush texture
	
/////////////////////////////////////////////////////////////////////////////////////////////
	// 1. Create an empty texture of size 128 by 128
	const int size = 128;
	// The texture we're going to render to (brushTexture)
	glGenTextures(1, &brushTexture);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_RGBA, GL_FLOAT, 0);
	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Unattach the texture
	glBindTexture(GL_TEXTURE_2D, 0);
////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. Create a Frame Buffer Object (FBO)
	// 2. a Generate the FBO id glGenFramebuffer(...)
	// 2. b Bind (attach) the texture to the VBO.
	// 2. c Set the viewport to 128 by 128.
	// 2. d Check for FBO completeness (No zbuffer is needed).
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brushTexture, 0);
	glViewport(0, 0, 128, 128);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
/////////////////////////////////////////////////////////////////////////////////////
	// 3. Create the gaussian shader (with size 128 and set it to the active shader.
	// 4. Draw a full screen quad to fill the texture with the gaussian image
	init();
	createShader("backdrop.vert", "backdrop.frag", "PassThru.geom");
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Set the shader program
	glUseProgram(programID);
	// Draw the quad
	glBindVertexArray(vertexArrayObjID);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// Be nice and reset things back to the default.
	glBindVertexArray(0);
	glUseProgram(0);
/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 5. Generate the texture mipmap levels using glGenerateMipmap.Need to bind the texture again first.
	
/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 6. Unattach the texture, Unbind the FBO and delete the FBO.
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &FramebufferName);
/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 7. Reset the viewport to be full screen.
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 8. Clean-up the drawing state (can delete most of it).
	// clear the screen
	// glClearColor controls the color after you resize the screen.
	glClearColor(0.13, 0.54, 0.13, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
/////////////////////////////////////////////////////////////////////////////////////////////////
// Return the original screen and start painting
	init();
	glBindTexture(GL_TEXTURE_2D, brushTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
/////////////////////////////////////////////////////////////////////////////////////////////
//Creating the brush shader
	createShader("PassThruFarPlane.vert", "SolidColor.frag", "PassThru_hw3.geom");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
/////////////////////////////////////////////////////////////////////////////////////////////////
//Handling the Mouse
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
//////////////////////////////////////////////////////////////////////////////////////////////////
//Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_SRC_ALPHA);

	glutMainLoop();
	
	// delete the texture
	glDeleteTextures(1, &brushTexture);
	return 0;

}