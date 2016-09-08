//-----------------------------------------------------------------------------
// File: GraphicsColumnLayout.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 31.5.2011
//
// Description:
// Graphics column layout which manages the ordering and positioning
// of graphics columns.
//-----------------------------------------------------------------------------

#include "GraphicsColumnLayout.h"

#include "GraphicsColumn.h"

#include <common/layouts/HStackedLayout.h>

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::GraphicsColumnLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout::GraphicsColumnLayout(QGraphicsScene* scene)
    : scene_(scene),
      layout_(new HStackedLayout<GraphicsColumn>(0.0)),
      columns_(),
      layoutWidth_(0.0),
      offsetY_(0),
      autoReorganized_(false),
      autoCreateColumnFunc_(0)
{
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::~GraphicsColumnLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout::~GraphicsColumnLayout()
{
    // Delete all columns and clear the column list.
    for (int i = 0; i < columns_.size(); ++i)
    {
        delete columns_.at(i);
    }

    columns_.clear();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::addItem()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::addItem(QGraphicsItem* item)
{
    for (int i = 0; i < columns_.size(); ++i)
    {
        if (columns_.at(i)->isItemAllowed(item))
        {
            columns_.at(i)->addItem(item);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
//  Function: GraphicsColumnLayout::addColumn()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::addColumn(GraphicsColumn* column, bool append)
{
    if (column->scene() == 0)
    {
        scene_->addItem(column);
    }

    if (append)
    {
        column->setPos(layoutWidth_, 0.0);
    }

    column->setOffsetY(offsetY_);

    // Add the column to the list of columns and update the layout width.
    columns_.append(column);
    layoutWidth_ += column->boundingRect().width();

    // Update the position in the layout.
    layout_->updateItemMove(columns_, column);
    layout_->setItemPos(columns_, column, 0.0);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::removeColumn()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::removeColumn(GraphicsColumn* column)
{
    if (columns_.removeAll(column) > 0)
    {
        layoutWidth_ -= column->boundingRect().width();
        layout_->updateItemPositions(columns_, 0.0, 0.0);
        scene_->removeItem(column);
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::findColumnAt()
//-----------------------------------------------------------------------------
GraphicsColumn* GraphicsColumnLayout::findColumnAt(QPointF pt)
{
    foreach (GraphicsColumn* column, columns_)
    {
        QRectF columnRect = column->sceneBoundingRect();

        // The point is inside the column if the x coordinate is within it (y can be anything).
        if (columnRect.left() <= pt.x() && pt.x() < columnRect.right())
        {
            return column;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::onMoveColumn()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::onMoveColumn(GraphicsColumn* column)
{
    // Clamp the coordinates.
    QPointF pos = column->pos();
    pos.setY(0.0);
    pos.setX(qMax(qMin(pos.x(), layoutWidth_ - (column->boundingRect().width())), 0.0));
    column->setPos(pos);
    column->update();

    layout_->updateItemMove(columns_, column);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::onReleaseColumn()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::onReleaseColumn(GraphicsColumn* column)
{
    layout_->setItemPos(columns_, column, 0.0);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::updateColumnPositions()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::updateColumnPositions()
{
    layout_->updateItemPositions(columns_, 0.0, 0.0);

    layoutWidth_ = 0.0;

    foreach (GraphicsColumn* column, columns_)
    {
        layoutWidth_ += column->boundingRect().width();
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::setOffsetY()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::setOffsetY(qreal y)
{
    for (int i = 0; i < columns_.size(); ++i)
    {
        columns_.at(i)->setOffsetY(y);
    }

    offsetY_ = y;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::getColumns()
//-----------------------------------------------------------------------------
QList<GraphicsColumn*> const& GraphicsColumnLayout::getColumns() const
{
    return columns_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::updatePositions()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::updatePositions()
{
    for (int i = 0; i < columns_.size(); ++i)
    {
        columns_[i]->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::setAutoReorganize()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::setAutoReorganized(bool autoReorganized)
{
    autoReorganized_ = autoReorganized;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::isAutoReorganized()
//-----------------------------------------------------------------------------
bool GraphicsColumnLayout::isAutoReorganized() const
{
    return autoReorganized_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::autoCreateColumn()
//-----------------------------------------------------------------------------
int GraphicsColumnLayout::autoCreateColumn()
{
    Q_ASSERT(autoCreateColumnFunc_ != 0);

    GraphicsColumn* column = autoCreateColumnFunc_(this);
    addColumn(column, true);

    return columns_.size() - 1;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::setAutoCreateColumnFunction()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::setAutoCreateColumnFunction(AutoCreateColumnFunc func)
{
    autoCreateColumnFunc_ = func;
}
