//-----------------------------------------------------------------------------
// File: vlnvdialer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// VLNVDialer is used to set options that specify what objects to display in library.
//-----------------------------------------------------------------------------

#include "vlnvdialer.h"

#include <QVBoxLayout>
#include <QIcon>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: VLNVDialer::VLNVDialer()
//-----------------------------------------------------------------------------
VLNVDialer::VLNVDialer(QWidget *parent):
QWidget(parent),
    filters_(this),
    dialer_(this),
    hideButton_(QIcon(":/icons/common/graphics/triangle_arrow_up.png"), QString(), this),
    hidden_(false)
{
	// set visual options for the hide/show button
	hideButton_.setFlat(true);
	hideButton_.setMaximumHeight(10);

	// check if the filters were visible or not previously
	QSettings settings;
	hidden_ = !settings.value("FilterWidget/Hidden", false).toBool();
	onHideShowClick();

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&filters_, 0);
	layout->addWidget(&hideButton_, 0);
	layout->addWidget(&dialer_, 1);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 4);

	connect(&hideButton_, SIGNAL(clicked(bool)), this, SLOT(onHideShowClick()), Qt::UniqueConnection);

	connect(&dialer_, SIGNAL(vendorChanged(const QString&)),
		this, SIGNAL(vendorChanged(const QString&)), Qt::UniqueConnection);
	connect(&dialer_, SIGNAL(libraryChanged(const QString&)),
		this, SIGNAL(libraryChanged(const QString&)), Qt::UniqueConnection);
	connect(&dialer_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&dialer_, SIGNAL(versionChanged(const QString&)),
		this, SIGNAL(versionChanged(const QString&)), Qt::UniqueConnection);

	connect(&filters_, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)),
		this, SIGNAL(firmnessChanged(const Utils::FirmnessOptions&)), Qt::UniqueConnection);
	connect(&filters_, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)),
		this, SIGNAL(implementationChanged(const Utils::ImplementationOptions&)), Qt::UniqueConnection);
	connect(&filters_, SIGNAL(typeChanged(const Utils::TypeOptions&)),
		this, SIGNAL(typeChanged(const Utils::TypeOptions&)), Qt::UniqueConnection);
	connect(&filters_, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)),
		this, SIGNAL(hierarchyChanged(const Utils::HierarchyOptions&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::~VLNVDialer()
//-----------------------------------------------------------------------------
VLNVDialer::~VLNVDialer()
{
	// save the visibility of the filter widget
	QSettings settings;
	settings.setValue("FilterWidget/Hidden", hidden_);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::setFilters()
//-----------------------------------------------------------------------------
void VLNVDialer::setFilters(FilterWidget::FilterOptions options)
{
    filters_.setFilters(options);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::getFilters()
//-----------------------------------------------------------------------------
FilterWidget::FilterOptions VLNVDialer::getFilters() const
{
    return filters_.getFilters();
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::onHideShowClick()
//-----------------------------------------------------------------------------
void VLNVDialer::onHideShowClick()
{
	// if filters were hidden
	if (hidden_)
    {
		hideButton_.setIcon(QIcon(":/icons/common/graphics/triangle_arrow_up.png"));
	}
	// if filters were visible
	else
    {
		hideButton_.setIcon(QIcon(":/icons/common/graphics/triangle_arrow_down.png"));
	}

	// change the hidden value
	hidden_ = !hidden_;
    filters_.setVisible(!hidden_);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::setRootItem()
//-----------------------------------------------------------------------------
void VLNVDialer::setRootItem(const LibraryItem* rootItem )
{
	dialer_.setRootItem(rootItem);
}

//-----------------------------------------------------------------------------
// Function: VLNVDialer::refreshLibrary()
//-----------------------------------------------------------------------------
void VLNVDialer::refreshLibrary()
{
	dialer_.refreshVendors();
}
