//-----------------------------------------------------------------------------
// File: ApiInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 3.4.2012
//
// Description:
// API interface class for defining properties of a specific API interface
// in a SW component.
//-----------------------------------------------------------------------------

#include "ApiInterface.h"

//-----------------------------------------------------------------------------
// Function: str2DependencyDirection()
//-----------------------------------------------------------------------------
DependencyDirection str2DependencyDirection(QString const& str, DependencyDirection defaultValue)
{
    if (str == QLatin1String("provider"))
    {
        return DEPENDENCY_PROVIDER;
    }
    else if (str == QLatin1String("requester"))
    {
        return DEPENDENCY_REQUESTER;
    }
    else
    {
        return defaultValue;
    }
}

//-----------------------------------------------------------------------------
// Function: dependencyDirection2Str()
//-----------------------------------------------------------------------------
QString dependencyDirection2Str(DependencyDirection dir)
{
    if (dir == DEPENDENCY_PROVIDER)
    {
        return QStringLiteral("provider");
    }
    else
    {
        return QStringLiteral("requester");
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface() :
NameGroup(),
apiType_(),
dependencyDir_(DEPENDENCY_PROVIDER),
defaultPos_()
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface(ApiInterface const& rhs) :
NameGroup(rhs),
apiType_(rhs.apiType_),
dependencyDir_(rhs.dependencyDir_),
defaultPos_(rhs.defaultPos_)
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterface::ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::ApiInterface(QDomNode& node) :
NameGroup(),
apiType_(),
dependencyDir_(DEPENDENCY_PROVIDER),
defaultPos_()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == QLatin1String("ipxact:name"))
        {
            setName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("ipxact:displayName"))
        {
            setDisplayName(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("ipxact:description"))
        {
            setDescription(childNode.firstChild().nodeValue());
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:apiType"))
        {
            apiType_ = VLNV::createVLNV(childNode, VLNV::APIDEFINITION);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:dependencyDirection"))
        {
            dependencyDir_ = str2DependencyDirection(childNode.childNodes().at(0).nodeValue(), DEPENDENCY_PROVIDER);
        }
        else if (childNode.nodeName() == QLatin1String("kactus2:position"))
        {
            defaultPos_.setX(childNode.attributes().namedItem(QStringLiteral("x")).nodeValue().toInt());
            defaultPos_.setY(childNode.attributes().namedItem(QStringLiteral("y")).nodeValue().toInt());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::~ApiInterface()
//-----------------------------------------------------------------------------
ApiInterface::~ApiInterface()
{

}

//-----------------------------------------------------------------------------
// Function: ApiInterface::clone()
//-----------------------------------------------------------------------------
ApiInterface* ApiInterface::clone() const
{
    return new ApiInterface(*this);
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::type()
//-----------------------------------------------------------------------------
QString ApiInterface::type() const
{
    return QStringLiteral("kactus2:apiInterface");
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::write()
//-----------------------------------------------------------------------------
void ApiInterface::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(QStringLiteral("kactus2:apiInterface"));

    writer.writeTextElement(QStringLiteral("ipxact:name"), name());

    if (!displayName().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:displayName"), displayName());
    }
    if (!description().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), description());
    }

    writer.writeEmptyElement(QStringLiteral("kactus2:apiType"));
    writer.writeAttribute(QStringLiteral("vendor"), apiType_.getVendor());
    writer.writeAttribute(QStringLiteral("library"), apiType_.getLibrary());
    writer.writeAttribute(QStringLiteral("name"), apiType_.getName());
    writer.writeAttribute(QStringLiteral("version"), apiType_.getVersion());

    writer.writeTextElement(QStringLiteral("kactus2:dependencyDirection"), dependencyDirection2Str(dependencyDir_));

    if (!defaultPos_.isNull())
    {
        writePosition(writer, defaultPos_);
    }

    writer.writeEndElement(); // kactus2:apiInterface
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::isValid()
//-----------------------------------------------------------------------------
bool ApiInterface::isValid(QStringList& errorList, QString const& parentId) const
{
    QString const thisId = QObject::tr("API interface '%1'").arg(name());
    bool valid = true;

    if (name().isEmpty())
    {
        errorList.append(QObject::tr("No name specified for an API interface in %1").arg(parentId));
        valid = false;
    }

    if (!apiType_.isEmpty() && !apiType_.isValid())
    {
        errorList.append(QObject::tr("Invalid API type set in %1").arg(thisId));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::isValid()
//-----------------------------------------------------------------------------
bool ApiInterface::isValid() const
{
    return (!name().isEmpty() && (apiType_.isEmpty() || apiType_.isValid()));
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setApiType()
//-----------------------------------------------------------------------------
void ApiInterface::setApiType(VLNV const& vlnv)
{
    apiType_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDependencyDirection()
//-----------------------------------------------------------------------------
void ApiInterface::setDependencyDirection(DependencyDirection dir)
{
    dependencyDir_ = dir;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getApiType()
//-----------------------------------------------------------------------------
VLNV const& ApiInterface::getApiType() const
{
    return apiType_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDependencyDirection()
//-----------------------------------------------------------------------------
DependencyDirection ApiInterface::getDependencyDirection() const
{
    return dependencyDir_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::operator=()
//-----------------------------------------------------------------------------
ApiInterface& ApiInterface::operator=(ApiInterface const& rhs)
{
    if (&rhs != this)
    {
        NameGroup::operator=(rhs);
        apiType_ = rhs.apiType_;
        dependencyDir_ = rhs.dependencyDir_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::setDefaultPos()
//-----------------------------------------------------------------------------
void ApiInterface::setDefaultPos(QPointF const& pos)
{
    defaultPos_ = pos;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::getDefaultPos()
//-----------------------------------------------------------------------------
QPointF const& ApiInterface::getDefaultPos() const
{
    return defaultPos_;
}

//-----------------------------------------------------------------------------
// Function: ApiInterface::writePosition()
//-----------------------------------------------------------------------------
void ApiInterface::writePosition(QXmlStreamWriter& writer, QPointF const& pos) const
{
    writer.writeEmptyElement(QStringLiteral("kactus2:position"));
    writer.writeAttribute(QStringLiteral("x"), QString::number(int(pos.x())));
    writer.writeAttribute(QStringLiteral("y"), QString::number(int(pos.y())));
}