//-----------------------------------------------------------------------------
// File: addressspacegeneraleditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.02.2012
//
// Description:
// Editor to set the general settings of an address space.
//-----------------------------------------------------------------------------

#include "addressspacegeneraleditor.h"

#include <common/utils.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>

#include <IPXACTmodels/Component/AddressSpace.h>

#include <QFormLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::AddressSpaceGeneralEditor()
//-----------------------------------------------------------------------------
AddressSpaceGeneralEditor::AddressSpaceGeneralEditor(QSharedPointer<AddressSpace> addrSpace,
    QStringList busInterfaceNames,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser,
    QWidget *parent):
QGroupBox(tr("General"), parent),
addrSpace_(addrSpace),
expressionParser_(expressionParser),
addrUnitEditor_(this),
rangeEditor_(new ExpressionEditor(parameterFinder, this)),
widthEditor_(new ExpressionEditor(parameterFinder, this)),
masterInterfaceBindingLabel_(new QLabel(this))
{
	Q_ASSERT(addrSpace_);

    addrUnitEditor_.setPlaceholderText("8");
    
    rangeEditor_->setFixedHeight(20);
    widthEditor_->setFixedHeight(20);

	// Set the back ground colors for mandatory fields.
	widthEditor_->setProperty("mandatoryField", true);
	rangeEditor_->setProperty("mandatoryField", true);

    ComponentParameterModel* parameterCompletionModel = new ComponentParameterModel(parameterFinder, this);
    parameterCompletionModel->setExpressionParser(expressionParser);

    ParameterCompleter* rangeEditorCompleter = new ParameterCompleter(this);
    rangeEditorCompleter->setModel(parameterCompletionModel);

    ParameterCompleter* widthEditorCompleter = new ParameterCompleter(this);
    widthEditorCompleter->setModel(parameterCompletionModel);

    rangeEditor_->setAppendingCompleter(rangeEditorCompleter);
    widthEditor_->setAppendingCompleter(widthEditorCompleter);

	QFormLayout* layout = new QFormLayout(this);
	layout->addRow(tr("Addressable unit bits (AUB):"),&addrUnitEditor_);
	layout->addRow(tr("Range (=size) [AUB], f(x):"), rangeEditor_);
    layout->addRow(tr("Width [bits], f(x):"), widthEditor_);
    layout->addRow(tr("Master interface binding(s):"), masterInterfaceBindingLabel_);
	layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	refresh(busInterfaceNames);

    connect(&addrUnitEditor_, SIGNAL(editingFinished()), this, SLOT(onAddrUnitChanged()), Qt::UniqueConnection);
    connect(&addrUnitEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

	connect(widthEditor_, SIGNAL(editingFinished()), this, SLOT(onWidthChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    
    connect(widthEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	connect(rangeEditor_, SIGNAL(editingFinished()), this, SLOT(onRangeChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(rangeEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::~AddressSpaceGeneralEditor()
//-----------------------------------------------------------------------------
AddressSpaceGeneralEditor::~AddressSpaceGeneralEditor()
{
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::isValid()
//-----------------------------------------------------------------------------
bool AddressSpaceGeneralEditor::isValid() const
{
	return !rangeEditor_->getExpression().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::refresh()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::refresh(QStringList masterInterfaceNames)
{
    widthEditor_->blockSignals(true);
    rangeEditor_->blockSignals(true);

    addrUnitEditor_.setText(addrSpace_->getAddressUnitBits());

    widthEditor_->setExpression(addrSpace_->getWidth());
    widthEditor_->setToolTip(format(addrSpace_->getWidth()));

	rangeEditor_->setExpression(addrSpace_->getRange());
    rangeEditor_->setToolTip(format(addrSpace_->getRange()));

    widthEditor_->blockSignals(false);
    rangeEditor_->blockSignals(false);

    if (masterInterfaceNames.isEmpty())
    {
        masterInterfaceBindingLabel_->setText(tr("No binding"));
    }
    else
    {
        masterInterfaceBindingLabel_->setText(masterInterfaceNames.join(", "));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::onAddrUnitChanged()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::onAddrUnitChanged()
{
    QString newAddressUnitBits = addrUnitEditor_.text();
    addrSpace_->setAddressUnitBits(newAddressUnitBits);

    addrUnitEditor_.setToolTip(format(newAddressUnitBits));

    emit assignNewAddressUnitBits(newAddressUnitBits);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::onWidthChanged()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::onWidthChanged()
{
    widthEditor_->finishEditingCurrentWord();

    addrSpace_->setWidth(widthEditor_->getExpression());   
    widthEditor_->setToolTip(format(widthEditor_->getExpression()));

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::onRangeChanged()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::onRangeChanged()
{
    rangeEditor_->finishEditingCurrentWord();
    addrSpace_->setRange(rangeEditor_->getExpression());
    rangeEditor_->setToolTip(format(rangeEditor_->getExpression()));

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::format()
//-----------------------------------------------------------------------------
QString AddressSpaceGeneralEditor::format(QString const& expression) const
{
    if (expressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(expressionParser_->parseExpression(expression),
            expressionParser_->baseForExpression(expression));
    }
    else if (expressionParser_->isPlainValue(expression))
    {
        return expression;
    }
    else
    {
        return "n/a";
    }
}
