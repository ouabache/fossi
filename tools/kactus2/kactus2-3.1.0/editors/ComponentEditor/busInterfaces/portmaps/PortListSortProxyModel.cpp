//-----------------------------------------------------------------------------
// File: PortListSortProxyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.10.2013
//
// Description:
// Sorting proxy model for port lists.
//-----------------------------------------------------------------------------

#include "PortListSortProxyModel.h"

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Port.h>

#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel()
//-----------------------------------------------------------------------------
PortListSortProxyModel::PortListSortProxyModel(QSharedPointer<Component> component, QObject *parent) :
QSortFilterProxyModel(parent),
    component_(component),
    filterDirection_(ANY),
    hideConnected_(true),
    connectedPorts_(),
    filterPorts_()
{
    onConnectionsReset();
}

//-----------------------------------------------------------------------------
// Function: ~PortListSortProxyModel()
//-----------------------------------------------------------------------------
PortListSortProxyModel::~PortListSortProxyModel()
{
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterDirection()
//-----------------------------------------------------------------------------
PortListSortProxyModel::DirectionFilter PortListSortProxyModel::filterDirection() const
{
    return filterDirection_;
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterPortName()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterPortName(QString const& portName)
{
    setFilterRegExp(QRegExp(portName, Qt::CaseInsensitive));
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterPorts()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterPorts(QStringList const& ports)
{
    filterPorts_ = ports;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterInDirection()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterInDirection(bool filterInDirection)
{
    if (filterDirection_ == IN && !filterInDirection)
    {
        filterDirection_ = ANY;
    }
    else if (filterDirection_ == OUT && filterInDirection)
    {
        filterDirection_ = INOUT;
    }
    else if (filterDirection_ == INOUT && !filterInDirection)
    {
        filterDirection_ = OUT;
    }
    else if (filterDirection_ == ANY && filterInDirection)
    {
        filterDirection_ = IN;
    }
    
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterOutDirection()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterOutDirection(bool filterOutDirection)
{
    if (filterDirection_ == IN && filterOutDirection)
    {
        filterDirection_ = INOUT; 
    }
    else if (filterDirection_ == OUT && !filterOutDirection)
    {
        filterDirection_ = ANY;
    }
    else if (filterDirection_ == INOUT && !filterOutDirection)
    {
        filterDirection_ = IN;
    }
    else if (filterDirection_ == ANY && filterOutDirection)
    {
        filterDirection_ = OUT;
    }

    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterHideConnected()
//-----------------------------------------------------------------------------
bool PortListSortProxyModel::filterHideConnected() const
{
    return hideConnected_;
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterPortNames()
//-----------------------------------------------------------------------------
QStringList PortListSortProxyModel::filterPortNames() const
{
    return filterPorts_;
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::setFilterHideConnected()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::setFilterHideConnected(bool hide)
{
    hideConnected_ = hide;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::onConnectionsChanged()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::onConnectionsReset()
{
    connectedPorts_.clear();
    foreach (QSharedPointer<BusInterface> busIf, *component_->getBusInterfaces())
    {
        if (!busIf->getAbstractionTypes()->isEmpty())
        {
            foreach (QSharedPointer<PortMap> portMap, *busIf->getPortMaps())
            {
                if (portMap->getPhysicalPort())
                {
                    if (!connectedPorts_.contains(portMap->getPhysicalPort()->name_))
                    {
                        connectedPorts_.append(portMap->getPhysicalPort()->name_);
                    }
                }
            }
        }        
    }
    invalidateFilter(); 
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool PortListSortProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{   
    QModelIndex index = sourceModel()->index(source_row, 0);
    QString portName = sourceModel()->data(index).toString();

    // Check filter for direction.
    QSharedPointer<Port> currentPort = component_->getPort(portName);
    if (filterDirection_ != ANY && currentPort->getDirection() != filterDirection_)
    {
        return false;
    }

    // Check filter for connected ports.
    if (hideConnected_ && connectedPorts_.contains(portName))
    {
        return false;
    }

    // Check filter for allowed port names.
    if (!filterPorts_.isEmpty() && !filterPorts_.contains(portName))
    {
        return false;
    }

    // Check filter for port name.
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::onPortConnected()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::onPortConnected(QString const& portName)
{
    if (!connectedPorts_.contains(portName))
    {
        connectedPorts_.append(portName);
        invalidateFilter();
    }
}

//-----------------------------------------------------------------------------
// Function: PortListSortProxyModel::onPortDisconnected()
//-----------------------------------------------------------------------------
void PortListSortProxyModel::onPortDisconnected(QString const& portName)
{
    if (connectedPorts_.contains(portName))
    {
        connectedPorts_.removeAll(portName);
        invalidateFilter();
    }
}
