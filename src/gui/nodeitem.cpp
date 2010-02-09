#include "nodeitem.h"
#include "portitem.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <boost/foreach.hpp>

#include <node.h>

typedef pair<std::string, Flow *> port_pair;

NodeItem::NodeItem(Node* node)
	: QGraphicsItem(), node(node), margin(30) {
	setFlag(ItemIsMovable, true);
	setFlag(ItemIsSelectable, true);
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
	setFlag(ItemSendsGeometryChanges, true);
#endif
	setZValue(1);
	nodeChanged();
}

PortItem *NodeItem::getInPort(QString id) {
	return in_ports[id.toStdString()];
}

PortItem *NodeItem::getOutPort(QString id) {
	return out_ports[id.toStdString()];
}

void NodeItem::nodeChanged() {
	BOOST_FOREACH(port_pair item, *node->const_in_ports) {
		if (in_ports.contains(item.first)) {
			continue;
		}
		QString pname = QString::fromStdString(item.first);
		PortItem *pitem = new PortItem(pname, this);
		in_ports[item.first] = pitem;
	}

	BOOST_FOREACH(port_pair item, *node->const_out_ports) {
		if (out_ports.contains(item.first)) {
			continue;
		}
		QString pname = QString::fromStdString(item.first);
		PortItem *pitem = new PortItem(pname, this);
		out_ports[item.first] = pitem;
	}

	updateBoundingRect();
	moveItems();
}

QString NodeItem::getClassName() const {
	return QString::fromStdString(node->getClassName());
}

QString NodeItem::getId() const {
	return QString::fromStdString(node->getId());
}

QRectF NodeItem::boundingRect() const {
	return bounding;
}

QPainterPath NodeItem::shape() const {
	QPainterPath path;
	path.addRoundedRect(boundingRect(), 5, 5);
	return path;
}

void NodeItem::moveItems() {
	double y = boundingRect().top();
	Q_FOREACH(PortItem *item, in_ports) {
		QRectF ir = item->boundingRect();
		item->setPos(bounding.left() - ir.left(), y - ir.top());
		y += item->boundingRect().height();
	}

	y = boundingRect().top();
	Q_FOREACH(PortItem *item, out_ports) {
		QRectF ir = item->boundingRect();
		item->setPos(bounding.right() - ir.right(), y - ir.top());
		y += item->boundingRect().height();
	}
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	QPainterPath path;
	path.addRoundedRect(boundingRect(), 5, 5);
	if (isSelected()) {
		painter->fillPath(path, Qt::gray);
	} else {
		painter->fillPath(path, Qt::white);
	}
	painter->strokePath(path, painter->pen());
	painter->drawText(0, 0, node->getClassName());
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem::mousePressEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem::mouseReleaseEvent(event);
}

void NodeItem::updateBoundingRect() {
	QFont f;
	QFontMetrics fm(f);
	QRectF r = QRectF(fm.boundingRect(node->getClassName()));
	qreal max_outp_width = 0;

	Q_FOREACH(PortItem *out, out_ports) {
		max_outp_width = max(max_outp_width, out->boundingRect().width());
	}

	qreal max_inp_width = 0;
	Q_FOREACH(PortItem *in, in_ports) {
		max_inp_width = max(max_inp_width, in->boundingRect().width());
	}

	qreal fh = fm.height() + 4;
	qreal height = fh * max(in_ports.size(), out_ports.size()) / 2;

	bounding = r.adjusted(-max(margin, max_inp_width), -max(margin,height),
						  max(margin, max_outp_width), max(margin,height));
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (change == ItemPositionChange) {
		//qDebug() << "ItemPositionChange";
		if (collidingItems().length() > 0) {
			//return pos();
			return value;
		}
	}
	if (change == ItemPositionHasChanged) {
		//qDebug() << "ItemPositionHasChanged";
		Q_FOREACH(PortItem *item, out_ports) {
			item->updateConnection();
		}
		Q_FOREACH(PortItem *item, in_ports) {
			item->updateConnection();
		}
	}
	if (change == ItemSelectedHasChanged) {
		update();
	}
	return QGraphicsItem::itemChange(change, value);
}
