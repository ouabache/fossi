//-----------------------------------------------------------------------------
// File: ComponentWizardImportPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 06.06.2013
//
// Description:
// Import page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardImportPage.h"
#include "ComponentWizardPages.h"

#include <wizards/ComponentWizard/ImportEditor/ImportEditor.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardImportPage::ComponentWizardImportPage(QSharedPointer<Component> component, 
	LibraryInterface* handler, PluginManager const& pluginMgr,
    QSharedPointer <ComponentParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent):
QWizardPage(parent),
editor_(new ImportEditor(component, handler, pluginMgr, parameterFinder, expressionFormatter, this))
{
    setTitle(tr("Import source file"));
    setSubTitle(tr("Choose the top-level file to import into component.\n"
        "Any model parameter not found in the input file will be removed. Any port not found "
        "in the input file will be set as phantom."));
    setFinalPage(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(editor_);

    connect(editor_, SIGNAL(contentChanged()), this, SIGNAL(completeChanged()), Qt::UniqueConnection);
    connect(editor_, SIGNAL(componentChanged(QSharedPointer<Component>)), 
        this, SIGNAL(componentChanged(QSharedPointer<Component>)), Qt::UniqueConnection);

    connect(editor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(editor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::~ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardImportPage::~ComponentWizardImportPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardImportPage::nextId() const
{
    return ComponentWizardPages::VIEWS;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::initializePage()
//-----------------------------------------------------------------------------
void ComponentWizardImportPage::initializePage()
{	
    editor_->initializeFileSelection();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardImportPage::isComplete() const
{
	return editor_->checkEditorValidity();
}
