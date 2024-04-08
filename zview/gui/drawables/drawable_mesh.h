#pragma once
#include "drawable_base.h"
#include "QtGui/QVector2D"

class DrawableMesh : public types::Mesh, public DrawableBase
{

    QOpenGLBuffer m_iBuff; //indices 3xm
    
public:
types::Shape getShape() const override;
	DrawableMesh(const std::string& name);
	~DrawableMesh();
	void initializeGL();
	void paintGL(const QMatrix4x4& mvp);
    types::VertData picking(const QVector3D& p, const QVector3D& n) const;
	types::Roi3d get3dbbox()const;
	const std::string& getName() const;

	//until we can get all the operation done directly in GPU - copy data to local memory
	bool updateVertexBuffer(const types::VertData* data,size_t n) override;

};

