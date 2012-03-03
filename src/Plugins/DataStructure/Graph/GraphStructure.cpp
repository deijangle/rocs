/*
    This file is part of Rocs.
    Copyright 2011       Tomaz Canabrava <tomaz.canabrava@gmail.com>
    Copyright 2011       Wagner Reck <wagner.reck@gmail.com>
    Copyright 2011-2012  Andreas Cord-Landwehr <cola@uni-paderborn.de>

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

#undef QT_STRICT_ITERATORS // boost property map can't work with iterators being classes

#include "GraphStructure.h"
#include "KDebug"
#include "Data.h"
#include "Pointer.h"
#include "Document.h"
#include "DataStructure.h"
#include <KMessageBox>
#include "GraphNode.h"

#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_concepts.hpp>

DataStructurePtr Rocs::GraphStructure::create(Document *parent)
{
    return DataStructure::create<GraphStructure>(parent);
}

DataStructurePtr Rocs::GraphStructure::create(DataStructurePtr other, Document *parent)
{
    boost::shared_ptr<GraphStructure> ds = boost::static_pointer_cast<GraphStructure>(DataStructure::create<GraphStructure>(parent));
    ds->importStructure(other);
    return ds;
}

Rocs::GraphStructure::GraphStructure(Document* parent) :
    DataStructure(parent)
{
    setGraphType(UNDIRECTED);
}

void Rocs::GraphStructure::importStructure(DataStructurePtr other)
{
    QHash <Data*, DataPtr> dataTodata;
    foreach(DataPtr n, other->dataList()) {
        DataPtr newdata = addData(n->name());
        newdata->setColor(n->color());
        newdata->setValue(n->value());
        newdata->setX(n->x());
        newdata->setY(n->y());
        newdata->setWidth(n->width());
        dataTodata.insert(n.get(), newdata);
    }
    foreach(PointerPtr e, other->pointers()) {
        DataPtr from =  dataTodata.value(e->from().get());
        DataPtr to =  dataTodata.value(e->to().get());

        PointerPtr newPointer = addPointer(from, to);
        newPointer->setColor(e->color());
        newPointer->setValue(e->value());
    }
    setGraphType(UNDIRECTED);
}

Rocs::GraphStructure::~GraphStructure()
{
}

QScriptValue Rocs::GraphStructure::addOverlay(const QString& name)
{
    return engine()->toScriptValue<int>(registerPointerType(name));
}

QScriptValue Rocs::GraphStructure::removeOverlay(int overlay)
{
    return engine()->toScriptValue<bool>(removePointerType(overlay));
}

QScriptValue Rocs::GraphStructure::overlayEdges(int overlay)
{
    QScriptValue array = engine()->newArray();
    foreach(PointerPtr n, pointers(overlay)) {
        array.property("push").call(array, QScriptValueList() << n->scriptValue());
    }
    return array;
}

QScriptValue Rocs::GraphStructure::list_nodes()
{
    QScriptValue array = engine()->newArray();
    foreach(DataPtr n, dataList()) {
        array.property("push").call(array, QScriptValueList() << n->scriptValue());
    }
    return array;
}

QScriptValue Rocs::GraphStructure::list_edges()
{
    QScriptValue array = engine()->newArray();
    foreach(PointerPtr n, pointers()) {
        array.property("push").call(array, QScriptValueList() << n->scriptValue());
    }
    return array;
}

QScriptValue Rocs::GraphStructure::add_node(const QString& name)
{
    DataPtr n = addData(name);
    n->setEngine(engine());
    return n->scriptValue();
}

QScriptValue Rocs::GraphStructure::add_edge(Data* fromRaw, Data* toRaw)
{
    return addOverlayEdge(fromRaw, toRaw, 0);
}

QScriptValue Rocs::GraphStructure::addOverlayEdge(Data* fromRaw, Data* toRaw, int overlay)
{
    if (fromRaw == 0 || toRaw == 0) {
        return QScriptValue();
    }
    if (!pointerTypeList().contains(overlay)) {
        return QScriptValue();
    }

    DataPtr from = fromRaw->getData();
    DataPtr to = toRaw->getData();

    PointerPtr edge = addPointer(from, to, overlay);
    if (edge) {
        edge->setEngine(engine());
        return edge->scriptValue();
    }

    return QScriptValue();
}

QScriptValue Rocs::GraphStructure::dijkstra_shortest_path(Data* fromRaw, Data* toRaw)
{
    if (fromRaw == 0 || toRaw == 0) {
        return QScriptValue();
    }
    DataPtr from = fromRaw->getData();
    DataPtr to = toRaw->getData();

    QScriptValue path_edges = engine()->newArray();

    typedef boost::adjacency_list < boost::listS, boost::vecS, boost::directedS,
            boost::no_property, boost::property <boost::edge_weight_t, qreal> > graph_t;
    typedef boost::graph_traits <graph_t>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits <graph_t>::edge_descriptor edge_descriptor;
    typedef std::pair<int, int> Edge;

    // create IDs for all nodes
    QMap<Data*, int> node_mapping;
    QMap<std::pair<int, int>, PointerPtr> edge_mapping; // to map all edges back afterwards
    int counter = 0;
    BOOST_FOREACH(DataPtr data, this->dataList()) {
        node_mapping[data.get()] = counter++;
    }

    // use doubled size for case of undirected edges
    QVector<Edge> edges(this->pointers().count() * 2);
    QVector<qreal> weights(this->pointers().count() * 2);

    counter = 0;
    BOOST_FOREACH(PointerPtr p, this->pointers()) {
        edges[counter] = Edge(node_mapping[p->from().get()], node_mapping[p->to().get()]);
        edge_mapping[std::make_pair < int, int > (node_mapping[p->from().get()], node_mapping[p->to().get()])] = p;
        if (!p->value().isEmpty()) {
            weights[counter] = p->value().toDouble();
        } else {
            weights[counter] = 1;
        }
        counter++;
        // if graph is directed, also add back-edges
        if (!this->directed()) {
            edges[counter] = Edge(node_mapping[p->to().get()], node_mapping[p->from().get()]);
            edge_mapping[std::make_pair < int, int > (node_mapping[p->to().get()], node_mapping[p->from().get()])] = p;
            if (!p->value().isEmpty()) {
                weights[counter] = p->value().toDouble();
            } else {
                weights[counter] = 1;
            }
            counter++;
        }
    }

    // setup the graph
    graph_t g(edges.begin(),
              edges.end(),
              weights.begin(),
              this->dataList().count()
             );

    // compute Dijkstra
    vertex_descriptor source = boost::vertex(node_mapping[from.get()], g);
    vertex_descriptor target = boost::vertex(node_mapping[to.get()], g);
    QVector<vertex_descriptor> p(boost::num_vertices(g));
    QVector<int> dist(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(g,
                                   source,
                                   boost::predecessor_map(p.begin()).distance_map(dist.begin())
                                  );

    // qDebug() << "length of shortest path : " << dist[node_mapping[to]];

    // walk search tree and setup solution
    vertex_descriptor predecessor = target;
    do {
        if (edge_mapping.contains(std::make_pair<int, int>(p[predecessor], predecessor))) {
            path_edges.property("push").call(
                path_edges,
                QScriptValueList() << edge_mapping[std::make_pair < int, int > (p[predecessor], predecessor)]->scriptValue()
            );
        }
        predecessor = p[predecessor];
    } while (p[predecessor] != predecessor);

    return path_edges;
}

void Rocs::GraphStructure::setGraphType(int type)
{
    if (_type == type) {
        return;
    }
    if (dataList().count() == 0) {
        return;
    }
    if ((_type  == MULTIGRAPH && type != MULTIGRAPH)
            || (_type == DIRECTED && type == UNDIRECTED)) {
        if (KMessageBox::warningContinueCancel(0, i18n("This action will probably remove some edges. Do you want to continue?")) != KMessageBox::Continue) {
            return;
        }
    }

    _type = static_cast<GRAPH_TYPE>(type);
    switch (_type) {
    case UNDIRECTED:
        foreach(DataPtr data, dataList()) {
            // Clear the 'self pointers', undirecetd graphs doesn't have self nodes.
            foreach(PointerPtr p, data->self_pointers()) {
                p->remove();
            }
            data->self_pointers().clear();

            // Clear the rest. there should be only one edge between two nodes.
            foreach(DataPtr data2, dataList()) {
                if (data == data2) {
                    continue;
                }

                bool foundOne = false;
                foreach(PointerPtr tmp, data->out_pointers()) {
                    if (tmp->to() == data2) {
                        if (!foundOne) {
                            foundOne = true;
                        } else {
                            data->out_pointers().removeOne(tmp);
                            data2->in_pointers().removeOne(tmp);
                            tmp->remove();
                        }
                    }
                }

                foreach(PointerPtr tmp, data->in_pointers()) {
                    if (tmp->from() == data2) {
                        if (!foundOne) {
                            foundOne = true;
                        } else {
                            data->in_pointers().removeOne(tmp);
                            data2->out_pointers().removeOne(tmp);
                            tmp->remove();
                        }
                    }
                }
            }
        } break;
    case DIRECTED:
        foreach(DataPtr data, dataList()) {
            // Just One self pointer allowed.
            bool foundSelfEdge = false;
            foreach(PointerPtr p, data->self_pointers()) {
                if (!foundSelfEdge) {
                    foundSelfEdge = true;
                } else {
                    data->self_pointers().removeOne(p);
                    p->remove();
                }
            }

            // Just one going in, and one going out.
            foreach(DataPtr data2, dataList()) {
                if (data == data2) {
                    continue;
                }

                bool foundOneOut = false;
                foreach(PointerPtr tmp, data->out_pointers()) {
                    if (tmp->to() == data2) {
                        if (!foundOneOut) {
                            foundOneOut = true;
                        } else {
                            data->out_pointers().removeOne(tmp);
                            data2->in_pointers().removeOne(tmp);
                            tmp->remove();
                        }
                    }
                }

                bool foundOneIn = false;
                foreach(PointerPtr tmp, data->in_pointers()) {
                    if (tmp->from() == data2) {
                        if (!foundOneIn) {
                            foundOneIn = true;
                        } else {
                            data->in_pointers().removeOne(tmp);
                            data2->out_pointers().removeOne(tmp);
                            tmp->remove();
                        }
                    }
                }
            }
        } break;
    default: break;
    }

    foreach(PointerPtr pointer, pointers()) {
        QMetaObject::invokeMethod(pointer.get(), "changed");
    }
}

Rocs::GraphStructure::GRAPH_TYPE Rocs::GraphStructure::graphType() const
{
    return _type;
}

bool Rocs::GraphStructure::directed() const
{
    return (_type == DIRECTED || _type == MULTIGRAPH);
}

PointerPtr Rocs::GraphStructure::addPointer(DataPtr from, DataPtr to, int pointerType)
{
    if (_type == UNDIRECTED) {
        if (from == to) {    // self-edges
            return PointerPtr();
        }

        if (from->pointers(to).size() >= 1) {     // back-edges
            return PointerPtr();
        }
    }

    if (_type == DIRECTED) {     // do not add double edges
        PointerList list = from->out_pointers();
        foreach(PointerPtr tmp, list) {
            if (tmp->to() == to) {
                return PointerPtr();
            }
        }
        if (from->self_pointers().size() >= 1) {
            return PointerPtr();
        }
    }

    return DataStructure::addPointer(from, to);
}

DataPtr Rocs::GraphStructure::addData(QString name, int dataType)
{
    if (readOnly()) {
        return DataPtr();
    }
    boost::shared_ptr<GraphNode> n = boost::static_pointer_cast<GraphNode>(
                                         GraphNode::create(getDataStructure(), generateUniqueIdentifier(), dataType)
                                     );
    n->setName(name);
    return addData(n);
}
