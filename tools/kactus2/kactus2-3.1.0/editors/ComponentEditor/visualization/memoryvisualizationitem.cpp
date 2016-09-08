//-----------------------------------------------------------------------------
// File: memoryvisualizationitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2012
//
// Description:
// A base class for graphics items to visualize memory objects.
//-----------------------------------------------------------------------------

#include "memoryvisualizationitem.h"
#include "memorygapitem.h"

#include <common/KactusColors.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QRectF>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::MemoryVisualizationItem()
//-----------------------------------------------------------------------------
MemoryVisualizationItem::MemoryVisualizationItem(QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent /* = 0 */):
ExpandableItem(parent),
childItems_(),
firstFreeAddress_(-1),
lastFreeAddress_(-1),
childWidth_(VisualizerItem::DEFAULT_WIDTH),
conflicted_(false),
overlapped_(false),
expressionParser_(expressionParser)
{
    QPen pen(Qt::gray);
    setPen(pen);
    setExpansionPen(pen);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::~MemoryVisualizationItem()
//-----------------------------------------------------------------------------
MemoryVisualizationItem::~MemoryVisualizationItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setBrush()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setBrush(QBrush const& brush)
{
    defaultBrush_ = brush;
    QAbstractGraphicsShapeItem::setBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::addChild()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::addChild(MemoryVisualizationItem* childItem)
{
    childItems_.insertMulti(childItem->getOffset(), childItem);

    childItem->setWidth(childWidth_);
    childItem->setVisible(isExpanded());

    reorganizeChildren();
    emit expandStateChanged();

    connect(childItem, SIGNAL(expandStateChanged()), this, SLOT(reorganizeChildren()), Qt::UniqueConnection);
    connect(childItem, SIGNAL(expandStateChanged()), this, SIGNAL(expandStateChanged()), Qt::UniqueConnection);

    connect(childItem, SIGNAL(destroyed(QObject*)), this, SLOT(reorganizeChildren()), Qt::UniqueConnection);
    connect(childItem, SIGNAL(destroyed(QObject*)), this, SIGNAL(expandStateChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::removeChild()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::removeChild(MemoryVisualizationItem* childItem)
{
    quint64 offset = childItems_.key(childItem);

    Q_ASSERT(childItems_.contains(offset));
    childItems_.remove(offset, childItem);

    showExpandIconIfHasChildren();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setWidth()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setWidth(qreal width)
{
    qreal newChildWidth = width - MemoryVisualizationItem::CHILD_INDENTATION;

    if (childWidth_ != newChildWidth)
    {
        childWidth_ = newChildWidth;

        VisualizerItem::setWidth(width);
        ExpandableItem::reorganizeChildren();

        int childCount = childItems_.count();
        for (int i = 0; i < childCount; i++)
        {
            childItems_.values().at(i)->setWidth(newChildWidth);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::boundingRect()
//-----------------------------------------------------------------------------
QRectF MemoryVisualizationItem::boundingRect() const
{
    return itemTotalRect();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setDisplayOffset()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setDisplayOffset(quint64 const& address)
{
    firstFreeAddress_ = address;
    setLeftTopCorner(firstFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_)
    {        
        setLeftBottomCorner("");
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::getDisplayOffset()
//-----------------------------------------------------------------------------
quint64 MemoryVisualizationItem::getDisplayOffset()
{
    return firstFreeAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setDisplayLastAddress()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setDisplayLastAddress(quint64 const& address)
{
    lastFreeAddress_ = address;
    setLeftBottomCorner(lastFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_)
    {
        setLeftBottomCorner("");
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::getDisplayLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryVisualizationItem::getDisplayLastAddress()
{
    return lastFreeAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setCompleteOverlap()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setCompleteOverlap()
{
    overlapped_ = true;
    setConflicted(true);
    
    setLeftTopCorner("");
    setLeftBottomCorner("");
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::isCompletelyOverlapped()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::isCompletelyOverlapped() const
{
    return overlapped_;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setConflicted()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setConflicted(bool conflicted)
{
    conflicted_ = conflicted;
    if (conflicted)
    {
        QAbstractGraphicsShapeItem::setBrush(KactusColors::MISSING_COMPONENT);
        setExpansionBrush(KactusColors::MISSING_COMPONENT);        
    }
    else
    {
        QAbstractGraphicsShapeItem::setBrush(defaultBrush());
        setExpansionBrush(defaultBrush());  
        overlapped_ = false;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::isConflicted()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::isConflicted() const
{
    return conflicted_;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::reorganizeChildren()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::reorganizeChildren()
{
    showExpandIconIfHasChildren();

    if (mustRepositionChildren())
    {    
        int childCountBeforeUpdate = childItems_.count();
        updateChildMap();
        int childCountAfterUpdate = childItems_.count();

        repositionChildren();

        if (childCountBeforeUpdate != childCountAfterUpdate)
        {
            emit expandStateChanged();
        }
    }

    ExpandableItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::isPresent()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::isPresent() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: memoryvisualizationitem::parseExpression()
//-----------------------------------------------------------------------------
int MemoryVisualizationItem::parseExpression(QString const& expression) const
{
    return expressionParser_->parseExpression(expression).toUInt();
}

//-----------------------------------------------------------------------------
// Function: memoryvisualizationitem::getExpressionParser()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionParser> MemoryVisualizationItem::getExpressionParser() const
{
    return expressionParser_;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::showExpandIconIfHasChildren()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::showExpandIconIfHasChildren()
{
    int childCount = childItems_.count();
    for (int i = 0; i < childCount; i++)
    {
        MemoryVisualizationItem* item = childItems_.values().at(i);

        MemoryGapItem* gap = dynamic_cast<MemoryGapItem*>(item);
        if (!gap)
        {
            ExpandableItem::setShowExpandableItem(true);
            return;
        }
    }

    ExpandableItem::setShowExpandableItem(false);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::updateChildMap()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::updateChildMap()
{
    QMap<quint64, MemoryVisualizationItem*> updatedMap;

    int childCount = childItems_.count();
    for (int i = 0; i < childCount; i++)
    {
        MemoryVisualizationItem* item = childItems_.values().at(i);

        MemoryGapItem* gap = dynamic_cast<MemoryGapItem*>(item);
        if (gap)
        {
            childItems_.remove(gap->getOffset(), gap);
            childCount = childItems_.count();
            delete gap;
        }
    }

    quint64 lastAvailableAddress = getLastAddress();
    childCount = childItems_.count();
    for (int i = 0; i < childCount; i++)
    {
        MemoryVisualizationItem* item = childItems_.values().at(i);

        item->updateDisplay();
        item->setConflicted(item->getLastAddress() > lastAvailableAddress);
        item->setVisible(isExpanded() && item->isPresent());

        updatedMap.insertMulti(item->getOffset(), item);
    }

    // Sort childs with same offset for stable order.
    foreach(quint64 offset, updatedMap.uniqueKeys())
    {
        if(updatedMap.count(offset) != 1)
        {
            QList<MemoryVisualizationItem*> childs = updatedMap.values(offset);
            updatedMap.remove(offset);

            qStableSort(childs.begin(), childs.end(), compareItems);

            foreach(MemoryVisualizationItem* child, childs)
            {
                updatedMap.insertMulti(offset, child);
            }
        }
    }

    childItems_ = updatedMap;

    quint64 lastAddressInUse = getOffset();

    MemoryGapItem* previousOverlap = 0;
    MemoryVisualizationItem* previous = 0;
    quint64 previousLastAddress = lastAddressInUse;
    childCount = childItems_.count();
    for (int i = 0; i < childCount; i++)
    {
        MemoryVisualizationItem* current = childItems_.values().at(i);

        if (current->isPresent())
        {
            quint64 currentOffset = current->getOffset();
            quint64 currentLastAddress = current->getLastAddress();
            if (emptySpaceBeforeFirstChild(current))
            {
                createMemoryGap(getOffset(), currentOffset - 1);
            }  
            else if (emptySpaceBeforeChild(current, lastAddressInUse))
            {
                createMemoryGap(lastAddressInUse + 1, currentOffset - 1);
            }
            else if (childrenOverlap(current, previous))
            {
                current->setConflicted(true);
                previous->setConflicted(true);

                if (currentLastAddress > previousLastAddress)
                {
                    current->setDisplayOffset(previousLastAddress + 1);

                    previous->setDisplayLastAddress(qMin(previousLastAddress, currentOffset - 1));

                    // If previous block is completely overlapped by the preceding block and this block.
                    if (previousOverlap && previousOverlap->getLastAddress() + 1 >= currentOffset)
                    {
                        previous->setCompleteOverlap();
                    }

                    previousOverlap = createConflictItem(currentOffset, previousLastAddress);
                }
                else
                {
                    quint64 nonOverlappingOffset = qMax(currentOffset, previous->getDisplayOffset());
                    current->setDisplayOffset(nonOverlappingOffset);
                }
            }
            else if (previous && currentOffset <= lastAddressInUse)
            {
                current->setConflicted(true);
            }


            lastAddressInUse = qMax(currentLastAddress, lastAddressInUse);

            previous = current;
            previousLastAddress = currentLastAddress;
        }
    }

    // Fill in any addresses left between children and the end of this item.
    if (!childItems_.isEmpty() && getLastAddress() > lastAddressInUse)
    {
        createMemoryGap(lastAddressInUse + 1, getLastAddress());
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::mustRepositionChildren()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::mustRepositionChildren()
{
    return !childItems_.isEmpty() && isExpanded();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::repositionChildren()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::repositionChildren()
{
    qreal yCoordinate = rect().bottom();

    int childCount = childItems_.count();
    for (int i = 0; i < childCount; i++)
    {
        MemoryVisualizationItem* current = childItems_.values().at(i);

        if (current->isPresent())
        {
            current->setPos(MemoryVisualizationItem::CHILD_INDENTATION, yCoordinate);
            yCoordinate = mapRectFromItem(current, current->itemTotalRect()).bottom();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::recursiveRefresh()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::recursiveRefresh()
{
    // Note: Refreshing children may change memory gap items, foreach for childItems_ will not work.
    int childCount = childItems_.count();
    for (int i = 0; i < childCount; i++)
    {
        childItems_.values().at(i)->recursiveRefresh();
    }

    refresh();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::mousePressEvent()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
    emit selectEditor();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setLeftTopCorner()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setLeftTopCorner(QString const& text) 
{
    VisualizerItem::setLeftTopCorner(groupByFourDigits(text));
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setLeftTopCorner()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setLeftTopCorner(quint64 address)
{
    VisualizerItem::setLeftTopCorner(toHexString(address));
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::toHexString()
//-----------------------------------------------------------------------------
QString MemoryVisualizationItem::toHexString(quint64 address)
{
    QString str = QString::number(address, 16);
    str = str.toUpper();

    int bitWidth = getBitWidth();

    // one hexadecimal number accounts for four bits
    int fieldSize = bitWidth / 4;
    if (fieldSize*4 < bitWidth)
    {
        fieldSize++; //Round upwards, e.g. 7bits needs 4 hex digits
    }

    QString padded = QString("%1").arg(str, fieldSize, QChar('0'));
    return groupByFourDigits(padded);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setLeftBottomCorner()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setLeftBottomCorner(QString const& text)
{
    VisualizerItem::setLeftBottomCorner(groupByFourDigits(text));
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setLeftBottomCorner()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setLeftBottomCorner(quint64 address)
{
    VisualizerItem::setLeftBottomCorner(toHexString(address));
}
//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::emptySpaceBeforeFirstChild()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::emptySpaceBeforeFirstChild(MemoryVisualizationItem* current) const
{
    return current == childItems_.first() && current->getOffset() > getOffset(); 
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::emptySpaceBeforeChild()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::emptySpaceBeforeChild(MemoryVisualizationItem* current, quint64 lastAddressInUse) const
{
    return current->getOffset() > lastAddressInUse + 1;
}
//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::childrenOverlap()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::childrenOverlap(MemoryVisualizationItem* current, MemoryVisualizationItem* previous) const
{
    return previous && previous->getLastAddress() >= current->getOffset();
}
//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::createConflictItem()
//-----------------------------------------------------------------------------
MemoryGapItem* MemoryVisualizationItem::createConflictItem(qint64 offset, qint64 lastAddress)
{
    MemoryGapItem* gap = new MemoryGapItem(expressionParser_, this);
    gap->setWidth(childWidth_);
    gap->setConflicted(true);
    gap->setName("conflicted");
    gap->setStartAddress(offset);
    gap->setEndAddress(lastAddress);               
    gap->setVisible(isExpanded());

    childItems_.insert(gap->getOffset(), gap);
    return gap;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::createMemoryGap()
//-----------------------------------------------------------------------------
MemoryGapItem* MemoryVisualizationItem::createMemoryGap(quint64 offset, quint64 lastAddress)
{
    MemoryGapItem* gap = new MemoryGapItem(expressionParser_, this);
    gap->setWidth(childWidth_);
    gap->setStartAddress(offset);
    gap->setEndAddress(lastAddress);
    gap->setVisible(isExpanded());

    childItems_.insert(gap->getOffset(), gap);
    return gap;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::compareItems()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::compareItems(const MemoryVisualizationItem* s1, const MemoryVisualizationItem* s2)
{
    // Item with bigger last address precedes the other.
    return s1->getLastAddress() > s2->getLastAddress();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::groupByFourDigits()
//-----------------------------------------------------------------------------
QString MemoryVisualizationItem::groupByFourDigits(QString const& text) const
{
    QString groupedText(text);

    int size = text.size();
    for (int i = size; i > 0; i -= 4) 
    {
        groupedText.insert(i, " ");
    }

    return groupedText;
}
