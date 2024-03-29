#pragma once

#include <QtGui/QMouseEvent>
#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector2D>
#include <QtCore/QBasicTimer>
#include <QtGui/qevent.h>
#include "zview/backend/vp_mat.h"
#include "zview/backend/state_machine/state_machine.h"
#include "zview/gui/drawables/backdrop.h"
#include "zview/gui/drawables/axes.h"
#include "zview/gui/drawables/grid.h"
#include "zview/gui/drawables/drawables_buffer.h"

class Canvas : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
    explicit Canvas(QWidget *parent=0);
	void addShape(const Types::Shape &obj);
    
    void setCamLookAt(const QVector3D& eye,const QVector3D& center,const QVector3D& up);
	
	
	void input(QInputEvent *e);
	
public slots:
	void setStatus(const QString& str);
	void forceUpdate();
	void resetView(int key=-1);
    QVector3D getLastRetargetPoint() const;


signals:
	void signal_setStatus(const QString& str);
	
protected:
	void initializeGL();

	void paintGL();
	void resizeGL(int w, int h);

    void mouseReleaseEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);



private:
	
	TrackStateMachine m_stateMachine;
	Backdrop m_backdrop;
	Axes m_axes;
	Grid m_grid;
	
};
