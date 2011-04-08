/* This file is part of Rocs,
   Copyright (C) 2008 by:
   Tomaz Canabrava <tomaz.canabrava@gmail.com>
   Ugo Sangiori <ugorox@gmail.com>

   Rocs is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Rocs is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Step; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "EdgeItem.h"
#include "GraphStructure.h"

#include "NodeItem.h"
#include "GraphScene.h"
#include "Data.h"
#include "Pointer.h"
#include "DataStructure.h"
#include "math_constants.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPainterPath>
#include <QLine>
#include <QPolygonF>
#include <QtAlgorithms>
#include <KDebug>
#include <math.h>
#include <QGraphicsSimpleTextItem>


EdgeItem::EdgeItem( Pointer *edge, QGraphicsItem *parent)
        : PointerItem(edge, parent)
{
    _loop = (pointer()->from() == pointer()->to());
    setPath(createCurves());
}

EdgeItem::~EdgeItem() {
//  dynamic_cast<GraphScene*>(scene())->removeGItem(this);
  kDebug() << "Oriented Edge Removed";
}

QPolygonF EdgeItem::createArrow(const QPointF& Pos1, const QPointF& Pos2) const {
    QLineF line(Pos1, Pos2);
    qreal angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0) {
        angle = TwoPi - angle;
    }
    qreal arrowSize = 10;

    QPointF destArrowP1 = Pos2 + QPointF(sin(angle - PI_3) * arrowSize,         cos(angle - PI_3) * arrowSize);
    QPointF destArrowP2 = Pos2 + QPointF(sin(angle - Pi + PI_3) * arrowSize,    cos(angle - Pi + PI_3) * arrowSize);
    QPolygonF arrow(QPolygonF() <<  destArrowP1 << Pos2 << destArrowP2);

    /// Put the arrow on the center of the nodes.
    qreal x = Pos2.x() - Pos1.x();
    qreal y = Pos2.y() - Pos1.y();

    x -= x/2;
    y -= y/2;
    arrow.translate(-x, -y );
    return arrow;
}

QPainterPath EdgeItem::createLoop(QPointF pos) const {
    if ( !pointer() ) return QPainterPath();
    QPainterPath p;
    DataStructure *g = qobject_cast<DataStructure*>(pointer()->parent());
    qreal size = 30 + (20 * index());
    qreal angle = atan2((pos.x() - g->relativeCenter().x()), (pos.y() - g->relativeCenter().y()));
    qreal posx = (pos.x()-(((size/2) * sin(angle)) * -1)-(size/2));
    qreal posy = (pos.y()+(((size/2) * cos(angle)))-(size/2));
    p.addEllipse( posx, posy, size, size);
    return p;
}


QPainterPath EdgeItem::createCurves() {
    if ( !pointer() ) return QPainterPath();

    if (pointer()->from() == 0 || pointer()->to() == 0){
	pointer()->self_remove();
        return QPainterPath();
    }

    QPointF Pos1(pointer()->from()->x(), pointer()->from()->y());

    if ( _loop ) return createLoop(Pos1);

    QPointF Pos2(pointer()->to()->x(), pointer()->to()->y());

    QPolygonF arrow = createArrow(Pos1,  Pos2);

    if (Pos1.x() > Pos2.x()) {
        qSwap(Pos1, Pos2);
    }

    Rocs::GraphStructure* graph = qobject_cast<Rocs::GraphStructure*>( pointer()->dataStructure() );
    if (graph->directed()==false){
        QPainterPath p;
        p.moveTo(Pos1);
        p.lineTo(Pos2);
        return p;
    }

    qreal x = Pos2.x() - Pos1.x();
    qreal y = Pos2.y() - Pos1.y();
    qreal angle = atan2(y,x);

    /// Calculate the size of the inclination on the curve.
    qreal theta = angle + PI_2;
    qreal finalX = cos(theta);
    qreal finalY = sin(theta);
    int lastIndex = index();

    if (lastIndex & 1) { // If the number is Odd.
        ++lastIndex;
        finalX *= (-1);
        finalY *= (-1);
    }

    qreal size = sqrt(pow((x*0.1),2) + pow((y*0.1),2)) * lastIndex;

    finalX *= size;
    finalY *= size;
    finalX += Pos1.x() + x/2;
    finalY += Pos1.y() + y/2;

    /// Draw the Arc.
    QPainterPath p;
    p.moveTo(Pos1);
    p.quadTo(finalX, finalY, Pos2.x(), Pos2.y());


    /// puts the arrow on its correct position
    QPointF middle = p.pointAtPercent(0.5);

    x = Pos1.x() + (Pos2.x() - Pos1.x())/2;
    y = Pos1.y() + (Pos2.y() - Pos1.y())/2;
    QLineF line2(QPointF(x,y) , middle);

    arrow.translate(+line2.dx() , +line2.dy());
    p.addPolygon(arrow);


    return p;
}

#include "EdgeItem.moc"