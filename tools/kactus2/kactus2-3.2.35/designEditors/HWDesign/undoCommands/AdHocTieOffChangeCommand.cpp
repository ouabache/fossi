//-----------------------------------------------------------------------------
// File: AdHocTieOffChangeCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.05.2016
//
// Description:
// Undo command for changing tie off in ad hoc editor.
//-----------------------------------------------------------------------------

#include "AdHocTieOffChangeCommand.h"

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/AdHocItem.h>
#include <designEditors/HWDesign/AdHocPortItem.h>
#include <designEditors/HWDesign/AdHocInterfaceItem.h>
#include <designEditors/HWDesign/HWDesignDiagram.h>

#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::AdHocTieOffChangeCommand()
//-----------------------------------------------------------------------------
AdHocTieOffChangeCommand::AdHocTieOffChangeCommand(AdHocItem* portItem, QSharedPointer<AdHocConnection> connection,
    QString const& newTieOffValue, QString newParsedTieOff, QString newFormattedTieOff, int newBase,
    QString const& oldTieOffValue, QString oldParsedTieOff, QString oldFormattedTieOff, int oldBase,
    HWDesignDiagram* designDiagram, QUndoCommand* parent):
QUndoCommand(parent),
tieOffConnection_(connection),
containingDiagram_(designDiagram),
containingDesign_(containingDiagram_->getDesign()),
oldTieOff_(oldTieOffValue),
parsedOldTieOff_(oldParsedTieOff),
formattedOldTieOff_(oldFormattedTieOff),
oldBase_(oldBase),
newTieOff_(newTieOffValue),
parsedNewTieOff_(newParsedTieOff),
formattedNewTieOff_(newFormattedTieOff),
newBase_(newBase),
valueFormatter_()
{
    if (!tieOffConnection_ && !newTieOff_.isEmpty())
    {
        tieOffConnection_ = createConnectionForTiedValue(portItem);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::~AdHocTieOffChangeCommand()
//-----------------------------------------------------------------------------
AdHocTieOffChangeCommand::~AdHocTieOffChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::createConnectionForTiedValue()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdHocTieOffChangeCommand::createConnectionForTiedValue(AdHocItem* portItem) const
{
    QString connectionName = createNameForTiedValueConnection(portItem);

    QSharedPointer<AdHocConnection> connection (new AdHocConnection(connectionName));

    QSharedPointer<PortReference> portReference (new PortReference(portItem->name()));

    ComponentItem* containingComponent = portItem->encompassingComp();
    if (containingComponent)
    {
        portReference->setComponentRef(containingComponent->name());
        connection->getInternalPortReferences()->append(portReference);
    }
    else
    {
        connection->getExternalPortReferences()->append(portReference);
    }

    return connection;
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::createNameForTiedValueConnection()
//-----------------------------------------------------------------------------
QString AdHocTieOffChangeCommand::createNameForTiedValueConnection(AdHocItem* portItem) const
{
    ComponentItem* containingComponent = portItem->encompassingComp();

    QString instanceName = "";

    if (containingComponent)
    {
        instanceName = containingComponent->name();
        instanceName.append("_");
    }

    QString portName = portItem->name();

    QString tiedValuePart = "_to_tiedValue";

    return instanceName + portName + tiedValuePart;
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::undo()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::undo()
{
    QUndoCommand::undo();

    if (tieOffConnection_)
    {
        tieOffConnection_->setTiedValue(oldTieOff_);

        changeTieOffSymbolsInConnectedPorts(oldTieOff_, parsedOldTieOff_, formattedOldTieOff_, oldBase_);

        addOrRemoveConnection(oldTieOff_);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::redo()
{
    if (tieOffConnection_)
    {
        tieOffConnection_->setTiedValue(newTieOff_);

        changeTieOffSymbolsInConnectedPorts(newTieOff_, parsedNewTieOff_, formattedNewTieOff_, newBase_);

        addOrRemoveConnection(newTieOff_);
    }

    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::changeTieOffSymbolsInConnectedPorts()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::changeTieOffSymbolsInConnectedPorts(QString const& tieOffValue,
    QString const& parsedTieOff, QString const& formattedTieOff, int tieOffBase) const
{
    if (!tieOffConnection_->getInternalPortReferences()->isEmpty())
    {
        foreach (QSharedPointer<PortReference> internalReference, *tieOffConnection_->getInternalPortReferences())
        {
            HWComponentItem* componentItem =
                containingDiagram_->getComponentItem(internalReference->getComponentRef());

            if (componentItem)
            {
                AdHocPortItem* portItem = componentItem->getAdHocPort(internalReference->getPortRef());

                if (portItem)
                {
                    drawTieOffSymbol(portItem, tieOffValue, parsedTieOff, formattedTieOff, tieOffBase);
                }
            }
        }
    }

    if (!tieOffConnection_->getExternalPortReferences()->isEmpty())
    {
        foreach (QSharedPointer<PortReference> externalReference, *tieOffConnection_->getExternalPortReferences())
        {
            HWConnectionEndpoint* endPoint =
                containingDiagram_->getDiagramAdHocPort(externalReference->getPortRef());
            if (endPoint)
            {
                AdHocItem* portItem = dynamic_cast<AdHocItem*>(endPoint);
                if (portItem)
                {
                    drawTieOffSymbol(portItem, tieOffValue, parsedTieOff, formattedTieOff, tieOffBase);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::drawTieOffSymbol()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::drawTieOffSymbol(AdHocItem* portItem, QString const& tieOffValue,
    QString parsedTieOff, QString formattedTieOff, int tieOffBase) const
{
    bool canConvertTieOffToInt = true;
    parsedTieOff.toInt(&canConvertTieOffToInt);

    if (tieOffValue.isEmpty())
    {
        portItem->removeTieOffItem();
    }
    else
    {
        QString tieOffWithBase = valueFormatter_.format(parsedTieOff, tieOffBase);

        portItem->changeTieOffLabel(formattedTieOff, tieOffWithBase, canConvertTieOffToInt);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::addOrRemoveConnection()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::addOrRemoveConnection(QString const& tieOffValue)
{
    if (tieOffValue.isEmpty() && containingDesign_->getAdHocConnections()->contains(tieOffConnection_))
    {
        containingDesign_->getAdHocConnections()->removeAll(tieOffConnection_);
    }
    else if (!containingDesign_->getAdHocConnections()->contains(tieOffConnection_))
    {
        containingDesign_->getAdHocConnections()->append(tieOffConnection_);
    }
}
