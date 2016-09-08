//-----------------------------------------------------------------------------
// File: RegisterExpressionGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a register and its fields.
//-----------------------------------------------------------------------------

#include "RegisterExpressionsGatherer.h"
#include "FieldExpressionsGatherer.h"

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

//-----------------------------------------------------------------------------
// Function: RegisterExpressionsGatherer::RegisterExpressionsGatherer()
//-----------------------------------------------------------------------------
RegisterExpressionsGatherer::RegisterExpressionsGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterExpressionsGatherer::~RegisterExpressionsGatherer()
//-----------------------------------------------------------------------------
RegisterExpressionsGatherer::~RegisterExpressionsGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterExpressionsGatherer::getExpressions()
//-----------------------------------------------------------------------------
QStringList RegisterExpressionsGatherer::getExpressions(QSharedPointer<Register> currentRegister) const
{
    QStringList expressionList;

    expressionList.append(currentRegister->getAddressOffset());
    expressionList.append(currentRegister->getSize());
    expressionList.append(currentRegister->getDimension());
    expressionList.append(currentRegister->getIsPresent());

    FieldExpressionsGatherer fieldGatherer;

    foreach (QSharedPointer<Field> field, *currentRegister->getFields())
    {
        expressionList.append(fieldGatherer.getExpressions(field));
    }

    return expressionList;
}
