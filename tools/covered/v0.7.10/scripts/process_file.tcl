################################################################################################
# Copyright (c) 2006-2010 Trevor Williams                                                      #
#                                                                                              #
# This program is free software; you can redistribute it and/or modify                         #
# it under the terms of the GNU General Public License as published by the Free Software       #
# Foundation; either version 2 of the License, or (at your option) any later version.          #
#                                                                                              #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;    #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    #
# See the GNU General Public License for more details.                                         #
#                                                                                              #
# You should have received a copy of the GNU General Public License along with this program;   #
# if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. #
################################################################################################

# This proc will take in a file, the list of uncovered lines and the
# the corresponding uncovered lines will be highlighted in the window

# SPECIAL NOTE : In order to avoid multiple re-read of the same file, it
# is much better to read in a file at the very first time when the
# corresponding tree-node is selected. The file contents are kept as 
# part of the global filecontent hashtable.

set fileContent(0)          0
set start_line              0
set end_line                0
set line_summary_hit        0
set line_summary_excluded   0
set line_summary_total      0
set toggle_summary_hit      0
set toggle_summary_excluded 0 
set toggle_summary_total    0
set memory_summary_hit      0
set memory_summary_excluded 0
set memory_summary_total    0
set comb_summary_hit        0
set comb_summary_excluded   0
set comb_summary_total      0
set fsm_summary_hit         0
set fsm_summary_excluded    0
set fsm_summary_total       0
set assert_summary_hit      0
set assert_summary_excluded 0
set assert_summary_total    0
set curr_block              0 
set line_excluded           0
set line_reason             ""

# TODO : 
# Suppose that a really large verilog file has a lot of lines uncovered.
# Obviously, if we use a list and lsearch then the time it will take to
# print the total listing will be a bit too-much. As of now, we are 
# using lsearch, but will certainly optimize later.

proc get_race_reason_from_start_line {start_line} {

  global race_info race_msgs race_lines

  set i 0
  while {[expr $i < [llength $race_lines]] && [expr [string compare [lindex [lindex $race_lines $i] 0] $start_line] == 0]} {
    incr i
  }

  return [lindex $race_msgs [lindex [lindex $race_info $i] 1]]

}

proc create_race_tags {} {

  global race_type race_info
  global race_fgColor race_bgColor

  # Set race condition information
  if {[expr $race_type == 1] && [expr [llength $race_info] > 0]} {
    set cmd_enter ".bot.right.txt tag add race_enter"
    set cmd_leave ".bot.right.txt tag add race_leave"
    foreach entry $race_info {
      set cmd_enter [concat $cmd_enter [lindex $entry 0]]
      set cmd_leave [concat $cmd_leave [lindex $entry 0]]
    }
    eval $cmd_enter
    eval $cmd_leave
    .bot.right.txt tag configure race_enter -foreground $race_fgColor -background $race_bgColor
    .bot.right.txt tag bind race_enter <Enter> {
      set curr_info   [.info cget -text]
      set curr_cursor [.bot.right.txt cget -cursor]
      .bot.right.txt configure -cursor question_arrow
      set reason [get_race_reason_from_start_line [lindex [.bot.right.txt tag prevrange race_enter {current + 1 chars}] 0]]
      .info configure -text "Race condition reason: $reason"
    }
    .bot.right.txt tag bind race_leave <Leave> {
      .bot.right.txt configure -cursor $curr_cursor
      .info          configure -text   $curr_info
    }
  }

}

#-----------  LINE COVERAGE  -------------------------------------------------

proc process_line_cov {} {

  global start_line end_line curr_block
  global line_summary_hit line_summary_total

  if {$curr_block != 0} {

    # Get the filename of the currently selected functional unit and read it in
    set file_name [tcl_func_get_filename $curr_block]

    if {[load_verilog $file_name 1]} {

      # Get start and end line numbers of this functional unit
      set line_pos   [tcl_func_get_funit_start_and_end $curr_block]
      set start_line [lindex $line_pos 0]
      set end_line   [lindex $line_pos 1]

      # Get line summary information and display this now
      tcl_func_get_line_summary $curr_block

      # If we have some uncovered values, enable the "next" pointer and menu item
      if {$line_summary_total != $line_summary_hit} {
        .bot.right.h.pn.next configure -state normal
        .menubar.view entryconfigure 0 -state normal
      } else {
        .bot.right.h.pn.next configure -state disabled
        .menubar.view entryconfigure 0 -state disabled
      }
      .bot.right.h.pn.prev configure -state disabled
      .menubar.view entryconfigure 1 -state disabled

      calc_and_display_line_cov

    }

  }

}

proc calc_and_display_line_cov {} {

  global uncovered_lines covered_lines race_info
  global curr_block start_line

  if {$curr_block != 0} {

    # Get list of uncovered/covered lines
    set uncovered_lines [tcl_func_collect_uncovered_lines $curr_block]
    set covered_lines   [tcl_func_collect_covered_lines   $curr_block]
    set race_info       [tcl_func_collect_race_lines      $curr_block $start_line]

    display_line_cov

  }

}

proc display_line_cov {} {

  global fileContent
  global uncov_fgColor uncov_bgColor
  global cov_fgColor cov_bgColor
  global uncovered_lines covered_lines
  global uncov_type cov_type
  global start_line end_line
  global line_summary_total line_summary_hit
  global curr_block

  if {$curr_block != 0} {

    set file_name [tcl_func_get_filename $curr_block]

    # Populate information bar
    if {$file_name != ""} {
      .info configure -text "Filename: $file_name"
    }

    .bot.right.txt tag configure uncov_colorMap -foreground $uncov_fgColor -background $uncov_bgColor
    .bot.right.txt tag configure cov_colorMap   -foreground $cov_fgColor   -background $cov_bgColor

    # Allow us to write to the text box
    .bot.right.txt configure -state normal

    # Clear the text-box before any insertion is being made
    .bot.right.txt delete 1.0 end

    set contents [split $fileContent($file_name) \n]
    set linecount 1

    if {$end_line != 0} {

      # Next, populate text box with file contents including highlights for covered/uncovered lines
      foreach phrase $contents {
        if [expr [expr $start_line <= $linecount] && [expr $end_line >= $linecount]] {
          set line [format {%3s  %7u  %s} "   " $linecount [append phrase "\n"]]
          set uncov_index [lsearch -index 0 $uncovered_lines $linecount]
          if {[expr $uncov_type == 1] && [expr $uncov_index != -1]} {
            if {[lindex [lindex $uncovered_lines $uncov_index] 1] == 0} {
              set line [string replace $line 1 1 "I"]
              .bot.right.txt insert end $line uncov_colorMap
            } else {
              set line [string replace $line 1 1 "E"]
              .bot.right.txt insert end $line cov_colorMap
            }
          } elseif {[expr $cov_type == 1] && [expr [lsearch -index 0 $covered_lines $linecount] != -1]} {
            .bot.right.txt insert end $line cov_colorMap
          } else {
            .bot.right.txt insert end $line
          }
        }
        incr linecount
      }

      # Perform syntax highlighting
      verilog_highlight .bot.right.txt

      # Create race condition tags
      create_race_tags

      # Finally, set line information
      if {[expr $uncov_type == 1] && [expr [llength $uncovered_lines] > 0]} {
        set cmd_enter  ".bot.right.txt tag add uncov_enter"
        set cmd_button ".bot.right.txt tag add uncov_button"
        set cmd_leave  ".bot.right.txt tag add uncov_leave"
        foreach entry $uncovered_lines {
          set tb_line [expr ([lindex $entry 0] - $start_line) + 1]
          set cmd_enter  [concat $cmd_enter  "$tb_line.1 $tb_line.2"]
          set cmd_button [concat $cmd_button "$tb_line.1 $tb_line.2"]
          set cmd_leave  [concat $cmd_leave  "$tb_line.1 $tb_line.2"]
        }
        eval $cmd_enter
        eval $cmd_button
        eval $cmd_leave
        .bot.right.txt tag configure uncov_button -underline true
        .bot.right.txt tag bind uncov_enter <Enter> {
          set curr_cursor [.bot.right.txt cget -cursor]
          set curr_info   [.info cget -text]
          .bot.right.txt configure -cursor hand2
          .info configure -text "Click left button to exclude/include line"
        }
        .bot.right.txt tag bind uncov_leave <Leave> {
          .bot.right.txt configure -cursor $curr_cursor
          .info configure -text $curr_info
        }
        .bot.right.txt tag bind uncov_button <ButtonPress-1> {
          set selected_line [expr [lindex [split [.bot.right.txt index current] .] 0] + ($start_line - 1)]
          if {[.bot.right.txt get current] == "E"} {
            set excl_value 0
          } else {
            set excl_value 1
          }
          set line_reason ""
          if {$exclude_reasons_enabled == 1 && $excl_value == 1} {
            set line_reason [get_exclude_reason .]
          }
          tcl_func_set_line_exclude $curr_block $selected_line $excl_value $line_reason
          set text_x [.bot.right.txt xview]
          set text_y [.bot.right.txt yview]
          process_line_cov
          .bot.right.txt xview moveto [lindex $text_x 0]
          .bot.right.txt yview moveto [lindex $text_y 0]
          populate_listbox
          enable_cdd_save
        }
        .bot.right.txt tag bind uncov_button <ButtonPress-3> {
          set selected_line [expr [lindex [split [.bot.right.txt index current] .] 0] + ($start_line - 1)]
          set entry [lsearch -index 0 -inline $uncovered_lines $selected_line]
          if {$entry != ""} {
            set line_excluded [lindex $entry 1]
            set line_reason   [lindex $entry 2]
            if {$line_excluded == 1 && $line_reason != ""} {
              balloon::show .bot.right.txt "Exclude Reason: $line_reason" $cov_bgColor $cov_fgColor
            }
          } else {
            set entry [lsearch -index 0 -inline $covered_lines $selected_line]
            set line_excluded [lindex $entry 1]
            set line_reason   [lindex $entry 2]
            if {$line_excluded == 1 && $line_reason != ""} {
              balloon::show .bot.right.txt "Exclude Reason: $line_reason" $cov_bgColor $cov_fgColor
            }
          }
        }
        .bot.right.txt tag bind uncov_button <ButtonRelease-3> {
          set selected_line [expr [lindex [split [.bot.right.txt index current] .] 0] + ($start_line - 1)]
          set entry [lsearch -index 0 -inline $uncovered_lines $selected_line]
          if {$entry != ""} {
            set line_excluded [lindex $entry 1]
            set line_reason   [lindex $entry 2]
            if {$line_excluded == 1 && $line_reason != ""} {
              balloon::hide .bot.right.txt
            }
          } else {
            set entry [lsearch -index 0 -inline $covered_lines $selected_line]
            set line_excluded [lindex $entry 1]
            set line_reason   [lindex $entry 2]
            if {$line_excluded == 1 && $line_reason != ""} {
              balloon::hide .bot.right.txt
            }
          }
        }

      }

    }

    # Now cause the text box to be read-only again
    .bot.right.txt configure -state disabled

  }

  return

}

#-----------  TOGGLE COVERAGE  -----------------------------------------------

proc process_toggle_cov {} {

  global fileContent start_line end_line
  global curr_block
  global toggle_summary_hit toggle_summary_total

  if {$curr_block != 0} {

    # Get the file name of the currently selected functional unit and read it in
    set file_name [tcl_func_get_filename $curr_block]

    if {[load_verilog $file_name 1]} {

      # Get start and end line numbers of this functional unit
      set line_pos   [tcl_func_get_funit_start_and_end $curr_block]
      set start_line [lindex $line_pos 0]
      set end_line   [lindex $line_pos 1]

      # Get line summary information and display this now
      tcl_func_get_toggle_summary $curr_block

      # If we have some uncovered values, enable the "next" pointer
      if {$toggle_summary_total != $toggle_summary_hit} {
        .bot.right.h.pn.next configure -state normal
      } else {
        .bot.right.h.pn.next configure -state disabled
      }
      .bot.right.h.pn.prev configure -state disabled

      calc_and_display_toggle_cov

    }

  }

}

proc calc_and_display_toggle_cov {} {

  global uncovered_toggles covered_toggles race_info
  global curr_block start_line

  if {$curr_block != 0} {

    # Get list of uncovered/covered lines
    set uncovered_toggles [tcl_func_collect_uncovered_toggles $curr_block $start_line]
    set covered_toggles   [tcl_func_collect_covered_toggles   $curr_block $start_line]
    set race_info         [tcl_func_collect_race_lines        $curr_block $start_line]

    display_toggle_cov

  }

}

proc display_toggle_cov {} {

  global fileContent
  global uncov_fgColor uncov_bgColor
  global cov_fgColor cov_bgColor
  global uncovered_toggles covered_toggles
  global uncov_type cov_type
  global start_line end_line
  global toggle_summary_total toggle_summary_hit
  global mod_inst_type
  global toggle01_verbose toggle10_verbose toggle_width
  global curr_block

  if {$curr_block != 0} {

    set file_name [tcl_func_get_filename $curr_block]

    # Populate information bar
    .info configure -text "Filename: $file_name"

    # Allow us to write to the text box
    .bot.right.txt configure -state normal

    # Clear the text-box before any insertion is being made
    .bot.right.txt delete 1.0 end

    set contents [split $fileContent($file_name) \n]
    set linecount 1

    if {$end_line != 0} {

      # Next, populate text box with file contents
      foreach phrase $contents {
        if [expr [expr $start_line <= $linecount] && [expr $end_line >= $linecount]] {
          set line [format {%3s  %7u  %s} "   " $linecount [append phrase "\n"]]
          .bot.right.txt insert end $line
        }
        incr linecount
      }

      # Perform syntax highlighting
      verilog_highlight .bot.right.txt

      # Create race condition tags
      create_race_tags

      # Finally, set toggle information
      if {[expr $uncov_type == 1] && [expr [llength $uncovered_toggles] > 0]} {
        set cmd_enter      ".bot.right.txt tag add uncov_enter"
        set cmd_button     ".bot.right.txt tag add uncov_button"
        set cmd_leave      ".bot.right.txt tag add uncov_leave"
        set cmd_ucov_uline ".bot.right.txt tag add uncov_uline"
        set cmd_excl_uline ".bot.right.txt tag add excl_uline"
        foreach entry $uncovered_toggles {
          set cmd_enter  [concat $cmd_enter  [lindex $entry 0]]
          set cmd_button [concat $cmd_button [lindex $entry 0]]
          set cmd_leave  [concat $cmd_leave  [lindex $entry 0]]
          if {[lindex $entry 1] == 0} {
            set cmd_ucov_uline [concat $cmd_ucov_uline [lindex $entry 0]]
          } else {
            set cmd_excl_uline [concat $cmd_excl_uline [lindex $entry 0]]
          } 
        }
        eval $cmd_enter
        eval $cmd_button
        eval $cmd_leave
        if {[llength $cmd_ucov_uline] > 4} {
          eval $cmd_ucov_uline
          .bot.right.txt tag configure uncov_uline -underline true -foreground $uncov_fgColor -background $uncov_bgColor
        }
        if {[llength $cmd_excl_uline] > 4} {
          eval $cmd_excl_uline
          .bot.right.txt tag configure excl_uline  -underline true -foreground $cov_fgColor   -background $cov_bgColor
        }
        .bot.right.txt tag bind uncov_enter <Enter> {
          set curr_cursor [.bot.right.txt cget -cursor]
          set curr_info   [.info cget -text]
          .bot.right.txt configure -cursor hand2
          .info configure -text "Click left button for detailed toggle coverage information"
        }
        .bot.right.txt tag bind uncov_leave <Leave> {
          .bot.right.txt configure -cursor $curr_cursor
          .info configure -text $curr_info
        }
        .bot.right.txt tag bind uncov_button <ButtonPress-1> {
          display_toggle current
        }
      } 

      if {[expr $cov_type == 1] && [expr [llength $covered_toggles] > 0]} {
        set cmd_cov ".bot.right.txt tag add cov_highlight"
        foreach entry $covered_toggles {
          set cmd_cov [concat $cmd_cov [lindex $entry 0]]
        }
        eval $cmd_cov
        .bot.right.txt tag configure cov_highlight -foreground $cov_fgColor -background $cov_bgColor
      }

    }

    # Now cause the text box to be read-only again
    .bot.right.txt configure -state disabled

  }

  return

}

#-----------  MEMORY COVERAGE  -----------------------------------------------

proc process_memory_cov {} {

  global fileContent start_line end_line
  global curr_block
  global memory_summary_hit memory_summary_total

  if {$curr_block != 0} {

    # Get the file name of the currently selected functional unit and read it in
    set file_name [tcl_func_get_filename $curr_block]

    if {[load_verilog $file_name 1]} {

      # Get start and end line numbers of this functional unit
      set line_pos   [tcl_func_get_funit_start_and_end $curr_block]
      set start_line [lindex $line_pos 0]
      set end_line   [lindex $line_pos 1]

      # Get line summary information and display this now
      tcl_func_get_memory_summary $curr_block

      # If we have some uncovered values, enable the "next" pointer
      if {$memory_summary_total != $memory_summary_hit} {
        .bot.right.h.pn.next configure -state normal
      } else {
        .bot.right.h.pn.next configure -state disabled
      }
      .bot.right.h.pn.prev configure -state disabled

      calc_and_display_memory_cov

    }

  }

}

proc calc_and_display_memory_cov {} {

  global uncovered_memories covered_memories race_info
  global curr_block start_line

  if {$curr_block != 0} {

    # Get list of uncovered/covered memories
    set uncovered_memories [tcl_func_collect_uncovered_memories $curr_block $start_line]
    set covered_memories   [tcl_func_collect_covered_memories   $curr_block $start_line]
    set race_info          [tcl_func_collect_race_lines         $curr_block $start_line]

    display_memory_cov

  }

}

proc display_memory_cov {} {

  global fileContent
  global uncov_fgColor uncov_bgColor
  global cov_fgColor cov_bgColor
  global uncovered_memories covered_memories
  global uncov_type cov_type
  global start_line end_line
  global memory_summary_total memory_summary_hit
  global mod_inst_type
  global memory01_verbose memory10_verbose memory_width
  global curr_block

  if {$curr_block != 0} {

    set file_name [tcl_func_get_filename $curr_block]

    # Populate information bar
    .info configure -text "Filename: $file_name"

    # Allow us to write to the text box
    .bot.right.txt configure -state normal

    # Clear the text-box before any insertion is being made
    .bot.right.txt delete 1.0 end

    set contents [split $fileContent($file_name) \n]
    set linecount 1

    if {$end_line != 0} {

      # Next, populate text box with file contents
      foreach phrase $contents {
        if [expr [expr $start_line <= $linecount] && [expr $end_line >= $linecount]] {
          set line [format {%3s  %7u  %s} "   " $linecount [append phrase "\n"]]
          .bot.right.txt insert end $line
        }
        incr linecount
      }

      # Perform syntax highlighting
      verilog_highlight .bot.right.txt

      # Create race condition tags
      create_race_tags

      # Finally, set memory information
      if {[expr $uncov_type == 1] && [expr [llength $uncovered_memories] > 0]} {
        set cmd_enter      ".bot.right.txt tag add uncov_enter"
        set cmd_button     ".bot.right.txt tag add uncov_button"
        set cmd_leave      ".bot.right.txt tag add uncov_leave"
        set cmd_ucov_uline ".bot.right.txt tag add uncov_uline"
        set cmd_excl_uline ".bot.right.txt tag add excl_uline"
        foreach entry $uncovered_memories {
          set cmd_enter  [concat $cmd_enter  [lindex $entry 0] [lindex $entry 1]]
          set cmd_button [concat $cmd_button [lindex $entry 0] [lindex $entry 1]]
          set cmd_leave  [concat $cmd_leave  [lindex $entry 0] [lindex $entry 1]]
          if {[lindex $entry 2] == 0} {
            set cmd_ucov_uline [concat $cmd_ucov_uline [lindex $entry 0] [lindex $entry 1]]
          } else {
            set cmd_excl_uline [concat $cmd_excl_uline [lindex $entry 0] [lindex $entry 1]]
          } 
        }
        eval $cmd_enter
        eval $cmd_button
        eval $cmd_leave
        if {[llength $cmd_ucov_uline] > 4} {
          eval $cmd_ucov_uline
          .bot.right.txt tag configure uncov_uline -underline true -foreground $uncov_fgColor -background $uncov_bgColor
        }
        if {[llength $cmd_excl_uline] > 4} {
          eval $cmd_excl_uline
          .bot.right.txt tag configure excl_uline  -underline true -foreground $cov_fgColor   -background $cov_bgColor
        }
        .bot.right.txt tag bind uncov_enter <Enter> {
          set curr_cursor [.bot.right.txt cget -cursor]
          set curr_info   [.info cget -text]
          .bot.right.txt configure -cursor hand2
          .info configure -text "Click left button for detailed memory coverage information"
        }
        .bot.right.txt tag bind uncov_leave <Leave> {
          .bot.right.txt configure -cursor $curr_cursor
          .info configure -text $curr_info
        }
        .bot.right.txt tag bind uncov_button <ButtonPress-1> {
          display_memory current
        }
      } 

      if {[expr $cov_type == 1] && [expr [llength $covered_memories] > 0]} {
        set cmd_cov ".bot.right.txt tag add cov_highlight"
        foreach entry $covered_memories {
          set cmd_cov [concat $cmd_cov [lindex $entry 0] [lindex $entry 1]]
        }
        eval $cmd_cov
        .bot.right.txt tag configure cov_highlight -foreground $cov_fgColor -background $cov_bgColor
      }

    }

    # Now cause the text box to be read-only again
    .bot.right.txt configure -state disabled

  }

  return

}

#-----------  COMBINATIONAL LOGIC COVERAGE  ----------------------------------
 
proc process_comb_cov {} {

  global fileContent start_line end_line
  global curr_block
  global comb_summary_total comb_summary_hit

  if {$curr_block != 0} {

    # Get the filename of the currently selected functional unit and read it in
    set file_name [tcl_func_get_filename $curr_block]

    if {[load_verilog $file_name 1]} {

      # Get start and end line numbers of this functional unit
      set line_pos   [tcl_func_get_funit_start_and_end $curr_block]
      set start_line [lindex $line_pos 0]
      set end_line   [lindex $line_pos 1]

      # Get line summary information and display this now
      tcl_func_get_comb_summary $curr_block

      # If we have found uncovered combinations in this module, enable the next button
      if {$comb_summary_total != $comb_summary_hit} {
        .bot.right.h.pn.next configure -state normal
      } else {
        .bot.right.h.pn.next configure -state disabled
      }
      .bot.right.h.pn.prev configure -state disabled

      calc_and_display_comb_cov

    }

  }

} 

proc calc_and_display_comb_cov {} {

  global uncovered_combs covered_combs race_info
  global curr_block start_line

  if {$curr_block != 0} {

    # Get list of uncovered/covered combinational logic 
    set uncovered_combs [tcl_func_collect_uncovered_combs $curr_block $start_line]
    set covered_combs   [tcl_func_collect_covered_combs   $curr_block $start_line]
    set race_info       [tcl_func_collect_race_lines      $curr_block $start_line]

    display_comb_cov

  }

}

proc display_comb_cov {} {
 
  global fileContent
  global uncov_fgColor uncov_bgColor
  global cov_fgColor cov_bgColor
  global uncovered_combs covered_combs race_lines
  global uncov_type cov_type
  global start_line end_line
  global comb_summary_total comb_summary_hit
  global mod_inst_type
  global curr_block

  if {$curr_block != 0} {

    set file_name [tcl_func_get_filename $curr_block]

    # Populate information bar
    .info configure -text "Filename: $file_name"

    # Allow us to write to the text box
    .bot.right.txt configure -state normal

    # Clear the text-box before any insertion is being made
    .bot.right.txt delete 1.0 end

    set contents [split $fileContent($file_name) \n]
    set linecount 1

    if {$end_line != 0} {

      # Next, populate text box with file contents including highlights for covered/uncovered lines
      foreach phrase $contents {
        if [expr [expr $start_line <= $linecount] && [expr $end_line >= $linecount]] {
          set line [format {%3s  %7u  %s} "   " $linecount [append phrase "\n"]]
          .bot.right.txt insert end $line
        }
        incr linecount
      }

      # Perform syntax highlighting
      verilog_highlight .bot.right.txt

      # Create race condition tags
      create_race_tags

      # Finally, set combinational logic information
      if {[expr $uncov_type == 1] && [expr [llength $uncovered_combs] > 0]} {
        set cmd_enter   ".bot.right.txt tag add uncov_enter"
        set cmd_button  ".bot.right.txt tag add uncov_button"
        set cmd_leave   ".bot.right.txt tag add uncov_leave"
        set cmd_ucov_hl ".bot.right.txt tag add uncov_highlight"
        set cmd_excl_hl ".bot.right.txt tag add excl_highlight"
        set cmd_lookup  ".bot.right.txt tag add uncov_lookup"
        foreach entry $uncovered_combs {
          if {[lindex $entry 3] == 0} {
            set cmd_ucov_hl [concat $cmd_ucov_hl [lindex $entry 0] [lindex $entry 1]]
          } else {
            set cmd_excl_hl [concat $cmd_excl_hl [lindex $entry 0] [lindex $entry 1]]
          }
          set cmd_lookup [concat $cmd_lookup [lindex $entry 0] [lindex $entry 1]]
          set sline [lindex [split [lindex $entry 0] .] 0]
          set eline [lindex [split [lindex $entry 1] .] 0]
          if {$sline != $eline} {
            set cmd_enter  [concat $cmd_enter  [lindex $entry 0] "$sline.end"]
            set cmd_button [concat $cmd_button [lindex $entry 0] "$sline.end"]
            set cmd_leave  [concat $cmd_leave  [lindex $entry 0] "$sline.end"]
            for {set i [expr $sline + 1]} {$i <= $eline} {incr i} {
              set line       [.bot.right.txt get $i.14 $i.end]
              set line_diff  [expr ([string length $line] - [string length [string trimleft $line]]) + 14]
              if {$i == $eline} {
                set cmd_enter  [concat $cmd_enter  "$i.$line_diff" [lindex $entry 1]]
                set cmd_button [concat $cmd_button "$i.$line_diff" [lindex $entry 1]]
                set cmd_leave  [concat $cmd_leave  "$i.$line_diff" [lindex $entry 1]]
              } else {
                set cmd_enter  [concat $cmd_enter  "$i.$line_diff" "$i.end"]
                set cmd_button [concat $cmd_button "$i.$line_diff" "$i.end"]
                set cmd_leave  [concat $cmd_leave  "$i.$line_diff" "$i.end"]
              }
            }
          } else {
            set cmd_enter  [concat $cmd_enter  [lindex $entry 0] [lindex $entry 1]]
            set cmd_button [concat $cmd_button [lindex $entry 0] [lindex $entry 1]]
            set cmd_leave  [concat $cmd_leave  [lindex $entry 0] [lindex $entry 1]]
          }
        }
        eval $cmd_enter
        eval $cmd_button
        eval $cmd_leave
        eval $cmd_lookup
        if {[llength $cmd_ucov_hl] > 4} {
          eval $cmd_ucov_hl
          .bot.right.txt tag configure uncov_highlight -foreground $uncov_fgColor -background $uncov_bgColor
        }
        if {[llength $cmd_excl_hl] > 4} {
          eval $cmd_excl_hl
          .bot.right.txt tag configure excl_highlight -foreground $cov_fgColor   -background $cov_bgColor
        }
        .bot.right.txt tag configure uncov_button -underline true
        .bot.right.txt tag bind uncov_enter <Enter> {
          set curr_cursor [.bot.right.txt cget -cursor]
          set curr_info   [.info cget -text]
          .bot.right.txt configure -cursor hand2
          .info configure -text "Click left button for detailed combinational logic coverage information" 
        }
        .bot.right.txt tag bind uncov_leave <Leave> {
          .bot.right.txt configure -cursor $curr_cursor
          .info configure -text $curr_info
        }
        .bot.right.txt tag bind uncov_button <ButtonPress-1> {
          display_comb current
        }
      }

      if {[expr $cov_type == 1] && [expr [llength $covered_combs] > 0]} {
        set cmd_cov ".bot.right.txt tag add cov_highlight"
        foreach entry $covered_combs {
          set cmd_cov [concat $cmd_cov [lindex $entry 0] [lindex $entry 1]]
        }
        eval $cmd_cov
        .bot.right.txt tag configure cov_highlight -foreground $cov_fgColor -background $cov_bgColor
      }

    }

    # Now cause the text box to be read-only again
    .bot.right.txt configure -state disabled

  }

  return

}

#-----------  FSM COVERAGE  --------------------------------------------------
 
proc process_fsm_cov {} {

  global fileContent start_line end_line
  global curr_block
  global fsm_summary_hit fsm_summary_total

  if {$curr_block != 0} {

    # Get the filename of the currently selected functional unit and read it in
    set file_name [tcl_func_get_filename $curr_block]

    if {[load_verilog $file_name 1]} {

      # Get start and end line numbers of this functional unit
      set line_pos   [tcl_func_get_funit_start_and_end $curr_block]
      set start_line [lindex $line_pos 0]
      set end_line   [lindex $line_pos 1]

      # Get line summary information and display this now
      tcl_func_get_fsm_summary $curr_block

      # If we have some uncovered values, enable the "next" pointer and menu item
      if {$fsm_summary_total != $fsm_summary_hit} {
        .bot.right.h.pn.next configure -state normal
        .menubar.view entryconfigure 0 -state normal
      } else {
        .bot.right.h.pn.next configure -state disabled
        .menubar.view entryconfigure 0 -state disabled
      }
      .bot.right.h.pn.prev configure -state disabled
      .menubar.view entryconfigure 1 -state disabled

      calc_and_display_fsm_cov

    }

  }

}

proc calc_and_display_fsm_cov {} {

  global uncovered_fsms covered_fsms race_info
  global curr_block start_line

  if {$curr_block != 0} {

    # Get list of uncovered/covered FSMs
    set uncovered_fsms [tcl_func_collect_uncovered_fsms $curr_block $start_line]
    set covered_fsms   [tcl_func_collect_covered_fsms   $curr_block $start_line]
    set race_info      [tcl_func_collect_race_lines     $curr_block $start_line]

    display_fsm_cov

  }

}

proc display_fsm_cov {} {

  global uncov_fgColor uncov_bgColor
  global cov_fgColor cov_bgColor
  global curr_block fileContent
  global fsm_summary_hit fsm_summary_total uncov_type cov_type
  global covered_fsms uncovered_fsms
  global start_line end_line

  if {$curr_block != 0} {

    set file_name [tcl_func_get_filename $curr_block]

    # Populate information bar
    if {$file_name != 0} {
      .info configure -text "Filename: $file_name"
    }

    # Allow us to write to the text box
    .bot.right.txt configure -state normal

    # Clear the text-box before any insertion is being made
    .bot.right.txt delete 1.0 end

    set contents [split $fileContent($file_name) \n]
    set linecount 1

    if {$end_line != 0} {

      # Next, populate text box with file contents including highlights for covered/uncovered lines
      foreach phrase $contents {
        if [expr [expr $start_line <= $linecount] && [expr $end_line >= $linecount]] {
          set line [format {%3s  %7u  %s} "   " $linecount [append phrase "\n"]]
          .bot.right.txt insert end $line
        }
        incr linecount
      }

      # Perform syntax highlighting
      verilog_highlight .bot.right.txt

      # Create race condition tags
      create_race_tags

      # Finally, set FSM information
      if {[expr $uncov_type == 1] && [expr [llength $uncovered_fsms] > 0]} {
        set cmd_enter   ".bot.right.txt tag add uncov_enter"
        set cmd_button  ".bot.right.txt tag add uncov_button"
        set cmd_leave   ".bot.right.txt tag add uncov_leave"
        set cmd_ucov_hl ".bot.right.txt tag add uncov_highlight"
        set cmd_excl_hl ".bot.right.txt tag add excl_highlight"
        foreach entry $uncovered_fsms {
          set cmd_enter  [concat $cmd_enter  [lindex $entry 0] [lindex $entry 1]]
          set cmd_button [concat $cmd_button [lindex $entry 0] [lindex $entry 1]]
          set cmd_leave  [concat $cmd_leave  [lindex $entry 0] [lindex $entry 1]]
          if {[lindex $entry 3] == 0} {
            set cmd_ucov_hl [concat $cmd_ucov_hl [lindex $entry 0] [lindex $entry 1]]
          } else {
            set cmd_excl_hl [concat $cmd_excl_hl [lindex $entry 0] [lindex $entry 1]]
          }
        }
        eval $cmd_enter
        eval $cmd_button
        eval $cmd_leave
        if {[llength $cmd_ucov_hl] > 4} {
          eval $cmd_ucov_hl
          .bot.right.txt tag configure uncov_highlight -foreground $uncov_fgColor -background $uncov_bgColor
        }
        if {[llength $cmd_excl_hl] > 4} {
          eval $cmd_excl_hl
          .bot.right.txt tag configure excl_highlight  -foreground $cov_fgColor   -background $cov_bgColor
        }
        .bot.right.txt tag configure uncov_button -underline true
        .bot.right.txt tag bind uncov_enter <Enter> {
          set curr_cursor [.bot.right.txt cget -cursor]
          set curr_info   [.info cget -text]
          .bot.right.txt configure -cursor hand2
          .info configure -text "Click left button for detailed FSM coverage information"
        }
        .bot.right.txt tag bind uncov_leave <Leave> {
          .bot.right.txt configure -cursor $curr_cursor
          .info configure -text $curr_info
        }
        .bot.right.txt tag bind uncov_button <ButtonPress-1> {
          display_fsm current
        }
      }

      if {[expr $cov_type == 1] && [expr [llength $covered_fsms] > 0]} {
        set cmd_cov ".bot.right.txt tag add cov_highlight"
        foreach entry $covered_fsms {
          set cmd_cov [concat $cmd_cov [lindex $entry 0] [lindex $entry 1]]
        }
        eval $cmd_cov
        .bot.right.txt tag configure cov_highlight -foreground $cov_fgColor -background $cov_bgColor
      }

    }

    # Now cause the text box to be read-only again
    .bot.right.txt configure -state disabled

  }

}

#-----------  ASSERTION COVERAGE  --------------------------------------------
 
proc process_assert_cov {} {

  global fileContent start_line end_line
  global curr_block
  global assert_summary_hit assert_summary_total

  if {$curr_block != 0} {

    # Get the filename of the currently selected functional unit and read it in
    set file_name [tcl_func_get_filename $curr_block]

    if {[load_verilog $file_name 1]} {

      # Get start and end line numbers of this functional unit
      set line_pos   [tcl_func_get_funit_start_and_end $curr_block]
      set start_line [lindex $line_pos 0]
      set end_line   [lindex $line_pos 1]

      # Get assertion summary information and display this now
      tcl_func_get_assert_summary $curr_block

      # If we have some uncovered values, enable the "next" pointer and menu item
      if {$assert_summary_total != $assert_summary_hit} {
        .bot.right.h.pn.next configure -state normal
        .menubar.view entryconfigure 0 -state normal
      } else {
        .bot.right.h.pn.next configure -state disabled
        .menubar.view entryconfigure 0 -state disabled
      }
      .bot.right.h.pn.prev configure -state disabled
      .menubar.view entryconfigure 1 -state disabled

      calc_and_display_assert_cov

    }

  }

}

proc calc_and_display_assert_cov {} {

  global uncovered_asserts covered_asserts race_info
  global curr_block start_line

  if {$curr_block != 0} {

    # Get list of uncovered/covered assertions
    set uncovered_asserts [tcl_func_collect_uncovered_assertions $curr_block]
    set covered_asserts   [tcl_func_collect_covered_assertions   $curr_block]
    set race_info         [tcl_func_collect_race_lines $curr_block $start_line]

    display_assert_cov

  }
}

proc display_assert_cov {} {

  global uncov_fgColor uncov_bgColor
  global cov_fgColor cov_bgColor
  global curr_block fileContent
  global assert_summary_hit assert_summary_total uncov_type cov_type
  global covered_asserts uncovered_asserts
  global start_line end_line

  if {$curr_block != 0} {

    set file_name [tcl_func_get_filename $curr_block]

    # Populate information bar
    if {$file_name != ""} {
      .info configure -text "Filename: $file_name"
    }

    # Allow us to write to the text box
    .bot.right.txt configure -state normal

    # Clear the text-box before any insertion is being made
    .bot.right.txt delete 1.0 end

    set contents [split $fileContent($file_name) \n]
    set linecount 1

    if {$end_line != 0} {

      # Next, populate text box with file contents including highlights for covered/uncovered lines
      foreach phrase $contents {
        if [expr [expr $start_line <= $linecount] && [expr $end_line >= $linecount]] {
          set line [format {%3s  %7u  %s} "   " $linecount [append phrase "\n"]]
          .bot.right.txt insert end $line
        }
        incr linecount
      }

      # Perform syntax highlighting
      verilog_highlight .bot.right.txt

      # Create race condition tags
      create_race_tags

      # Finally, set assertion information
      if {[expr $uncov_type == 1] && [expr [llength $uncovered_asserts] > 0]} {
        set cmd_enter    ".bot.right.txt tag add uncov_enter"
        set cmd_button   ".bot.right.txt tag add uncov_button"
        set cmd_leave    ".bot.right.txt tag add uncov_leave"
        set cmd_uncov_hl ".bot.right.txt tag add uncov_highlight"
        set cmd_excl_hl  ".bot.right.txt tag add excl_highlight"
        foreach entry $uncovered_asserts {
          set match_str ""
          append match_str {[^a-zA-Z0-9_]} [lindex $entry 0] {[^a-zA-Z0-9_]}
          set start_index [.bot.right.txt index "[.bot.right.txt search -count matching_chars -regexp $match_str 1.0] + 1 chars"]
          set end_index   [.bot.right.txt index "$start_index + [expr $matching_chars - 2] chars"]
          set cmd_enter  [concat $cmd_enter  $start_index $end_index]
          set cmd_button [concat $cmd_button $start_index $end_index]
          set cmd_leave  [concat $cmd_leave  $start_index $end_index]
          if {[lindex $entry 1] == 0} {
            set cmd_uncov_hl [concat $cmd_uncov_hl $start_index $end_index]
          } else {
            set cmd_excl_hl  [concat $cmd_excl_hl  $start_index $end_index]
          }
        }
        eval $cmd_enter
        eval $cmd_button
        eval $cmd_leave
        if {[llength $cmd_uncov_hl] > 4} {
          eval $cmd_uncov_hl
          .bot.right.txt tag configure uncov_highlight -foreground $uncov_fgColor -background $uncov_bgColor
        }
        if {[llength $cmd_excl_hl] > 4} {
          eval $cmd_excl_hl
          .bot.right.txt tag configure excl_highlight -foreground $cov_fgColor -background $cov_bgColor
        }
        .bot.right.txt tag configure uncov_button -underline true
        .bot.right.txt tag bind uncov_enter <Enter> {
          set curr_cursor [.bot.right.txt cget -cursor]
          set curr_info   [.info cget -text]
          .bot.right.txt configure -cursor hand2
          .info configure -text "Click left button for detailed assertion coverage information"
        }
        .bot.right.txt tag bind uncov_leave <Leave> {
          .bot.right.txt configure -cursor $curr_cursor
          .info configure -text $curr_info
        }
        .bot.right.txt tag bind uncov_button <ButtonPress-1> {
          display_assert current
        }
      }

      if {[expr $cov_type == 1] && [expr [llength $covered_asserts] > 0]} {
        set cmd_cov ".bot.right.txt tag add cov_highlight"
        foreach entry $covered_asserts {
          set match_str ""
          append match_str {[^a-zA-Z0-9_]} [lindex $entry 0] {[^a-zA-Z0-9_]}
          set start_index [.bot.right.txt index "[.bot.right.txt search -count matching_chars -regexp $match_str 1.0] + 1 chars"]
          set end_index   [.bot.right.txt index "$start_index + [expr $matching_chars - 2] chars"]
          set cmd_cov [concat $cmd_cov $start_index $end_index]
        }
        eval $cmd_cov
        .bot.right.txt tag configure cov_highlight -foreground $cov_fgColor -background $cov_bgColor
      }

    }

    # Now cause the text box to be read-only again
    .bot.right.txt configure -state disabled

  }

}
