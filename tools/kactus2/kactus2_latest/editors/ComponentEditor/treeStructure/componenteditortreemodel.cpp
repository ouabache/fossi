//-----------------------------------------------------------------------------
// File: componenteditortreemodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 07.05.2012
//
// Description:
// The tree model that manages the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditortreemodel.h"

#include <IPXACTmodels/Component/Component.h>

#include <QColor>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::ComponentEditorTreeModel()
//-----------------------------------------------------------------------------
ComponentEditorTreeModel::ComponentEditorTreeModel(QObject* parent) :
QAbstractItemModel(parent),
rootItem_()
{
	setObjectName(tr("ComponentEditorTreeModel"));
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::~ComponentEditorTreeModel()
//-----------------------------------------------------------------------------
ComponentEditorTreeModel::~ComponentEditorTreeModel()
{
	rootItem_.clear();
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::setRootItem()
//-----------------------------------------------------------------------------
void ComponentEditorTreeModel::setRootItem(QSharedPointer<ComponentEditorRootItem> root)
{
	beginResetModel();
    rootItem_ = root;
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::rowCount()
//-----------------------------------------------------------------------------
int ComponentEditorTreeModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
	// only one column exists
	if (parent.column() > 0)
    {
		return 0;
	}

	ComponentEditorItem* parentItem = 0;

	// if the given item is invalid, it is interpreted as root item
	if (!parent.isValid())
    {
		parentItem = rootItem_.data();
	}
	else
    {
		parentItem = static_cast<ComponentEditorItem*>(parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->rowCount();
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::hasChildren()
//-----------------------------------------------------------------------------
bool ComponentEditorTreeModel::hasChildren( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
	ComponentEditorItem* parentItem = 0;

	// if the given item is invalid, it is interpreted as root item
	if (!parent.isValid())
    {
		parentItem = rootItem_.data();
	}
	else
    {
		parentItem = static_cast<ComponentEditorItem*>(parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->hasChildren();
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::columnCount()
//-----------------------------------------------------------------------------
int ComponentEditorTreeModel::columnCount( const QModelIndex&) const
{
	return 1;
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::data()
//-----------------------------------------------------------------------------
QVariant ComponentEditorTreeModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const
{
	if (!index.isValid())
    {
		return QVariant();
	}

	ComponentEditorItem* item = static_cast<ComponentEditorItem*>(index.internalPointer());
	Q_ASSERT(item);

	if (role == Qt::DisplayRole)
    {
		QString text = item->text();

		// if the object has no name
		if (text.isEmpty())
        {
			text = tr("unnamed");
		}

		return text;
	}
	else if (role == Qt::FontRole)
    {
		return item->getFont();
	}
	else if (role == Qt::ForegroundRole)
    {
        if (!item->isValid())
        {
            return QColor("red");
        }         
        else if (item->highlight())
        {
            return QColor("blue");
        }  
        else
        {
            return QColor("black");
        }
	}
	else if (role == Qt::ToolTipRole)
    {
		return item->getTooltip();
	}
    else if( role == Qt::DecorationRole && item->hasIcon() )
    {
        return item->getIcon();
    }
	// not supported role
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ComponentEditorTreeModel::flags( const QModelIndex& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::onContentChanged()
//-----------------------------------------------------------------------------
void ComponentEditorTreeModel::onContentChanged(ComponentEditorItem* item )
{
	QModelIndex index = ComponentEditorTreeModel::index(item);
	emit dataChanged(index, index);
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::index()
//-----------------------------------------------------------------------------
QModelIndex ComponentEditorTreeModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/ )
    const
{
	if (!hasIndex(row, column, parent))
    {
		return QModelIndex();
	}

	ComponentEditorItem* parentItem = 0;

	// if parent is invalid the asked index is for the root item
	if (!parent.isValid())
    {
		parentItem = rootItem_.data();
	}
	else
    {
		parentItem = static_cast<ComponentEditorItem*>(parent.internalPointer());
	}

	Q_ASSERT(parentItem);

	// get pointer to specified child of the parent
	QSharedPointer<ComponentEditorItem> child = parentItem->child(row);

	// if the item was found
	if (child)
    {
		return createIndex(row, column, child.data());
	}
	// if child was not found
	else
    {
		return QModelIndex();
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::index()
//-----------------------------------------------------------------------------
QModelIndex ComponentEditorTreeModel::index(ComponentEditorItem* item ) const
{
	Q_ASSERT(item);

	// if the item is the root item then it's model index is invalid
	if (item == rootItem_.data())
    {
		return QModelIndex();
	}
	else
    {
		return createIndex(item->row(), 0, item);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::parent()
//-----------------------------------------------------------------------------
QModelIndex ComponentEditorTreeModel::parent( const QModelIndex& index ) const
{
	// if the child parameter is invalid then there is no parent
	if (!index.isValid())
    {
		return QModelIndex();
	}

	// pointer to the child item
	ComponentEditorItem* childItem = static_cast<ComponentEditorItem*>(index.internalPointer());
	ComponentEditorItem* parent = childItem->parent();

	// if the parent does not exist then this item is root item
	if (!parent)
    {
		return QModelIndex();
	}

	// if row is invalid then the grandparent does not exist and parent is
	// a root item so we return an invalid QModelIndex
	int row = parent->row();
	if (row == -1)
    {
		return QModelIndex();
	}

	// create new index and return it
	return createIndex(row, 0, parent);
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::setLocked()
//-----------------------------------------------------------------------------
void ComponentEditorTreeModel::setLocked( bool locked )
{
	rootItem_->setLocked(locked);
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::addItem()
//-----------------------------------------------------------------------------
void ComponentEditorTreeModel::addItem( ComponentEditorItem* parentItem, int childIndex )
{
	Q_ASSERT(parentItem);
	Q_ASSERT(childIndex >= 0);

	// find the index of the parent item
	QModelIndex parentIndex = index(parentItem);

	beginInsertRows(parentIndex, childIndex, childIndex);
	parentItem->createChild(childIndex);
	endInsertRows();

	emit expandItem(parentIndex);
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::removeItem()
//-----------------------------------------------------------------------------
void ComponentEditorTreeModel::removeItem( ComponentEditorItem* parentItem, int childIndex )
{
	Q_ASSERT(parentItem);
	Q_ASSERT(childIndex >= 0);

	// find the index of the parent item
	QModelIndex parentIndex = index(parentItem);

	beginRemoveRows(parentIndex, childIndex, childIndex);
	parentItem->removeChild(childIndex);
	endRemoveRows();
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::moveItem()
//-----------------------------------------------------------------------------
void ComponentEditorTreeModel::moveItem( ComponentEditorItem* parentItem, int childSource, int childtarget )
{
	Q_ASSERT(parentItem);
	Q_ASSERT(childSource >= 0);
	Q_ASSERT(childtarget >= 0);

	// if the location of the item does not change
	if (childSource == childtarget)
    {
		return;
	}

	// the index identifying the parent object
	QModelIndex parentIndex = index(parentItem);

	// target index is told to the parent item when moving
	int targetIndex = childtarget;

	// if item is moved down then the target must be incremented for abstract item's
	// way to inform view of row moves
	if (childtarget > childSource)
    {
		++childtarget;
	}

	// make the moving of the item
	beginMoveRows(parentIndex, childSource, childSource, parentIndex, childtarget);
	parentItem->moveChild(childSource, targetIndex);
	endMoveRows();
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::onSelectBusInterface()
//-----------------------------------------------------------------------------
void ComponentEditorTreeModel::onSelectBusInterface( const QString& interfaceName )
{
	const QSharedPointer<ComponentEditorItem> busIfItem = rootItem_->getBusInterfaceItem(interfaceName);
	if (!busIfItem)
    {
		return;
	}

	// inform component editor to select the item
	QModelIndex busIfIndex = index(busIfItem.data());
	emit selectItem(busIfIndex);
}

//-----------------------------------------------------------------------------
// Function: componenteditortreemodel::onSelectItem()
//-----------------------------------------------------------------------------
void ComponentEditorTreeModel::onSelectItem( ComponentEditorItem* item )
{
	QModelIndex itemIndex = index(item);
	emit selectItem(itemIndex);
}
