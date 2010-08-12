/* This file is part of KGraphViewer.
   Copyright (C) 2006-2007 Gael de Chalendar <kleag@free.fr>

   KGraphViewer is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA
*/


#include "GMLGraphParsingHelper.h"
// #include "dotgraph.h"
#include "GMLGrammar.h"
// #include "dotdefaults.h"
//#include "graphsubgraph.h"
// #include "graphnode.h"
// #include "graphedge.h"

#include <boost/throw_exception.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_distinct.hpp>
#include <boost/spirit/include/classic_loops.hpp>
#include <boost/spirit/include/classic_confix.hpp>


#include <iostream>

#include <kdebug.h>

#include <QFile>
#include<QUuid>
#include "Core/DynamicPropertiesList.h"
#include <graphDocument.h>

// using namespace std;
using namespace Rocs;
using namespace GMLPlugin;

extern GMLGraphParsingHelper* phelper;

#define KGV_MAX_ITEMS_TO_LOAD std::numeric_limits<int>::max()

GMLGraphParsingHelper::GMLGraphParsingHelper():
         edgeSource(),
         edgeTarget(),
         _actualState(begin),
         actualGraph(0),
         actualNode(0),
         actualEdge(0),
         gd(0){

}

void GMLGraphParsingHelper::startList(const QString& key){
  kDebug () << "starting a list with key:" <<key;
  if (_actualState == begin && key.compare("graph", Qt::CaseInsensitive) == 0){
    createGraph();
    return;
  }else if (_actualState == graph ){
     if(key.compare("node", Qt::CaseInsensitive)==0){
        createNode();
        return;
    }else if(key.compare("edge", Qt::CaseInsensitive)==0){
        createEdge();
        return;
    }
  }
  _properties.append(key);
}


void GMLGraphParsingHelper::endList(){
  if (!_properties.isEmpty()){
    _properties.removeLast();
    return;
  }
  switch (_actualState){
    case begin: kDebug () << "Ending a list without begin a item??"; break;
    case node: actualNode = 0;
               _actualState = graph;
               break;
    case edge: actualEdge = 0;
               _actualState = graph;
               break;
    case graph:
              actualGraph = 0;
              _actualState = begin;
              break;
  }


}

const QString GMLGraphParsingHelper::processKey(const QString& key){
    QString ret = key;
    if (key.compare("id", Qt::CaseInsensitive) == 0){
      ret = "name";
    }

    return ret;
}


void GMLGraphParsingHelper::setAtribute(const QString& key, const QString& value)
{
  switch(_actualState){
    case begin: break;
    case graph:
        if (!_properties.isEmpty()){
           QString joined = _properties.join(".");
           joined.append('.').append(key);
           actualGraph->setProperty(joined.toAscii(),value);
        }else{
           kDebug() << "seting property to graph" << key << value;
          actualGraph->setProperty(processKey(key).toAscii(),value);
        }
        break;
    case edge:
        if (!_properties.isEmpty()){ //is a list of properties of edge
           QString joined = _properties.join(".");
           joined.append('.').append(key);
           if (actualEdge){
              actualEdge->setProperty(joined.toAscii(),value);
           }else{
            _edgeProperties.insert(joined, value);
           }
        }else if (key.compare("source", Qt::CaseInsensitive)){   // search for source....
              edgeSource = value;
              createEdge();
        }else if (key.compare("target", Qt::CaseInsensitive)){   // .... and target
              edgeTarget = value;
              createEdge();
        }else if (actualEdge){       //if edge was created.
              actualEdge->setProperty(key.toAscii(),value);
        }else{
            _edgeProperties.insert(processKey(key).toAscii(), value); //store to be inserted later
        }
        break;
    case node:
        if (!_properties.isEmpty()){
          QString joined = _properties.join(".");
           joined.append('.').append(key);
           actualNode->setProperty(joined.toAscii(),value);
        }else{
          kDebug() << "seting property to node" << key << value;
          actualNode->setProperty(processKey(key).toAscii(),value);
        }
        break;
  }
}


void GMLGraphParsingHelper::createGraph(){
    if (_actualState == begin){
      actualGraph = gd->addGraph();
      _actualState = graph;
    }
}

void GMLGraphParsingHelper::createNode(){
  if (_actualState == graph){
    kDebug () << "Creating a node";
    _actualState = node;
    actualNode = actualGraph->addNode("NewNode");
  }
}



void GMLGraphParsingHelper::createEdge(){
    if (!edgeSource.isEmpty() && !edgeTarget.isEmpty()){
      kDebug () << "Creating a edge";
      _actualState = edge;
      actualEdge = actualGraph->addEdge(edgeSource, edgeTarget);
      edgeSource = edgeTarget = QString();
    }else if (_actualState == graph){
      kDebug () << "Creating a Edge";
      _actualState = edge;
      actualEdge = 0;
    }
}

