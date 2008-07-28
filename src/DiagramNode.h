#ifndef DIAGRAM_NODE_H
#define DIAGRAM_NODE_H

#include <QGraphicsEllipseItem>

 #include <QGraphicsItem>
 #include <QList>
 #include <QtScript>

class Edge;
class GraphWidget;
class QGraphicsSceneMouseEvent;
class DiagramScene;
class QString;
class Group;

 class Node :public QObject, public QGraphicsItem
 {
 Q_OBJECT

 public slots:
	void setName(QString s);
	
	//! Partes Acessiveis Pelo Script =D
	void setXY(double x, double y){ QGraphicsItem::setPos(x, y);}

	bool isIsolated() { return edgeList.size() == 0;         }
	bool isTerminal() { return edgeList.size() == 1;         }
	bool isEven()     { return ( edgeList.size() % 2 ) == 0; }
	bool isOdd()      { return ( edgeList.size() % 2 ) == 1; }
	
	bool isStartNode(){ return isStartPoint; }
	bool isEndNode()  { return isEndPoint;   }
	
	double getX()     { return pos().x();  }
	double getY()     { return pos().y();  }
	bool isSelected() { return selected;   }
	bool isVisited()  { return visited;    }
	void setVisited(bool v) { visited = v; }
	
	QScriptValue edges();
	void remove();
	
 public:
     Node(DiagramScene *diagramScene);
	~Node();
      void addEdge(Edge *edge);

     enum { Type = UserType + 1 };
     int type() const { return Type; }

     QRectF boundingRect() const; //! gives the area that represents the node.
     QPainterPath shape()  const; //! gives the clicable shape of the node.

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void setColour(QColor c) { colour = c; update(); }
	void setGroup(Group *g);
	void removeGroup();
	void removeEdge(Edge *e);
	Group* getGroup() const;
	bool hasGroup();
	void toogleIndexVisibility() { showIndex = !showIndex; update(); }
	bool operator==(Node *n);
    
	bool selected;
	bool isStartPoint;
	bool isEndPoint;
	int index;
protected:
	//! when the item is moved across the drawable area, it will invoke this.
     QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	//! mouse events. wah.
     void mousePressEvent(QGraphicsSceneMouseEvent *event);
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

 private:
     QList<Edge *> edgeList;
     QPointF newPos;
     DiagramScene *diagramScene;
	 QString name;
	 QRectF *ellipse;
	 QColor colour;
     Group *group;
	 bool visited;
	 bool showIndex;
 };

#endif
