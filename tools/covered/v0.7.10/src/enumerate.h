#ifndef __ENUMERATE_H__
#define __ENUMERATE_H__

/*
 Copyright (c) 2006-2010 Trevor Williams

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program;
 if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*!
 \file     enumerate.h
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     8/29/2006
 \brief    Contains functions for handling enumerations.
*/

#include "defines.h"


/*! \brief Allocates, initializes and adds a new enumerated item to the given functional unit */
void enumerate_add_item( vsignal* enum_sig, static_expr* value, func_unit* funit );

/*! \brief Sets the last status of the last item in the enumerated list */
void enumerate_end_list( func_unit* funit );

/*! \brief Resolves all enumerations within the given functional unit instance */
void enumerate_resolve( funit_inst* inst );

/*! \brief Deallocates all memory associated with the given enumeration. */
void enumerate_dealloc( enum_item* ei );

/*! \brief Deallocates enumeration list from given functional unit */
void enumerate_dealloc_list( func_unit* funit );


/*
 $Log: enumerate.h,v $
 Revision 1.4.16.2  2010/01/04 22:05:06  phase1geo
 Updating copyright dates to include 2010.  Updating files for 0.7.8 stable
 release (this won't be happening for a bit).

 Revision 1.4.16.1  2009/04/23 16:49:04  phase1geo
 Fixed a few more user documentation issues and updated the copyright dates in
 all source code files.

 Revision 1.4  2007/11/20 05:28:58  phase1geo
 Updating e-mail address from trevorw@charter.net to phase1geo@gmail.com.

 Revision 1.3  2007/09/14 06:22:12  phase1geo
 Filling in existing functions in struct_union.  Completed parser code for handling
 struct/union declarations.  Code compiles thus far.

 Revision 1.2  2006/10/06 22:45:57  phase1geo
 Added support for the wait() statement.  Added wait1 diagnostic to regression
 suite to verify its behavior.  Also added missing GPL license note at the top
 of several *.h and *.c files that are somewhat new.

 Revision 1.1  2006/08/29 22:49:31  phase1geo
 Added enumeration support and partial support for typedefs.  Added enum1
 diagnostic to verify initial enumeration support.  Full regression has not
 been run at this point -- checkpointing.

*/

#endif

