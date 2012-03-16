/*
    This file is part of Rocs.
    Copyright 2004-2011  Tomaz Canabrava <tomaz.canabrava@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "KrossBackend.h"
#include "DataStructure.h"
#include "Data.h"
#include <kross/core/action.h>
#include <kross/core/manager.h>
#include <QDebug>

KrossBackend::KrossBackend(QVariantList *dataStructures)
{
    _dataStructures = dataStructures;
}

void KrossBackend::setScript(const QString& s)
{
    _script = s;
}

void KrossBackend::setBackend(const QString& backend)
{
    _backend = backend;
}

Kross::Action* KrossBackend::execute()
{
    qDebug() << "entering the Execute part";

    if (_backend == "javascript") {
        qDebug() << "implementing the jsDefaults";
        jsDefaults();
    } else if (_backend == "python") {
        qDebug() << "implementing the Python Defaults";
        pyDefaults();
    } else if (_backend  == "ruby") {
        qDebug() << "implementing the Ruby  Defaults ";
        rbDefaults();
    } else {
        qDebug() << "Backend Not Implemented Yet.";
        return 0;
    }
    qDebug() << "creating the action";
    Kross::Action *action =  new Kross::Action(0, "myScript");

    qDebug() << "setting the backend" << _backend.toAscii();
    action->setInterpreter(_backend);
    qDebug()  << "interpreter set.";

    // action->addObject( _dataStructures, "Graphs" );


    DataStructure *g = 0;
    foreach(const QVariant & v, (*_dataStructures)) {
        qDebug()  << "Got inside of the foreach";
        g = qobject_cast<DataStructure*>(v.value<QObject*>());
        if (!g) {
            qDebug() << "Graph is NULL";
            continue;
        }
        if (g->property("name") != QVariant()) {
            qDebug() << "adding the Graph " << g->property("name").toString().toAscii();
            action->addObject(g, g->property("name").toString());
        }
        /*!  EXPERIMENTAL! */
//      Datum *n = 0;
//      foreach( QVariant v2, g->data()){
//          n = v2.value<Datum*>();
//          if (n == 0){
//              qDebug() << "Datum is NULL";
//              continue;
//          }
//          if ( n->property("name") != QVariant() ){
//              qDebug() << "adding the Graph " << n->property("name").toString().toAscii();
//              action->addObject( n, n->property("name").toString());
//          }
//      }
    }

    QString codeToBeExecuted = "";

    codeToBeExecuted += _autoGeneratedScript;
    codeToBeExecuted += _script;
    qDebug() << "Code to be Executed: \n" << codeToBeExecuted.toAscii();
    qDebug() << " \nsetting the code";
    action->setCode(codeToBeExecuted.toAscii());

    qDebug()  << "triggering the code";
    action->trigger();

    qDebug() << "triggering the action";
    return action;
}


void KrossBackend::javaDefaults()
{

}

void KrossBackend::raptorDefaults()
{

}

void KrossBackend::pyDefaults()
{
    _autoGeneratedScript.clear();
    //_autoGeneratedScript +=  "import Graphs \n";

    //! Adding  each graph as a separate object only IF it has a valid name.
    DataStructure *g = 0;
    foreach(const QVariant & v, (*_dataStructures)) {
        g = qobject_cast<DataStructure*>(v.value<QObject*>());
        _autoGeneratedScript += "# -*- coding: utf-8 -*- \n";
        if (g->property("name") !=  QVariant()) {
            _autoGeneratedScript += "import ";
            _autoGeneratedScript += g->property("name").toString();
            _autoGeneratedScript +=  '\n';
        }
    }

}

void KrossBackend::rbDefaults()
{
    _autoGeneratedScript.clear();
    //_autoGeneratedScript +=  "require 'Graphs' \n";

    //! Adding  each graph as a separate object only IF it has a valid name.
    DataStructure *g = 0;
    foreach(const QVariant & v, (*_dataStructures)) {
        g = qobject_cast<DataStructure*>(v.value<QObject*>());
        if (g->property("name") != QVariant()) {
            _autoGeneratedScript += "require '";
            _autoGeneratedScript += g->property("name").toString();
            _autoGeneratedScript +=  "' \n";
        }
    }

}

void KrossBackend::jsDefaults()
{

}

void KrossBackend::luaDefaults()
{

}

void KrossBackend::csDefaults()
{

}

void KrossBackend::loadFile(const QString& file)
{
    qDebug() << "Got in here";
    _script.clear();

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File not found";
        return;
    }

    while (! f.atEnd()) {
        QByteArray line = f.readLine();
        _script += line;
    }
    _script += '\n';
}
