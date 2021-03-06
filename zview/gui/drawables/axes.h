#pragma once

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>



class Axes: public QOpenGLFunctions
{
    
public:

    void setScale(float s);
    explicit Axes();
       
    void initializeGL();

    void paintGL(const QMatrix4x4& mvp);

private:
	QOpenGLShaderProgram m_shader;
	QOpenGLBuffer m_verts;
    float m_scale;
    
    
};


