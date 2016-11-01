//-----------------------------------------------------------------------------
// File: componenteditorregisteritem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The item for single register in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorregisteritem.h"
#include "componenteditorfielditem.h"

#include <editors/ComponentEditor/memoryMaps/SingleRegisterEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registergraphitem.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::ComponentEditorRegisterItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterItem::ComponentEditorRegisterItem(QSharedPointer<Register> reg,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterValidator> registerValidator, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
visualizer_(NULL),
registerDimensions_(),
expressionParser_(expressionParser),
registerValidator_(registerValidator)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorRegisterItem"));

	foreach(QSharedPointer<Field> field, *reg_->getFields())
    {
		if (field)
        {
			QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
				reg, field, model, libHandler, component, parameterFinder, referenceCounter, expressionParser_,
                registerValidator_->getFieldValidator(), this));
			childItems_.append(fieldItem);

            connect(fieldItem.data(), SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::~ComponentEditorRegisterItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterItem::~ComponentEditorRegisterItem()
{

}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getToolTip()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterItem::getTooltip() const
{
	return tr("Contains details of a single register within an address block.");
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterItem::text() const
{
	return reg_->name();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRegisterItem::isValid() const
{
    return registerValidator_->validate(reg_);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorRegisterItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleRegisterEditor(reg_, component_, libHandler_, parameterFinder_, expressionFormatter_,
            expressionParser_, registerValidator_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::createChild( int index )
{
	QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
		reg_, reg_->getFields()->at(index), model_, libHandler_, component_, parameterFinder_, 
        referenceCounter_, expressionParser_, registerValidator_->getFieldValidator(), this));
	fieldItem->setLocked(locked_);
	
	if (visualizer_)
    {
		fieldItem->setVisualizer(visualizer_);
	}


    connect(fieldItem.data(), SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);

	childItems_.insert(index, fieldItem);
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onEditorChanged()
{
	// on register also the grand parent must be updated
	if (parent() && parent()->parent())
    {
		emit contentChanged(parent()->parent());

		// on register also the grand grand parent must be updated
		if (parent()->parent()->parent())
        {
			emit contentChanged(parent()->parent()->parent());
		}
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorRegisterItem::visualizer()
{
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::setVisualizer(MemoryMapsVisualizer* visualizer)
{
	visualizer_ = visualizer;
    connect(visualizer_, SIGNAL(displayed()), this, SLOT(updateGraphics()), Qt::UniqueConnection);

    resizeGraphicsToCurrentDimensionSize();
	
	// update the visualizers for field items
	foreach (QSharedPointer<ComponentEditorItem> item, childItems_)
    {
		QSharedPointer<ComponentEditorFieldItem> fieldItem = item.staticCast<ComponentEditorFieldItem>();
		fieldItem->setVisualizer(visualizer_);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorRegisterItem::getGraphicsItem()
{
    if (!registerDimensions_.isEmpty())
    {
        return registerDimensions_.first();
    }
    else
    {
	    return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::updateGraphics()
{
    resizeGraphicsToCurrentDimensionSize();

	foreach (RegisterGraphItem* registerDimension, registerDimensions_)
    {
		registerDimension->refresh();
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::onGraphicsChanged()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ComponentEditorItem::onGraphicsChanged();

    parent()->updateGraphics();
    parent()->parent()->updateGraphics();
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: componenteditorregisteritem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeGraphicsItem()
{
    // get the graphics item for the address block.
    MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    Q_ASSERT(parentItem);

    foreach (RegisterGraphItem* registerDimension, registerDimensions_)
    {	
		parentItem->removeChild(registerDimension);
		registerDimension->setParent(0);

		disconnect(registerDimension, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        registerDimensions_.removeAll(registerDimension);

		delete registerDimension;
		registerDimension = 0;
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::resizeToCurrentDimensionSize()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::resizeGraphicsToCurrentDimensionSize()
{
    MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    
    if(!parentItem)
    {
        return;
    }

    const int DIMENSION_SIZE = qMax(expressionParser_->parseExpression(reg_->getDimension()).toInt(), 1);
   
    for (int currentDimension = registerDimensions_.count(); currentDimension < DIMENSION_SIZE; currentDimension++)
    {
        createDimensionGraphicsItem(currentDimension, parentItem);
    }

    for (int currentDimension = registerDimensions_.count() - 1; currentDimension >= DIMENSION_SIZE;
        currentDimension--)
    {
        removeDimensionGraphicsItem(currentDimension, parentItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::createDimensionGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::createDimensionGraphicsItem(int dimensionIndex,
    MemoryVisualizationItem* parentItem)
{
    RegisterGraphItem* newDimension = new RegisterGraphItem(reg_, expressionParser_, parentItem);
    newDimension->setDimensionIndex(dimensionIndex);

    parentItem->addChild(newDimension);
    registerDimensions_.append(newDimension);

    connect(newDimension, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterItem::removeDimensionGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterItem::removeDimensionGraphicsItem(int dimensionIndex, 
    MemoryVisualizationItem* parentItem)
{
    RegisterGraphItem* removedDimension = registerDimensions_.takeAt(dimensionIndex);
    parentItem->removeChild(removedDimension);
    removedDimension->setParent(0);

    disconnect(removedDimension, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

    delete removedDimension;
    removedDimension = 0;
}