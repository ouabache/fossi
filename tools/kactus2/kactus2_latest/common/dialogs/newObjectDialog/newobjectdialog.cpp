//-----------------------------------------------------------------------------
// File: newobjetdialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// NewObjectDialog is used to query the VLNV and path of a newly created design
//-----------------------------------------------------------------------------

#include "newobjectdialog.h"

#include <common/widgets/kactusAttributeEditor/KactusAttributeEditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: NewObjectDialog()
//-----------------------------------------------------------------------------
NewObjectDialog::NewObjectDialog(LibraryInterface* libInterface, VLNV::IPXactType type, bool showAttributes,
                                 QWidget *parent):
QDialog(parent), 
lh_(libInterface),
attributeEditor_(0), 
vlnvEditor_(0),
directoryEditor_(0), 
okButton_(0)
{
    attributeEditor_ = new KactusAttributeEditor(this);
    attributeEditor_->setVisible(showAttributes);
    connect(attributeEditor_, SIGNAL(productHierarchyChanged()), this, SLOT(onProductHierarchyChanged()));

    vlnvEditor_ = new VLNVEditor(type, libInterface, this, this, true);
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(vlnvEditor_, SIGNAL(contentChanged()), this, SLOT(updateDirectory()));

    directoryEditor_ = new LibrarySelectorWidget(this);
    directoryEditor_->layout()->setContentsMargins(0,0,0,0);
	connect(directoryEditor_, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));

    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    okButton_ = new QPushButton(tr("&OK"));
    okButton_->setEnabled(false);
    connect(okButton_, SIGNAL(released()), this, SLOT(accept()));

    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
    connect(cancelButton, SIGNAL(released()), this, SLOT(reject()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(okButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(attributeEditor_);
    mainLayout->addWidget(vlnvEditor_);
    mainLayout->addWidget(directoryEditor_);
    mainLayout->addWidget(separator);
    mainLayout->addWidget(buttonBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    setWindowTitle("New HW Design");
    setFixedHeight(sizeHint().height());
    resize(400, sizeHint().height());

    okButton_->setDefault(true);

    onProductHierarchyChanged();
}

//-----------------------------------------------------------------------------
// Function: getVLNV()
//-----------------------------------------------------------------------------
VLNV NewObjectDialog::getVLNV()
{
    return vlnvEditor_->getVLNV();
}

//-----------------------------------------------------------------------------
// Function: getPath()
//-----------------------------------------------------------------------------
QString NewObjectDialog::getPath()
{
    return directoryEditor_->getPath();
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void NewObjectDialog::onContentChanged()
{
    // Enable/disable the ok button if the contents are valid/invalid.
    okButton_->setEnabled(directoryEditor_->isValid() && vlnvEditor_->isValid());
}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void NewObjectDialog::accept()
{
    // Make sure that the VLNV is not already in use.
    if (lh_->contains(vlnvEditor_->getVLNV()))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                           tr("The design cannot be created because the VLNV already exists."),
                           QMessageBox::Ok, this);
        
        msgBox.exec();
        return;
    }

    QDialog::accept();
}

void NewObjectDialog::setVLNV( const VLNV& vlnv ) {
	vlnvEditor_->setVLNV(vlnv);
    updateDirectory();
}

//-----------------------------------------------------------------------------
// Function: setKactusAttributes()
//-----------------------------------------------------------------------------
void NewObjectDialog::setKactusAttributes(KactusAttribute::ProductHierarchy prodHier,
                                          KactusAttribute::Firmness firmness)
{
    attributeEditor_->setAttributes(prodHier, firmness);
}

//-----------------------------------------------------------------------------
// Function: getProductHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy NewObjectDialog::getProductHierarchy() const
{
    return attributeEditor_->getProductHierarchy();
}

//-----------------------------------------------------------------------------
// Function: getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness NewObjectDialog::getFirmness() const
{
    return attributeEditor_->getFirmness();
}

//-----------------------------------------------------------------------------
// Function: saveAsDialog()
//-----------------------------------------------------------------------------
bool NewObjectDialog::saveAsDialog(QWidget* parent, LibraryInterface* lh, VLNV const& oldVLNV,
                                   KactusAttribute::ProductHierarchy& prodHier,
                                   KactusAttribute::Firmness& firmness,
                                   VLNV& vlnv, QString& directory)
{
    // Create a suggestion for the new vlnv without the version number.
    VLNV suggestion = oldVLNV;
    suggestion.setVersion(QString());

    NewObjectDialog dialog(lh, oldVLNV.getType(), oldVLNV.getType() != VLNV::BUSDEFINITION, parent);
    dialog.setWindowTitle(tr("Save As"));
    dialog.setVLNV(suggestion);
    dialog.setKactusAttributes(prodHier, firmness);

    if (dialog.exec() == QDialog::Rejected)
    {
        return false;
    }

    prodHier = dialog.getProductHierarchy();
    firmness = dialog.getFirmness();
    vlnv = dialog.getVLNV();
    directory = dialog.getPath();
    return true;
}

//-----------------------------------------------------------------------------
// Function: saveAsDialog()
//-----------------------------------------------------------------------------
bool NewObjectDialog::saveAsDialog(QWidget* parent, LibraryInterface* lh,
                                   VLNV const& oldVLNV, VLNV& vlnv, QString& directory,
								   const QString& windowTitle)
{
    // Create a suggestion for the new vlnv without the version number.
    VLNV suggestion = oldVLNV;
    suggestion.setVersion(QString());

    NewObjectDialog dialog(lh, oldVLNV.getType(), false, parent);
    dialog.setWindowTitle(windowTitle);
    dialog.setVLNV(suggestion);

    if (dialog.exec() == QDialog::Rejected)
    {
        return false;
    }

    vlnv = dialog.getVLNV();
    directory = dialog.getPath();
    return true;
}

//-----------------------------------------------------------------------------
// Function: updateDirectory()
//-----------------------------------------------------------------------------
void NewObjectDialog::updateDirectory()
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
// Function: onProductHierarchyChanged()
//-----------------------------------------------------------------------------
void NewObjectDialog::onProductHierarchyChanged()
{
    // Update the VLNV's library field if it is either empty or any of the predefined ones.
    VLNV vlnv = vlnvEditor_->getVLNV();

    if (vlnv.getLibrary().isEmpty())
    {
        vlnv.setLibrary(KactusAttribute::hierarchyToString(attributeEditor_->getProductHierarchy()).toLower());
    }
    else
    {
        for (unsigned int i = 0; i < KactusAttribute::KTS_PRODHIER_COUNT; ++i)
        {
            if (vlnv.getLibrary().toLower() ==
                KactusAttribute::hierarchyToString(static_cast<KactusAttribute::ProductHierarchy>(i)).toLower())
            {
                vlnv.setLibrary(KactusAttribute::hierarchyToString(attributeEditor_->getProductHierarchy()).toLower());
                break;
            }
        }
    }

    vlnvEditor_->setVLNV(vlnv);
}