//-----------------------------------------------------------------------------
// File: PortVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Class for writing port declarations in Verilog.
//-----------------------------------------------------------------------------

#include "PortVerilogWriter.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::PortVerilogWriter()
//-----------------------------------------------------------------------------
PortVerilogWriter::PortVerilogWriter(QSharedPointer<const Port> port, 
    QSharedPointer<ExpressionFormatter> expressionFormatter)
    : port_(port), formatter_(expressionFormatter)
{

}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::PortVerilogWriter()
//-----------------------------------------------------------------------------
PortVerilogWriter::~PortVerilogWriter()
{

}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::write()
//-----------------------------------------------------------------------------
void PortVerilogWriter::write( QTextStream& outputStream ) const
{
    if (!nothingToWrite())
    {
        outputStream << createDeclaration();
    }
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::nothingToWrite()
//-----------------------------------------------------------------------------
bool PortVerilogWriter::nothingToWrite() const
{
    return port_.isNull() || 
        port_->getDirection() == DirectionTypes::DIRECTION_PHANTOM || port_->getDirection() == DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::createDeclaration() const
{
    QString portDeclaration("<direction> <type> <bounds> <name>");

    portDeclaration.replace("<direction>", direction().leftJustified(6));
    portDeclaration.replace("<type>", port_->getTypeName().leftJustified(7));
    portDeclaration.replace("<bounds>", arrayAndVectorBounds().leftJustified(20));
    portDeclaration.replace("<name>", port_->name());

    return portDeclaration;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::direction()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::direction() const
{
    QString directionString;

    if (port_->getDirection() == DirectionTypes::IN)
    {
        directionString = "input";
    }
    else if (port_->getDirection() == DirectionTypes::OUT)    
    {
        directionString = "output";
    }
    else if (port_->getDirection() == DirectionTypes::INOUT)   
    {
        directionString = "inout";
    }

    return directionString;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::arrayAndVectorBounds()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::arrayAndVectorBounds() const
{
    QString arrayDefinition = "[" + port_->getArrayLeft() + ":" + port_->getArrayRight() + "]";

    arrayDefinition.remove(" ");

    if (arrayDefinition == "[:]")
    {
        arrayDefinition.clear();
    }

    QString vectorDefinition = "[" + port_->getLeftBound() + ":" + port_->getRightBound() + "]";

    vectorDefinition.remove(" ");

    if (vectorDefinition == "[0:0]" || vectorDefinition == "[:]")
    {
        vectorDefinition.clear();
    }

    return formatter_->formatReferringExpression(arrayDefinition + vectorDefinition);
}
