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

set sig_name         ""
set curr_toggle_ptr  ""
set toggle_geometry  ""
set toggle_gui_saved 0

proc display_toggle {curr_index} {

  global prev_toggle_index next_toggle_index curr_toggle_ptr

  # Get range of current signal
  set curr_range [.bot.right.txt tag prevrange uncov_button "$curr_index + 1 chars"]

  # Calculate the current signal string
  set curr_signal [string trim [lindex [split [.bot.right.txt get [lindex $curr_range 0] [lindex $curr_range 1]] "\["] 0]]

  # Make sure that the selected signal is visible in the text box and is shown as selected
  set_pointer curr_toggle_ptr [lindex [split [lindex $curr_range 0] .] 0]
  goto_uncov [lindex $curr_range 0]

  # Get range of previous signal
  set prev_toggle_index [lindex [.bot.right.txt tag prevrange uncov_button [lindex $curr_index 0]] 0]

  # Get range of next signal
  set next_toggle_index [lindex [.bot.right.txt tag nextrange uncov_button [lindex $curr_range 1]] 0]

  # Now create the toggle window
  create_toggle_window $curr_signal

}

proc create_toggle_window {signal} {

  global sig_name prev_toggle_index next_toggle_index toggle_excluded toggle_reason
  global toggle_msb toggle_lsb
  global curr_block
  global curr_toggle_ptr HOME
  global toggle_geometry toggle_gui_saved

  set sig_name $signal

  # Now create the window and set the grab to this window
  if {[winfo exists .togwin] == 0} {

    set toggle_gui_saved 0

    # Create new window
    toplevel .togwin
    wm title .togwin "Toggle Coverage - Verbose"

    ttk::frame .togwin.f -relief raised -borderwidth 1

    # Add toggle information
    ttk::label     .togwin.f.l_01 -anchor w -text "Toggle 0->1"
    ttk::label     .togwin.f.l_10 -anchor w -text "Toggle 1->0"
    text           .togwin.f.t -height 2 -width 40 -xscrollcommand ".togwin.f.hb set" -wrap none -spacing1 2 -spacing3 3
    ttk::scrollbar .togwin.f.hb -orient horizontal -command ".togwin.f.t xview"

    # Create bottom information bar
    ttk::label .togwin.f.info -anchor e

    # Create exclude checkbutton
    ttk::checkbutton .togwin.f.excl -text "Exclude" -variable toggle_excluded -command {
      set toggle_reason ""
      if {$exclude_reasons_enabled == 1 && $toggle_excluded == 1} {
        set toggle_reason [get_exclude_reason .togwin]
      }
      tcl_func_set_toggle_exclude $curr_block $sig_name $toggle_excluded $toggle_reason
      set text_x [.bot.right.txt xview]
      set text_y [.bot.right.txt yview]
      process_toggle_cov
      .bot.right.txt xview moveto [lindex $text_x 0]
      .bot.right.txt yview moveto [lindex $text_y 0]
      populate_listbox
      enable_cdd_save
      set_pointer curr_toggle_ptr $curr_toggle_ptr
    }
    set_exclude_reason_balloon .togwin.f.excl {$toggle_excluded} {$toggle_reason}
    set_balloon .togwin.f.excl "If set, excludes this signal from toggle coverage consideration"

    # Create bottom button bar
    ttk::frame .togwin.bf -relief raised -borderwidth 1
    ttk::button .togwin.bf.close -text "Close" -width 10 -command {
      rm_pointer curr_toggle_ptr
      destroy .togwin
    }
    help_button .togwin.bf.help chapter.gui.toggle ""
    ttk::button .togwin.bf.prev -image [image create photo -file [file join $HOME scripts left_arrow.gif]] -command {
      display_toggle $prev_toggle_index
    }
    set_balloon .togwin.bf.prev "Click to view the previous uncovered signal in this window"
    ttk::button .togwin.bf.next -image [image create photo -file [file join $HOME scripts right_arrow.gif]] -command {
      display_toggle $next_toggle_index
    }
    set_balloon .togwin.bf.next "Click to view the next uncovered signal in this window"

    # Pack the buttons into the button frame
    pack .togwin.bf.prev  -side left
    pack .togwin.bf.next  -side left
    pack .togwin.bf.help  -side right -padx 4 -pady 4
    pack .togwin.bf.close -side right -padx 4 -pady 4

    # Pack the widgets into the bottom frame
    grid rowconfigure    .togwin.f 3 -weight 1
    grid columnconfigure .togwin.f 0 -weight 1
    grid columnconfigure .togwin.f 1 -weight 0
    grid .togwin.f.t    -row 0 -rowspan 2 -column 0 -sticky new
    grid .togwin.f.l_01 -row 0 -column 1 -sticky new
    grid .togwin.f.l_10 -row 1 -column 1 -sticky new
    grid .togwin.f.hb   -row 2 -column 0 -sticky new
    grid .togwin.f.info -row 3 -column 0 -sticky new
    grid .togwin.f.excl -row 3 -column 1 -sticky new

    pack .togwin.f  -fill both -expand yes
    pack .togwin.bf -fill x

    # Set the geometry of the toggle window if it was saved
    if {$toggle_geometry != ""} {
      wm geometry .togwin $toggle_geometry
    }

    # Bind the destructor
    wm protocol . WM_DELETE_WINDOW {
      save_toggle_gui_elements 0
      destroy .togwin
    }
    bind .togwin <Destroy> {
      save_toggle_gui_elements 0
    }

  }

  # Disable next or previous buttons if valid
  if {$prev_toggle_index == ""} {
    .togwin.bf.prev configure -state disabled
  } else {
    .togwin.bf.prev configure -state normal
  }
  if {$next_toggle_index == ""} {
    .togwin.bf.next configure -state disabled
  } else {
    .togwin.bf.next configure -state normal
  }

  # Get verbose toggle information
  set toggle_info      [tcl_func_get_toggle_coverage $curr_block $signal]
  set toggle_msb       [lindex $toggle_info 0]
  set toggle_lsb       [lindex $toggle_info 1]
  set toggle01_verbose [lindex $toggle_info 2]
  set toggle10_verbose [lindex $toggle_info 3]
  set toggle_excluded  [lindex $toggle_info 4]
  set toggle_reason    [lindex $toggle_info 5]

  # Allow us to clear out text box
  .togwin.f.t configure -state normal

  # Remove any tags associated with the toggle text box
  .togwin.f.t tag delete tog_motion

  # Clear the text-box before any insertion is being made
  .togwin.f.t delete 1.0 end

  # Write toggle 0->1 and 1->0 information in text box
  .togwin.f.t insert end "$toggle01_verbose\n$toggle10_verbose" 

  # Keep user from writing in text boxes
  .togwin.f.t configure -state disabled

  # Create leave bindings for textboxes
  bind .togwin.f.t <Leave> {
    .togwin.f.info configure -text "$sig_name\[$toggle_msb:$toggle_lsb\]"
  }

  # Add toggle tags and bindings
  .togwin.f.t tag add  tog_motion 1.0 {end - 1 chars}
  .togwin.f.t tag bind tog_motion <Motion> {
    set tmp [lindex [split [.togwin.f.t index current] "."] 1]
    set tmp [expr $toggle_msb - $tmp]
    .togwin.f.info configure -text "$sig_name\[$tmp\]"
  }

  # Right justify the toggle information
  if {[expr [expr $toggle_msb - $toggle_lsb] + 1] <= 32} { 
    .togwin.f.t tag configure tog_motion -justify right
  } else {
    .togwin.f.t tag configure tog_motion -justify left
    .togwin.f.t xview moveto 1.0
  }

  .togwin.f.info configure -text "$sig_name\[$toggle_msb:$toggle_lsb\]"

  # Raise this window to the foreground
  raise .togwin

}

# Updates the GUI elements of the toggle window when some type of event causes the
# current metric mode to change.
proc update_toggle {} {

  global cov_rb prev_toggle_index next_toggle_index curr_toggle_ptr

  # If the combinational window exists, update the GUI
  if {[winfo exists .togwin] == 1} {

    # If the current metric mode is not toggle, disable the prev/next buttons
    if {$cov_rb != "Toggle"} {

      .togwin.bf.prev configure -state disabled
      .togwin.bf.next configure -state disabled

    } else {

      # Restore the pointer if it is set
      set_pointer curr_toggle_ptr $curr_toggle_ptr

      # Restore the previous/next button enables
      if {$prev_toggle_index != ""} {
        .togwin.bf.prev configure -state normal
      } else {
        .togwin.bf.prev configure -state disabled
      }
      if {$next_toggle_index != ""} {
        .togwin.bf.next configure -state normal
      } else {
        .togwin.bf.next configure -state disabled
      }

    }

  }

}

# This is called when the current CDD is closed.  It resets all associated variables
# and destroys the window (if it is currently opened).
proc clear_toggle {} {

  global curr_toggle_ptr

  # Clear the current toggle pointer value
  set curr_toggle_ptr ""

  # Destroy the window
  destroy .togwin

}

# Saves the GUI elements from the toggle window setup that should be saved
proc save_toggle_gui_elements {main_exit} {

  global toggle_geometry toggle_gui_saved

  if {$toggle_gui_saved == 0} {
    if {$main_exit == 0 || [winfo exists .togwin] == 1} {
      set toggle_gui_saved 1
      set toggle_geometry  [winfo geometry .togwin]
    }
  }

}
