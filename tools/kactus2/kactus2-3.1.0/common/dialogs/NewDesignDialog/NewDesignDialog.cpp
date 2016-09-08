//-----------------------------------------------------------------------------
// File: NewDesignDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 2.8.2012
//
// Description:
// Dialog used in creation of new designs to existing components.
//-----------------------------------------------------------------------------

#include "NewDesignDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCoreApplication>

#include <IPXACTmodels/Component/Component.h>

#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>
#include <common/widgets/LineEditEx/LineEditEx.h>
#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::NewDesignDialog()
//-----------------------------------------------------------------------------
NewDesignDialog::NewDesignDialog(LibraryInterface* libInterface,
                                 QSharedPointer<Component> component,
                                 KactusAttribute::Implementation designType,
                                 QWidget *parent)
    : QDialog(parent), 
      lh_(libInterface),
      component_(component),
      viewNameLabel_(new QLabel(tr("View name:"), this)),
      viewNameMatcher_(),
      viewNameEdit_(new LineEditEx(this)),
      vlnvEditor_(new VLNVEditor(VLNV::DESIGN, libInterface, this, this, true)),
      directoryEditor_(new LibrarySelectorWidget(this)), 
      okButton_(new QPushButton(tr("&OK"))),
      designExt_(),
      designConfExt_()
{
    switch (designType)
    {
    case KactusAttribute::HW:
        {
            viewNameLabel_->setText(tr("Name for a new view to HW component:"));
            viewNameEdit_->setDisallowedInputs(component->getViewNames());
            viewNameEdit_->setMessageTemplate("View name '%1' already exists!");
            vlnvEditor_->setTitle("VLNV for new HW design and design configuration");
            designExt_ = ".design";
            designConfExt_ = ".designcfg";
            break;
        }

    case KactusAttribute::SW:
        {
            viewNameLabel_->setText(tr("Name for a new SW view to component:"));
            viewNameEdit_->setDisallowedInputs(component->getSWViewNames());
            viewNameEdit_->setMessageTemplate("SW view name '%1' already exists!");
            vlnvEditor_->setTitle("VLNV for new SW design and design configuration");
            designExt_ = ".swdesign";
            designConfExt_ = ".swdesigncfg";
            break;
        }

    case KactusAttribute::SYSTEM:
        {
            viewNameLabel_->setText(tr("Name for a new system view to HW component:"));
            viewNameEdit_->setDisallowedInputs(component->getSystemViewNames());
            viewNameEdit_->setMessageTemplate("System view name '%1' already exists!");
            vlnvEditor_->setTitle("VLNV for new system design and design configuration");
            designExt_ = ".sysdesign";
            designConfExt_ = ".sysdesigncfg";
            break;
        }

    default:
        {
            Q_ASSERT(false);
            break;
        }
    }

    viewNameEdit_->setMessageIcon(QPixmap(":/icons/common/graphics/exclamation.png"));
    viewNameEdit_->setContentMatcher(&viewNameMatcher_);

    connect(viewNameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(updateVlnvName()));
    connect(viewNameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(onContentChanged()));

    vlnvEditor_->addNameExtension(designExt_);
    vlnvEditor_->addNameExtension(designConfExt_);
    vlnvEditor_->addContentType(VLNV::DESIGNCONFIGURATION);

    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));
        
    directoryEditor_->layout()->setContentsMargins(0,11,0,11);
	connect(directoryEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));

    okButton_->setEnabled(false);
    connect(okButton_, SIGNAL(released()), this, SLOT(accept()));

    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
    connect(cancelButton, SIGNAL(released()), this, SLOT(reject()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(okButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(viewNameLabel_);
    mainLayout->addWidget(viewNameEdit_);
    mainLayout->addWidget(vlnvEditor_);
    mainLayout->addWidget(directoryEditor_);
    mainLayout->addWidget(buttonBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    setWindowTitle("New HW Design");
    setFixedHeight(sizeHint().height());
    resize(400, sizeHint().height());
    okButton_->setDefault(true);
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::setViewName()
//-----------------------------------------------------------------------------
void NewDesignDialog::setViewName(QString const& viewName)
{
    viewNameEdit_->setText(viewName);
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getViewName()
//-----------------------------------------------------------------------------
QString NewDesignDialog::getViewName() const
{
    return viewNameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getDesignVLNV()
//-----------------------------------------------------------------------------
VLNV NewDesignDialog::getDesignVLNV() const
{
    VLNV vlnv = vlnvEditor_->getVLNV();
    vlnv.setType(VLNV::DESIGN);
    vlnv.setName(vlnv.getName().remove(".comp") + designExt_);
    return vlnv;
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getDesignConfVLNV()
//-----------------------------------------------------------------------------
VLNV NewDesignDialog::getDesignConfVLNV() const
{
    VLNV vlnv = vlnvEditor_->getVLNV();
    vlnv.setType(VLNV::DESIGNCONFIGURATION);
    vlnv.setName(vlnv.getName().remove(".comp") + designConfExt_);
    return vlnv;
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::getPath()
//-----------------------------------------------------------------------------
QString NewDesignDialog::getPath() const
{
    return directoryEditor_->getPath();
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void NewDesignDialog::onContentChanged()
{
    // Enable/disable the ok button if the contents are valid/invalid.
    okButton_->setEnabled(!viewNameEdit_->text().isEmpty() && viewNameEdit_->isInputValid() &&
                          directoryEditor_->isValid() && vlnvEditor_->isValid());
}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void NewDesignDialog::accept()
{
    // Make sure that the VLNVs are not already in use.
    if (lh_->contains(getDesignVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("VLNV %1 already exists in the library.").arg(getDesignVLNV().toString()),
                           QMessageBox::Ok, this);
        
        msgBox.exec();
        return;
    }

    if (lh_->contains(getDesignConfVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("VLNV %1 already exists in the library.").arg(getDesignConfVLNV().toString()),
                           QMessageBox::Ok, this);

        msgBox.exec();
        return;
    }

    QDialog::accept();
}

void NewDesignDialog::setVLNV( const VLNV& vlnv ) {
	vlnvEditor_->setVLNV(vlnv);
    updateDirectory();
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewDesignDialog::updateDirectory()
{
    QString vlnvDir;

    VLNV vlnv = vlnvEditor_->getVLNV();

    if (!vlnv.getVendor().isEmpty())
    {
        vlnvDir += "/" + vlnv.getVendor();

        if (!vlnv.getLibrary().isEmpty())
        {
            vlnvDir += "/" + vlnv.getLibrary();

            if (!vlnv.getName().isEmpty())
            {
                vlnvDir += "/" + vlnv.getName();

                if (!vlnv.getVersion().isEmpty())
                {
                    vlnvDir += "/" + vlnv.getVersion();
                }
            }
        }
    }

    directoryEditor_->updatePath(vlnvDir);
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::updateVlnvName()
//-----------------------------------------------------------------------------
void NewDesignDialog::updateVlnvName()
{
    vlnvEditor_->setName(component_->getVlnv().getName().remove(".comp") + "." + getViewName());
}

//-----------------------------------------------------------------------------
// Function: NewDesignDialog::setViewNameSuggestions()
//-----------------------------------------------------------------------------
void NewDesignDialog::setViewNameSuggestions(QStringList const& suggestions)
{
    viewNameMatcher_.setItems(suggestions);
}
