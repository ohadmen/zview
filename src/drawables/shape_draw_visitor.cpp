#include "shape_draw_visitor.h"
#include "src/opengl_backend/imgui_impl_glfw.h"
#include "src/opengl_backend/imgui_impl_opengl3.h"
#include "src/opengl_backend/opengl_shader.h"
#include <GL/glew.h> // Initialize with glewInit()
#include <GLFW/glfw3.h>
#include <iostream>


    void ShapeDrawVisitor::operator()(const types::Pcl &obj){return;}
    void ShapeDrawVisitor::operator()(const types::Edges &obj){return;}
    void ShapeDrawVisitor::operator()(const types::Mesh &obj) {
        

           glBindVertexArray(obj.vao());
     glDrawElements(GL_TRIANGLES, obj.f().size()*3, GL_UNSIGNED_INT, NULL);
     glBindVertexArray(0);

        }
