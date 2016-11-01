/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormodelparamsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormodelparamsitem.h"

#include <editors/ComponentEditor/modelParameters/modelparametereditor.h>

#include <IPXACTmodels/choice.h>

ComponentEditorModelParamsItem::ComponentEditorModelParamsItem( ComponentEditorTreeModel* model,
															   LibraryInterface* libHandler,
															   QSharedPointer<Component> component,
                                                               QSharedPointer<ReferenceCounter> refCounter,
                                                               QSharedPointer<ParameterFinder> parameterFinder,
                                                               QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                               ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
modelParams_(component->getModelParameters())
{
	setReferenceCounter(refCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
}

ComponentEditorModelParamsItem::~ComponentEditorModelParamsItem()
{
}

QFont ComponentEditorModelParamsItem::getFont() const 
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!modelParams_->empty());
    return font;
}

QString ComponentEditorModelParamsItem::text() const 
{
	return tr("Model parameters");
}

bool ComponentEditorModelParamsItem::isValid() const 
{
    // if one model parameter is invalid
    foreach (QSharedPointer<ModelParameter> modelParam, *modelParams_) 
    {
        if (!component_->validateParameter(modelParam))
        {
            return false;
        }
    }

	return true;
}

ItemEditor* ComponentEditorModelParamsItem::editor()
{
	if (!editor_) {
		editor_ = new ModelParameterEditor(component_, libHandler_, parameterFinder_, expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();

        connectReferenceTree();
	}
	return editor_;
}

QString ComponentEditorModelParamsItem::getTooltip() const
{
	return tr("Contains the implementation parameters");
}
