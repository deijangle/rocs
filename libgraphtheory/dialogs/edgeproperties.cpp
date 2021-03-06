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

#include "edgeproperties.h"
#include "edge.h"
#include "graphdocument.h"

#include <KLocalizedString>
#include <KGuiItem>
#include <KStandardGuiItem>
#include <QDialogButtonBox>
#include <QPointer>
#include <QPushButton>
#include <QDebug>

using namespace GraphTheory;

EdgeProperties::EdgeProperties(QWidget* parent)
    : QDialog(parent)
    , m_edge(EdgePtr())
{
    setWindowTitle(i18nc("@title:window", "Edge Properties"));

    QWidget *widget = new QWidget(this);
    ui = new Ui::EdgeProperties;
    ui->setupUi(widget);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->addWidget(widget);

    // buttons
    QDialogButtonBox *buttons = new QDialogButtonBox(this);

    QPushButton *okButton = new QPushButton;
    KGuiItem::assign(okButton, KStandardGuiItem::ok());
    okButton->setShortcut(Qt::Key_Return);

    QPushButton *cancelButton = new QPushButton;
    KGuiItem::assign(cancelButton, KStandardGuiItem::cancel());
    cancelButton->setShortcut(Qt::Key_Escape);

    buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
    buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
    mainLayout->addWidget(buttons);
    connect(okButton, &QPushButton::clicked, this, &EdgeProperties::accept);
    connect(cancelButton, &QPushButton::clicked, this, &EdgeProperties::reject);

    connect(this, &QDialog::accepted, this, &EdgeProperties::apply);

    setAttribute(Qt::WA_DeleteOnClose);
}

void EdgeProperties::setData(EdgePtr edge)
{
    if (m_edge == edge) {
        return;
    }
    m_edge = edge;

    // update types
    ui->type->clear();
    int selectedType = -1;
    for (int i = 0; i < edge->from()->document()->edgeTypes().count(); ++i) {
        EdgeTypePtr type = edge->from()->document()->edgeTypes().at(i);
        if (type == edge->type()) {
            selectedType = i;
        }
        QString typeString = QString("%1").arg(type->name());
        ui->type->addItem(typeString, QVariant(i));
    }
    ui->type->setCurrentIndex(ui->type->findData(QVariant(selectedType)));

    // dynamic properties
    ui->dynamicProperties->setColumnCount(2);
    ui->dynamicProperties->setRowCount(edge->dynamicProperties().count());
    QTableWidgetItem *nameHeaderItem = new QTableWidgetItem(i18n("Name"));
    QTableWidgetItem *valueHeaderItem = new QTableWidgetItem(i18n("Value"));
    ui->dynamicProperties->setHorizontalHeaderItem(0, nameHeaderItem);
    ui->dynamicProperties->setHorizontalHeaderItem(1, valueHeaderItem);
    for (int i = 0; i < edge->dynamicProperties().count(); ++i) {
        QString propertyName = edge->dynamicProperties().at(i);
        QTableWidgetItem *nameItem = new QTableWidgetItem(propertyName);
        nameItem->setFlags(Qt::NoItemFlags);
        QTableWidgetItem *valueItem = new QTableWidgetItem(edge->dynamicProperty(propertyName).toString());
        ui->dynamicProperties->setItem(i, 0, nameItem);
        ui->dynamicProperties->setItem(i, 1, valueItem);
    }

    ui->dynamicProperties->horizontalHeader()->setProperty("stretchLastSection", true);
}

void EdgeProperties::apply()
{
    m_edge->setType(m_edge->from()->document()->edgeTypes().at(ui->type->currentIndex()));

    for (int i = 0; i < ui->dynamicProperties->rowCount(); ++i) {
        QString name = ui->dynamicProperties->item(i, 0)->data(Qt::DisplayRole).toString();
        QVariant value =ui->dynamicProperties->item(i, 1)->data(Qt::DisplayRole);
        m_edge->setDynamicProperty(name, value);
    }
}
