/*
    This file is part of Rocs.
    Copyright 2008  Tomaz Canabrava <tomaz.canabrava@gmail.com>
    Copyright 2008  Ugo Sangiori <ugorox@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "AddConnectionHandAction.h"
#include "Scene/GraphScene.h"
#include "Scene/DataItem.h"
#include "Scene/PointerItem.h"

#include "DataStructure.h"
#include "Data.h"
#include "Pointer.h"
#include <PointerType.h>

#include <KLocale>
#include <KDebug>
#include <DocumentManager.h>
#include <Document.h>

AddConnectionHandAction::AddConnectionHandAction(GraphScene *scene, QObject *parent)
    : AbstractAction(scene, parent),
      _pointerType(PointerTypePtr())
{
    setText(i18nc("@action:intoolbar", "Add Edge"));
    setToolTip(i18nc("@info:tooltip", "Creates a new edge between 2 nodes"));
    setIcon(KIcon("rocsaddedge"));

    _from = 0;
    _to   = 0;
    _tmpLine  = 0;
    _working = false;
    _name = "rocs-hand-add-edge";
}

AddConnectionHandAction::AddConnectionHandAction(GraphScene *scene, PointerTypePtr pointerType, QObject *parent)
    : AbstractAction(scene, parent),
      _pointerType(pointerType)
{
    setText(i18nc("@action:intoolbar", "Add %1", pointerType->name()));
    setToolTip(i18nc("@info:tooltip", "Creates a new edge between 2 nodes"));
    setIcon(KIcon("rocsaddedge"));

    _from = 0;
    _to   = 0;
    _tmpLine  = 0;
    _working = false;
    _name = "rocs-hand-add-edge";
}

AddConnectionHandAction::~AddConnectionHandAction()
{
}

bool AddConnectionHandAction::executePress(QPointF pos)
{
    if (_working) {
        return false;
    }
    if (!  DocumentManager::self().activeDocument()->activeDataStructure()
            || DocumentManager::self().activeDocument()->activeDataStructure()->readOnly()) {
        return false;
    }

    if ((_from = qgraphicsitem_cast<DataItem*>(_graphScene->itemAt(pos)))) {
        _working = true;
        _startPos = QPointF(_from->data()->x(), _from->data()->y());
        //FIXME workaround for rooted tree creation
        _from->data()->setProperty("ClickPosition", QVariant::fromValue<QPointF>(_from->mapFromScene(pos)));
        return true;
    }

    return false;
}

bool AddConnectionHandAction::executeMove(QPointF pos)
{
    if (!DocumentManager::self().activeDocument()->activeDataStructure()
            || !_from) {
        return false;
    }

    if (!_tmpLine) {
        _tmpLine = new QGraphicsLineItem();
        _graphScene->addItem(_tmpLine);
    }

    _tmpLine->setLine(_startPos.x(), _startPos.y(), pos.x(), pos.y());
    return true;
}

bool AddConnectionHandAction::executeRelease(QPointF pos)
{
    DataStructurePtr activeDataStructure = DocumentManager::self().activeDocument()->activeDataStructure();

    if (!_working || !activeDataStructure) {
        return false;
    }

    delete _tmpLine;
    _tmpLine = 0;

    // if no dataType set, we use default data type
    int pointerTypeIdentifier = 0;
    if (_pointerType) {
        pointerTypeIdentifier = _pointerType->identifier();
    }

    if ((_to = qgraphicsitem_cast<DataItem*>(_graphScene->itemAt(pos)))) {
        //FIXME workaround for rooted tree creation
        _to->data()->setProperty("ClickPosition", _to->mapFromScene(pos));
        activeDataStructure->createPointer(_from->data(),  _to->data(), pointerTypeIdentifier);
        _to->data()->setProperty("ClickPosition", QVariant());
    }
    _to = 0;
    _from->data()->setProperty("ClickPosition", QVariant());
    _from = 0;
    _working = false;
    return true;
}