//-----------------------------------------------------------------------------
// File: generaleditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 7.2.2011
//
// Description:
// General editor for editing the component's general data.
//-----------------------------------------------------------------------------

#include "generaleditor.h"

#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QStringList>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: GeneralEditor()
//-----------------------------------------------------------------------------
GeneralEditor::GeneralEditor(LibraryInterface* libHandler, QSharedPointer<Component> component, QWidget *parent):
ItemEditor(component, libHandler, parent),
vlnvDisplayer_(0),
attributeEditor_(0),
authorEditor_(0),
descEditor_(NULL),
headerEditor_(NULL),
previewBox_(0)
{
    Q_ASSERT(libHandler != 0);
    Q_ASSERT(component != 0);

    // Create the VLNV displayer and attribute & description editors.
    QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

    const QString xmlPath = libHandler->getPath(component->getVlnv());

    vlnvDisplayer_ = new VLNVDisplayer(this, component->getVlnv());
	vlnvDisplayer_->setPath(xmlPath);

    attributeEditor_ = new KactusAttributeEditor(this);

    QGroupBox* authorBox = new QGroupBox("Author", this);
    authorEditor_ = new QLineEdit(authorBox);   
    QVBoxLayout* authorLayout = new QVBoxLayout(authorBox);
    authorLayout->addWidget(authorEditor_);

    descEditor_ = new DescEditor();

	headerEditor_ = new DescEditor();
	headerEditor_->setTitle(tr("XML header"));

    previewBox_ = new ComponentPreviewBox(libHandler);
    previewBox_->setInteractive(true);
    previewBox_->setComponent(component);
    previewBox_->setFixedWidth(280);
    previewBox_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(vlnvDisplayer_);
    layout->addWidget(attributeEditor_);
    layout->addWidget(authorBox);    
    layout->addWidget(descEditor_);
	layout->addWidget(headerEditor_);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addLayout(layout, 1);
    topLayout->addWidget(previewBox_);

    // Connect the contentChanged() signals.
    connect(previewBox_, SIGNAL(endpointsRearranged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(attributeEditor_, SIGNAL(contentChanged()), this, SLOT(onAttributesChange()), Qt::UniqueConnection);
    connect(descEditor_, SIGNAL(contentChanged()), this, SLOT(onDescriptionChange()), Qt::UniqueConnection);
	connect(headerEditor_, SIGNAL(contentChanged()), this, SLOT(onHeaderChange()), Qt::UniqueConnection);
    connect(authorEditor_, SIGNAL(textChanged(const QString &)),
        this, SLOT(onAuthorChange()), Qt::UniqueConnection);

    refresh();
}

//-----------------------------------------------------------------------------
// Function: ~GeneralEditor()
//-----------------------------------------------------------------------------
GeneralEditor::~GeneralEditor()
{

}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool GeneralEditor::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::refresh()
//-----------------------------------------------------------------------------
void GeneralEditor::refresh()
{
	disconnect(attributeEditor_, SIGNAL(contentChanged()), this, SLOT(onAttributesChange()));

    if (component()->getImplementation() == KactusAttribute::HW)
	{
        KactusAttribute::ProductHierarchy prodHier = component()->getHierarchy();
        KactusAttribute::Firmness firmness = component()->getFirmness();
		attributeEditor_->setAttributes(prodHier, firmness);
	}
	else
	{
		attributeEditor_->hideAttributes();
	}

    attributeEditor_->setImplementation(component()->getImplementation());
	connect(attributeEditor_, SIGNAL(contentChanged()), this, SLOT(onAttributesChange()), Qt::UniqueConnection);

	disconnect(authorEditor_, SIGNAL(textChanged(const QString &)), this, SLOT(onAuthorChange()));
    QString author = component()->getAuthor();
	authorEditor_->setText(component()->getAuthor());
	connect(authorEditor_, SIGNAL(textChanged(const QString &)),
        this, SLOT(onAuthorChange()), Qt::UniqueConnection);

	disconnect(descEditor_, SIGNAL(contentChanged()), this, SLOT(onDescriptionChange()));
	descEditor_->setDescription(component()->getDescription());
	connect(descEditor_, SIGNAL(contentChanged()), this, SLOT(onDescriptionChange()), Qt::UniqueConnection);

	disconnect(headerEditor_, SIGNAL(contentChanged()), this, SLOT(onHeaderChange()));
	QStringList comments = component()->getTopComments();
	QString comment = comments.join("\n");
	headerEditor_->setDescription(comment);
	connect(headerEditor_, SIGNAL(contentChanged()), this, SLOT(onHeaderChange()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onAttributesChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onAttributesChange()
{
    if (component()->getImplementation() == KactusAttribute::HW)
	{
        component()->setHierarchy(attributeEditor_->getProductHierarchy());
        component()->setFirmness(attributeEditor_->getFirmness());
	}
	emit contentChanged();

	QSettings settings;
	emit hierarchyChanged(settings);
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onDescriptionChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onDescriptionChange()
{
	component()->setDescription(descEditor_->description());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::showEvent()
//-----------------------------------------------------------------------------
void GeneralEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/general.html");

    previewBox_->updatePreview();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onHeaderChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onHeaderChange()
{
	component()->setTopComments(headerEditor_->description());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneralEditor::onAuthorChange()
//-----------------------------------------------------------------------------
void GeneralEditor::onAuthorChange()
{
    component()->setAuthor(authorEditor_->text());
    emit contentChanged();
}