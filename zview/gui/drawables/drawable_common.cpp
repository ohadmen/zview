#include "drawable_common.h"
#include  <stdexcept>
void DrawableCommon::initShadar(QOpenGLShaderProgram* shader,const QString& shaderName)
{
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/v" + shaderName+".glsl"))
        throw std::runtime_error("could not find glsl vertex shader file");

    // Compile fragment shader
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/f" + shaderName+".glsl"))
        throw std::runtime_error("could not find glsl Fragment shader file");
    // Link shader pipeline
    if (!shader->link())
        throw std::runtime_error("could not link shader");
}
