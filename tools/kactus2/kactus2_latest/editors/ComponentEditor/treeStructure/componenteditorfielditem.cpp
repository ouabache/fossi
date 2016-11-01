//-----------------------------------------------------------------------------
// File: componenteditorfielditem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.08.2012
//
// Description:
// The item for single field in component editor's navigation tree
//-----------------------------------------------------------------------------

#include "componenteditorfielditem.h"

#include <editors/ComponentEditor/memoryMaps/SingleFieldEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/fieldgraphitem.h>

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
    
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::ComponentEditorFieldItem()
//-----------------------------------------------------------------------------
ComponentEditorFieldItem::ComponentEditorFieldItem(QSharedPointer<Register> reg, QSharedPointer<Field> field,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<FieldValidator> fieldValidator,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
field_(field),
visualizer_(NULL),
graphItem_(NULL),
expressionParser_(expressionParser),
fieldValidator_(fieldValidator)
{
	Q_ASSERT(field_);

    setParameterFinder(parameterFinder);
    setReferenceCounter(referenceCounter);

	setObjectName(tr("ComponentEditorFieldItem"));
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::~ComponentEditorFieldItem()
//-----------------------------------------------------------------------------
ComponentEditorFieldItem::~ComponentEditorFieldItem()
{

}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorFieldItem::getTooltip() const
{
	return tr("Contains details of a single field within a register.");
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorFieldItem::text() const
{
	return field_->name();
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorFieldItem::isValid() const 
{
    return fieldValidator_->validate(field_);
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorFieldItem::editor()
{
	if (!editor_)
    {
        editor_ = new SingleFieldEditor(field_, component_, libHandler_, parameterFinder_, expressionParser_,
            fieldValidator_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::onEditorChanged()
{
	// on field also the grand parent must be updated
	if (parent() && parent()->parent())
    {
		emit contentChanged(parent()->parent());

		// on field also the grand grand parent must be updated
		if (parent()->parent()->parent())
        {
			emit contentChanged(parent()->parent()->parent());

			// on field also the grand grand grand parent must be updated
			if (parent()->parent()->parent()->parent())
            {
				emit contentChanged(parent()->parent()->parent()->parent());
			}
		}
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorFieldItem::visualizer()
{
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	// get the graphics item for the memory map
	MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
	Q_ASSERT(parentItem);

	// create the graph item for the address block
	graphItem_ = new FieldGraphItem(field_, expressionParser_, parentItem);

	// register the addr block graph item for the parent
	parentItem->addChild(graphItem_);

	connect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorFieldItem::getGraphicsItem()
{
	return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::updateGraphics()
{
	if (graphItem_)
    {
		graphItem_->refresh();
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorfielditem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorFieldItem::removeGraphicsItem()
{
	if (graphItem_)
    {
		// get the graphics item for the memory map
		MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
		Q_ASSERT(parentItem);

		// unregister addr block graph item from the memory map graph item
		parentItem->removeChild(graphItem_);

		// take the child from the parent
		graphItem_->setParent(NULL);

		disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

		// delete the graph item
		delete graphItem_;
		graphItem_ = NULL;
	}
}
