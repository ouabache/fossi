//-----------------------------------------------------------------------------
// File: FileDependency.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 21.01.2013
//
// Description:
// File dependency class.
//-----------------------------------------------------------------------------

#include "FileDependency.h"

#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: FileDependency::FileDependency()
//-----------------------------------------------------------------------------
FileDependency::FileDependency() :
QObject(),
file1_(),
file2_(),
desc_(),
locked_(false),
bidirectional_(false),
manual_(false),
status_(STATUS_UNCHANGED)
{

}

//-----------------------------------------------------------------------------
// Function: FileDependency::FileDependency()
//-----------------------------------------------------------------------------
FileDependency::FileDependency(FileDependency const& rhs) :
QObject(),
file1_(rhs.file1_),
file2_(rhs.file2_),
desc_(rhs.desc_),
locked_(rhs.locked_),
bidirectional_(rhs.bidirectional_),
manual_(rhs.manual_),
status_(rhs.status_)
{

}

//-----------------------------------------------------------------------------
// Function: FileDependency::FileDependency()
//-----------------------------------------------------------------------------
FileDependency::FileDependency(QDomNode const& node) :
QObject(),
file1_(),
file2_(),
desc_(),
locked_(false),
bidirectional_(false),
manual_(false),
status_(STATUS_UNCHANGED)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "kactus2:fileRef1")
        {
            file1_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:fileRef2")
        {
            file2_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "ipxact:description")
        {
            desc_ = childNode.childNodes().at(0).nodeValue();
        }
    }

    locked_ = General::str2Bool(node.attributes().namedItem("kactus2:locked").nodeValue(), false);
    bidirectional_ = General::str2Bool(node.attributes().namedItem("kactus2:bidirectional").nodeValue(), false);
    manual_ = General::str2Bool(node.attributes().namedItem("kactus2:manual").nodeValue(), false);
}

//-----------------------------------------------------------------------------
// Function: FileDependency::~FileDependency()
//-----------------------------------------------------------------------------
FileDependency::~FileDependency()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependency::clone()
//-----------------------------------------------------------------------------
FileDependency* FileDependency::clone() const
{
    return new FileDependency(*this);
}

//-----------------------------------------------------------------------------
// Function: FileDependency::type()
//-----------------------------------------------------------------------------
QString FileDependency::type() const
{
    return QString("kactus2:fileDependency");
}

//-----------------------------------------------------------------------------
// Function: FileDependency::write()
//-----------------------------------------------------------------------------
void FileDependency::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:fileDependency");
    writer.writeAttribute("manual", General::bool2Str(manual_));
    writer.writeAttribute("bidirectional", General::bool2Str(bidirectional_));
    writer.writeAttribute("locked", General::bool2Str(locked_));

    writer.writeTextElement("kactus2:fileRef1", file1_);
    writer.writeTextElement("kactus2:fileRef2", file2_);
    writer.writeTextElement("ipxact:description", desc_);

    writer.writeEndElement(); // kactus2:fileDependency
}

//-----------------------------------------------------------------------------
// Function: FileDependency::setFile1()
//-----------------------------------------------------------------------------
void FileDependency::setFile1(QString const& filename)
{
    file1_ = filename;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::setFile2()
//-----------------------------------------------------------------------------
void FileDependency::setFile2(QString const& filename)
{
    file2_ = filename;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::setDescription()
//-----------------------------------------------------------------------------
void FileDependency::setDescription(QString const& desc)
{
    desc_ = desc;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::setLocked()
//-----------------------------------------------------------------------------
void FileDependency::setLocked(bool locked)
{
    locked_ = locked;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::setBidirectional()
//-----------------------------------------------------------------------------
void FileDependency::setBidirectional(bool bidirectional)
{
    bidirectional_ = bidirectional;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::setManual()
//-----------------------------------------------------------------------------
void FileDependency::setManual(bool manual)
{
    manual_ = manual;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::getFile1()
//-----------------------------------------------------------------------------
QString const& FileDependency::getFile1() const
{
    return file1_;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::getFile2()
//-----------------------------------------------------------------------------
QString const& FileDependency::getFile2() const
{
    return file2_;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::getDescription()
//-----------------------------------------------------------------------------
QString const& FileDependency::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::isLocked()
//-----------------------------------------------------------------------------
bool FileDependency::isLocked() const
{
    return locked_;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::isBidirectional()
//-----------------------------------------------------------------------------
bool FileDependency::isBidirectional() const
{
    return bidirectional_;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::isManual()
//-----------------------------------------------------------------------------
bool FileDependency::isManual() const
{
    return manual_;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::operator=()
//-----------------------------------------------------------------------------
FileDependency& FileDependency::operator=(FileDependency const& rhs)
{
    if (&rhs != this)
    {
        file1_ = rhs.file1_;
        file2_ = rhs.file2_;
        desc_ = rhs.desc_;
        locked_ = rhs.locked_;
        bidirectional_ = rhs.bidirectional_;
        manual_ = rhs.manual_;
        status_ = rhs.status_;
    }

    return *this;
}


//-----------------------------------------------------------------------------
// Function: FileDependency::reverse()
//-----------------------------------------------------------------------------
void FileDependency::reverse()
{
    qSwap(file1_, file2_);
}

//-----------------------------------------------------------------------------
// Function: FileDependency::setStatus()
//-----------------------------------------------------------------------------
void FileDependency::setStatus(Status status)
{
    status_ = status;
}

//-----------------------------------------------------------------------------
// Function: FileDependency::getStatus()
//-----------------------------------------------------------------------------
FileDependency::Status FileDependency::getStatus() const
{
    return status_;
}
