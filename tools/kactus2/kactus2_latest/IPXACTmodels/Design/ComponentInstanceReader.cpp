//-----------------------------------------------------------------------------
// File: ComponentInstanceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.09.2015
//
// Description:
// XML reader class for IP-XACT component instance.
//-----------------------------------------------------------------------------

#include "ComponentInstanceReader.h"

#include <IPXACTmodels/Design/ComponentInstance.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::ComponentInstanceReader()
//-----------------------------------------------------------------------------
ComponentInstanceReader::ComponentInstanceReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::ComponentInstanceReader()
//-----------------------------------------------------------------------------
ComponentInstanceReader::~ComponentInstanceReader()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::createComponentInstanceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> ComponentInstanceReader::createComponentInstanceFrom(
    QDomNode const& instanceNode) const
{
    QString instanceName = instanceNode.firstChildElement(QStringLiteral("ipxact:instanceName")).firstChild().nodeValue();
    QString displayName = instanceNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue();
    QString description = instanceNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue();
    QString isPresent = instanceNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue();

    QDomNode componentRefNode = instanceNode.firstChildElement(QStringLiteral("ipxact:componentRef"));
    QSharedPointer<ConfigurableVLNVReference> componentRef =
        parseConfigurableVLNVReference(componentRefNode, VLNV::COMPONENT);

    QSharedPointer<ComponentInstance> newComponentInstance (new ComponentInstance(instanceName, componentRef));
    newComponentInstance->setIsPresent(isPresent);
    newComponentInstance->setDisplayName(displayName);
    newComponentInstance->setDescription(description);

    parseExtensions(instanceNode, newComponentInstance);

    return newComponentInstance;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseExtensions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseExtensions(const QDomNode& componentInstanceNode,
    QSharedPointer<ComponentInstance> instance) const
{
    QDomElement extensionsNode = componentInstanceNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));
    QDomNodeList extensionNodeList = extensionsNode.childNodes();

    QDomElement uuidElement = extensionsNode.firstChildElement(QStringLiteral("kactus2:uuid"));
    if (!uuidElement.isNull())
    {
        instance->setUuid(uuidElement.firstChild().nodeValue());
    }

    QDomElement draftNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:draft"));
    parseDraft(draftNode, instance);

    QDomElement positionNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:position"));
    parsePosition(positionNode, instance);
    
    QDomElement importNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:imported"));
    parseImport(importNode, instance);
    
    QDomElement portPositionNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:portPositions"));
    parsePortPositions(portPositionNode, instance);

    QDomElement adHocVisibilityNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:adHocVisibilities"));
    parseAdHocVisibilities(adHocVisibilityNode, instance);

    QDomElement apiPositionsNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:apiInterfacePositions"));
    parseApiInterfacePositions(apiPositionsNode, instance);
 
    QDomElement comPositionsNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:comInterfacePositions"));
    parseComInterfacePositions(comPositionsNode, instance);

    QDomElement propertyNode = extensionsNode.firstChildElement(QStringLiteral("kactus2:propertyValues"));
    parsePropertyValues(propertyNode, instance);

    parseVendorExtensions(componentInstanceNode, instance);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseDraft()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseDraft(QDomElement const& draftNode,
    QSharedPointer<ComponentInstance> instance) const
{
    if (!draftNode.isNull())
    {
        instance->setDraft(true);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePosition()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePosition(QDomElement const& positionElement,
    QSharedPointer<ComponentInstance> instance) const
{
    if (!positionElement.isNull())
    {
        int positionX = positionElement.attribute(QStringLiteral("x")).toInt();
        int positionY = positionElement.attribute(QStringLiteral("y")).toInt();
        instance->setPosition(QPointF(positionX, positionY));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseImport()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseImport(QDomElement const& importElement,
    QSharedPointer<ComponentInstance> instance) const
{
    if (!importElement.isNull())
    {
        QString importReference = importElement.attribute(QStringLiteral("importRef"));

        if (!importReference.isEmpty())
        {
            instance->setImportRef(importReference);
        }
        else
        {
            instance->setImported(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePortPositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePortPositions(QDomElement const& portPositionsElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> portPositions = createMappedPositions(portPositionsElement, 
        QStringLiteral("kactus2:portPosition"), QStringLiteral("busRef"));
    QMapIterator<QString, QPointF> portIterator(portPositions);
    while (portIterator.hasNext())
    {
        portIterator.next();
        instance->updateBusInterfacePosition(portIterator.key(), portIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseAdHocVisibilities()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseAdHocVisibilities(QDomElement const& adHocElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> adHocPositions = createMappedPositions(adHocElement,
        QStringLiteral("kactus2:adHocVisible"), QStringLiteral("portName"));
    QMapIterator<QString, QPointF> adHocIterator(adHocPositions);
    while (adHocIterator.hasNext())
    {
        adHocIterator.next();
        instance->updateAdHocPortPosition(adHocIterator.key(), adHocIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseApiInterfacePositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseApiInterfacePositions(QDomElement const& apiElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> apiInterfacePositions = createMappedPositions(apiElement,
        QStringLiteral("kactus2:apiInterfacePosition"), QStringLiteral("apiRef"));
    QMapIterator<QString, QPointF> apiInterfaceIterator(apiInterfacePositions);
    while (apiInterfaceIterator.hasNext())
    {
        apiInterfaceIterator.next();
        instance->updateApiInterfacePosition(apiInterfaceIterator.key(), apiInterfaceIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parseComInterfacePositions()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parseComInterfacePositions(QDomElement const& comElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QMap<QString, QPointF> comInterfacePositions = createMappedPositions(comElement,
        QStringLiteral("kactus2:comInterfacePosition"), QStringLiteral("comRef"));
    QMapIterator<QString, QPointF> comInterfaceIterator(comInterfacePositions);
    while (comInterfaceIterator.hasNext())
    {
        comInterfaceIterator.next();
        instance->updateComInterfacePosition(comInterfaceIterator.key(), comInterfaceIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::parsePropertyValues()
//-----------------------------------------------------------------------------
void ComponentInstanceReader::parsePropertyValues(QDomElement const& propertyElement,
    QSharedPointer<ComponentInstance> instance) const
{
    QDomNodeList propertyValueNodes = propertyElement.elementsByTagName(QStringLiteral("kactus2:propertyValue"));
    QMap<QString, QString> newPropertyValues;

    for (int propertyIndex = 0; propertyIndex < propertyValueNodes.count(); ++propertyIndex)
    {
        QDomNode propertyNode = propertyValueNodes.at(propertyIndex);
        QDomNamedNodeMap propertyAttributes = propertyNode.attributes();

        QString propertyName = propertyAttributes.namedItem(QStringLiteral("name")).firstChild().nodeValue();
        QString propertyValue = propertyAttributes.namedItem(QStringLiteral("value")).firstChild().nodeValue();
        newPropertyValues.insert(propertyName, propertyValue);
    }

    instance->setPropertyValues(newPropertyValues);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceReader::createMappedPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> ComponentInstanceReader::createMappedPositions(QDomElement const& positionElement,
    QString const& itemIdentifier, QString const& referenceIdentifier) const
{
    QMap<QString, QPointF> positionMap;

    QDomNodeList positionNodeList = positionElement.elementsByTagName(itemIdentifier);
    for (int positionIndex = 0; positionIndex < positionNodeList.count(); ++positionIndex)
    {
        QDomNode singlePositionNode = positionNodeList.at(positionIndex);
        QDomElement positionElement = singlePositionNode.toElement();

        QString interfaceReference = positionElement.attribute(referenceIdentifier);

        if (!positionElement.hasAttribute(QStringLiteral("x")))
        {
            positionElement = positionElement.firstChildElement(QStringLiteral("kactus2:position"));
        }

        int positionX = positionElement.attribute(QStringLiteral("x")).toInt();
        int positionY = positionElement.attribute(QStringLiteral("y")).toInt();

        positionMap.insert(interfaceReference, QPointF(positionX, positionY));
    }

    return positionMap;
}
