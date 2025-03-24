//
//  BaseShader.hpp
//  ogl4
//
//  Created by Philipp Lensing on 19.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef BaseShader_hpp
#define BaseShader_hpp

#include <stdio.h>
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include <iostream>
#include <assert.h>
#include "color.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"

class BaseShader
{
public:
    BaseShader();
    virtual ~BaseShader() {}
    virtual void modelTransform(const Matrix& m) { ModelTransform = m; }
    virtual const Matrix& modelTransform() const { return ModelTransform; }
    
    virtual void activate(const BaseCamera& Cam) const;
    virtual void deactivate() const;
protected:
    GLuint createShaderProgram( const char* VScode, const char* FScode );
    Matrix ModelTransform;
    GLuint ShaderProgram;
    
    static const BaseShader* ShaderInPipe;
};

#endif /* BaseShader_hpp */
