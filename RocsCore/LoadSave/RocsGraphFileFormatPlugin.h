/*
    This file is part of Rocs.
    Copyright 2012       Andreas Cord-Landwehr <cola@uni-paderborn.de>

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

#ifndef ROCSGRAPHFILEFORMATPLUGIN_H
#define ROCSGRAPHFILEFORMATPLUGIN_H

#include "GraphFilePluginInterface.h"

class RocsGraphFileFormatPluginPrivate;

/** \brief class RocsGraphFileFormatPlugin: Import and Export Plugin for internal graph format.
 *
 * A Document Internal format may look like this:
 *
 * #                                '#'s are comments.
 *
 * [Document Properties]            # Canvas Size and Data Structure initialization.
 * Width  : [integer]
 * Height : [integer]
 * DataStructurePlugin : [string]
 *
 * # data types
 * [DataType Identifier1]
 * Name     : [string]
 * IconName : [string]
 * Color    : [RGB in hex]
 *
 * # pointer types
 * [PointerType Identifier1]
 * Name  : [string]
 * Color : [RGB in hex]
 *
 * # Data Structure Definitions.
 * # Data Structures are layered, so there can be N of them.
 * # Every Data and Pointer below a declaration of a DataStructure
 * # belongs to that data structure.
 *
 * [DataStructure Name1]
 *
 * DataColor    : [RGB in hex]  # Color of newly created Data elements ( Nodes )
 * Pointercolor : [RBG in Hex]  # Color of newly created Data elements ( Nodes )
 * name         : [string]      # Name of the data structure acessible in the scripting interface.
 *
 * visibility : [bool]           # is this DataStructure visible?
 *
 * ShowNamesInData      : [bool] # should the canvas show the name of the data on the screen?
 * ShowNamesInNPointers : [bool] # ↑
 * ShowValuesInData     : [bool] # ↑
 * ShowValuesInPointers : [bool] # ↑
 *
 * PluginDefinedProperty1 : propertyValue; # the plugins can define other properties for the data structure.
 * PluginDefinedProperty1 : propertyValue;
 * PluginDefinedProperty1 : propertyValue;
 *
 * UserDefinedProperty1 : propertyValue1 # the user can define other properties for the data structure.
 * UserDefinedProperty2 : propertyValue2
 * UserDefinedProperty3 : propertyValue3
 *
 * [Data 1]
 * type  : [int]
 * x     : [real]
 * y     : [real]
 * color : [string in "0xFFFFFF" format]
 * name  : [string]
 * value : [string]
 * size  : [float]
 *
 * property1 : propertyValue1
 * property2 : propertyValue2
 * property3 : propertyValue3
 *
 * [Data 2] <- same as above.
 * type  : [int]
 * x     : [integer]
 * y     : [integer]
 * color : [string in "0xFFFFFF" format]
 * name  : [string]
 * value : [string]
 * size  : [float]
 *
 * property1 : propertyValue1
 * property2 : propertyValue2
 * property3 : propertyValue3
 *
 * [Pointer 1 -> 2]
 * type     : [int]
 * name     : [string]
 * value    : [string]
 * style    : [string]
 * color    : [string in "0xFFFFFF" format]
 * width    : [float]
 *
 * property1 : propertyValue1
 * property2 : propertyValue2
 * property3 : propertyValue3
 */



class RocsGraphFileFormatPlugin: public GraphFilePluginInterface
{
    Q_OBJECT
public:
    explicit RocsGraphFileFormatPlugin(QObject *parent);
    ~RocsGraphFileFormatPlugin();

    /**
     * File extensions that are common for this file type.
     */
    virtual const QStringList extensions() const;

    /**
     * Writes given graph document to formerly specified file \see setFile().
     * \param graph is graphDocument to be serialized
     */
    virtual void writeFile(Document &graph);

    /**
     * Open given file and imports it into internal format.
     */
    virtual void readFile();

private:
    QString serialize(const Document &document);
    void serializeProperties(QObject *o);

    RocsGraphFileFormatPluginPrivate* d;
};

#endif // ROCSGRAPHFILEFORMATPLUGIN_H