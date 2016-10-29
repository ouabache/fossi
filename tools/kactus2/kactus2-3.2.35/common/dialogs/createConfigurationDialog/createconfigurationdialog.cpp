//-----------------------------------------------------------------------------
// File: createconfigurationdialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 30.08.2011
//
// Description:
// Dialog for user to input the info needed to create a new configuration for a component.
//-----------------------------------------------------------------------------

#include "createconfigurationdialog.h"

#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/common/VLNV.h>

#include <common/widgets/LineEditEx/LineEditEx.h>

#include <IPXACTmodels/Component/Component.h>

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QCoreApplication>
#include <QSettings>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::CreateConfigurationDialog()
//-----------------------------------------------------------------------------
CreateConfigurationDialog::CreateConfigurationDialog(LibraryInterface* handler,
													 QSharedPointer<Component> component,
                                                     KactusAttribute::Implementation configType,
													 QWidget *parent,
													 Qt::WindowFlags f):
QDialog(parent, f),
handler_(handler),
component_(component),
configType_(configType),
configNameEdit_(new LineEditEx(this)),
configNameMatcher_(),
useExistingRadio_(new QRadioButton(tr("Use current design"), this)),
useExistingDescLabel_(new QLabel(tr("Creates only a new configuration for the current design."), this)),
createNewRadio_(new QRadioButton(tr("Create new design"), this)),
createNewDescLabel_(new QLabel(tr("Creates a new empty design and a configuration for it."), this)),
createCopyRadio_(new QRadioButton(tr("Copy old design to new configuration"), this)),
createCopyDescLabel_(new QLabel(tr("Duplicates the current design with a new VLNV and creates a configuration for it."), this)),
vlnvEdit_(new VLNVEditor(VLNV::DESIGNCONFIGURATION, handler, this, this, true)),
implementationCombo_(new QComboBox(this)),
okButton_(new QPushButton(tr("OK"), this)),
cancelButton_(new QPushButton(tr("Cancel"), this))
{
	Q_ASSERT(component);
	Q_ASSERT(handler);

	configNameEdit_->setMessageIcon(QPixmap(":/icons/common/graphics/exclamation.png"));
    configNameEdit_->setContentMatcher(&configNameMatcher_);

    QSettings settings;

    if (configType == KactusAttribute::HW)
    {
        QStringList suggestions = settings.value("Policies/HWViewNames").toStringList();
        configNameMatcher_.setItems(suggestions);

        configNameEdit_->setDisallowedInputs(component->getHierViews());
        configNameEdit_->setMessageTemplate("View '%1' already exists!");
    }
    else if (configType == KactusAttribute::SW)
    {
        QStringList suggestions = settings.value("Policies/SWViewNames").toStringList();
        configNameMatcher_.setItems(suggestions);

        configNameEdit_->setDisallowedInputs(component->getSWViewNames());
        configNameEdit_->setMessageTemplate("SW view '%1' already exists!");
    }
    else if (configType == KactusAttribute::SYSTEM)
    {
        QStringList suggestions = settings.value("Policies/SysViewNames").toStringList();
        configNameMatcher_.setItems(suggestions);

        configNameEdit_->setDisallowedInputs(component->getSystemViewNames());
        configNameEdit_->setMessageTemplate("System view '%1' already exists!");
    }
    else
    {
        Q_ASSERT(false);
    }

    QFont font = useExistingRadio_->font();
    font.setBold(true);
    useExistingRadio_->setFont(font);
    useExistingRadio_->setStyleSheet("QRadioButton::indicator { width: 15px; height: 15px; }");
    createNewRadio_->setFont(font);
    createNewRadio_->setStyleSheet("QRadioButton::indicator { width: 15px; height: 15px; }");
    createCopyRadio_->setFont(font);
    createCopyRadio_->setStyleSheet("QRadioButton::indicator { width: 15px; height: 15px; }");

	// by default an existing design is used.
	useExistingRadio_->toggle();

    useExistingDescLabel_->setStyleSheet("QLabel { padding-left: 19px; }");
    useExistingDescLabel_->setWordWrap(true);
    createNewDescLabel_->setStyleSheet("QLabel { padding-left: 19px; }");
    createNewDescLabel_->setWordWrap(true);
    createCopyDescLabel_->setStyleSheet("QLabel { padding-left: 19px; }");
    createCopyDescLabel_->setWordWrap(true);

	// set the views to the combo box
	implementationCombo_->addItem(QString(""));
	implementationCombo_->addItems(component->getViewNames());

	// set the title for vlnv editor
	vlnvEdit_->setTitle(tr("Configuration VLNV"));
	VLNV compVLNV = component_->getVlnv();
	vlnvEdit_->setVendor(compVLNV.getVendor());
	vlnvEdit_->setLibrary(compVLNV.getLibrary());
	vlnvEdit_->setVersion(compVLNV.getVersion());

    setupLayout();
    setupConnections();

	setWindowTitle(tr("Create New Configuration"));
    setFixedSize(470, heightForWidth(470));
    
    prevalidate();
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::~CreateConfigurationDialog()
//-----------------------------------------------------------------------------
CreateConfigurationDialog::~CreateConfigurationDialog()
{

}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::setupLayout()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::setupLayout()
{
    QGroupBox* radioGroup = new QGroupBox(tr("Action"), this);

	QVBoxLayout* radioLayout = new QVBoxLayout(radioGroup);
	radioLayout->addWidget(useExistingRadio_);
    radioLayout->addWidget(useExistingDescLabel_);
	radioLayout->addWidget(createNewRadio_);
    radioLayout->addWidget(createNewDescLabel_);
	radioLayout->addWidget(createCopyRadio_);
    radioLayout->addWidget(createCopyDescLabel_);
	
	QFormLayout* comboLayout = new QFormLayout();
	comboLayout->addRow(tr("Reference to this component's top-level\n"
		                   "implementation view (optional)"), implementationCombo_);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton_, 0, Qt::AlignRight);
	buttonLayout->addWidget(cancelButton_, 0, Qt::AlignRight);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(new QLabel(tr("Configuration name:"), this));
    topLayout->addWidget(configNameEdit_);
    topLayout->addWidget(radioGroup);
	topLayout->addWidget(vlnvEdit_);
	topLayout->addLayout(comboLayout);
	topLayout->addLayout(buttonLayout);
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::setupConnections()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::setupConnections()
{
	connect(okButton_, SIGNAL(clicked(bool)), this, SLOT(onOk()), Qt::UniqueConnection);
	connect(cancelButton_, SIGNAL(clicked(bool)), this, SLOT(reject()), Qt::UniqueConnection);
	connect(configNameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onConfNameChanged(const QString&)), Qt::UniqueConnection);

    connect(configNameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(prevalidate()), Qt::UniqueConnection);
    connect(vlnvEdit_, SIGNAL(contentChanged()), this, SLOT(prevalidate()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::onOk()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::onOk()
{
    // Make sure that the VLNVs are not already in use.
    if (handler_->contains(getConfigurationVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("VLNV %1 already exists in the library.").arg(getConfigurationVLNV().toString()),
                           QMessageBox::Ok, this);
        msgBox.exec();
        return;
    }

    if (!useExistingRadio_->isChecked() && handler_->contains(getDesignVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("VLNV %1 already exists in the library.").arg(getDesignVLNV().toString()),
                           QMessageBox::Ok, this);
        msgBox.exec();
        return;
    }

    accept();
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::getConfigurationVLNV()
//-----------------------------------------------------------------------------
VLNV CreateConfigurationDialog::getConfigurationVLNV() const
{
	VLNV configVLNV = vlnvEdit_->getVLNV();
	configVLNV.setType(VLNV::DESIGNCONFIGURATION);
	return configVLNV;
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::getConfigurationName()
//-----------------------------------------------------------------------------
QString CreateConfigurationDialog::getConfigurationName() const
{
	return configNameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::getImplementationViewName()
//-----------------------------------------------------------------------------
QString CreateConfigurationDialog::getImplementationViewName() const
{
	return implementationCombo_->currentText();
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::getDesignVLNV()
//-----------------------------------------------------------------------------
VLNV CreateConfigurationDialog::getDesignVLNV() const
{
	// first get the inputted values
	VLNV designVLNV = getConfigurationVLNV();
	designVLNV.setType(VLNV::DESIGN);

	// remove the possible ".designcfg" from the end
	QString name = designVLNV.getName().remove(QString(".designcfg"));

	// add .design to the end
	designVLNV.setName(name + ".design");
	return designVLNV;
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::onConfNameChanged()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::onConfNameChanged( const QString& newName )
{
	vlnvEdit_->setName(component_->getVlnv().getName() + "." + newName + ".designcfg");
}

//-----------------------------------------------------------------------------
// Function: createconfigurationdialog::designSelection()
//-----------------------------------------------------------------------------
CreateConfigurationDialog::DesignSelection CreateConfigurationDialog::designSelection() const
{
	if (useExistingRadio_->isChecked())
    {
		return CreateConfigurationDialog::USE_EXISTING;
    }
	else if (createNewRadio_->isChecked())
    {
		return CreateConfigurationDialog::CREATE_EMPTY;
    }
	else
    {
		return CreateConfigurationDialog::CREATE_COPY;
    }
}

//-----------------------------------------------------------------------------
// Function: CreateConfigurationDialog::prevalidate()
//-----------------------------------------------------------------------------
void CreateConfigurationDialog::prevalidate()
{
    okButton_->setEnabled(!configNameEdit_->text().isEmpty() && configNameEdit_->isInputValid() &&
                          vlnvEdit_->isValid());
}
