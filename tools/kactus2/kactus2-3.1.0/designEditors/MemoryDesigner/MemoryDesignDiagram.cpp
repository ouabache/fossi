//-----------------------------------------------------------------------------
// File: MemoryDesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M��tt�
// Date: 14.9.2012
//
// Description:
// Implements the memory design diagram class.
//-----------------------------------------------------------------------------

#include "MemoryDesignDiagram.h"

#include "MemoryDesignWidget.h"
#include "MemoryItem.h"
#include "AddressSpaceItem.h"
#include "AddressSectionItem.h"
#include "MemoryColumn.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>

#include <library/LibraryManager/libraryinterface.h>

#include <common/utils.h>
#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumn.h>

#include <designEditors/HWDesign/columnview/ColumnEditDialog.h>
#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>
#include <designEditors/common/StickyNote/StickyNote.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Channel.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/SlaveInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>


//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram()
//-----------------------------------------------------------------------------
MemoryDesignDiagram::MemoryDesignDiagram(LibraryInterface* lh, 
                                         QSharedPointer<IEditProvider> editProvider,
                                         MemoryDesignWidget* parent)
    : DesignDiagram(lh, editProvider, parent),
      parent_(parent),
      oldSelection_(0),
      resizingSubsection_(false),
      dualSubsectionResize_(false),
      subsectionResizeBottom_(0.0)
{
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(editProvider.data(), SIGNAL(modified()), this, SIGNAL(contentChanged()));
}

//-----------------------------------------------------------------------------
// Function: ~MemoryDesignDiagram()
//-----------------------------------------------------------------------------
MemoryDesignDiagram::~MemoryDesignDiagram()
{
}

//-----------------------------------------------------------------------------
// Function: setDesign()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::loadDesign(QSharedPointer<Design> design)
{
    getLayout()->setAutoReorganized(true);
    getLayout()->setAutoCreateColumnFunction(&MemoryDesignDiagram::createDefaultColumn);

    if (!design->getColumns().isEmpty())
    {
        design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Available Memory", ColumnTypes::BUSES, 0, COLUMN_WIDTH)));
        design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Required Address Spaces", ColumnTypes::COMPONENTS, 0, COLUMN_WIDTH)));
    }

    foreach(QSharedPointer<ColumnDesc> desc, design->getColumns())
    {
        GraphicsColumn* column = new MemoryColumn(desc, getLayout().data());
        getLayout()->addColumn(column, true);
    }

    // Create (HW) component instances.
    foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
    {
        QSharedPointer<Document> libComponent = getLibraryInterface()->getModel(*instance->getComponentRef());
        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        if (!component)
        {
            emit errorMessage(tr("The component '%1' instantiated in the design '%2' "
                "was not found in the library").arg(
                instance->getComponentRef()->getName()).arg(design->getVlnv().getName()));

            // Create an unpackaged component so that we can still visualize the component instance->
            component = QSharedPointer<Component>(new Component(*instance->getComponentRef()));
            component->setImplementation(KactusAttribute::HW);
        }

        foreach (QSharedPointer<MemoryMap> map, *component->getMemoryMaps())
        {
            MemoryItem* item = new MemoryItem(getLibraryInterface(), instance->getInstanceName(),
                                              component, map, 0);
            getLayout()->addItem(item);
        }

        foreach (QSharedPointer<AddressSpace> addressSpace, *component->getAddressSpaces())
        {
            AddressSpaceItem* item = new AddressSpaceItem(getLibraryInterface(), instance->getInstanceName(),
                                                          component, addressSpace, 0);
            getLayout()->getColumns().at(1)->addItem(item);
        }
    }

    // Refresh the layout so that all components are placed in correct positions according to the stacking.
    getLayout()->updatePositions();

    design_ = design;
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // If other than left button was pressed return back to select mode.
    if (event->button() != Qt::LeftButton)
    {
        setMode(MODE_SELECT);
        return;
    }

    if (getMode() == MODE_DRAFT)
    {
    }
    else if (getMode() == MODE_SELECT)
    {
        // Handle the mouse press.
        QGraphicsScene::mousePressEvent(event);
    }
    else if(getMode() == MODE_LABEL)
    {
        if (!isProtected())
        {
            createNoteAt(event->scenePos());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onSelected()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::onSelected(QGraphicsItem* newSelection)
{
    ComponentItem* selectedComponent = dynamic_cast<ComponentItem*>(newSelection);

    // Activate the correct views when something has been selected.
    if (selectedComponent)
    { 
        emit componentSelected(selectedComponent);
    }
    else
    {
        // Clear the selection.
        emit clearItemSelection();
        emit helpUrlRequested("memorydesign/memorydesign.html");
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // Process the normal mouse release event.
    QGraphicsScene::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> MemoryDesignDiagram::createDesign(VLNV const& vlnv) const
{
    QSharedPointer<Design> design(new Design(vlnv));
    // TODO:
    return design;
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::addColumn(QSharedPointer<ColumnDesc> desc)
{
    GraphicsColumn* column = new MemoryColumn(desc, getLayout().data());

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(getLayout().data(), column, getDesign()));
    getEditProvider()->addCommand(cmd);
    cmd->redo();
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
// void MemoryDesignDiagram::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
// {
//     // Allow double click only when the mode is select.
//     if (getMode() != MODE_SELECT)
//     {
//         return;
//     }
// 
//     // Find the item under the cursor.
//     QGraphicsItem *item = itemAt(snapPointToGrid(event->scenePos()));
// 
//     if (item == 0)
//     {
//         return;
//     }
// 
// //     if (dynamic_cast<SystemComponentItem*>(item) != 0)
// //     {
// //     }
// //     else
//     if (item->type() == GraphicsColumn::Type)
//     {
//         if (!isProtected())
//         {
//             item->setSelected(true);
//             GraphicsColumn* column = qgraphicsitem_cast<GraphicsColumn*>(item);
// 
//             // Columns don't have adjustable content settings.
//             ColumnEditDialog dialog((QWidget*)parent(), false, column);
//             dialog.hideContentSettings();
//             
//             if (dialog.exec() == QDialog::Accepted)
//             {
//                 ColumnDesc desc(dialog.name(), dialog.getContentType(), dialog.getAllowedItems(),
//                                 column->getColumnDesc().getWidth());
// 
//                 QSharedPointer<QUndoCommand> cmd(new GraphicsColumnChangeCommand(column, desc));
//                 getEditProvider()->addCommand(cmd);
//             }
//         }
//     }
// }

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::updateHierComponent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::updateHierComponent()
{
}

//-----------------------------------------------------------------------------
// Function: onSelectionChanged()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::onSelectionChanged()
{
    // Retrieve the new selection.
    QGraphicsItem* newSelection = 0;

    if (!selectedItems().isEmpty())
    {
        newSelection = selectedItems().front();
    }

    onSelected(newSelection);

    // Save the current selection as the old selection.
    oldSelection_ = newSelection;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::beginResize()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::beginResizeSubsection(bool dual, qreal bottom)
{
    dualSubsectionResize_ = dual;
    subsectionResizeBottom_ = bottom;
    resizingSubsection_ = true;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::updateResize()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::updateResizeSubsection(qreal bottom)
{
    subsectionResizeBottom_ = bottom;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::endResize()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::endResizeSubsection()
{
    resizingSubsection_ = false;
    invalidate();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::drawForeground()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::drawForeground(QPainter* painter, const QRectF& rect)
{
    painter->setWorldMatrixEnabled(true);

    if (resizingSubsection_)
    {
        QPen pen(Qt::black, 0);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);

        painter->drawLine(rect.left(), subsectionResizeBottom_, rect.right(), subsectionResizeBottom_);

        if (dualSubsectionResize_)
        {
            painter->drawLine(rect.left(), subsectionResizeBottom_ + 10, rect.right(), subsectionResizeBottom_ + 10);
        }
    }

    drawMemoryDividers(painter, rect);

    // Draw guides for all selected items.
    foreach (QGraphicsItem* item, selectedItems())
    {
        MemoryBaseItem* baseItem = dynamic_cast<MemoryBaseItem*>(item);

        if (baseItem != 0)
        {
            baseItem->drawGuides(painter, rect);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::createDefaultColumn()
//-----------------------------------------------------------------------------
GraphicsColumn* MemoryDesignDiagram::createDefaultColumn(GraphicsColumnLayout* layout)
{
    QSharedPointer<ColumnDesc> desc(new ColumnDesc("Required Address Spaces",
        ColumnTypes::COMPONENTS, 0, COLUMN_WIDTH));
    return new MemoryColumn(desc, layout);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::drawMemoryDividers()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::drawMemoryDividers(QPainter* painter, QRectF const& rect)
{
    // Draw section dividers to the address spaces based on the memory map positions.
    MemoryColumn* memoryColumn = 0;

    foreach (GraphicsColumn* column, getLayout()->getColumns())
    {
        MemoryColumn* memColumn = static_cast<MemoryColumn*>(column);

        if (memColumn->getContentType() == ColumnTypes::BUSES)
        {
            memoryColumn = memColumn;
            break;
        }
    }

    foreach (QGraphicsItem* item, memoryColumn->getItems())
    {
        MemoryItem* memItem = static_cast<MemoryItem*>(item);

        foreach (AddressSectionItem* block, memItem->getSections())
        {
            int top = block->sceneBoundingRect().top();
            int bottom = block->sceneBoundingRect().bottom();
                
            if (top >= rect.top() && top < rect.bottom())
            {
                foreach (GraphicsColumn* column, getLayout()->getColumns())
                {
                    MemoryBaseItem* addrSpaceItem = static_cast<MemoryColumn*>(column)->findItemAt(top);
                    
                    if (addrSpaceItem != 0)
                    {
                        quint64 address = addrSpaceItem->convertAddress(block->getStartAddress(), memItem);
                        addrSpaceItem->drawStartAddressDivider(painter, rect, top, address);
                    }
                }
            }

            if (bottom >= rect.top() && bottom < rect.bottom())
            {
                foreach (GraphicsColumn* column, getLayout()->getColumns())
                {
                    MemoryBaseItem* addrSpaceItem = static_cast<MemoryColumn*>(column)->findItemAt(bottom);

                    if (addrSpaceItem != 0)
                    {
                        quint64 address = addrSpaceItem->convertAddress(block->getEndAddress(), memItem);
                        addrSpaceItem->drawEndAddressDivider(painter, rect, bottom, address);
                    }
                }
            }
        }
    }

    foreach (GraphicsColumn* column, getLayout()->getColumns())
    {
        // Check if this is an address space column.
        if (column->getContentType() == ColumnTypes::COMPONENTS)
        {
            MemoryColumn* memColumn = static_cast<MemoryColumn*>(column);

            foreach (QGraphicsItem* item, memColumn->getItems())
            {
                AddressSpaceItem* addrSpaceItem = static_cast<AddressSpaceItem*>(item);

                foreach (AddressSectionItem* segment, addrSpaceItem->getSections())
                {
                    int top = segment->sceneBoundingRect().top();
                    int bottom = segment->sceneBoundingRect().bottom();

                    if (top >= rect.top() && top < rect.bottom())
                    {
                        MemoryBaseItem* memItem = memoryColumn->findItemAt(top);

                        if (memItem != 0)
                        {
                            quint64 address = memItem->convertAddress(segment->getStartAddress(), addrSpaceItem);
                            memItem->drawStartAddressDivider(painter, rect, top, address);
                        }
                    }

                    if (bottom >= rect.top() && bottom < rect.bottom())
                    {
                        MemoryBaseItem* memItem = memoryColumn->findItemAt(bottom);

                        if (memItem != 0)
                        {
                            quint64 address = memItem->convertAddress(segment->getEndAddress(), addrSpaceItem);
                            memItem->drawEndAddressDivider(painter, rect, bottom, address);
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::isConnected()
//-----------------------------------------------------------------------------
bool MemoryDesignDiagram::isConnected(AddressSpaceItem const* addrSpaceItem, MemoryItem const* memoryItem,
                                      quint64* addressOffset) const
{
    // Find the route from the component containing the given address space to a component
    // containing the given memory map.
    foreach (QSharedPointer<BusInterface> busIf, *addrSpaceItem->getComponent()->getBusInterfaces())
    {
        // Check if the bus interface has the correct address space as a reference.
        if (busIf->getInterfaceMode() == General::MASTER &&
            busIf->getMaster()->getAddressSpaceRef() == addrSpaceItem->getAddressSpace()->name())
        {
            quint64 addressOffsetTemp = 0;

            if (findRoute(addrSpaceItem->getInstanceName(), busIf, memoryItem, addressOffsetTemp))
            {
                *addressOffset = addressOffsetTemp;
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::findRoute()
//-----------------------------------------------------------------------------
bool MemoryDesignDiagram::findRoute(QString const& instanceName, QSharedPointer<BusInterface const> busIf,
                                    MemoryItem const* memoryItem, quint64& addressOffset) const
{
    // Check all connections that start from the bus interface.
    foreach (QSharedPointer<Interconnection> conn, *design_->getInterconnections())
    {
        ActiveInterface const* interface = 0;

        QSharedPointer<ActiveInterface> startInterface = conn->getStartInterface();
        QSharedPointer<ActiveInterface> endInterface = conn->getActiveInterfaces()->first();

        if (startInterface->references(instanceName, busIf->name()) )
        {
            interface = endInterface.data();
        }

        if (endInterface->references(instanceName, busIf->name()))
        {
            interface = startInterface.data();
        }

        // Check if we found a matching connection.
        if (interface != 0)
        {
            // Retrieve the component referenced by the connection.
            QString connectionComponentInstance = interface->getComponentReference();
            QSharedPointer<Component const> component = getComponentByInstanceName(connectionComponentInstance);
            
            if (component != 0)
            {
                // Retrieve the correct bus interface.
                QString connectionInterface = interface->getBusReference();
                QSharedPointer<BusInterface const> otherBusIf = component->getBusInterface(connectionInterface);

                if (otherBusIf != 0)
                {
                    // Master can connect to either slave or mirrored master.
                    if (busIf->getInterfaceMode() == General::MASTER)
                    {
                        // If the other bus interface is slave, we either have a connection to the correct
                        // component or then this was a wrong route.
                        if (otherBusIf->getInterfaceMode() == General::SLAVE &&
                            connectionComponentInstance == memoryItem->getInstanceName())
                        {
                            addressOffset = Utils::str2Uint(busIf->getMaster()->getBaseAddress());
                            return true;
                        }
                        // With mirrored master, the route continues through a channel.
                        else if (otherBusIf->getInterfaceMode() == General::MIRROREDMASTER)
                        {
                            foreach (QSharedPointer<Channel> channel, *component->getChannels())
                            {
                                // Check if the channel contains the bus interface in question.
                                if (channel->getInterfaces().contains(connectionInterface))
                                {
                                    foreach (QString const& chIfName, channel->getInterfaces())
                                    {
                                        if (chIfName != connectionInterface)
                                        {
                                            // Retrieve the bus interface.
                                            QSharedPointer<BusInterface const> nextBusIf = component->getBusInterface(chIfName);

                                            if (nextBusIf != 0)
                                            {
                                                quint64 addressOffsetTemp = 0;

                                                if (findRoute(connectionComponentInstance, nextBusIf, memoryItem,
                                                              addressOffsetTemp))
                                                {
                                                    addressOffset = addressOffsetTemp + Utils::str2Uint(busIf->getMaster()->getBaseAddress());
                                                    return true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            return false;
                        }
                    }
                    // Mirrored slave can only connect to a slave.
                    else if (busIf->getInterfaceMode() == General::MIRROREDSLAVE &&
                             otherBusIf->getInterfaceMode() == General::SLAVE)
                    {
                        // Check if we ended up in the correct component.
                        if (connectionComponentInstance == memoryItem->getInstanceName())
                        {
                            addressOffset = Utils::str2Uint(busIf->getMirroredSlave()->getRemapAddresses()->first()->remapAddress_);
                            return true;
                        }

                        // Otherwise check if the route continues through bridges.
                        foreach (QSharedPointer<SlaveInterface::Bridge const> bridge, 
                            *otherBusIf->getSlave()->getBridges())
                        {
                            QSharedPointer<BusInterface const> nextBusIf = component->getBusInterface(bridge->masterRef_);

                            if (nextBusIf != 0)
                            {
                                quint64 addressOffsetTemp = 0;

                                if (findRoute(connectionComponentInstance, nextBusIf, memoryItem,
                                    addressOffsetTemp))
                                {
                                    addressOffset = addressOffsetTemp + Utils::str2Uint(busIf->getMaster()->getBaseAddress());
                                    return true;
                                }
                            }
                        }
                    }

                    break;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::getComponentByInstanceName()
//-----------------------------------------------------------------------------
QSharedPointer<Component const> MemoryDesignDiagram::getComponentByInstanceName(QString const& componentRef) const
{
    foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        if (instance->getInstanceName() == componentRef)
        {
            QSharedPointer<Document const> libComp =
                getLibraryInterface()->getModelReadOnly(*instance->getComponentRef());
            return libComp.dynamicCast<Component const>();
        }
    }

    return QSharedPointer<Component const>();
}
