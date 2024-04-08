
#pragma once
#include "drawable_base.h"

class DrawablePcl : public types::Pcl, public DrawableBase
{

public:
	types::Shape getShape() const override;
	DrawablePcl(const std::string &name);
	~DrawablePcl();
	void initializeGL();
	void paintGL(const QMatrix4x4 &mvp);
	types::Roi3d get3dbbox() const;
	types::VertData picking(const QVector3D &p, const QVector3D &n) const;
	const std::string &getName() const;

	//until we can get all the operation done directly in GPU - copy data to local memory
	bool updateVertexBuffer(const types::VertData *data, size_t n) override;
};
