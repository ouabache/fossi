//-----------------------------------------------------------------------------
// File: FieldWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Writer class for IP-XACT field element.
//-----------------------------------------------------------------------------

#include "FieldWriter.h"
#include "Field.h"
#include "WriteValueConstraint.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/EnumeratedValueWriter.h>

//-----------------------------------------------------------------------------
// Function: FieldWriter::FieldWriter()
//-----------------------------------------------------------------------------
FieldWriter::FieldWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FieldWriter::~FieldWriter()
//-----------------------------------------------------------------------------
FieldWriter::~FieldWriter()
{

}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeField()
//-----------------------------------------------------------------------------
void FieldWriter::writeField(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    writer.writeStartElement(QStringLiteral("ipxact:field"));

    writeID(writer, field->getId());

    writeNameGroup(writer, field);

    writeIsPresent(writer, field->getIsPresent());

    writer.writeTextElement(QStringLiteral("ipxact:bitOffset"), field->getBitOffset());

    writeReset(writer, field);

    writeTypeIdentifier(writer, field);

    writer.writeTextElement(QStringLiteral("ipxact:bitWidth"), field->getBitWidth());

    writeVolatile(writer, field);

    writeAccess(writer, field);

    writeEnumerations(writer, field);

    writeModifiedWriteValue(writer, field);

    writeWriteValueConstraint(writer, field->getWriteConstraint());

    writeReadAction(writer, field);

    writeTestable(writer, field);

    writeReserved(writer, field);

    writeParameters(writer, field->getParameters());

    writeVendorExtensions(writer, field);

    writer.writeEndElement(); // ipxact:field
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeID()
//-----------------------------------------------------------------------------
void FieldWriter::writeID(QXmlStreamWriter& writer, QString const& fieldID) const
{
    if (!fieldID.isEmpty())
    {
        writer.writeAttribute(QStringLiteral("fieldID"), fieldID);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeField()
//-----------------------------------------------------------------------------
void FieldWriter::writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, field);
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeReset()
//-----------------------------------------------------------------------------
void FieldWriter::writeReset(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (!field->getResetTypeReference().isEmpty() || !field->getResetValue().isEmpty() ||
        !field->getResetMask().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:resets"));

        writer.writeStartElement(QStringLiteral("ipxact:reset"));

        if (!field->getResetTypeReference().isEmpty())
        {
            writer.writeAttribute(QStringLiteral("resetTypeRef"), field->getResetTypeReference());
        }

        writer.writeTextElement(QStringLiteral("ipxact:value"), field->getResetValue());

        if (!field->getResetMask().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:mask"), field->getResetMask());
        }

        writer.writeEndElement(); // ipxact:reset

        writer.writeEndElement(); // ipxact:resets
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeTypeIdentifier()
//-----------------------------------------------------------------------------
void FieldWriter::writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (!field->getTypeIdentifier().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:typeIdentifier"), field->getTypeIdentifier());
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeVolatile()
//-----------------------------------------------------------------------------
void FieldWriter::writeVolatile(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (!field->getVolatile().toString().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:volatile"), field->getVolatile().toString());
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeAccess()
//-----------------------------------------------------------------------------
void FieldWriter::writeAccess(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (field->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString access = AccessTypes::access2Str(field->getAccess());
        writer.writeTextElement(QStringLiteral("ipxact:access"), access);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeEnumerations()
//-----------------------------------------------------------------------------
void FieldWriter::writeEnumerations(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (!field->getEnumeratedValues()->isEmpty())
    {
        EnumeratedValueWriter enumerationWriter;
        writer.writeStartElement(QStringLiteral("ipxact:enumeratedValues"));

        foreach (QSharedPointer<EnumeratedValue> enumeration, *field->getEnumeratedValues())
        {
            enumerationWriter.writeEnumeratedValue(writer, enumeration);
        }

        writer.writeEndElement(); // ipxact:enumeratedValues
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeModifiedWriteValue()
//-----------------------------------------------------------------------------
void FieldWriter::writeModifiedWriteValue(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (field->getModifiedWrite() != General::MODIFIED_WRITE_COUNT)
    {
        writer.writeStartElement(QStringLiteral("ipxact:modifiedWriteValue"));

        if (!field->getModifiedWriteModify().isEmpty())
        {
            writer.writeAttribute(QStringLiteral("modify"), field->getModifiedWriteModify());
        }

        QString modifiedWriteValue = General::modifiedWrite2Str(field->getModifiedWrite());
        writer.writeCharacters(modifiedWriteValue);

        writer.writeEndElement(); // ipxact:modifiedWriteValue
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldWriter::writeWriteValueConstraint(QXmlStreamWriter& writer,
    QSharedPointer<WriteValueConstraint> writeConstraint) const
{
    if (!writeConstraint.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:writeValueConstraint"));

        if (writeConstraint->getType() == WriteValueConstraint::WRITE_AS_READ)
        {
            writer.writeTextElement(QStringLiteral("ipxact:writeAsRead"), QStringLiteral("true"));
        }
        else if (writeConstraint->getType() == WriteValueConstraint::USE_ENUM)
        {
            writer.writeTextElement(QStringLiteral("ipxact:useEnumeratedValues"), QStringLiteral("true"));
        }
        else if (writeConstraint->getType() == WriteValueConstraint::MIN_MAX)
        {
            writer.writeTextElement(QStringLiteral("ipxact:minimum"), writeConstraint->getMinimum());
            writer.writeTextElement(QStringLiteral("ipxact:maximum"), writeConstraint->getMaximum());
        }

        writer.writeEndElement(); // ipxact:writeValueConstraint
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeReadAction()
//-----------------------------------------------------------------------------
void FieldWriter::writeReadAction(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (field->getReadAction() != General::READ_ACTION_COUNT)
    {
        QString readAction = General::readAction2Str(field->getReadAction());

        writer.writeStartElement(QStringLiteral("ipxact:readAction"));

        if (!field->getReadActionModify().isEmpty())
        {
            writer.writeAttribute(QStringLiteral("modify"), field->getReadActionModify());
        }
        writer.writeCharacters(readAction);

        writer.writeEndElement(); // ipxact:readAction
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeTestable()
//-----------------------------------------------------------------------------
void FieldWriter::writeTestable(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (!field->getTestable().toString().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:testable"));

        if (field->getTestConstraint() != General::TESTCONSTRAINT_COUNT)
        {
            QString testConstraint = General::testConstraint2Str(field->getTestConstraint());
            writer.writeAttribute(QStringLiteral("testConstraint"), testConstraint);
        }

        writer.writeCharacters(field->getTestable().toString());

        writer.writeEndElement(); // ipxact:testable
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeReserved()
//-----------------------------------------------------------------------------
void FieldWriter::writeReserved(QXmlStreamWriter& writer, QSharedPointer<Field> field) const
{
    if (!field->getReserved().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:reserved"), field->getReserved());
    }
}