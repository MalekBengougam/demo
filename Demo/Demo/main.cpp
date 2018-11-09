//
//  main.cpp
//  Demo
//
//  Created by Malek on 02/11/2018.
//  Copyright © 2018 Malek. All rights reserved.
//

#include <iostream>

#ifdef _WIN32
#include "GL/glew.h"
#else
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#endif

static const int width = 960;
static const int height = 768;

#define OPENGL_MAJOR 2
#define OPENGL_MINOR 1

#include <math.h>
#include <GLFW/glfw3.h>

static unsigned int myLut[width*height];
static const int TEX = 512;
static unsigned int myTex[TEX*TEX];


// https://www.shadertoy.com/view/Ms2SWW
void initLut( void )
{
    for( int j=0; j<height; j++ )
        for( int i=0; i<width; i++ )
        {
            const float x = -1.0f + (float)i*(2.0f/(float)width);
            const float y =  1.0f - (float)j*(2.0f/(float)height);
            #if 0
            const float r = sqrtf( x*x+y*y );
            #else
            float e = 4.0;
            e = 0.1;
            const float r = pow( pow(x*x,e) + pow(y*y,4*e), 1.0/(2.0*e) );
            #endif
            const float a = atan2f( y, x );
            
            const float u = 1.0f/r;
            const float v = a*(3.0f/3.14159f);
            float w = r*r;
            if( w>1.0f ) w=1.0f;
            
            const int iu = (u*255.0f);
            const int iv = (v*255.0f);
            const int iw = (w*255.0f);
            
            myLut[width*j+i] = ((iw&255)<<16) | ((iv&255)<<8) | (iu&255);
        }
}


void initTexture( void )
{
    for( int j=0; j<TEX; j++ )
        for( int i=0; i<TEX; i++ )
        {
            int r = (i ^ j);
            int g = (((i>>6)&1)^((j>>6)&1))*255;
            g = (g*5 + 3*r)>>3;
            myTex[TEX*j+i] = 0xff000000 | (g<<16) | (g<<8) | g;
        }
}
void intro_init( void )
{
    initTexture();
    initLut();
}


static unsigned int colScale( unsigned int col, int sca )
{
    return ((((col&0x00ff00ff)*sca)>>8)&0x00ff00ff) + ((((col&0x0000ff00)*sca)>>8)&0x0000ff00);
    
}
void intro_do( unsigned int *buffer, long itime )
{
    for( int i=0; i<width*height; i++ )
    {
        const unsigned int val = myLut[i];
        const unsigned int col = myTex[ ((val&0x0000ffff)+(itime>>2))&0x0000ffff ];
        buffer[i] = colScale( col, (val>>16) );
    }
}

// Resize the window
void resize(unsigned int width, unsigned int height)
{
    
}

void onWindowResize(GLFWwindow* window, int width, int height)
{
    resize(width, height);
}

static unsigned int buffer[width*height] = { 0 };

GLFWwindow* m_window;            // GLFW handler to the window
// Run the application until its end
void run()
{
    intro_init();
    
    GLuint bufferTex;
    glGenTextures(1, &bufferTex);
    glBindTexture(GL_TEXTURE_2D, bufferTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glEnable(GL_TEXTURE_2D);
    
    glfwSwapInterval(1);
    glfwSetWindowSizeCallback(m_window, onWindowResize);
    std::cout << "--- Running" << std::endl;
    
    long to = 0;// = timeGetTime();
    do {
        intro_do(buffer, to);//timeGetTime()-to);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        
        glBegin(GL_QUADS);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(-1.f, 1.f);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(-1.f, -1.f);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(1.f, -1.f);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(1.f, 1.f);
        glEnd();
        
        glfwSwapBuffers(m_window);
        glfwPollEvents();
        
        to += 16;
    } while (glfwWindowShouldClose(m_window) == 0);
    
    glDeleteTextures(1, &bufferTex);
}

int main(int argc, const char * argv[]) {
    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error("Could not init GLFW");
    // No need for MSAA, we aren't rasterizing
    //glfwWindowHint(GLFW_SAMPLES, MSAA);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
#ifdef _WIN32
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#else
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    m_window = glfwCreateWindow(width, height, "Demo", NULL, NULL);
    if (m_window == NULL) {
        glfwTerminate();
        throw std::runtime_error("Could not init window");
    }
    //glfwSetWindowUserPointer(m_window, this);
    glfwMakeContextCurrent(m_window);
    
    run();
    
    return 0;
}
