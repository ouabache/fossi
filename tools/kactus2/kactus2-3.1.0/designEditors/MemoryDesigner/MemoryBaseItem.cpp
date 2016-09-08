//-----------------------------------------------------------------------------
// File: MemoryBaseItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 22.10.2012
//
// Description:
// Memory base item class.
//-----------------------------------------------------------------------------

#include "MemoryBaseItem.h"

#include "AddressSectionItem.h"

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::MemoryBaseItem()
//-----------------------------------------------------------------------------
MemoryBaseItem::MemoryBaseItem(QGraphicsItem* parent) : QGraphicsRectItem(parent)
{
}

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::~MemoryBaseItem()
//-----------------------------------------------------------------------------
MemoryBaseItem::~MemoryBaseItem()
{
}

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::drawGuides()
//-----------------------------------------------------------------------------
void MemoryBaseItem::drawGuides(QPainter* painter, QRectF const& rect) const
{
    foreach (AddressSectionItem* section, getSections())
    {
        section->drawGuides(painter, rect);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::drawStartAddressDivider()
//-----------------------------------------------------------------------------
void MemoryBaseItem::drawStartAddressDivider(QPainter* painter, QRectF const& rect, int y,
                                             quint64 address) const
{
    foreach (AddressSectionItem* section, getSections())
    {
        if (y >= section->sceneBoundingRect().top() && y <= section->sceneBoundingRect().bottom())
        {
            section->drawStartAddressDivider(painter, rect, y, address);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::drawEndAddressDivider()
//-----------------------------------------------------------------------------
void MemoryBaseItem::drawEndAddressDivider(QPainter* painter, QRectF const& rect, int y,
                                           quint64 address) const
{
    foreach (AddressSectionItem* section, getSections())
    {
        if (y >= section->sceneBoundingRect().top() && y <= section->sceneBoundingRect().bottom())
        {
            section->drawEndAddressDivider(painter, rect, y, address);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::getSections()
//-----------------------------------------------------------------------------
QList<AddressSectionItem*> const& MemoryBaseItem::getSections() const
{
    static QList<AddressSectionItem*> list;
    return list;
}

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::convertAddress()
//-----------------------------------------------------------------------------
quint64 MemoryBaseItem::convertAddress(quint64 /*address*/, MemoryBaseItem* /*source*/) const
{
    return 0xDEADC0DE;
}
