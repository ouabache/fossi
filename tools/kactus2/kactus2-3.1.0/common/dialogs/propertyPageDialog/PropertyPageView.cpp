//-----------------------------------------------------------------------------
// File: PropertyPageView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 8.6.2011
//
// Description:
// Base class for property page views.
//-----------------------------------------------------------------------------

#include "PropertyPageView.h"

//-----------------------------------------------------------------------------
// Function: PropertyPageView()
//-----------------------------------------------------------------------------
PropertyPageView::PropertyPageView() : QWidget()
{
}

//-----------------------------------------------------------------------------
// Function: ~PropertyPageView()
//-----------------------------------------------------------------------------
PropertyPageView::~PropertyPageView()
{
}

//-----------------------------------------------------------------------------
// Function: prevalidate()
//-----------------------------------------------------------------------------
bool PropertyPageView::prevalidate() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool PropertyPageView::validate()
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool PropertyPageView::onPageChange()
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void PropertyPageView::apply()
{
}

