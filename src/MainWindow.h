/***************************************************************************
 *   Copyright (C) 2005-2006 by Tomaz Canabrava and Ugo Sangiori           *
 *   tomaz.canabrava@gmail.com                                             *
 *   ugorox@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QtScript>
#include <QScriptEngine>
#include <KXmlGuiWindow>
#include "ui_mainwindow.h"
#include "DiagramScene.h"
#include "DiagramEdge.h"
#include "DiagramNode.h"


class QScriptEngine;

class MainWindow : public KXmlGuiWindow, public Ui::MainWindow
{

  Q_OBJECT
  private:
    DiagramScene *scene;
    
    void setupActions();
    void runScript();
  /*!*************************************************************
    * JavaScript Engine. CHANGE IT TO KROSS AS SOON AS POSSIBLE!  *
    ***************************************************************/

  private slots:
  
  //! Left Sidebar buttons.
  
  //! everything that involves Mouse-Iteraction is here.
  void on_btnAddNode_clicked()      { scene -> action = scene -> InsertNode;       }
  void on_btnAddEdge_clicked()      { scene -> action = scene -> InsertEdge;       }
  void on_btnSelection_clicked()    { scene -> action = scene -> SelectItem;       }
  void on_btnSquareSelect_clicked() { scene -> action = scene -> SquareSelect;     }
  void on_btnMove_clicked()         { scene -> action = scene -> MoveItem;         }
  void on_btnAddEdgeArrow1_clicked(){ scene -> action = scene -> InsertEdgeArrow1; }
  void on_btnAddEdgeArrow2_clicked(){ scene -> action = scene -> InsertEdgeArrow2; }
  void on_btnMakeKGraph_clicked()   { scene -> action = scene -> InsertKGraph;     }
  void on_btnMakeCGraph_clicked()   { scene -> action = scene -> InsertCGraph;     }
  
  //! Here are those that Do not need mouse-iteraction.
  void on_btnGroup_clicked()           { scene->group();     }
  void on_btnUngroup_clicked()         { scene->ungroup();         }
  void on_btnRemoveFromGroup_clicked() { scene->removeFromGroup(); }
  void on_btnRemove_clicked()          { scene->removeSelection();          }

  void on_btnRunScript_clicked() { runScript();  }

  /*! #################
     #! Node Control #
     #################*/
  
  //! Changes the beginning and end of the graph
  void on_btnStartNode_clicked()    { scene->setStartNode();   }
  void on_btnEndNode_clicked()      { scene->setEndNode();     }
  
  //! Allignment Options
  void on_btnAllignVLeft_clicked()  { scene->allignVLeft();     }
  void on_btnAllignVRight_clicked() { scene->allignVRight();    }
  void on_btnAllignVCenter_clicked(){ scene->allignVCenter();  }
  void on_btnAllignHTop_clicked()   { scene->allignHTop();      }  
  void on_btnAllignHBottom_clicked(){ scene->allignHBottom();   }
  void on_btnAllignHCenter_clicked(){ scene->allignHCenter();  }
  void on_btnNodeIndex_clicked()    { scene -> toogleIndexVisibility(); }
  void on_btnNodeColor_clicked()    { setColor();              }

  
//! Slots for the KActions.
public slots:
  void setColor();

  void openFile();              //! TODO
  void newGraph();              //! TODO
  void saveGraph();             //! TODO
  void saveGraphAs();           //! TODO
  void newScript();             //! TODO
  void saveScript();            //! TODO
  void saveScriptAs();          //! TODO
  void closeThisGraph();        //! TODO
  void closeAllGraphs();        //! TODO
  void closeAllButThisGraph();  //! TODO
  void closeThisScript();       //! TODO
  void closeAllScripts();       //! TODO
  void closeAllButThisScript(); //! TODO

  //! Those below should be only at Diagram Scene, so please, 
  //! when you got the time, change everything to there.
  void undo();                  //! TODO
  void redo();                  //! TODO
  void copy();                  //! TODO
  void cut();                   //! TODO
  void paste();                 //! TODO
  
  public:
   
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void connectEdgeSignals   (Edge *e);
  void disconnectEdgeSignals(Edge *e);
  void connectNodeSignals   (Node *d);
  void disconnectNodeSignals(Node *d);
    
  void showNodeControls(bool show);
  void showNodeAllign(bool show)  ;
  void showEdgeControls(bool show);
  void resetSpinEdgeLength(bool showSpin, qreal value);

  void debug(const QString& s);
  
};

#endif


