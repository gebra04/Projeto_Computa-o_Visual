#define GLM_ENABLE_EXPERIMENTAL

#include <cstdio>
#include <cstdlib>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../Projeto/lib/utils.h"

/* Globals */
/** Window width. */
int win_width  = 800;
/** Window height. */
int win_height = 600;

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VAO;
/** Vertex buffer object. */
unsigned int VBO;

/** Rotation angle. */
float angle = 0.0f;
/** Rotation increment. */
float angle_inc = 0.5f;

float x = 0.4f;
float x_inc = 0.01f;

/** Rotation mode. */
int mode = 1;

int md = 1;
int cond = 0;

const char *vertex_shader_src = R"glsl(
#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vColor     = color;
	gl_Position = projection * view * model * vec4(position, 1.0);
}
)glsl";
  

/** Grid */
unsigned int gridVAO, gridVBO; // Objetos para o grid
int gridShaderProgram;         // Shader simplificado para o grid
constexpr float size = 10.0f; // Tamanho do grid (ex.: 10 unidades em cada direção)
constexpr float step = 1.0f;  // Espaçamento entre linhas
constexpr int numLines = static_cast<int>(size / step) * 2 + 1; // Número total de linhas

/** Grid shader */
const char *grid_vertex_code = R"(
#version 460 core
layout (location = 0) in vec3 position;
void main() {
	gl_Position = vec4(position, 1.0);
}
)";

const char *grid_fragment_code = R"(
#version 460 core
out vec4 FragColor;
void main() {
	FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}
)";

/** Vertex shader. */
const char *vertex_code = "\n"
"#version 460 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec3 normal;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"out vec3 vNormal;\n"
"out vec3 vColor;\n"
"out vec3 fragPosition;\n"
"\n"
"void main() {\n"
"    gl_Position = projection * view * model * vec4(position, 1.0);\n"
"    vNormal = mat3(transpose(inverse(model)))*normal;\n"
"    vColor = color;\n"
"	 fragPosition = vec3(model * vec4(position, 1.0));\n"
"}\0";

/** Fragment shader. */
const char *fragment_code = "\n"
"#version 460 core\n"
"\n"
"in vec3 vNormal;\n"
"in vec3 vColor;\n"
"in vec3 fragPosition;\n"
"\n"
"out vec4 fragColor;\n"
"\n"
"uniform vec3 lightColor;\n"
"uniform vec3 lightPosition;\n"
"uniform vec3 cameraPosition;\n"
"\n"
"void main() {\n"
"    float ka = 0.5;\n"
"    vec3 ambient = ka * lightColor;\n"
"\n"
"    float kd = 1.0;\n"
"    vec3 n = normalize(vNormal);\n"
"    vec3 l = normalize(lightPosition - fragPosition);\n"
"\n"
"    float diff = max(dot(n,l), 0.0);\n"
"    vec3 diffuse = kd * diff * lightColor;\n"
"\n"
"    float ks = 1.0;\n"
"    vec3 v = normalize(cameraPosition - fragPosition);\n"
"    vec3 r = reflect(-l, n);\n"
"\n"
"    float spec = pow(max(dot(v, r), 0.0), 3.0);\n"
"    vec3 specular = ks * spec * lightColor;\n"
"\n"
"    vec3 light = (ambient + diffuse + specular) * vColor;\n"
"    fragColor = vec4(light, 1.0);\n"
"}\0";

/** 
* Drawing function.
*
* A simple drawing function that only clears the color buffer.
*/
void display() {
	/* Set RGBA color to "paint" cleared color buffer (background color). */
	glClearColor(0.2, 0.3, 0.3, 1.0);

	/* Clears color buffer to the RGBA defined values. */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Desenha o grid primeiro (fundo)
	glUseProgram(gridShaderProgram);
	glBindVertexArray(gridVAO);
	glDrawArrays(GL_LINES, 0, numLines * 2 * 2); // Desenha como linhas (GL_LINES)

	glUseProgram(program);
	glBindVertexArray(VAO);

	const glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
  
	glm::mat4 M = Rx*Ry*Rz;

	// Retrieve location of tranform variable in shader.
	int loc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(M));

	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-5.0f));
	loc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(win_width)/static_cast<float>(win_height), 0.1f, 100.0f);
 	loc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

	// Light color.
	loc = glGetUniformLocation(program, "lightColor");
	glUniform3f(loc, 1.0, 1.0, 1.0);
  
	// Light position.
	loc = glGetUniformLocation(program, "lightPosition");
	glUniform3f(loc, 6.0, 0.0, 2.0);
  
	// Camera position.
	loc = glGetUniformLocation(program, "cameraPosition");
	glUniform3f(loc, 0.0, 0.0, 5.0);

	glDrawArrays(GL_TRIANGLES, 0, 12*3);

	/* Demand to draw to the window.*/
	glutSwapBuffers();
}

/**
* Reshape function.
*
* Called when window is resized.
*
* @param width New window width.
* @param height New window height.
*/
void reshape(const int width, const int height) {
    // Save new window size in case it may be need elsewhere (not in this program).
    win_width = width;
    win_height = height;

    // Set the viewport (rectangle of visible area in the window).
    glViewport(0, 0, width, height);

    // Demand OpenGL to redraw scene (call display function).
    glutPostRedisplay();
}


/** 
* Keyboard function.
*
* Called to treat pressed keys.
*
* @param key Pressed key.
* @param x Mouse x coordinate when key pressed.
* @param y Mouse y coordinate when key pressed.
*/

void keyboard(const unsigned char key, const int x, const int y) {
	/* Closing a window using the keyboard. */
    switch (key) {
        /* Escape key.*/
        case 27:
        case 'q':
        case 'Q':
    		glutLeaveMainLoop();
    		break;
        case 'x':
			angle = angle+angle_inc < 180.0f ? angle + angle_inc : 180.0 - angle + angle_inc;
			break;
    }
}

/**
 * Idle function.
 *
 * Called continuously.
 */
void idle() {
	glutPostRedisplay();
}

/** 
 * Init grid data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
*/
void initGridData() {
	// Cada linha tem 2 vértices (início e fim)
	float vertices[numLines * 4 * 3]; // 4 linhas (2 horizontais + 2 verticais) * 2 vértices * 3 coordenadas (x,y,z)

	int index = 0;
	float color[] = {0.5f, 0.5f, 0.5f}; // Cor do grid (cinza)

	// Linhas horizontais (eixo X)
	for (float y = -size; y <= size; y += step) {
		vertices[index++] = -size; // x1
		vertices[index++] = y;     // y1
		vertices[index++] = 0.0f;  // z1
		vertices[index++] = size;  // x2
		vertices[index++] = y;     // y2
		vertices[index++] = 0.0f;  // z2
	}

	// Linhas verticais (eixo Y)
	for (float x = -size; x <= size; x += step) {
		vertices[index++] = x;      // x1
		vertices[index++] = -size;  // y1
		vertices[index++] = 0.0f;   // z1
		vertices[index++] = x;      // x2
		vertices[index++] = size;   // y2
		vertices[index++] = 0.0f;   // z2
	}

	// Cria um VAO e VBO específicos para o grid
	glGenVertexArrays(1, &gridVAO);
	glBindVertexArray(gridVAO);

	glGenBuffers(1, &gridVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configura o atributo de posição (apenas coordenadas, sem cor)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // Desvincula o VAO
}

/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData() {
    // Set triangle vertices.
	const float vertices[] = {
		// First piramid
		// First triangle (Red)
	    0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 0.000000f,  0.000000f, 1.000000f, //A
	    -0.4f,-0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 0.000000f,  0.000000f, 1.000000f,// D
	    -0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 0.000000f,  0.000000f, 1.000000f,// C
	    // Second triangle (Green)
	    0.4f, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f, 0.000000f,  0.000000f,  1.000000f,//A
	    -0.4f,-0.4f, 0.4f, 0.0f, 1.0f, 0.0f,0.000000f,  0.000000f,  1.000000f, // D
	    0.4f,-0.4f, 0.4f, 0.0f, 1.0f, 0.0f, 0.000000f,  0.000000f,  1.000000f,//B
	    // Third triangle (Blue)
	    0.4f, 0.4f, 0.4f, 0.0f, 0.0f, 1.0f, 0.000000f, -0.707107f,  0.707107f,//A
	    -0.4f, 0.4f, 0.4f, 0.0f, 0.0f, 1.0f,0.000000f, -0.707107f,  0.707107f, // C
	    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.000000f, -0.707107f,  0.707107f,// O
	    // Fourth triangle (Yellow)
	    -0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 0.0f,0.707107f,  0.000000f,  0.707107f, // C
	    -0.4f,-0.4f, 0.4f, 1.0f, 1.0f, 0.0f,0.707107f,  0.000000f,  0.707107f, // D
	    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.707107f,  0.000000f,  0.707107f,// O
	    // Fifth triangle (Cyan)
	    0.4f,-0.4f, 0.4f, 0.0f, 1.0f, 1.0f, -0.000000f, -0.707107f, -0.707107f,//B
	    -0.4f,-0.4f, 0.4f, 0.0f, 1.0f, 1.0f,-0.000000f, -0.707107f, -0.707107f, // D
	    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, -0.000000f, -0.707107f, -0.707107f,// O
	    // Sixth triangle (Magenta)
	    0.4f,-0.4f, 0.4f, 1.0f, 0.0f, 1.0f, -0.707107f,  0.000000f,  0.707107f,//B
	    0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 1.0f, -0.707107f,  0.000000f,  0.707107f,//A
	    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, -0.707107f,  0.000000f,  0.707107f,// O

	    // Second piramid
	    // First triangle (Red)
	    -0.4f,-0.4f,-0.4f, 1.0f, 0.0f, 0.0f,0.000000f,  0.000000f, -1.000000f, //H
	    -0.4f, 0.4f,-0.4f, 1.0f, 0.0f, 0.0f,0.000000f,  0.000000f, -1.000000f, //I
	    0.4f, 0.4f,-0.4f, 1.0f, 0.0f, 0.0f, 0.000000f,  0.000000f, -1.000000f,//F
	    // Second triangle (Green)
	    -0.4f,-0.4f,-0.4f, 0.0f, 1.0f, 0.0f,0.000000f,  0.000000f, -1.000000f, //H
	    0.4f, 0.4f,-0.4f, 0.0f, 1.0f, 0.0f, 0.000000f,  0.000000f, -1.000000f,//F
	    0.4f,-0.4f,-0.4f, 0.0f, 1.0f, 0.0f, 0.000000f,  0.000000f, -1.000000f,//G
	    // Third triangle (Blue)
	    -0.4f,-0.4f,-0.4f, 0.0f, 0.0f, 1.0f,0.707107f,  0.000000f, -0.707107f, //H
	    -0.4f, 0.4f,-0.4f, 0.0f, 0.0f, 1.0f,0.707107f,  0.000000f, -0.707107f, //I
	    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.707107f,  0.000000f, -0.707107f,//O
	    // Fourth triangle (Yellow)
	    -0.4f, 0.4f,-0.4f, 1.0f, 1.0f, 0.0f,0.000000f, -0.707107f, -0.707107f, //I
	    0.4f, 0.4f,-0.4f, 1.0f, 1.0f, 0.0f, 0.000000f, -0.707107f, -0.707107f,//F
	    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.000000f, -0.707107f, -0.707107f,//O
	    // Fifth triangle (Cyan)
	    0.4f,-0.4f,-0.4f, 0.0f, 1.0f, 1.0f, 0.000000f,  0.707107f, -0.707107f,//G
	    -0.4f,-0.4f,-0.4f, 0.0f, 1.0f, 1.0f,0.000000f,  0.707107f, -0.707107f, //H
	    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.000000f,  0.707107f, -0.707107f,//O
	    // Sixth triangle (Magenta)
	    0.4f,-0.4f,-0.4f, 1.0f, 0.0f, 1.0f, 0.707107f, 0.000000f,  0.707107, //G
	    0.4f, 0.4f,-0.4f, 1.0f, 0.0f, 1.0f, 0.707107f, 0.000000f,  0.707107, //F
	    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.707107f, 0.000000f,  0.707107//O
	};
    
    // Vertex array.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Set attributes.ph  
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal vector
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind Vertex Array Object.
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

/** Create program (shaders).
 * 
 * Compile shaders and create the program.
 */
void initShaders() {
	// Request a program and shader slots from GPU
	program = createShaderProgram(vertex_code, fragment_code);
	gridShaderProgram = createShaderProgram(grid_vertex_code, grid_fragment_code);
}

int main(int argc, char** argv) {
	/* Init glut (always called). */
	glutInit(&argc, argv);

	/* Set OpenGL context version to use "Modern OpenGL" */
	glutInitContextVersion(4, 6);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	/* Set glut to use double buffering with RGBA color attributes. */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	/* Set the size of the window. */
	glutInitWindowSize(win_width,win_height);

	/* Create window. */
	glewExperimental = GL_TRUE;
	glutCreateWindow(argv[0]);

	/* Init GLEW, a extension loading library for different operating systems. */
	glewInit();

	// Init vertex data for the triangle.
	initData();

	initGridData(); // Chame esta função após initData()

	// Create shaders.
	initShaders();  
  
	/* Bind callback functions. */
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	/* Give control to GLUT, main loop that ends when the program ends. */
	glutMainLoop();
}
