//-----------------------------------------------------------------------------
// File: vlnvdisplayer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.03.2011
//
// Description:
// VLNVDisplayer is a widget to display a vlnv tag in a GUI item.
//-----------------------------------------------------------------------------

#include "vlnvdisplayer.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QSizePolicy>

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::VLNVDisplayer()
//-----------------------------------------------------------------------------
VLNVDisplayer::VLNVDisplayer(QWidget *parent, const VLNV& vlnv, bool compact):
QGroupBox(parent),
vendorLabel_(tr("Vendor:"), this),
libraryLabel_(tr("Library:"), this),
nameLabel_(tr("Name:"), this),
versionLabel_(tr("Version:"), this),
vendor_(this), library_(this),
name_(this),
version_(this),
layout_(0)
{
	layout_ = new QGridLayout();
	layout_->addWidget(&vendorLabel_, 0, 0, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&libraryLabel_, 1, 0, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&nameLabel_, 2, 0, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&versionLabel_, 3, 0, 1, 1, Qt::AlignLeft);

	layout_->addWidget(&vendor_, 0, 1, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&library_, 1, 1, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&name_, 2, 1, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&version_, 3, 1, 1, 1, Qt::AlignLeft);
    layout_->setColumnStretch(2, 1);

    compactLayout_ = new QHBoxLayout();
    compactLayout_->setSpacing(1);
    compactLayout_->addWidget(&vendor_);
    compactLayout_->addWidget(&library_);
    compactLayout_->addWidget(&name_);
    compactLayout_->addWidget(&version_);
    compactLayout_->addStretch(1);

	setVLNV(vlnv, compact);
    setFlat(true);
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::VLNVDisplayer()
//-----------------------------------------------------------------------------
VLNVDisplayer::VLNVDisplayer( QWidget *parent, const VLNV* vlnv, bool compact ):
QGroupBox(parent),
vendorLabel_(tr("Vendor:"), this), 
libraryLabel_(tr("Library:"), this),
nameLabel_(tr("Name:"), this),
versionLabel_(tr("Version:"), this),
vendor_(this), 
library_(this),
name_(this),
version_(this)
{
    layout_ = new QGridLayout();
	layout_->addWidget(&vendorLabel_, 0, 0, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&libraryLabel_, 1, 0, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&nameLabel_, 2, 0, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&versionLabel_, 3, 0, 1, 1, Qt::AlignLeft);

	layout_->addWidget(&vendor_, 0, 1, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&library_, 1, 1, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&name_, 2, 1, 1, 1, Qt::AlignLeft);
	layout_->addWidget(&version_, 3, 1, 1, 1, Qt::AlignLeft);
    layout_->setColumnStretch(2, 1);

    compactLayout_ = new QHBoxLayout();
    compactLayout_->setSpacing(1);
    compactLayout_->addWidget(&vendor_);
    compactLayout_->addWidget(&library_);
    compactLayout_->addWidget(&name_);
    compactLayout_->addWidget(&version_);
    compactLayout_->addStretch(1);

	// if vlnv pointer is valid
	if (vlnv->isValid()) 
    {
		setVLNV(*vlnv, compact);
    }
    else
    {
        setVLNV(VLNV(), compact);
    }

	setFlat(true);

    // this widget can not be expanded
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::VLNVDisplayer()
//-----------------------------------------------------------------------------
VLNVDisplayer::VLNVDisplayer( QWidget* parent ):
QGroupBox(parent),
vendorLabel_(tr("Vendor:"), this),
libraryLabel_(tr("Library:"), this),
nameLabel_(tr("Name:"), this),
versionLabel_(tr("Version:"), this),
vendor_(this),
library_(this),
name_(this),
version_(this)
{
    layout_ = new QGridLayout();
    layout_->addWidget(&vendorLabel_, 0, 0, 1, 1, Qt::AlignLeft);
    layout_->addWidget(&libraryLabel_, 1, 0, 1, 1, Qt::AlignLeft);
    layout_->addWidget(&nameLabel_, 2, 0, 1, 1, Qt::AlignLeft);
    layout_->addWidget(&versionLabel_, 3, 0, 1, 1, Qt::AlignLeft);

    layout_->addWidget(&vendor_, 0, 1, 1, 1, Qt::AlignLeft);
    layout_->addWidget(&library_, 1, 1, 1, 1, Qt::AlignLeft);
    layout_->addWidget(&name_, 2, 1, 1, 1, Qt::AlignLeft);
    layout_->addWidget(&version_, 3, 1, 1, 1, Qt::AlignLeft);
    layout_->setColumnStretch(2, 1);

    compactLayout_ = new QHBoxLayout();
    compactLayout_->setSpacing(1);
    compactLayout_->addWidget(&vendor_);
    compactLayout_->addWidget(&library_);
    compactLayout_->addWidget(&name_);
    compactLayout_->addWidget(&version_);
    compactLayout_->addStretch(1);

    setFlat(true);
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::~VLNVDisplayer()
//-----------------------------------------------------------------------------
VLNVDisplayer::~VLNVDisplayer()
{

}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setVLNV()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setVLNV( const VLNV& vlnv, bool compact )
{
    vendor_.setText(vlnv.getVendor());
    library_.setText(vlnv.getLibrary());
    name_.setText(vlnv.getName());
    version_.setText(vlnv.getVersion());

    if (!vlnv.isValid())
    {
        if (vlnv.getVendor().isEmpty())
        {
            vendor_.setText("UNSET");
        }
        if (vlnv.getLibrary().isEmpty())
        {
            library_.setText("UNSET");
        }
        if (vlnv.getName().isEmpty())
        {
            name_.setText("UNSET");
        }
        if (vlnv.getVersion().isEmpty())
        {
            version_.setText("UNSET");
        }
    }

    if (compact)
    {
        vendor_.setStyleSheet("border: 1px solid grey");
        vendor_.setMinimumWidth(20);
        library_.setStyleSheet("border: 1px solid grey");
        library_.setMinimumWidth(20);
        name_.setStyleSheet("border: 1px solid grey");
        version_.setStyleSheet("border: 1px solid grey");
        
        // Hide the labels.
        vendorLabel_.setVisible(false);
        libraryLabel_.setVisible(false);
        nameLabel_.setVisible(false);
        versionLabel_.setVisible(false);

        // Set the compact layout.
        setLayout(compactLayout_);
    }
    else
    {
        vendor_.setStyleSheet("");
        vendor_.setMinimumWidth(vendor_.sizeHint().width());
        library_.setStyleSheet("");
        library_.setMinimumWidth(library_.sizeHint().width());
        
        library_.setVisible(true);
        version_.setVisible(true);

        // Show the labels.
        vendorLabel_.setVisible(true);
        libraryLabel_.setVisible(true);
        nameLabel_.setVisible(true);
        versionLabel_.setVisible(true);

        // Set the non-compact layout.
        setLayout(layout_);
    }
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setPath()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setPath( const QString& path )
{
	QLabel* pathHeader = new QLabel(tr("Path:"), this);
	QLabel* pathLabel = new QLabel(path, this);
    pathLabel->setMinimumWidth(20);
    pathLabel->setToolTip(path);

	layout_->addWidget(pathHeader, 4, 0, 1, 1, Qt::AlignLeft);
	layout_->addWidget(pathLabel, 4, 1, 1, 1, Qt::AlignLeft);
}
