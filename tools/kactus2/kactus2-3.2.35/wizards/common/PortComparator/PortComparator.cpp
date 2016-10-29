//-----------------------------------------------------------------------------
// File: PortComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.10.2014
//
// Description:
// Comparator for finding differences in ports.
//-----------------------------------------------------------------------------

#include "PortComparator.h"

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: PortComparator::PortComparator()
//-----------------------------------------------------------------------------
PortComparator::PortComparator()
{

}

//-----------------------------------------------------------------------------
// Function: PortComparator::~PortComparator()
//-----------------------------------------------------------------------------
PortComparator::~PortComparator()
{

}

//-----------------------------------------------------------------------------
// Function: PortComparator::compare()
//-----------------------------------------------------------------------------
bool PortComparator::compare(QSharedPointer<const Port> first, QSharedPointer<const Port> second) const
{
    return IPXactElementComparator::compare(first, second);
}

//-----------------------------------------------------------------------------
// Function: PortComparator::compareFields()
//-----------------------------------------------------------------------------
bool PortComparator::compareFields(QSharedPointer<const Port> first, QSharedPointer<const Port> second) const
{
    return first->name() == second->name() &&       
        first->getTypeName() == second->getTypeName() &&
        first->getTypeDefinitions() == second->getTypeDefinitions() &&
        first->getDefaultValue() == second->getDefaultValue() &&
        first->getDirection() == second->getDirection() &&
        first->getLeftBound() == second->getLeftBound() &&
        first->getRightBound() == second->getRightBound();
}

//-----------------------------------------------------------------------------
// Function: PortComparator::compare()
//-----------------------------------------------------------------------------
bool PortComparator::compare(QList<QSharedPointer<Port> > const first, 
    QList<QSharedPointer<Port> > const second) const
{
    return ListComparator::compare(first, second);
}

//-----------------------------------------------------------------------------
// Function: PortComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > PortComparator::diff(QSharedPointer<const Port> reference,
    QSharedPointer<const Port> subject) const
{
    return IPXactElementComparator::diff(reference, subject);
}

//-----------------------------------------------------------------------------
// Function: PortComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > PortComparator::diffFields(QSharedPointer<const Port> reference, 
    QSharedPointer<const Port> subject) const
{
    QSharedPointer<IPXactDiff> portDiff(new IPXactDiff(elementType(), reference->name()));
    portDiff->setChangeType(IPXactDiff::MODIFICATION);

    portDiff->checkForChange("type name", reference->getTypeName(), subject->getTypeName());
    portDiff->checkForChange("type definitions", reference->getTypeDefinitions().join(", "), 
        subject->getTypeDefinitions().join(", "));

    portDiff->checkForChange("default value", reference->getDefaultValue(), subject->getDefaultValue());

    portDiff->checkForChange("direction", DirectionTypes::direction2Str(reference->getDirection()), 
        DirectionTypes::direction2Str(subject->getDirection()));

    portDiff->checkForChange("left bound", reference->getLeftBound(), subject->getLeftBound());
    portDiff->checkForChange("right bound", reference->getRightBound(), subject->getRightBound());

    QList<QSharedPointer<IPXactDiff> > list;
    list.append(portDiff);
    return list;
}

//-----------------------------------------------------------------------------
// Function: PortComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > PortComparator::diff(QList<QSharedPointer<Port> > const references, 
    QList<QSharedPointer<Port> > const subjects) const
{
    return ListComparator::diff(references, subjects);
}

//-----------------------------------------------------------------------------
// Function: PortComparator::elementType()
//-----------------------------------------------------------------------------
QString PortComparator::elementType() const
{
    return QObject::tr("port");
}
