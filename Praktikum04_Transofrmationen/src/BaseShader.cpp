//
//  BaseShader.cpp
//  ogl4
//
//  Created by Philipp Lensing on 19.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "BaseShader.h"

const BaseShader* BaseShader::ShaderInPipe = NULL;

BaseShader::BaseShader()
{
    ModelTransform.identity();
}


GLuint BaseShader::createShaderProgram( const char* VScode, const char* FScode )
{
    ModelTransform.identity();
    const unsigned int LogSize = 64*1024;
    char ShaderLog[LogSize];
    GLsizei WrittenToLog=0;
    GLint Success = 0;
    
    GLuint VS = glCreateShader(GL_VERTEX_SHADER);
    GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
    
    GLenum Error = glGetError();
    if(Error!=0)
    {
        std::cout << "Unable to create shader objects. Please ensure that PhongShader is used for the first time AFTER successful creation of an OpenGL context!";
        exit(0);
    }
    
    glShaderSource(VS, 1, &VScode, NULL);
    glShaderSource(FS, 1, &FScode, NULL);
    
    glCompileShader(VS);
    glGetShaderiv(VS, GL_COMPILE_STATUS, &Success);
    if(Success==GL_FALSE)
        glGetShaderInfoLog(VS, LogSize, &WrittenToLog, ShaderLog);
    
    
    glCompileShader(FS);
    glGetShaderiv(FS, GL_COMPILE_STATUS, &Success);
    if(Success==GL_FALSE)
    {
        GLsizei Written=0;
        glGetShaderInfoLog(FS, LogSize-WrittenToLog, &Written, &ShaderLog[WrittenToLog]);
        WrittenToLog+=Written;
    }
    
    if( WrittenToLog > 0 )
    {
        // compilation failed
        std::cout << ShaderLog;
        exit(0);
    }
    
    ShaderProgram = glCreateProgram();
    assert(ShaderProgram);
    
    glAttachShader(ShaderProgram, VS);
    glDeleteShader(VS);
    glAttachShader(ShaderProgram, FS);
    glDeleteShader(FS);
    glLinkProgram(ShaderProgram);
    
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if(Success==GL_FALSE)
        glGetShaderInfoLog(ShaderProgram, LogSize-WrittenToLog, NULL, &ShaderLog[WrittenToLog]);

    if( WrittenToLog > 0 )
    {
        // compilation failed
        std::cout << ShaderLog;
        exit(0);
    }
    return ShaderProgram;
}

void BaseShader::activate(const BaseCamera& Cam) const
{
    if(ShaderInPipe != this)
        glUseProgram(ShaderProgram);
    ShaderInPipe = this;
}


void BaseShader::deactivate() const
{
    glUseProgram(0);
}



