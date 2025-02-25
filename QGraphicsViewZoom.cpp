#include "QGraphicsViewZoom.h"
#include <QMouseEvent>
#include <QApplication>
#include <qmath.h>

QGraphicsViewZoom::QGraphicsViewZoom(QGraphicsView* view)
	: QObject(view), _view(view)
{
	_view->viewport()->installEventFilter(this);
	_view->setMouseTracking(true);
	_modifiers = Qt::ControlModifier;
	_zoom_factor_base = 1.0015;
}

void QGraphicsViewZoom::gentle_zoom(double factor) 
{
	_view->scale(factor, factor);
	_view->centerOn(target_scene_pos);
	QPointF center(_view->viewport()->width() / 2.0, _view->viewport()->height() / 2.0);
	QPointF delta_viewport_pos = target_viewport_pos - center; 
	QPointF viewport_center = _view->mapFromScene(target_scene_pos) - delta_viewport_pos;
	_view->centerOn(_view->mapToScene(viewport_center.toPoint()));
	Q_EMIT zoomed();
}

void QGraphicsViewZoom::set_modifiers(Qt::KeyboardModifiers modifiers) 
{
	_modifiers = modifiers;
}

void QGraphicsViewZoom::set_zoom_factor_base(double value) 
{
	_zoom_factor_base = value;
}

bool QGraphicsViewZoom::eventFilter(QObject *object, QEvent *event) 
{
	if (event->type() == QEvent::MouseMove) {
		QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
		QPointF delta = target_viewport_pos - mouse_event->pos();
		if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
			target_viewport_pos = mouse_event->pos();
			target_scene_pos = _view->mapToScene(mouse_event->pos());
		}
	}
	else if (event->type() == QEvent::Wheel) {
		QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
		if (QApplication::keyboardModifiers() == _modifiers) {
            //if (wheel_event->orientation() == Qt::Vertical) {
            double angle = wheel_event->angleDelta().y();
            if (angle != 0) {
                //double angle = wheel_event->angleDelta().y();
				double factor = qPow(_zoom_factor_base, angle);
				gentle_zoom(factor);
				return true;
			}
		}
	}
	Q_UNUSED(object)
	return false;
}
