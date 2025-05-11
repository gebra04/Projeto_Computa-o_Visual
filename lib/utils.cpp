/**
 * @file utils.cpp
 * Util functions.
 *
 * Implements frequently used functions.
 *
 * @author Ricardo Dutra da Silva
 */

#include "utils.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

/** 
 * Create program.
 *
 * Creates a program from given shader codes.
 *
 * @param vertex_code String with code for vertex shader.
 * @param fragment_code String with code for fragment shader.
 * @return Compiled program.
 */
int createShaderProgram(const char *vertex_code, const char *fragment_code) {
	
    int success;
    char error[512];

    // Request a program and shader slots from GPU
    const int program  = glCreateProgram();
    const int vertex   = glCreateShader(GL_VERTEX_SHADER);
    const int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Set shaders source
    glShaderSource(vertex, 1, &vertex_code, nullptr);
    glShaderSource(fragment, 1, &fragment_code, nullptr);

    // Compile shaders
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
		glGetShaderInfoLog(vertex, 512, nullptr, error);
		std::cout << "ERROR: Shader compilation error: " << error << std::endl;
    }
                
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
		glGetShaderInfoLog(fragment, 512, nullptr, error);
		std::cout << "ERROR: Shader compilation error: " << error << std::endl;
    }

    // Attach shader objects to the program
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    // Build program
    glLinkProgram(program);
    glGetShaderiv(program, GL_LINK_STATUS, &success);
    if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, error);
		std::cout << "ERROR: Program link error: " << error << std::endl;
    }

    // Get rid of shaders (not needed anymore)
    glDetachShader(program, vertex);
    glDetachShader(program, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}