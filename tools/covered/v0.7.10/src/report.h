#ifndef __REPORT_H__
#define __REPORT_H__

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
 \file     report.h
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     11/29/2001
 \brief    Contains functions for report command.
*/


/*! \brief Parses command-line for report command and performs report functionality. */
void command_report(
  int          argc,
  int          last_arg,
  const char** argv
);

/*! \brief Recursively gathers instance statistics in each instance structure */
void report_gather_instance_stats(
  funit_inst* root
);

/*! \brief Prints header of report. */
void report_print_header(
  FILE* ofile
);

/*! \brief Parses arguments on report command command-line. */
bool report_parse_args(
  int          argc,
  int          last_arg,
  const char** argv
);

/*! \brief Reads the CDD and readies the design for reporting */
void report_read_cdd_and_ready(
  const char* ifile
);

/*! \brief Closes the currently loaded CDD */
void report_close_cdd();

/*! \brief Saves the currently loaded CDD as the specified filename */
void report_save_cdd(
  const char* filename
);

/*! \brief Outputs the given exclusion reason to the given output stream */
void report_output_exclusion_reason(
  FILE*       ofile,
  int         leading_spaces,
  const char* msg,
  bool        header
);

#endif

