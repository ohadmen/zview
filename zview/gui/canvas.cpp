#include "zview/gui/canvas.h"
#include "zview/common/params.h"
#include <math.h>

Canvas::Canvas(QWidget *parent ) : QOpenGLWidget(parent), m_stateMachine()
{

	setMouseTracking(true);

	QObject::connect(&m_stateMachine, &TrackStateMachine::signal_setStatus, this, &Canvas::setStatus);
	QObject::connect(&m_stateMachine, &TrackStateMachine::signal_canvasUpdate, this, &Canvas::forceUpdate);
	setCamLookAt({1,1,1},{0,0,0},{0,0,1});
	
}


    void Canvas::setCamLookAt(const QVector3D& eye,const QVector3D& center,const QVector3D& up)
	{
		QMatrix4x4 m;
		
		m.lookAt(eye,center,up);
		
		m_stateMachine.setViewMatrix(m);
		update();
	}

void Canvas::setStatus(const QString &str)
{
	emit signal_setStatus(str);
}


void Canvas::forceUpdate() { update(); }
void Canvas::resetView(int key)
{
    static const float pi = std::acos(0)*2;
    static const float deg2rad =  pi/180.0;
	// camera is always at (0,0,0), looking tawards negative z.
	// rotation center is always (0,0,-1). for init, set object to (0,0,-1), and rescale it to fit in image.
	// static const float deg2rad = std::acosf(0.0) / 90;
	Types::Roi3d objsbbox = drawablesBuffer.get3dbbox(key);

	float a = std::tan(deg2rad * Params::camFOV() / 2);
	float s = 2 * a / (objsbbox.rangey() + objsbbox.rangez() * a);

	QVector3D t = -objsbbox.mid();
	QMatrix4x4 vm = VPmat::translate({0, 0, -1}) * VPmat::scale(s) * VPmat::translate(t);

	m_stateMachine.setViewMatrix(vm);
	m_axes.setScale(1/s);
	update();
	
}

void Canvas::initializeGL()
{
	initializeOpenGLFunctions();

	glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST); //draw object back tp front
	glEnable(GL_LINE_SMOOTH);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendEquation(GL_FUNC_ADD);


	m_backdrop.initializeGL();
	m_axes.initializeGL();
	m_grid.initializeGL();
}

void Canvas::paintGL()
{

	QMatrix4x4 vp = m_stateMachine.getVPmatrix();
	m_backdrop.paintGL();
	if(Params::viewAxes())
		m_axes.paintGL(vp);
	if(Params::viewGrid())
		m_grid.paintGL(vp);

	for (auto &d : drawablesBuffer)
	{
		d.second.get()->paintGL(vp);
	}
}
void Canvas::resizeGL(int w, int h)
{

	m_stateMachine.setWinSize(w, h);
}

void Canvas::addShape(const Types::Shape &obj)
{
	DrawablesBuffer::i().addShape(obj);
	resetView();
}

void Canvas::input(QInputEvent *e)
{
	m_stateMachine.input(e);
}

void Canvas::mouseReleaseEvent(QMouseEvent *e) { m_stateMachine.input(e); }
void Canvas::mousePressEvent(QMouseEvent *e) { m_stateMachine.input(e); }
void Canvas::mouseDoubleClickEvent(QMouseEvent *e) { m_stateMachine.input(e); }
void Canvas::wheelEvent(QWheelEvent *e) { m_stateMachine.input(e); }
void Canvas::mouseMoveEvent(QMouseEvent *e)
{
	m_stateMachine.input(e);
	repaint();
}
