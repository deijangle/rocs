/*
 *  Copyright 2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "edgetype.h"
#include "edgetypestyle.h"
#include "graphdocument.h"
#include <QDebug>

using namespace GraphTheory;

// initialize number of edge type objects
uint EdgeType::objectCounter = 0;

class GraphTheory::EdgeTypePrivate {
public:
    EdgeTypePrivate()
        : m_id(-1)
        , m_style(new EdgeTypeStyle)
        , m_direction(EdgeType::Unidirectional)
        , m_name(QString())
        , m_valid(false)
    {

    }

    ~EdgeTypePrivate()
    {
        m_style->deleteLater();
    }

    EdgeTypePtr q;
    GraphDocumentPtr m_document;
    int m_id;
    QStringList m_dynamicProperties;
    EdgeTypeStyle *m_style;
    EdgeType::Direction m_direction;
    QString m_name;
    bool m_valid;
};

EdgeType::EdgeType()
    : QObject()
    , d(new EdgeTypePrivate)
{
    ++EdgeType::objectCounter;
}

EdgeType::~EdgeType()
{
    --EdgeType::objectCounter;
}

EdgeTypePtr EdgeType::self() const
{
    return d->q;
}

EdgeTypePtr EdgeType::create(GraphDocumentPtr document)
{
    EdgeTypePtr pi(new EdgeType);
    pi->setQpointer(pi);
    pi->d->m_document = document;
    pi->d->m_id = document->generateId();
    pi->d->m_valid = true;

    // insert completely initialized node type into document
    document->insert(pi->d->q);
    return pi;
}

bool EdgeType::isValid() const
{
    return d->m_valid;
}

void EdgeType::destroy()
{
    d->m_valid = false;
    d->m_document->remove(d->q);

    // reset last reference to this object
    d->q.reset();
}

GraphDocumentPtr EdgeType::document() const
{
    return d->m_document;
}

void EdgeType::setName(const QString& name)
{
    if (d->m_name == name) {
        return;
    }
    d->m_name = name;
    emit nameChanged(name);
}

QString EdgeType::name() const
{
    return d->m_name;
}

int EdgeType::id() const
{
    return d->m_id;
}

void EdgeType::setId(int id)
{
    if (id == d->m_id) {
        return;
    }
    d->m_id = id;
    emit idChanged(id);
}

EdgeTypeStyle * EdgeType::style() const
{
    return d->m_style;
}

QStringList EdgeType::dynamicProperties() const
{
    return d->m_dynamicProperties;
}

void EdgeType::addDynamicProperty(const QString& property)
{
    if (d->m_dynamicProperties.contains(property)) {
        return;
    }
    emit dynamicPropertyAboutToBeAdded(property, d->m_dynamicProperties.count());
    d->m_dynamicProperties.append(property);
    emit dynamicPropertyAdded();
}

void EdgeType::removeDynamicProperty(const QString& property)
{
    if (!d->m_dynamicProperties.contains(property)) {
        return;
    }
    int index = d->m_dynamicProperties.indexOf(property);
    emit dynamicPropertiesAboutToBeRemoved(index, index);
    d->m_dynamicProperties.removeOne(property);
    emit dynamicPropertyRemoved(property);
}

void EdgeType::renameDynamicProperty(const QString& oldProperty, const QString& newProperty)
{
    Q_ASSERT(d->m_dynamicProperties.contains(oldProperty));
    Q_ASSERT(!d->m_dynamicProperties.contains(newProperty));

    if (!d->m_dynamicProperties.contains(oldProperty) || d->m_dynamicProperties.contains(newProperty)) {
        return;
    }
    int index = d->m_dynamicProperties.indexOf(oldProperty);
    d->m_dynamicProperties[index] = newProperty;
    emit dynamicPropertyRenamed(oldProperty, newProperty);
    emit dynamicPropertyChanged(index);
}

EdgeType::Direction EdgeType::direction() const
{
    return d->m_direction;
}

void EdgeType::setDirection(EdgeType::Direction direction)
{
    if (d->m_direction == direction) {
        return;
    }
    d->m_direction = direction;
    emit directionChanged(direction);
}

void EdgeType::setQpointer(EdgeTypePtr q)
{
    d->q = q;
}
