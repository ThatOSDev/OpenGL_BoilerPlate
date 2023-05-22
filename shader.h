#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include <cglm/cglm.h>
#define LOGGING_IMPLEMENTATION
#include "logging.h"
#include <stdbool.h>
#include <stdio.h>         // printf()   FILE*
#include <stdlib.h>        // malloc()

unsigned int LoadEmbeddedShaders(const char* vertex_shader_text, const char* fragment_shader_text);
unsigned int LoadShaders(const char* vertexPath, const char* fragmentPath);
void useShader(unsigned int programID);
void setBool(unsigned int programID, const char* name, bool value);
void setInt(unsigned int programID, const char* name, int value);
void setFloat(unsigned int programID, const char* name, float value);
int  loadShaderFromFile(const char* fileName, int shaderType);
void checkCompileErrors(unsigned int shader);
void ShaderCleanUp(unsigned int programID);

void useShader(unsigned int programID)
{
    glUseProgram(programID);
}

void setBool(unsigned int programID, const char* name, bool value)
{
    glUniform1i(glGetUniformLocation(programID, name), (int)value);
}

void setInt(unsigned int programID, const char* name, int value)
{
    glUniform1i(glGetUniformLocation(programID, name), value);
}

void setFloat(unsigned int programID, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(programID, name), value);
}

void setVec2(unsigned int programID, const char* name, const vec2 value)
{
    glUniform2fv(glGetUniformLocation(programID, name), 1, &value[0]);
}

void setVec2_XY(unsigned int programID, const char* name, float x, float y)
{
    glUniform2f(glGetUniformLocation(programID, name), x, y);
}

void setVec3(unsigned int programID, const char* name, const vec3 value)
{
    glUniform3fv(glGetUniformLocation(programID, name), 1, &value[0]);
}

void setVec3_XYZ(unsigned int programID, const char* name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(programID, name), x, y, z);
}

void setVec4(unsigned int programID, const char* name, const vec4 value)
{
    glUniform4fv(glGetUniformLocation(programID, name), 1, &value[0]);
}

void setVec4_XYZW(unsigned int programID, const char* name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(programID, name), x, y, z, w);
}

void setMat2(unsigned int programID, const char* name, const mat2 mat)
{
    glUniformMatrix2fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
}

void setMat3(unsigned int programID, const char* name, const mat3 mat)
{
    glUniformMatrix3fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
}

void setMat4(unsigned int programID, const char* name, const mat4 mat)
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
}

unsigned int LoadEmbeddedShaders(const char* vertex_shader_text, const char* fragment_shader_text)
{
    GLuint vertex_shader, fragment_shader, program;

    int success;
    char infoLog[512];

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        logging("ERROR : SHADER->VERTEX - COMPILATION_FAILED\n%s", infoLog);
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        logging("ERROR : SHADER->FRAGMENT - COMPILATION_FAILED\n%s", infoLog);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        logging("ERROR : SHADER->PROGRAM - LINKING_FAILED\n%s", infoLog);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

unsigned int LoadShaders(const char* vertexPath, const char* fragmentPath)
{
    unsigned int programID;
    unsigned int vertex = loadShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    unsigned int fragment = loadShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);

    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);
    checkCompileErrors(programID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return programID;
}

int loadShaderFromFile(const char* fileName, int shaderType)
{
	FILE* shaderSource = fopen(fileName, "rb");
	if (shaderSource != NULL)
    {
        char* shader;
        fseek(shaderSource, 0, SEEK_END);
        size_t TOTAL_SIZE = ftell(shaderSource);
        rewind(shaderSource);

        shader = (char*)malloc(TOTAL_SIZE + 1);
        fread(shader, 1, TOTAL_SIZE, shaderSource);
        shader[TOTAL_SIZE] = 0;
        fclose(shaderSource);

        GLuint shaderID = 0;
        shaderID = glCreateShader(shaderType);
        glShaderSource(shaderID, 1, (const char**)&shader, NULL);
        glCompileShader(shaderID);

        GLint fragment_compiled;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &fragment_compiled);
        if (fragment_compiled != GL_TRUE)
        {
            GLsizei logLength;
            GLchar  log[1024];
            glGetShaderInfoLog(shaderID, sizeof(log), &logLength, log);
            logging("ERROR : %s -->   %s\n", fileName, log);
        } else {
            return shaderID;
        }
	} else {
	    logging("ERROR : Unable to open shader file : %s\n", fileName);
	}
    return -1;
}

void checkCompileErrors(unsigned int shader)
{
    int success;
    char infoLog[1024];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        logging("ERROR : SHADER->PROGRAM - LINKING_FAILED\n%s", infoLog);
    }
}

void ShaderCleanUp(unsigned int programID)
{
    glDeleteProgram(programID);
}

#endif // SHADER_H
