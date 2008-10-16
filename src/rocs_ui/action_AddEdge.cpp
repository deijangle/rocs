
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
#include "action_AddEdge.h"
#include "UI_GraphScene.h"
#include "libgraph_Graph.h"
#include "libgraph_Node.h"
#include "graphicsitem_Node.h"
#include <KLocale>

#include <KDebug>

AddEdgeAction::AddEdgeAction(GraphScene *scene, QObject *parent) 
: AbstractAction(scene, parent){
  setText(i18n ( "Add Edge" ));
  setToolTip ( i18n ( "Creates a new edge between 2 nodes" ) );
  setIcon ( KIcon ( "add-edge" ) );

  _nodeFrom = 0;
  _nodeTo   = 0;
  _tmpLine  = 0;
}

AddEdgeAction::~AddEdgeAction(){
  kDebug() << "Destroyed";
}

void AddEdgeAction::executePress(QPointF pos){
  if (_graph == 0){ return; }
   _nodeFrom = qgraphicsitem_cast<NodeItem*>(_graphScene->itemAt(pos));
}

void AddEdgeAction::executeMove(QPointF pos){
  if (_graph == 0){ return; }
  if (_nodeFrom == 0){ return; }
  if ( _tmpLine ) { delete _tmpLine; }

  QPointF startPos = _nodeFrom->node()->position();
  _tmpLine = new QGraphicsLineItem(startPos.x(), startPos.y(), pos.x(), pos.y());
  _graphScene->addItem(_tmpLine);
}

void AddEdgeAction::executeRelease(QPointF pos){
  if (_graph == 0){ return; }
  if( _tmpLine ){ delete _tmpLine; _tmpLine = 0; }
  _nodeTo = qgraphicsitem_cast<NodeItem*>(_graphScene->itemAt(pos));
  if ( _nodeTo == 0){ return; }

  _graph->createEdge( _nodeFrom->node(), _nodeTo->node());
}