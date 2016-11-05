Fizzim - an open-source FSM design environment.
==============================================
 Paul Zimmer - Zimmer Design Services
 Michael Zimmer - Zimmer Design Services and UCSB
 Brian Zimmer - Zimmer Design Services and UCD

Readme for versions:
 v11.09.25 of fizzim.jar
 v5.20 of fizzim.pl

New features / bug fixes:
 2.0:
 (GUI unchanged)
 fizzim.pl:
   -  Added support for whitespace in .fzm file
   -  Changed both regdp and comb block output structures to use a default
      value ahead of the case statement, then only output values that don't
      match the default.  If no default is supplied, fizzim.pl issues a
      warning and uses 0.  This improves out-of-the-box synthesis results.
   -  Official support of -terse mode.  Added alias of -sunburst to -terse.
   -  Changed case (1) in regdp output to use 1'b1 and parallel_case full_case
      just like nextstate comb block.  This improves out-of-the-box synthesis 
      results for onehot.
   -  Added onehot_pragma attribute to allow override of parallel_case
      full_case.

 3.0
 (GUI unchanged)
 fizzim.pl:
   -  Added SystemVerilog output format
   -  Changed verilog+onehot+default_state_is_x to work like sv+onehot
   -  Changed handling of constants.  Numbers are now assumed to be decimal.  Binary numbers must now have 'b on them (except single-bit values since these are the same in binary and decimal).

 3.1
 Gui:
   -  Changed to store the size info in the .fzm file so that it restores to
      the previous size.
   -  Mantis items 24-30 fixed.
 fizzim.pl:
   -  Changed to allow "fizzim.pl <file.fzm>" as well as input from stdin
   -  Made -addversion default on
 
  3.6
  Gui:
   -  Fixed bug involving changing the name of the reset state after the
      reset attributes were created.
   -  File > open now starts in cwd, instead of home directory.
   -  Added default state bubble size to preferences.
   -  Page setup changes now saved in the .fzm file.
   -  Misc small bug fixes (see Mantis).
  fizzim.pl
   -  Fixed (harmless) bug where XXX param/enum was added when it wasn't used.
   -  Fixed bug where "$" in insert... code was being swallowed.
   -  Added enhancement to make indentation work better on insert... code.
   -  Added support for parameters.
   -  Added code to suppress [0:0] on 1-bit reg/wire
   -  Made implied_loopback the default
   -  Added -minversion

  3.6a
  Gui:
    - fixed issue with one-button mouse Mac's
  fizzim.pl
  (same as 3.6)

  3.71 
  Gui (v11.03.08):
    Added flags, UserAtts, ResetVal
  fizzim.pl
    - Added undefined_states_go_here
    - Added flags
    - Added suppress_portlist
    - Changed regd to use resetval if set
    
  4.0  
  Gui (v11.08.22):
    Fixed bug that gives incorrect message about resetvalue
  fizzim.pl
    - Changed heros state assignment algorithm to speed it up.
    - Add -minbits and -iterations to control state assignment algorithm
    - Added warning code for missing include file
    - Added &verbose subroutine

  4.01
  Gui (v11.08.29)
    Added support for newline in transition equations
    Fixed implementation of newline support in free text
  fizzim.pl
    Added support for newline in transition equations
    
  4.41
  Gui (v11.09.25)
    Just changed help pointer for tutorial
  fizzim.pl
    Fixed bug with mealy handling when fsm input with newer versions of gui
    
  4.42
  Gui (v11.09.25)
    No changes
  fizzim.pl
    Added warning T20 to warn when too many reg bits are requested.
  pdf generated out of word so that TOC links will work
    
  4.44
  Gui (v11.09.25)
    No changes
  fizzim.pl
    Fixed bug in onehot datapath output (full_case used while suppressing cases)
    Fixed undesired spaces on blank lines
    
  5.0 
  Gui (v14.02.26)
    type reg displays at "statebit"
    file open, etc defaults to cwd or most recently used directory
  fizzim.pl
    vhdl output added
    
  5.1
  Gui (v14.02.26)
    No changes
  fizzim.pl
    Change regexp handling to make script compatible with perl version v5.22.0 and above
    Fixed -minversion bug introduced in version 4.44 by changes to RCS 
    Added user-submitted fix that allows parameters to be used in a flag definition.

  5.2
  Gui (v14.02.26)
    No changes
  fizzim.pl
    Changed vhdl output to remove clk and reset signals from sens list of COMB process
    Fixed bug with "," at end of a comment
    Added -insert_messages switch
    Added feature to allow suffixes on all insert_* attributes to make it easier to do multiline comments


Contents:
fizzim.jar - the java gui
fizzim.pl - the backend perl script
fizzim_tutorial_*.pdf - tutorial
examples - directory of example files (referenced in tutorial)

Running the gui:
Fizzim is written in Java and runs under the Java Runtime Environment.  This
is installed on most systems.  On PC, just double-clicking the .jar file will
usually be enough to start the app.  If that doesn't work, you can start it from
the command line like this:

java -jar fizzim.jar

Java version 1.5 ("Java 5") or greater is required to run fizzim.  If you only
have Java 1.4, there is a eval version of fizzim.jar for 1.4 on fizzim.com.
But this is intended for evals only.  Don't bother to submit bugs on it -
they won't be fixed.  Upgrade to 1.5 or greater.

Running the backend:
The backend is written in perl.  This is loaded on all unix systems.
You can download it for your PC from perl.org.

On unix systems, the "sh-bang" at the start of the file will tell the shell to
run perl, so you can just execute the script:

fizzim.pl < your_fsm.fzm

On PC's, your shell might not recognize sh-bang, so you may have to use this:

perl -S fizzim.pl < your_fsm.fzm

Bugs:
To report bugs, go to fizzim.com and follow the "bug tracking" link.

Submitting modifications:
Please send proposed code modifications to:
contributions@fizzim.com

