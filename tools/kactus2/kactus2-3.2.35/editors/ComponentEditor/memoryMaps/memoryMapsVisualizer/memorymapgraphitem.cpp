//-----------------------------------------------------------------------------
// File: localmemorymapgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 17.12.2012
//
// Description:
// The graph item that visualizes a local memory map within address space.
//-----------------------------------------------------------------------------

#include "memorymapgraphitem.h"

#include <editors/ComponentEditor/visualization/memorygapitem.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::MemoryMapGraphItem()
//-----------------------------------------------------------------------------
MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> parentMemoryMap,
    QSharedPointer<MemoryMapBase> memoryRemap, QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent /* = 0 */):
MemoryVisualizationItem(expressionParser, parent),
memoryMap_(memoryRemap),
parentMemoryMap_(parentMemoryMap)
{
    Q_ASSERT(memoryMap_);
    QBrush brush(KactusColors::MEM_MAP_COLOR);
    setDefaultBrush(brush);

    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::~MemoryMapGraphItem()
//-----------------------------------------------------------------------------
MemoryMapGraphItem::~MemoryMapGraphItem() 
{
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::refresh()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::refresh() 
{
    updateDisplay();
    reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::updateDisplay()
{
    setName(memoryMap_->name());

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + memoryMap_->name() + "<br>" +
        "<b>AUB: </b>" + QString::number(getAddressUnitSize()) + "<br>" +
        "<b>First address: </b>" + toHexString(offset) + "<br>" +
        "<b>Last address: </b>" + toHexString(lastAddress));
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getOffset() const 
{
    if (childItems_.isEmpty())
    {
        return 0;
    }

    quint64 offset = childItems_.last()->getOffset();
    foreach (MemoryVisualizationItem* child, childItems_.values())
    {
        if (dynamic_cast<MemoryGapItem*>(child) == 0)
        {
            offset = qMin(child->getOffset(), offset);  
        }
    }

    return offset;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int MemoryMapGraphItem::getBitWidth() const 
{
    int width = 0;
    foreach (MemoryVisualizationItem* child, childItems_.values())
    {
        if (dynamic_cast<MemoryGapItem*>(child) == 0)
        {
            width = qMax(child->getBitWidth(), width);  
        }
    }
    return width;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int MemoryMapGraphItem::getAddressUnitSize() const 
{
    return parseExpression(parentMemoryMap_->getAddressUnitBits());
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getLastAddress() const 
{
    quint64 lastAddress = 0;
    foreach (MemoryVisualizationItem* child, childItems_.values())
    {
        if (dynamic_cast<MemoryGapItem*>(child) == 0)
        {
            lastAddress = qMax(child->getLastAddress(), lastAddress);  
        }
    }

    return lastAddress;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::itemTotalWidth()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphItem::itemTotalWidth() const 
{
    return VisualizerItem::DEFAULT_WIDTH;
}
