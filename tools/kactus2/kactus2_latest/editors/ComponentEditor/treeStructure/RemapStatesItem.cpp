//-----------------------------------------------------------------------------
// File: RemapStatesItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.04.2015
//
// Description:
// The Remap states-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "RemapStatesItem.h"
#include "SingleRemapStateItem.h"

#include <editors/ComponentEditor/remapStates/RemapStatesEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/validators/RemapStateValidator.h>
//-----------------------------------------------------------------------------
// Function: RemapStatesItem::RemapStatesItem()
//-----------------------------------------------------------------------------
RemapStatesItem::RemapStatesItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, 
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, 
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
remapStates_(component->getRemapStates()),
validator_(new RemapStateValidator(expressionParser, component->getPorts()))
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setReferenceCounter(referenceCounter);

    foreach(QSharedPointer<RemapState> remapState, *remapStates_)
    {
        QSharedPointer<SingleRemapStateItem> singleRemapItem(new SingleRemapStateItem(remapState, model,
            libHandler, component, referenceCounter, parameterFinder, expressionFormatter, validator_, this));

        childItems_.append(singleRemapItem);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::~RemapStatesItem()
//-----------------------------------------------------------------------------
RemapStatesItem::~RemapStatesItem()
{

}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::getFont()
//-----------------------------------------------------------------------------
QFont RemapStatesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!childItems_.isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::text()
//-----------------------------------------------------------------------------
QString RemapStatesItem::text() const
{
    return tr("Remap states");
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* RemapStatesItem::editor()
{
    if (!editor_)
    {
        editor_ = new RemapStatesEditor(component_, libHandler_, parameterFinder_);
        editor_->setProtection(locked_);
        
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::getTooltip()
//-----------------------------------------------------------------------------
QString RemapStatesItem::getTooltip() const
{
    return tr("Contains the remap states of the component");
}

//-----------------------------------------------------------------------------
// Function: RemapStatesItem::createChild()
//-----------------------------------------------------------------------------
void RemapStatesItem::createChild(int index)
{
    QSharedPointer<SingleRemapStateItem> remapItem(new SingleRemapStateItem(remapStates_->at(index), model_,
        libHandler_, component_, referenceCounter_, parameterFinder_, expressionFormatter_, validator_, this));

    remapItem->setLocked(locked_);

    childItems_.insert(index, remapItem);
}
