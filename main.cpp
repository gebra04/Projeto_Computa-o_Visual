#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../Projeto_Computacao_Visual/lib/utils.h"

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

/** Translate. */
float translate_x = 0.0f;
float translate_y = 0.0f;
float translate_inc = 0.2f;

/** Scale. */
float scale = 1.0f;
float scale_inc = 0.2f;

/** Rotation angle. */
float angle = 0.0f;
float angle_x = 0.0f;
float angle_y = 0.0f;
float angle_z = 0.0f;
float angle_inc = 0.5f;

/** Array for storing key status (256 ASCII keys). */
bool keyStates[256] = {false};

/** Vertex shader. */
const char *vertex_code = "\n"
"#version 400 core\n"
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
"#version 400 core\n"
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
"    fragColor = vec4(light, 0.5);\n"
"}\0";

/** 
* Drawing function.
*
* A simple drawing function that only clears the color buffer.
*/
void display() {
	// Clear the background with the color.
	glClearColor(0.2, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the program with shaders.
	glUseProgram(program);
	glBindVertexArray(VAO);

	// Translate matrix.
	const glm::mat4 To = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(translate_x, translate_y, 0.0f));

	// Scale matrix.
	const glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));

	// Rotation matrix.
	const glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), glm::radians(angle_x), glm::vec3(1.0f,0.0f,0.0f));
	const glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), glm::radians(angle_y), glm::vec3(0.0f,1.0f,0.0f));
	const glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), glm::radians(angle_z), glm::vec3(0.0f,0.0f,1.0f));

	// Final matrix.
	glm::mat4 M = T*Rx*Ry*Rz*S*To;

	// Retrieve location of transform variable in shader.
	int loc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(M));

	// Set the cam location.
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-5.0f));
	loc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

	// Set the projection.
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

	// Disable depth test for the hourglass transparent effect.
	glDepthMask(GL_FALSE);

	// Draw the hourglass.
	glDrawArrays(GL_TRIANGLES, 0, 12*3);

	// Restore depth test.
	glDepthMask(GL_TRUE);

	// Demand to draw to the window.
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
    // Save new window size.
    win_width = width;
    win_height = height;

    // Set the viewport (rectangle of visible area in the window).
    glViewport(0, 0, win_width, win_height);

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
    keyStates[key] = true;
    // Keys that do not depend on “hold” continue to function normally.
    switch (key) {
    	// Exit the program.
    	case 27:
		case 'q':
		case 'Q':
			glutLeaveMainLoop();
			break;
    	// Scale the hourglass.
		case '+':
			scale = scale + scale_inc;
			break;
		case '-':
			scale = scale - scale_inc;
			break;
		// Translate the hourglass.
    	case '8':
    		translate_y += translate_inc;
    		break;
    	case '2':
    		translate_y -= translate_inc;
    		break;
    	case '6':
    		translate_x += translate_inc;
    		break;
    	case '4':
    		translate_x -= translate_inc;
    		break;
    }

	// Demand OpenGL to redraw scene (call display function).
    glutPostRedisplay();
}

/**
 * Function for handling key releases.
 *
 * @param key Pressed key.
 * @param x Mouse x coordinate when key pressed.
 * @param y Mouse y coordinate when key pressed.
 */
void keyboardUp(const unsigned char key, const int x, const int y) {
    keyStates[key] = false;
}

/**
 *	Modified idle function to apply continuous rotation with multiple keys.
 */
void idle() {
    if (keyStates['w']) angle_x = ((angle_x - angle_inc) < 360.0f) ? angle_x - angle_inc : angle_x + angle_inc;
    if (keyStates['s']) angle_x = ((angle_x + angle_inc) < 360.0f) ? angle_x + angle_inc : 360.0-angle_x + angle_inc;
    if (keyStates['a']) angle_y = ((angle_y - angle_inc) < 360.0f) ? angle_y - angle_inc : angle_y + angle_inc;
    if (keyStates['d']) angle_y = ((angle_y + angle_inc) < 360.0f) ? angle_y + angle_inc : 360.0-angle_y + angle_inc;
    if (keyStates['z']) angle_z = ((angle_z - angle_inc) < 360.0f) ? angle_z - angle_inc : angle_z + angle_inc;
    if (keyStates['x']) angle_z = ((angle_z + angle_inc) < 360.0f) ? angle_z + angle_inc : 360.0-angle_z + angle_inc;

	// Demand OpenGL to redraw scene (call display function).
    glutPostRedisplay();
}

/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData() {
		// Set hourglass vertices.
		const float vertices[] = {
			// First pyramid.

			// First triangle (Cyan).
			 0.4f,  0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f,  1.000000f, // A
			-0.4f,  0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f,  1.000000f, // D
			-0.4f,  0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f,  1.000000f, // C
			// Second triangle (Cyan).
			 0.4f,  0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f,  1.000000f, // A
			-0.4f,  0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f,  1.000000f, // D
			 0.4f,  0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f,  1.000000f, // B
			// Third triangle (Cyan).
			 0.4f,  0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f, -0.707107f,  0.707107f, // A
			-0.4f,  0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f, -0.707107f,  0.707107f, // C
			 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.000000f, -0.707107f,  0.707107f, // O
			// Fourth triangle (Cyan).
			-0.4f,  0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f,  0.707107f, // C
			-0.4f,  0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f,  0.707107f, // D
			 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f,  0.707107f, // O
			// Fifth triangle (Cyan).
			 0.4f,  0.4f, -0.4f,  0.0f,  1.0f,  1.0f, -0.000000f, -0.707107f, -0.707107f, // B
			-0.4f,  0.4f, -0.4f,  0.0f,  1.0f,  1.0f, -0.000000f, -0.707107f, -0.707107f, // D
			 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, -0.000000f, -0.707107f, -0.707107f, // O
			// Sixth triangle (Cyan).
			 0.4f,  0.4f, -0.4f,  0.0f,  1.0f,  1.0f, -0.707107f,  0.000000f,  0.707107f, // B
			 0.4f,  0.4f,  0.4f,  0.0f,  1.0f,  1.0f, -0.707107f,  0.000000f,  0.707107f, // A
			 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, -0.707107f,  0.000000f,  0.707107f, // O

			// Second pyramid.

			// First triangle (Cyan).
			-0.4f, -0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f, -1.000000f, // H
			-0.4f, -0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f, -1.000000f, // I
			 0.4f, -0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f, -1.000000f, // F
			// Second triangle (Cyan).
			-0.4f, -0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f, -1.000000f, // H
			 0.4f, -0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f, -1.000000f, // F
			 0.4f, -0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.000000f, -1.000000f, // G
			// Third triangle (Cyan).
			-0.4f, -0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f, -0.707107f, // H
			-0.4f, -0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f, -0.707107f, // I
			 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f, -0.707107f, // O
			// Fourth triangle (Cyan).
			-0.4f, -0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f, -0.707107f, -0.707107f, // I
			 0.4f, -0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.000000f, -0.707107f, -0.707107f, // F
			 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.000000f, -0.707107f, -0.707107f, // O
			// Fifth triangle (Cyan).
			 0.4f, -0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.707107f, -0.707107f, // G
			-0.4f, -0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.707107f, -0.707107f, // H
			 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.000000f,  0.707107f, -0.707107f, // O
			// Sixth triangle (Cyan).
			 0.4f, -0.4f, -0.4f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f,  0.707107, // G
			 0.4f, -0.4f,  0.4f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f,  0.707107, // F
			 0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.707107f,  0.000000f,  0.707107, // O
		};
    
    // Vertex array.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex buffer.
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set coordinate attribute.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

	// Set color attribute.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set normal attribute.
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

	// Enable depth for 3D.
	glEnable(GL_DEPTH_TEST);

    // Unbind Vertex Array Object.
    glBindVertexArray(0);
}

/**
 * Create program (shaders).
 * 
 * Compile shaders and create the program.
 */
void initShaders() {
	// Request a program and shader slots from GPU
	program = createShaderProgram(vertex_code, fragment_code);
}

int main(int argc, char** argv) {
	// Init glut (always called).
	glutInit(&argc, argv);

	// Set OpenGL context version to use "Modern OpenGL".
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Create window with double-buffering, RGBA (RGB with alpha), and depth for 3D figures.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(win_width,win_height);
	glutCreateWindow("CMCO05 - hourglass");

	// Init GLEW, an extension loading library for different operating systems.
	glewExperimental = GL_TRUE;
	glewInit();

	// Init vertex data and shaders.
	initData();
	initShaders();  
  
	// Bind callback functions.
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutIdleFunc(idle);

	// Give control to GLUT, main loop that ends when the program ends.
	glutMainLoop();
}
