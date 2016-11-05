#!/usr/bin/env perl
#    This file is part of fizzim
#    Copyright (C) 2007 Zimmer Design Services
#    Copyright (C) 2008 Zimmer Design Services
#    Copyright (C) 2009 Zimmer Design Services
#    Copyright (C) 2010 Zimmer Design Services
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 

#  Although not specifically required by GPL, if you improve/modify this
#  software, please make it available to other users either by posting
#  it yourself or sending it back to ZDS (paulzimmer@zimmerdesignservices.com)
#  Consider this an ethical obligation rather than a legal one.
#
# $Id: fizzim.pl,v 5.20 2016/04/24 18:32:19 pzimmer Exp $ 

$| = 1;  # force immediate output

# Make sure calling location is in INC path
use File::Basename;
$my_dir = &dirname($0);
$me = &basename($0);
unshift(@INC,"$my_dir");

# Figure out revision info
# The reference to Revision below will get RCS to input the revision
$scriptversion = '$Revision: 5.20 $';
# Extract the rev number
($scriptversion) = ($scriptversion =~ /([\d\.]+)/); # just grab digits and dots

# Scan for -help and -version for early exit.  If found, call parser
if (join(" ",@ARGV) =~ /-h/) {
  &process_options;
  exit; # redundant - will exit anyway
}
if (join(" ",@ARGV) =~ /-vers/) {
  &process_options;
  exit; # redundant - will exit anyway
} elsif (join(" ",@ARGV) =~ /-v(\s|$)/) {
  print STDERR "Note that -v is ambiguous.  If you want version, you must use at least -vers \n";
}

# Scan for -debug be_cmd before calling process_options
$debug_sections{"be_cmd"} = (join(" ",@ARGV) =~ /-d\S*\s+be_cmd/);
# Scan for -debug parse_input before calling parser
$debug_sections{"parse_input"} = (join(" ",@ARGV) =~ /-d\S*\s+parse_input/);

# Need to fetch the be_cmd from the file before parsing inputs.
# Call the option processing now to clear out options and leave only the
# file name.  This makes "fizzim.pl foo.fzm" work as well as 
# "fizzim.pl < foo.fzm".
# Then we can parse the file, add the be_cmd options, and
# parse again.
@orig_argv = @ARGV;
&process_options;

if ($language eq "vhdl") {
  $comm = "--";
} else {
  $comm = "//";
}

# Parse the input.  Do this before option processing so that we can get
# be_cmd.
&set_myattributes; # sets the myattributes hash to know what to parse
&parse_input;

# Process command-line options
# Re-build @ARGV using the options from be_cmd FIRST (so actual command-line
# options will have priority)
($be_options = $globals{machine}{be_cmd}{value}) =~ s/^[^-]*//;
@ARGV = (split(/\s+/,$be_options),@orig_argv);

&process_options;

# check for minversion
if ($minversion && ($scriptversion < $minversion)) {
  &error(0,"Version $scriptversion is less than required minversion $minversion");
}

# If -inst is set, create the instance and exit
# Not done yet...
if ($inst) {
  foreach $output ((keys %reg_outputs),(keys %comb_outputs)) {
    push(@lines,"${indent}.${output} (${output}),\n");
  }
  print @lines;
}

#print "is_stateline: " . &is_stateline("      state[S1_BIT]: if (in[0] && in[1])"). "\n"; exit;

# Output is a bit messy.  To allow for -terse, output is held in a buffer 
# (pbuf) and printed at the end.  Warnings and "assertions" are held in their 
# own arrays, with indexes to match the line they follow in pbuf.
# "assertions" are comment lines, other than warnings, to be added to the
# output.
# Init printbuffer
$pbuf[0] = "";

# Rename hashes
%states = %state; undef %state;
%transitions = %transition; undef %transition;

# Set clk, reset_signal
$reset_signal = $globals{machine}{reset_signal}{value};
# Check to make sure clock data is ok
if (exists $globals{machine}{clock}{value}) {
  $clk = $globals{machine}{clock}{value};
} else {
  &error($indent,"No clock specified");
}

# Massage output data structures to more convenient ones.
foreach $output (sort keys %{ $globals{outputs} }) {
  if (($name,$range) = ($output =~ /^([^\[]+)\s*(\[.*\])/)) {
    ($upper,$lower) = $range =~ /(\d+):(\d+)/;
    $widths{$name} = abs($upper - $lower) + 1;
    #print "set $name : $widths{$name}\n";
  } else {
    $widths{$output} = 1;
  }
  if ($globals{outputs}{$output}{type} eq "comb") {
    if (exists $globals{"trans"}{$att}{"value"} && ($globals{"trans"}{$att}{"value"} ne "")) {
      $comb_outputs{$output}{"default_value"} = $globals{"trans"}{$att}{"value"} ; # default value defined in transitions table (old gui did this)
    } else {
      $comb_outputs{$output}{"default_value"} = $globals{outputs}{$output}{value};
    }
    #print "debug: comb default: $comb_outputs{$output}{default_value}\n";
    foreach $trans (keys %transitions) {
      if (exists $transitions{$trans}{attributes}{$output}{value} && ($transitions{$trans}{attributes}{$output}{value} ne "")
        && ($comb_outputs{$output}{"default_value"} ne $transitions{$trans}{attributes}{$output}{value})
        ) {
          #print "debug: in mot for $output due to transition $trans\n";
        $in_mealy_on_transitions_mode{$output} = 1;
      }
    }
  } elsif ($globals{outputs}{$output}{type} eq "reg") {
    #print "Setting reg $output\n";
    if ($encoding eq "onehot" || $encoding eq "none") {
      # For onehot and "none", all reg's are effectively regdp's
      $regdp_outputs{$output}{"default_value"} = $globals{outputs}{$output}{value};
    } else {
      $reg_outputs{$output}{"default_value"} = &convert2bin($globals{outputs}{$output}{value},1);
    }
  } elsif ($globals{outputs}{$output}{type} eq "regdp") {
    $regdp_outputs{$output}{"default_value"} = $globals{outputs}{$output}{value};
  } elsif ($globals{outputs}{$output}{type} eq "flag") {
    # no defaults for flags, but create the flags array
    $flags{$output} = 1;
    $flag_outputs{$output}{"default_value"} = $globals{outputs}{$output}{value};
    #print "debug: flag default: $flag_outputs{$output}{default_value}\n";
    foreach $trans (keys %transitions) {
      if (exists $transitions{$trans}{attributes}{$output}{value} && ($transitions{$trans}{attributes}{$output}{value} ne "")
        && ($flag_outputs{$output}{"default_value"} ne $transitions{$trans}{attributes}{$output}{value})
        ) {
        #print "debug: in mot for flag $output due to transition $trans\n";
        $in_flag_on_transitions_mode{$output} = 1;
      }
    }
  } else {
    &error($indent,"No type found for output $output");
  }
  # Keep track of max output length for formatting
  if (length($output) > $max_output_length) {
    $max_output_length = length($output);
  }
}

# For encoding=none, gray code not allowed
if ($encoding eq "none") {
  foreach $trans (keys %transitions) {
    if ($transitions{$trans}{attributes}{graycode}{value} || $transitions{$trans}{attributes}{graytransition}{value}) {
      
      &error($indent,"Cannot use encoding=none with gray codes");
    }
  }
  
}

foreach $input (sort keys %{ $globals{inputs} }) {
  # Keep track of max input length for formatting
  if (length($input) > $max_input_length) {
    $max_input_length = length($input);
  }
}

# Figure max i/o length for formatting
if ($max_output_length > $max_input_length) {
  $max_io_length = $max_output_length;
} else {
  $max_io_length = $max_input_length;
}
$io_statement_length = $max_io_length + length("output logic , ");

# Create mapping array for vhdl
if ($language eq "vhdl") {
  foreach $output (sort keys %{ $globals{outputs} }) {
    # always rename
    ($base = $output) =~ s/\[.*$//;
    ($port2var{$output} = ${output}) =~ s/^(\w+)(.*)/\1${vhdl_intvar_suffix}\2/;
    $widths{"${base}${vhdl_intvar_suffix}"} = $widths{$base}; # width is the same

#    $create_entry = 0;
#    #print "output $output\n";
#    $match = "(\\b|!)";
#    if (($base) = ($output =~ /(.*)\[/)) {
#      #print "output: $output base: $base\n";
#      $match .= "$base\\[";
#      #print "output: $output base: $base match \"$match\"\n";
#    } else {
#      $match .= "$output\\b";
#      $base = $output;
#    }
#    foreach $trans (keys %transitions) {
#      $equation = $transitions{$trans}{attributes}{equation}{value};
#      #print "equation: $equation\n";
#      if ($equation =~ /$match/) {
#        $create_entry = 1;
#        #print "equation: $equation matches on output \"$output\"\n";
#      }
#
#    }
#    foreach $state (sort keys %states) {
#      $value = $states{$state}{attributes}{$output}{value};
#      #print "state: $state value $value\n";
#      if ($value =~ /$match/) {
#        #print "state: $state matches on assignment $value of output \"$output\"\n";
#        $create_entry = 1;
#      }
#    }
#
#    if ($create_entry) {
#      ($port2var{$output} = ${output}) =~ s/^(\w+)(.*)/\1${vhdl_intvar_suffix}\2/;
#      $widths{"${base}${vhdl_intvar_suffix}"} = $widths{$base}; # width is the same
#    }
  }
}

# Search for parameters
foreach $attname (sort keys %{ $globals{machine} }) {
  if ($globals{machine}{$attname}{type} eq "parameter") {
    $parameters{$attname} = $globals{machine}{$attname}{value};
  }
}
#foreach $param (sort keys %parameters) {
#  print "$param : $parameters{$param}\n";
#}

# Determine undefined_states info
$use_undefined_goto_state = 0; # default off
$undefined_goto_state = ""; # default off
if ($globals{machine}{undefined_states_go_to}{value}) {
  $undefined_goto_state = $globals{machine}{undefined_states_go_to}{value};
  $use_undefined_goto_state = 1; # might get switched off - see below
} else {
  foreach $state (keys %states) {
    if ($states{$state}{attributes}{undefined_states_go_here}{value}) {
      if ($undefined_goto_state) { # already defined!
        &error($indent,"Attribute undefined_states_go_here set on more than one state ($undefined_goto_state and $state)");
      } else {
        $undefined_goto_state = $state;
        $use_undefined_goto_state = 1; # might get switched off - see below
      }
    }
  }
}


# Encodings:
# HEROS:
# Highly Encoded with Registered Outputs as Statebits
# All registered outputs will be encoded as state bits.  Additional
# statebits will be added as needed to meet other requirements 
# (mininum encoding, graycode transitions).
#
# onehot:
# One-hot encoding per Steve Golson.  All reg'd outputs are effectively
# regdp's.
#
# Encoding cannot easily be an attribute on the fsm, because we need to know
# the encoding before we parse...

#######################
# State assignment 
#######################
$section = "assign_states"; # for debug

# allstates is an array of all the states in alpha order, but with the 
# reset state moved to the front (FIRST)
if (exists $globals{machine}{reset_state}{value}) {
  $reset_state = $globals{machine}{reset_state}{value};
  push(@allstates,$reset_state);
} else {
  # If no reset, warn user
  &warning($indent,"No reset specified");
}
foreach $state (sort keys %states) {
  unless ($state eq $reset_state) {
    push(@allstates,$state);
  }
}
&debug("allstates is @allstates\n",0,"assign_states");

if ($encoding eq "none") {
  # sv needs to know total statebits because the type must still be logic to allow 'x to be used
  print STDERR "encoding none is not supported and will not work except in some very specific cases\n";

} elsif ($encoding eq "heros") {
  # Massage the data structures before calling assign_states
  # Look at all registered outputs and assign them to state bits.

  @outputs2check = (keys %reg_outputs);

  # While doing this, check check to see if any comb outputs are actually 
  # constants in all states.  If so, convert them to reg type.
  # Don't do this is there are any gray coded transitions.
  if ($comb2reg) {
    foreach $trans (keys %transitions) {
      if ($transitions{$trans}{attributes}{graycode}{value} || $transitions{$trans}{attributes}{graytransition}{value}) {
        $design_has_graycodes = 1; last;
      }
    }
  
    if (!$design_has_graycodes) {
      push(@outputs2check,keys %comb_outputs)
    }
  }

  $statebit = 0; # init statebit pointer 
  # Loop through all reg'd outputs
  OUTPUT: foreach $output (sort @outputs2check) {
    # Set flag for comb outputs
    if ($globals{outputs}{$output}{type} eq "comb") {
      $is_comb = 1;
    } else {
      $is_comb = 0;
    }
    &debug("Working on $output : is_comb is $is_comb",0,"$section");
    # Determine upper and lower parts of [upper:lower] 
    if (($upper,$lower) = ($output =~ /\[(\d+):(\d+)\]/)) {
      $reg_outputs{$output}{"range"} = "$upper:$lower";
    } else {
      # If no range match, this must be single bit
      $upper = 0; $lower = 0;
    }
    # Set the lower (where the range will start to the current pointer)
    $statebit_lower = $statebit;
    # Bump the pointer up until we reach $upper
    for (my $bit = $lower; $bit < $upper; $bit++) {
      $statebit++;
    }
    # Set %fixedbits on all states to show these bits as fixed
    foreach $state (keys %states) {
      if ((!exists $states{$state}{attributes}{$output}{value}) || ($states{$state}{attributes}{$output}{value} eq "")) {
        $value = $reg_outputs{$output}{"default_value"};
        $value = &convert_value_from_vhdl_to_verilog_if_required($value);
        if ($value eq "") {
          &error($indent,"No value for reg output $output in state $state and no default value set");
        } else {
          &debug("${comm} Warning: No value for Moore bit $output for state $state - using default $value",0,"$section") if (!$is_comb);
        }
      } else {
        $value = &convert2bin($states{$state}{attributes}{$output}{value},0);
        $value = &convert_value_from_vhdl_to_verilog_if_required($value);
        #print "$states{$state}{attributes}{$output}{value} became $value\n"; # temp
      }
      # Make sure it is a legal binary value and the correct size
      if ($value !~ /^[01]+$/) {
        if ($is_comb) {
          &debug("Bailed out on converting $output from comb to reg because it could not be converted to binary",0,"$section");
          next OUTPUT;
        } else {
          &error($indent,"Value \"$states{$state}{attributes}{$output}{value}\" of output $output for state $state is not a legal binary number after conversion to \"$value\"");
        }
      }
      # Checking the bit width make it impossible to use 'h0f (tic without size)
      if (length($value) != $upper - $lower + 1) {
        &error($indent,"Value $value of output $output for state $state is not the correct bit width") if (!$is_comb);
      }
      $target_valuebit = 0;
      for ($target_statebit = $statebit; $target_statebit >= $statebit_lower; $target_statebit--) {
        $bitvalue = substr($value,$target_valuebit,1);
        if ($bitvalue eq "") {
          if ($is_comb) {
            &debug("Bailed out on converting $output from comb to reg because value had fewer bits than required",0,"$section");
            next OUTPUT;
          } else {
            &error($indent,"Value of output $output in state $state has fewer bits than required\n");
          }
        }
        &debug("Setting fixedbit $target_statebit to $bitvalue for output $output in state $state",0,"$section");
        $fixedbits{$state}{$target_statebit} = $bitvalue;
        $target_valuebit++;
      }
    }
    if ($is_comb) {
      &debug("Converted $output from comb to reg",0,"$section");
      $globals{outputs}{$output}{type} = "reg";
      $reg_outputs{$output}{"default_value"} = $comb_outputs{$output}{"default_value"};
      delete $comb_outputs{$output};
    }

    if ($statebit == $statebit_lower) { # single bit
      $reg_outputs{$output}{"statebit_range"} = $statebit;
      &debug("Assigned $output to statebits $reg_outputs{$output}{statebit_range}",0,"$section");
    } else {
      $reg_outputs{$output}{"statebit_range"} = "$statebit:$statebit_lower";
      &debug("Assigned $output to statebits $reg_outputs{$output}{statebit_range}",0,"$section");
    }
    $reg_outputs{$output}{"statebit_lower"} = $statebit_lower;
    $reg_outputs{$output}{"statebit_upper"} = $statebit;
    $statebit++;

  }
  $regbits = $statebit;
  
  # Create graytransitions hash
  foreach $trans (keys %transitions) {
    if ($transitions{$trans}{attributes}{graycode}{value} || $transitions{$trans}{attributes}{graytransition}{value}) {
      &debug("Setting transition $trans as gray",0,"$section");
      push( @{ $graytransitions{"$transitions{$trans}{startState}"} },$transitions{$trans}{endState});
      $design_has_graycodes = 1;
    }
  }
  
  # Calculate limits of state assignment search
  $required_bits = &required_bits($#allstates + 1);
  $minbits = $required_bits;
  # regbits is the number of fixed bits used
  $minbits = $regbits if ($regbits > $minbits);
  foreach ($statebit = 0; $statebit <= $regbits-1 ; $statebit++) {
    $num0fixbits = 0;
    $num1fixbits = 0;
    foreach $state (@allstates) {	
      if ($fixedbits{$state}{$statebit} == 0) {
        $num0fixbits++;
      }
      if ($fixedbits{$state}{$statebit} == 1) {
        $num1fixbits++;
      }
    }
    #print "num0fixbits for statebit $statebit is $num0fixbits\n";
    #print "num1fixbits for statebit $statebit is $num1fixbits\n";
    # Make sure we have enough bits to handle this fixbit value.  If there are 13 0's, for example, we would need to make
    # sure minbits is at least 5 (4 bits would only allow 16 0 values).
    while ( ((2**$minbits)/2 < $num0fixbits) || ((2**$minbits)/2 < $num1fixbits)) {
      $minbits++;
    }
  }
  $minbits = $userminbits if ($userminbits); # option override

  $maxbits = $required_bits + $regbits;
  # Bump maxbits in the presence of graycodes
  $maxbits = $maxbits + 2 if (scalar(keys %graytransitions));

  $maxbits = $usermaxbits if ($usermaxbits); # option override
  &debug("minbits is $minbits, maxbits is $maxbits",0,"assign_states");

  # Force reset state to be allones or allzeros if requested by setting
  # fixbits values.  
  if (exists $globals{machine}{reset_state}{value}) {
    $reset_state = $globals{machine}{reset_state}{value};
    if ($globals{machine}{reset_state}{type} eq "allzeros") {
       for ($bit = 0; $bit <= $maxbits - 1 ; $bit++) {
         # If there is a comflicting fixedbits setting from registered
         # outputs, error out.
         if ((exists $fixedbits{$reset_state}{$bit}) && $fixedbits{$reset_state}{$bit} == 1) {
           &error($indent,"allzeros constraint for reset state $reset_state is incompatible with output setting");
         }
         $fixedbits{$reset_state}{$bit} = 0;
         &debug("Setting fixedbit $bit to 0 for reset state $reset_state",0,"$section");
       }
    } elsif ($globals{machine}{reset_state}{type} eq "allones") {
       for ($bit = 0; $bit <= $maxbits - 1 ; $bit++) {
         # If there is a comflicting fixedbits setting from registered
         # outputs, error out.
         if ((exists $fixedbits{$reset_state}{$bit}) && $fixedbits{$reset_state}{$bit} == 0) {
           &error($indent,"allones constraint for bit $bit of reset state $reset_state is incompatible with output setting");
         }
         $fixedbits{$reset_state}{$bit} = 1;
         &debug("Setting fixedbit $bit to 1 for reset state $reset_state",0,"$section");
       }
    }
  }
  
  # Debug output prior to calling &assign_states
  if ($global_debug || $debug_sections{"$section"}) {
    foreach $state (@allstates) {
      for ($statebit = 0; $statebit < $regbits; $statebit++) {
        if (exists $fixedbits{$state}{$statebit}) {
          &debug("fixedbits{$state}{$statebit} is $fixedbits{$state}{$statebit}",0,"$section");
        }
      }
    }
    &debug("regbits is $regbits",0,"$section");
    foreach $trans (keys %graytransitions) {
      &debug("graytransitions{$trans} is @{ $graytransitions{$trans} }",0,"$section");
    }
  }

  # If undefined_states_go_to is set:
  # 1) Check to see if there are not transitions into this state, and that
  #    it is not the reset state.
  # 2) If not, check to try state assignment without this state.  
  # 3) If this assignment is "full", drop the uknown_states_go_to state.

  if ($undefined_goto_state && !$force_keep_undefined_goto_state) {
    foreach $trans (keys %transitions) {
      if ($transitions{$trans}{"endState"} eq $undefined_goto_state) {
        #&debug("Exiting undefined check because found a transition into state $undefined_goto_state\n",0,$section);
        &debug("Found input transition to undefined_goto_state $undefined_goto_state - leaving it in\n",0,$section);
        $has_input_transition = 1;
        last;
      }
    }
    if (!$has_input_transition && !($undefined_goto_state eq $reset_state)) {
      &debug("Trying state assignment without $undefined_goto_state\n",0,$section);
      @allstates_save = @allstates;
      @allstates = &remove_from_array($undefined_goto_state,@allstates_save);
      &debug("temporary allstates is @allstates\n",0,"assign_states");
      @save = ($required_bits,$minbits);
      $required_bits = &required_bits($#allstates + 1);
      $minbits = $required_bits;
      $minbits = $regbits if ($regbits > $minbits);
      # Try state assignment without undefined_goto_state
      &assign_states_hero;
      # If the result is a full state graph, drop undefined_goto_state
      if (2**$bits == $#allstates + 1) {
        &debug("State assignment is full without $undefined_goto_state, dropping it\n",0,$section);
        $use_undefined_goto_state = 0;
        $states_assigned_without_undefined_goto_state = 1;
      # Else, restore values 
      } else {
        &debug("State assignment is not full without $undefined_goto_state, keeping it\n",0,$section);
        ($required_bits,$minbits) = @save;
        @allstates = @allstates_save;
      }
    }
  }

  unless ($states_assigned_without_undefined_goto_state) {
    # Call &assign_states_hero to do the state assignment
    &assign_states_hero;
  }

  # Rename variables to standard names
  $total_statebits = $bits;
  $widths{$statevar} = $total_statebits;
  $widths{$nextstatevar} = $total_statebits;
  %state_val2name = %$v2nref;

} elsif ($encoding eq "onehot") {

  # Using undefined_states_go_to in onehot is likely to result in a very poor
  # implementation.  To keep the encoding independent of the state machine,
  # just refuse to do this unless user has provided command line overrride.
  if ($use_undefined_goto_state && !$force_undefined_goto_in_onehot) {
    &warning($indent,"undefined_states_go_here attribute is set.  With onehot encoding this is likely to result in a very poor implementation, so the attribute is being ignored.  You can force it using -force_undefined_goto_in_onehot.\n");
    $use_undefined_goto_state = 0;
    # Suppress the state if it has no transition into it and it is not reset
    foreach $trans (keys %transitions) {
      if ($transitions{$trans}{"endState"} eq $undefined_goto_state) {
        #&debug("Exiting undefined check because found a transition into state $undefined_goto_state\n",0,$section);
        &debug("Found input transition to undefined_goto_state $undefined_goto_state - leaving it in\n",0,$section);
        $has_input_transition = 1;
        last;
      }
    }
    if (!$has_input_transition && !($undefined_goto_state eq $reset_state)) {
      @allstates = &remove_from_array($undefined_goto_state, @allstates);
    }
  }

  # Check for errors

  # No graycodes
  foreach $trans (keys %transitions) {
    if ($transitions{$trans}{attributes}{graycode}{value} || $transitions{$trans}{attributes}{graytransition}{value}) {
      &error($indent,"Cannot have graycode transitions with onehot encoding");
    }
  }

  # Reset state must not be allones or allzeros
  if ($globals{machine}{reset_state}{type} =~ /(allones)|(allzeros)/) {
    &error($indent,"Cannot have reset_state type of allones or allzeros with onehot encoding.  Try changing to \"anyvalue\"");
  }

  $total_statebits = scalar(keys %states); 
  $widths{$statevar} = $total_statebits;
  $widths{$nextstatevar} = $total_statebits;
  #$statebit = $total_statebits - 1 ; # init statebit pointer 
  $statebit = 0;
  #foreach $state (keys %states) {
  foreach $state (@allstates) {
    $state_val2name{$statebit} = $state;
    $statebit++;
  }
  $zero_padded_one = sprintf("%s'b%s%s",$total_statebits,"0" x ($total_statebits - 1), "1");

} else {
  &error($indent,"undefined encoding $encoding\n");
}

$last_statebit = $total_statebits - 1;

# Figure out longest state name for formatting purposes
foreach $state (@allstates) {
  $statename_length = length($state) if (length($state) > $statename_length);
}
# length of "state[IDLE]"
$statename_length_onehot = $statename_length + length($statevar) +2; 
$nextstatename_length_onehot = $statename_length + length($nextstatevar) +2; 
$level4indent = $indentstring x 4;
#eval "\$level4indent = \"$indentstring\" x 4";


#######################
# module open
#######################

# Handle top-of-file comments
if ($include_at_top_of_file = $globals{machine}{include_at_top_of_file}{value}) {
  open(FILE,"<$include_at_top_of_file") or &warning($indent,"Could not open file $include_at_top_of_file specified by attribute include_at_top_of_file");
  while (<FILE>) {
    print;
  }
}

# Insert string at top of file if requested
&insert_text("insert_at_top_of_file");

# Output version data
if ($addversion) {
  ($sec,$min,$hour,$day, $month, $year) = (localtime)[0..5];
  $date = sprintf("%04d:%02d:%02d",$year+1900,$month+1,$day);
  $time = sprintf("%02d:%02d:%02d",$hour,$min,$sec);
  &print($indent,"${comm} Created by $me version $scriptversion on $date at $time (www.fizzim.com)\n\n");
  if ($be_options) {
    &print($indent,"${comm} The following options were set in the be_cmd attribute and will be used unless overridden on the command line: $be_options\n\n");
  }
  #&print(0,"\n");
}

# Handle module comments
if ($globals{machine}{name}{comment}) {
  $comment = " ${comm} $globals{machine}{name}{comment}";
} else {
  $comment = "";
}

# Print module line
if ($language eq "vhdl") {
  &print($indent,"library ieee;\n");
  &print($indent,"use IEEE.STD_LOGIC_1164.all;\n");
  &print($indent,"use IEEE.STD_LOGIC_UNSIGNED.all;\n");
  &print(0,"\n");
  &print($indent,"entity $globals{machine}{name}{value} is\n");
  &print($indent++,"port (\n");
} else {
  if (%parameters) { # If parameters are present, take care of them
    &print($indent++,"module $globals{machine}{name}{value} $comment\n");
    &print($indent++,"#(\n");
    foreach $param (sort keys %parameters) {
      &print($indent,"parameter $param = $parameters{$param},\n");
    }
    # Remove , in final line
    &strip_comma_from_last_line;
    &print(--$indent,")(\n");
  } else {
    &print($indent++,"module $globals{machine}{name}{value} ($comment\n");
  }
  # Insert string in module declaration if requested
  &insert_text("insert_in_module_declaration");
}

# outputs
foreach $output (sort keys %{ $globals{outputs} }) {
  # Check for reserved words
  if (exists $reserved_words{$output}) {
    &error($indent,"
Cannot use reserved word \"$output\" as an output.
If you are trying to bring out the internal variables, look at the stateout and nextstateout attributes.
If you have an actual output of this name, try using the -statevar, -nextstatevar, or -statenamevar switches to rename the internal signal to avoid the conflict.

");
  }

  # skip the rest if suppress_portlist is set
  next if ($globals{outputs}{$output}{useratts} =~ /suppress_portlist/);

  &print($indent,&create_wirereg_statement("output ",$output,",",$io_statement_length));
}

# Handle stateout, nextstateout (hooks to allow state and nextstate to
# be outputs of the module - see documentation).
foreach $att ("stateout", "nextstateout") {
  ($var = $att) =~ s/out//;  # turns "stateout" into "state"
  $var = eval "\$${var}var"; # makes var = $statevar
  if ($stateout = $globals{"machine"}{$att}{"value"}) {
    # Check to make sure no bounds given
    if ($stateout =~ /\[|\]/) {
      &error($indent,"stateout signal \"$stateout\" should not have bit field - this will be determined automatically");
    }
    if ($language eq "verilog" || $language eq "vhdl") {
      if ($stateout eq $var) {
        # suppress [0:0]
        if ($last_statebit == 0) {
          if ($language eq "vhdl") {
            &print($indent,"$stateout : out $vhdl_vector_type;\n");
          } else {
            &print($indent,"output reg $stateout,\n");
          }
        } else {
          if ($language eq "vhdl") {
            &print($indent,"$stateout : out $vhdl_vector_type($last_statebit downto 0);\n");
          } else {
            &print($indent,"output reg [$last_statebit:0] $stateout,\n");
          }
        }
      } else {
        # suppress [0:0]
        if ($last_statebit == 0) {
          if ($language eq "vhdl") {
            &print($indent,"$stateout : out $vhdl_vector_type;\n");
          } else {
            &print($indent,"output wire $stateout,\n");
          }
        } else {
          if ($language eq "vhdl") {
            &print($indent,"$stateout : out $vhdl_vector_type($last_statebit downto 0);\n");
          } else {
            &print($indent,"output wire [$last_statebit:0] $stateout,\n");
          }
        }
      }
    } elsif ($language eq "systemverilog") {
      if ($stateout eq $var) {
        &error($indent,"Cannot use state or nextstate variables as module ports in SystemVerilog - you must rename them.  See documentation for details.");
      } else {
        &print($indent,"output logic [$last_statebit:0] $stateout,\n");
      }
    }
  }
}

# Inputs (still doing the module statement...)
if ($language eq "verilog") {
  $type = "wire";
} elsif ($language eq "systemverilog") {
  $type = "logic";
}

foreach $input (sort keys %{ $globals{inputs} }) {
  # Check for reserved words
  if (exists $reserved_words{$input}) {
    &error($indent,"
Cannot use reserved word \"$input\" as an input.
If you have an actual input of this name, try using the -statevar, -nextstatevar, or -statenamevar switches to rename the internal signal to avoid the conflict.

");
  }
  # Handle comments
  if ($globals{inputs}{$input}{comment}) {
    $comment = "${comm} $globals{inputs}{$input}{comment}";
  } else {
    $comment = "";
  }

  # Handle multibit inputs
  if (($name,$range) = ($input =~ /^([^\[]+)\s*(\[.*\])/)) {
    if ($language eq "vhdl") {
      &print($indent,sprintf("%-${io_statement_length}s %-s\n","$name : in ${vhdl_vector_type}(" . &vhdl_range($range) . ");",$comment));
    } else {
      &print($indent,sprintf("%-${io_statement_length}s %-s\n","input $type $range $name,",$comment));
    }
  } else {
    if ($language eq "vhdl") {
      &print($indent,sprintf("%-${io_statement_length}s %-s\n","$input : in ${vhdl_bit_type};",$comment));
    } else {
      &print($indent,sprintf("%-${io_statement_length}s %-s\n","input $type $input,",$comment));
    }
  }
  # Remove trailing blanks
  $pbuf[$#pbuf] =~ s/\s*\n$/\n/;
}

# print last line of module statement and close
if ($language eq "vhdl") {
  if ($pbuf[$#pbuf] =~ s/^(.*);/\1/) {
    $pbuf[$#pbuf] = $pbuf[$#pbuf] . ");\n"; # Add ); on a new line
    &print(--$indent,"end $globals{machine}{name}{value};\n");
    &print($indent,"\narchitecture fizzim of $globals{machine}{name}{value} is\n");
  } else {
    &error($indent,"No inputs or outputs specified");
  }
} else {
  if (&strip_comma_from_last_line) {
    $pbuf[$#pbuf] = $pbuf[$#pbuf] . ");\n"; # Add ); on a new line
  } else {
    &error($indent,"No inputs or outputs specified");
  }
}
&print(0,"\n");

# Insert string at top of module if requested
&insert_text("insert_at_top_of_module");

#######################
# Print out state parameter definition
#######################
#&print($indent,"\n\n");
&print($indent,"${comm} state bits\n");

# The systemverilog output in onehot is a slightly different format
# than the verilog output in onehot.  Both use indexing, but the sv version
# assigns the state values to full vectors (with one bit set) rather than
# setting one bit in a zero vector.  This is based on Cliff Cumming's 
# preferred coding style, and allows for the default_state_is_x functionality
# in onehot.
#
# 4/30/2009: Changed the default_state_is_x verilog onehot to work like the
# systemverilog version, but with verilog syntax.  That means that the output
# using verilog will look very different depending on whether default_state_is_x# is set or not.

# To accomplish this, sv+onehot requires an extra enum:
if ($language eq "systemverilog" && $encoding eq "onehot") {
  &print($indent++,"enum {\n");
  foreach $state (@allstates) {
    &print($indent,"${state}_BIT,\n");
  }
  # Remove , 
  &strip_comma_from_last_line;
  # print close of enum
  &print(--$indent,"} $indexvar;\n\n");

# And verilog, onehot, defaultx requires an extra parameter statement:
} elsif ($language eq "verilog" && $encoding eq "onehot" && $globals{machine}{default_state_is_x}{value}) {
  &print($indent,"parameter\n");
  foreach $state (@allstates) {
    # Search through all assigned values looking for the one that matches $state
    foreach $val (keys %state_val2name) {
      if ($state_val2name{$val} eq $state) {
        &print($indent,"${state}_BIT = $val,\n");
      }
    }
  }
  # Change , in final line to ;
  $pbuf[$#pbuf] =~ s/,/;/;
  &print(0,"\n");
}


if ($encoding eq "none") {
  if ($language eq "verilog") {
    &error("Cannot use encoding none in verilog");
  } elsif ($language eq "systemverilog") {
    #&error("encoding none not yet supported in systemverilog");
    $temp = &required_bits($#allstates + 1);
    $temp = $temp - 1;
    &print($indent++,"enum logic [$temp:0] {\n");
    foreach $state (@allstates) {
      &print($indent,"$state,\n");
    }
  } elsif ($language eq "vhdl") {
    &print($indent++,"type state_type is (\n");
    foreach $state (@allstates) {
      &print($indent,"$state,\n");
    }
  }
} else {
  if ($language eq "verilog") {
    # DC gives a warning that not all tool support bounds on parameters, so
    # don't use them.
    #&print($indent,"parameter [$last_statebit:0]\n");
    &print($indent,"parameter \n");
  } elsif ($language eq "systemverilog") {
    # SV uses enumerated types instead of parameters, but many of the details
    # are the same.
    &print($indent,"enum logic [$last_statebit:0] {\n");
    $indent++; # indent the enum values
  } elsif ($language eq "vhdl") {
    &print($indent,"subtype state_type is ${vhdl_vector_type}($last_statebit downto 0);\n\n");
  }
  
  
  # Print out state encoding in @allstates order
  foreach $state (@allstates) {
    $comment = "";
    # Search through all assigned values looking for the one that matches $state
    foreach $val (keys %state_val2name) {
      if ($state_val2name{$val} eq $state) {
        # If there are output encoded ($regbits is not 0), prepare a comment
        # that shows the value of each output in this state.
        if ($regbits) {
          $val_length = length($val);
          foreach $regout (sort {
            $reg_outputs{$a}{statebit_lower} <=>
            $reg_outputs{$b}{statebit_lower}
          } keys %reg_outputs) {
            $lower = $reg_outputs{$regout}{statebit_lower};
            $upper = $reg_outputs{$regout}{statebit_upper};
            $bitvalue = substr($val,$val_length - $upper - 1,$upper - $lower + 1);
            $comment = "$regout=${bitvalue} ${comment}";
          }
  
          # Do extra bits
          if ($val_length - 1 > $upper) {
            #$bitvalue = substr($val,($val_length - 1) - ($upper + 1),$val_length - $upper - 1);
            $bitvalue = substr($val,0,$val_length - $upper - 1);
            $comment = "extra=${bitvalue} ${comment}";
          }
  
          $comment = "${comm} $comment";
        } # end of encoded output comment code
  
  
        if ($encoding eq "heros") {
          if ($language eq "vhdl") {
            &print($indent,sprintf("%-${statename_length}s%s","constant $state",": state_type:=\"${val}\"; ${comment}\n"));
          } else {
            &print($indent,sprintf("%-${statename_length}s%s","$state"," = ${total_statebits}'b${val}, ${comment}\n"));
          }
  
        } elsif ($encoding eq "onehot") {
          if ($language eq "systemverilog" || ($language eq "verilog" && ($globals{machine}{default_state_is_x}{value} || exists $globals{machine}{default_state}{value}))) {
            &print($indent,sprintf("%-${statename_length}s%s","$state"," = ${total_statebits}\'b1<<${state}_BIT, ${comment}\n"));
          } elsif ($language eq "verilog") {
            &print($indent,sprintf("%-${statename_length}s%s","$state"," = ${val}, ${comment}\n"));
          } elsif ($language eq "vhdl") {
            $binvalue = &convert2bin(2**$val);
            $binvalue = sprintf("%0${total_statebits}s",$binvalue);
            &print($indent,sprintf("%-${statename_length}s%s","constant $state",": state_type:=\"${binvalue}\";${comment}\n"));
            &print($indent,sprintf("%-${statename_length}s%s","constant ${state}_BIT",": natural:=${val};${comment}\n"));
          }
        }
      }
    }
  }
}
  
# Add XXX state if required
if ($language eq "verilog") {
  if ($encoding eq "onehot" && $globals{machine}{default_state_is_x}{value}) {
    #&print($indent, sprintf("XXX = %s\'b%s;\n",$total_statebits,"x" x $total_statebits));
    &print($indent, sprintf("%-${statename_length}s = %s\'b%s;\n","XXX",$total_statebits,"x"));
  } else {
    # Substitute ; for , and print last line.
    $pbuf[$#pbuf] =~ s/^(.*),/\1;/;
  }
} elsif ($language eq "systemverilog") {
  # Print line
  if ($globals{machine}{default_state_is_x}{value}) {
    # Add XXX
    &print($indent, "XXX = \'x\n");
  } else {
    &strip_comma_from_last_line;
  }
  # Add closing of enum statement
  &print(--$indent, "} $statevar, $nextstatevar;\n");
} elsif ($language eq "vhdl") {
  if ($encoding eq "none") {
    # remove  , on last line and add close
    &strip_comma_from_last_line;
    &print(--$indent,");\n");
  }
  # Print line
  if ($globals{machine}{default_state_is_x}{value}) {
    # Add XXX
    $val = "X" x $total_statebits;
    &print($indent, "constant XXX: state_type:=\"$val\";\n");
  } else {
  }
}

# If undefined_states_go_to check caused state to be dropped, add comment
if ($states_assigned_without_undefined_goto_state) {
  &print($indent,"${comm} Note: State $undefined_goto_state (with undefined_states_go_here attribute) dropped because it had no transitions into it and the state map was full without it.  Use -force_keep_undefined_goto_state to override this.\n");
}


#######################
# state, nextstate reg lines
#######################
&print(0, "\n");
# Handle stateout, nextstateout attributes
if ($language eq "vhdl") {
  foreach $att ("stateout", "nextstateout") {
    ($var = $att) =~ s/out//;  # turns "stateout" into "state"
    $varname = "${var}var"; # varname will be "statevar" or "nextstatevar"
    $var = eval "\$${var}var"; # makes var = $statevar
    if ($stateout = $globals{"machine"}{$att}{"value"}) { # if attribute is set
      if ($stateout eq $var) { # if state output names is the same as the state name
        # create the remap line for later use (like "state <= state_internal")
        push(@vhdl_remap_lines,"$var <= ${var}${vhdl_intvar_suffix};\n"); 
        # Change the var to be the new name
        #print "\$$varname = ${var}${vhdl_intvar_suffix}\n";
        eval "\$$varname = ${var}${vhdl_intvar_suffix}";
        # create width entry
        eval "\$widths\{\"${var}${vhdl_intvar_suffix}\"\} = \$widths\{\"$var\"\}";
      } else {
        # create the remap line for later use (like "state <= state_internal")
        push(@vhdl_remap_lines,"$stateout <= ${var};\n"); 
      }
    } else {
      # No attribute set - print out reg statement.  For SV, do nothing (declared
      # as logic type in the enum statement).
      if ($language eq "verilog") {
        # suppress [0:0]
        if ($last_statebit == 0) {
          &print($indent, "reg $var;\n");
        } else {
          &print($indent, "reg [$last_statebit:0] $var;\n");
        }
      }
    }
  }
  &print($indent, "signal $statevar,$nextstatevar: state_type;\n");
} else {
  foreach $att ("stateout", "nextstateout") {
    ($var = $att) =~ s/out//;  # turns "stateout" into "state"
    $var = eval "\$${var}var"; # makes var = $statevar
    if ($stateout = $globals{"machine"}{$att}{"value"}) {
      if ($stateout eq $var) {
        # If stateout is set the same as $statevar, then the reg statement was 
        # done in the module header.  Do nothing.
      } else {
        # If stateout is NOT set the same as $statevar, then the module statement
        # had a "wire" for stateout and we need to create the reg statement
        # here and an assign of the wire to the reg.
        if ($language eq "verilog") {
          # suppress [0:0]
          if ($last_statebit == 0) {
            &print($indent, "reg $var;\n");
          } else {
            &print($indent, "reg [$last_statebit:0] $var;\n");
          }
        } elsif ($language eq "systemverilog") {
          # No need to do anything for sv.  The state bits are enum types
          #&print($indent, "logic [$last_statebit:0] $var;\n");
        }
        &print($indent, "assign $stateout = $var;\n");
      }
    } else {
      # No attribute set - print out reg statement.  For SV, do nothing (declared
      # as logic type in the enum statement).
      if ($language eq "verilog") {
        # suppress [0:0]
        if ($last_statebit == 0) {
          &print($indent, "reg $var;\n");
        } else {
          &print($indent, "reg [$last_statebit:0] $var;\n");
        }
      }
    }
  }
}

#######################
# reg lines for outputs with suppress_portlist
#######################
foreach $output (sort keys %{ $globals{outputs} }) {
  next if (!($globals{outputs}{$output}{useratts} =~ /suppress_portlist/));
  &print($indent,&create_wirereg_statement("",$output,";",$max_io_length + length("reg ")));
}

#######################
# reg lines for flag "next_..." variables
#######################
foreach $flag (sort keys %flags) {
  if ($language eq "vhdl") {
    $line = &translate_signal_statement_to_vector_if_required("signal next_${flag}: ${vhdl_bit_type};\n");
    &print($indent,$line);
  } else {
    $line = &create_wirereg_statement("",$flag,";",$max_io_length + length("reg next"));
    $flag =~ s/\s*\[\S+:\S+\]\s*//; # get rid of range field
    $line =~ s/$flag/next_${flag}/;
    $line =~ s/;\s*\/\/.*$/;/; # get rid of comment
    &print($indent,$line);
  }
}

foreach $port (sort keys %port2var) {
  &print($indent,&translate_signal_statement_to_vector_if_required("signal $port2var{$port}: ${vhdl_bit_type};\n"));
}


# Insert string ahead of comb block if requested
&insert_text("insert_ahead_of_comb_block");

#######################
# Output combinational block
#######################
$section = "comb_block";
&print(0, "\n");
&print($indent, "${comm} comb always block\n");
if ($language eq "verilog") {
  &print($indent++, "always @* begin\n");
} elsif ($language eq "systemverilog") {
  &print($indent++, "always_comb begin\n");
} elsif ($language eq "vhdl") {
  &print($indent++, "begin\n");
  &print($indent++, "COMB: process(");
  &print(0,"$statevar,");
  foreach $input (sort keys %{ $globals{inputs} }) {
    unless ($input eq $clk || $input eq $reset_signal) {
      &print(0,&translate_range_to_vhdl_if_required($input) . ",");
    }
  }
#  foreach $port (sort keys %port2var) {
#    &print(0,&translate_range_to_vhdl_if_required($port2var{$port}) . ",");
#  }
  &strip_comma_from_last_line;
  &print(0,") begin\n");
  $comb_process_ptr = $#pbuf;
}

# Defaults
if ($encoding eq "heros" || $encoding eq "none") {
  # For heros, defaults ahead of case depend on fsm attributes
  if (!($globals{machine}{implied_loopback}{value} || $globals{machine}{default_state_is_x}{value})) {
    &warning($indent,"Neither implied_loopback nor default_state_is_x attribute is set on state machine - defaulting to implied_loopback to avoid latches being inferred");
    $globals{machine}{implied_loopback}{value} = 1;
  }
  if ($globals{machine}{implied_loopback}{value}) {
    &print($indent, "$nextstatevar = $statevar; ${comm} default to hold value because implied_loopback is set\n");
    &translate_last_line_to_vhdl_if_required("assignment");
  } elsif ($globals{machine}{default_state_is_x}{value}) {
    if ($language eq "verilog") {
#      &print($indent, sprintf("$nextstatevar = ${total_statebits}\'b%s; ${comm} default to x because default_state_is_x is set\n","x"));
      &print($indent, sprintf("$nextstatevar = ${total_statebits}\'b%s; ${comm} default to x because default_state_is_x is set\n","x" x ${total_statebits}));
    } elsif ($language eq "systemverilog") {
      &print($indent, sprintf("$nextstatevar = XXX; ${comm} default to x because default_state_is_x is set\n"));
    } elsif ($language eq "vhdl") {
      &print($indent, sprintf("$nextstatevar <= XXX; ${comm} default to x because default_state_is_x is set\n"));
      &translate_last_line_to_vhdl_if_required("assignment");
    }
  } else {
    &error("Internal error - cannot get here...");
  }

} elsif ($encoding eq "onehot") {
#  if ($language eq "systemverilog" || ($language eq "verilog" && $globals{machine}{default_state_is_x}{value})) {
#    &print($indent, sprintf("$nextstatevar = XXX; ${comm} default to x because default_state_is_x is set\n"));
#  } elsif ($language eq "verilog") {
#    &print($indent, sprintf("$nextstatevar = ${total_statebits}\'b%s;\n","0" x $total_statebits));
#  }
  if ($language eq "vhdl") {
    #&print($indent, sprintf("$nextstatevar = $statevar; ${comm} default to current state in onehot vhdl\n"));
    #&translate_last_line_to_vhdl_if_required("assignment");
    #$zerovalue = sprintf("%0${total_statebits}s",0);
    $zerovalue = &stringzero($nextstatevar,"-quote_unless_translate");
    &print($indent,"$nextstatevar = $zerovalue;\n");
    &translate_last_line_to_vhdl_if_required("assignment");
  } elsif ($globals{machine}{default_state_is_x}{value}) {
    &print($indent, sprintf("$nextstatevar = XXX; ${comm} default to x because default_state_is_x is set\n"));
    &translate_last_line_to_vhdl_if_required("assignment");
  } elsif ($language eq "verilog") {
    &print($indent, sprintf("$nextstatevar = ${total_statebits}\'b%s;\n","0" x $total_statebits));
  }
}

# Combinational defaults.  
# To get better synth results "out of the box", use a 0 default if no
# default is defined.  To get the old "hold value" 
# behavior, it will be necessary for the user to set the default to 
# be the variable name (ie, if the variable name is "foo", make the default
# value "foo").
foreach $combout (sort keys %comb_outputs) {
  if (exists $comb_outputs{$combout}{default_value} && ($comb_outputs{$combout}{default_value} ne "") ) {
    $comb_defaults{$combout} = $comb_outputs{$combout}{default_value};
    &print($indent,"$combout = $comb_defaults{$combout}; ${comm} default\n");
    &translate_last_line_to_vhdl_if_required("assignment");
    # If this requires a rename, do it
    $temp = &translate_port2var_in_first_instance($combout,$combout);
    if ($temp ne $combout) {
      $temp = &translate_range_to_vhdl_if_required($temp);
      $pbuf[$comb_process_ptr] =~ s/\) begin/,$temp) begin/ if ($language eq "vhdl"); # insert into sens list
    }
  } else {
    $comb_defaults{$combout} = "0"; # Save for later
    $comb_default = &stringzero($combout,"-quote_unless_translate");
    &warning($indent,"Combinational output $combout has no default value - using 0");
    &print($indent,"$combout = $comb_default; ${comm} default to zero for better synth results (no default set in .fzm file)\n");
    &translate_last_line_to_vhdl_if_required("assignment");
    # If this requires a rename, do it
    $temp = &translate_port2var_in_first_instance($combout,$combout);
    if ($temp ne $combout) {
      $temp = &translate_range_to_vhdl_if_required($temp);
      $pbuf[$comb_process_ptr] =~ s/\) begin/,$temp) begin/ if ($language eq "vhdl"); # insert into sens list
    }
    #&print($indent,"$combout = $widths{$combout}'b$comb_defaults{$combout}; ${comm} default to zero for better synth results (no default set in .fzm file)\n");
  }
}

# Do flags.  They always default to their current value.
foreach $flag (sort keys %flags) {
  $flag_defaults{$flag} = $flags{$flag}{default_value};
  if (exists($port2var{$flag})) {
    &print($indent,"next_$flag = $port2var{$flag};\n");
  } else {
    &print($indent,"next_${flag} = $flag;\n");
  }
  &translate_last_line_to_vhdl_if_required("assignment");
  $temp = &translate_range_to_vhdl_if_required(&translate_port2var_in_first_instance($flag,$flag));
  $pbuf[$comb_process_ptr] =~ s/\) begin/,$temp) begin/ if ($language eq "vhdl"); # insert into sens list
}

# State "case" block
&print_case_statement($statevar);
#&assertion($indent,"${comm} Test assertions");

# Output state code in @allstates order
$default_inserted = 0;
foreach $state (@allstates) {
  if ($encoding eq "onehot") {
    $keep_state = 1; # always keep all cases in onehot
  } else {
    $keep_state = 0; # flag to keep this state
  }
  %non_default_on_state_value_found = (); # hash to keep track of mealy/flag info
  $endif = 0; # flag for vhdl to print endif
  if ($states{$state}{attributes}{comment}) {
    $comment = " ${comm} $states{$state}{attributes}{comment}";
  } else {
    $comment = "";
  }
  if ($encoding eq "heros" || $encoding eq "none") {
    #&print($indent++,"$state:" . " begin${comment}\n");
    &print($indent++,sprintf("%-${statename_length}s:",$state) . " begin${comment}\n");
    &translate_last_line_to_vhdl_if_required("case item");

  } elsif ($encoding eq "onehot") {
    if ($language eq "systemverilog" || ($language eq "verilog" && $globals{machine}{default_state_is_x}{value})) {
      &print($indent++,sprintf("%-${statename_length_onehot}s:","$statevar\[${state}_BIT\]") . " begin${comment}\n");
    } elsif ($language eq "verilog") {
      &print($indent++,sprintf("%-${statename_length_onehot}s:","$statevar\[$state\]") . " begin${comment}\n");
    } elsif ($language eq "vhdl") {
      &print($indent++,"if ($statevar(${state}_BIT) = '1') then" . "${comment}\n");
    }
  }
  # Search state attributes for outputs.  If a combinational or flag output is 
  # found, assign its value here (maybe).  
  foreach $comb_output (sort keys %comb_outputs) {
    if (exists $states{$state}{attributes}{$comb_output}{value}) {
      $value = $states{$state}{attributes}{$comb_output}{value};
    } else {
      $value = $comb_outputs{$comb_output}{default_value};
    }

    # Check to see if output is assigned on transitions.  If so, check to see
    # if a non-default value has been assigned in this state.  
    # If so, output assignment statement, set flag and issue warning.
    if ($in_mealy_on_transitions_mode{$comb_output}) {
      if ($value ne $comb_outputs{$comb_output}{default_value}) {
        # If output has a non-default value in this state, warn user and print it anyway
        &warning($indent,"Combinational output $comb_output is assigned on transitions, but has a non-default value \"$value\" in state $state");
        # Check that resulting value is non-null
        &error($indent,"Value of comb output $comb_output cannot be determined in state $state") if ($value eq "");
        # Print the default statement anyway if it doesn't match default from above
        unless ($value eq $comb_defaults{$comb_output}) {
          &print($indent,"$comb_output = $value;\n");
          &translate_last_line_to_vhdl_if_required("assignment");
        }
        # Set flag to suppress non-default on-transition values in this state
        $non_default_on_state_value_found{$comb_output} = $value;
      }
    } else {
      # NOT in mealy-on-transitions mode, so print the default statement
      # Check that resulting value is non-null
      &error($indent,"Value of comb output $comb_output cannot be determined in state $state") if ($value eq "");
      # Only print the default statement if the comb output is NOT assigned 
      # on transitions (and it doesn't match default above)
      unless ($value eq $comb_defaults{$comb_output}) {
        &print($indent,"$comb_output = $value;\n");
        &translate_last_line_to_vhdl_if_required("assignment");
      }
    }
  }

  # Now do flags.  
  foreach $flag (sort keys %flags) {
    if ($states{$state}{attributes}{$flag}{value} ne "") {
      # Check to see if flag is assigned on transitions.  If so, and it is
      # assigned in this state, set flag and issue warning.
      #print "debug: looking at ifot mode for flag $flag\n";
      if ($in_flag_on_transitions_mode{$flag}) {
        &warning($indent,"Flag $flag is assigned on transitions, but is also assigned value \"$states{$state}{attributes}{$flag}{value}\" in state $state");
      }
      # output assignment statement
      &print($indent,"next_${flag} = $states{$state}{attributes}{$flag}{value};\n");
      &translate_last_line_to_vhdl_if_required("assignment");
    }
  }

  # Search transitions for all those starting on this state.  
  # Sort by priority, then output vlog if/elseif/else
  # equation to switch to correct endState.
  @transitions_from_this_state = ();
  foreach $trans (keys %transitions) {
    if ($transitions{$trans}{"startState"} eq $state) {
      &debug("Transition $trans starts on state $state, adding to array",0,$section);
      push(@transitions_from_this_state,$trans);
    }
  }

  # nextstate determination
  # Sort by priority, followed by equation.  The equation part ensures
  # that "1" will be last.
  @transitions_from_this_state = sort sort_by_priority_then_equation_equal_1 @transitions_from_this_state;
  #print("After sort, transitions_from_this_state for state $state is @transitions_from_this_state\n");
  &debug("After sort, transitions_from_this_state for state $state is @transitions_from_this_state",0,$section);

  # Check for problems with priority
  undef %priorities_seen;
  if ($#transitions_from_this_state > 0) {
    for (my $i = 0; $i<=$#transitions_from_this_state; $i++) {
      $trans = $transitions_from_this_state[$i];
      $equation = $transitions{$trans}{attributes}{equation}{value};
      $equation =~ s/\n//g; # remove embedded cr's
      # If no priority exists, warn unless
      # 1) this is the last transition and equation is "1"
      # 2) There are only two transitions, and the last one has equation of "1"
      if (!($pri = $transitions{$trans}{attributes}{priority}{value})) {
          #print "looking at $trans: $i of $#transitions_from_this_state\n";
          #print "equation: $equation\n";
        unless (
          # last and equation is 1
          ($equation eq "1" &&
           $i == $#transitions_from_this_state)
          ||
          # Exactly 2 transitions and last has equation of 1
          ($#transitions_from_this_state == 1 &&
           $transitions{$transitions_from_this_state[$#transitions_from_this_state]}{attributes}{equation}{value} eq "1")
         ) {
          &warning($indent,"State $state has multiple exit transitions, and transition $trans has no defined priority");
        }
      } else {
        # If priority exists, but it's a duplicate, warn unless THIS transition
        # has an equation of 1 (note that sort means that if two transitions 
        # have the same priority, the one with an equation of 1 will come last).
        if (exists $priorities_seen{$pri}) {
          unless ($equation eq "1") {
            &warning($indent,"State $state has multiple exit transitions, and transition $trans has the same priority as transition $priorities_seen{$pri}");
          }
        } else {
          $priorities_seen{$pri} = $trans;
        }
      }
    }
  }

  $no_transitions_from_this_state = 1; # temp flag
  @ifs = ();
  for (my $i = 0; $i<=$#transitions_from_this_state; $i++) {
    @assignments = ();
    $no_transitions_from_this_state = 0; # temp flag
    $trans = $transitions_from_this_state[$i];
    $equation = $transitions{$trans}{attributes}{equation}{value};
    $equation =~ s/\n//g; # remove embedded cr's
    #print "trans is $trans\n";
    #print STDERR "equation is $equation\n";
    $equation = 1 if ($equation eq ""); # force null string to "1"
    $eq1seen = 0;

    # Check for unreachable transitions.  If the equation is 1 and 
    # there are more transitions following, it's an error.
    if ($equation eq "1" && $#transitions_from_this_state > $i) {
      &error($indent,"State $state has an always true equation for transition $trans, but has lower priority transitions as well");
    }

    $nextstate = $transitions{$trans}{endState};
    # Handle transition comments
    if ($transitions{$trans}{attributes}{comment}) {
      $comment = " ${comm} $transitions{$trans}{attributes}{comment}";
    } else {
      $comment = "";
    }
    if ($encoding eq "heros" || $encoding eq "none") {
      foreach $graystate (@{ $graytransitions{$state} }) {
        if ($graystate eq $nextstate) {
          $comment = $comment . " ${comm} graycoded";
        }
      }
      push(@assignments,"$nextstatevar = $nextstate;$comment\n");
    } elsif ($encoding eq "onehot") {
      if ($language eq "systemverilog" || ($language eq "verilog" && $globals{machine}{default_state_is_x}{value})) {
        push(@assignments,"$nextstatevar = $nextstate;$comment\n");
      } elsif ($language eq "verilog") {
        push(@assignments,"$nextstatevar\[$nextstate\] = 1'b1;$comment\n");
      } elsif ($language eq "vhdl") {
        if ($translate2vhdl) {
          push(@assignments,"$nextstatevar(${nextstate}_BIT) = 1;$comment\n");
        } else {
          push(@assignments,"$nextstatevar(${nextstate}_BIT) = '1';$comment\n");
        }
      }
    }

    # Add comb outputs and flags
    foreach $att (keys %{ $transitions{$trans}{attributes} }) {
      if ($transitions{$trans}{attributes}{$att}{type} eq "output") {
        if (exists $comb_outputs{$att}) {
          # comb outputs
          # Check if specific value exists for this comb output on this transition
          if (exists $transitions{$trans}{attributes}{$att}{value} && 
             ($value = $transitions{$trans}{attributes}{$att}{value} ne "")
          ) {
            $value = $transitions{$trans}{attributes}{$att}{value};
          # if not, set to default
          } else {
            $value = $comb_outputs{$att}{"default_value"};
          }

          # Check that resulting value is non-null
          &error($indent,"Value of comb output $att cannot be determined on transition $trans") if ($value eq "");
          # Print if non-default state value was found and this value doesn't match either it or the default
          # or if no non-default state value was found and this value doesn't match the comb default
          #&print($indent,"debug: nondef: \"$non_default_on_state_value_found{$att}\" value: \"$value\" combdef: \"$comb_outputs{$att}{default_value}\"\n");
          if (exists $non_default_on_state_value_found{$att} 
                && ($value ne $non_default_on_state_value_found{$att})
                && ($value ne $comb_outputs{$att}{default_value})
            || !exists $non_default_on_state_value_found{$att} 
                && ($value ne $comb_outputs{$att}{default_value})
            ) {
              push(@assignments,"$att = $value;\n");
          }
        }
        if (exists $flags{$att}) {
          # flags
          if ($transitions{$trans}{attributes}{$att}{value} ne "") {
            push(@assignments,"next_${att} = $transitions{$trans}{attributes}{$att}{value};\n");
          }
        }
      }
    }

    if ($equation eq "1") {
      $eq1seen = 1;
    }
    #print "pushing \"$equation\"\n";
    push(@ifs,[$equation,@assignments]);
  }
  # implied_loopback is handled differently for onehot.  Instead of
  # inserting a default value of "state" ahead of the case statement, we
  # need to actually add explicit "else..." clauses for any transition
  # that isn't a "1".  
  # Also, watch for the case where there are NO
  # transitions out of the state - we need to add the extra code to stay
  # put there as well.
  #
  # Use this same code to insert an XXX clause for vhdl
  if ($encoding eq "onehot") {
    @assignments = ();
    $equation = 1;
    if (!$eq1seen || $no_transitions_from_this_state) {
      if (!($globals{machine}{implied_loopback}{value} || $globals{machine}{default_state_is_x}{value})) {
        &warning($indent,"Neither implied_loopback nor default_state_is_x attribute is set on state machine - defaulting to implied_loopback to avoid latches being inferred");
        $globals{machine}{implied_loopback}{value} = 1;
      }
      # Note that implied loopback has priority over default_state_is_x
      if ($globals{machine}{implied_loopback}{value}) {
#        if ($no_transitions_from_this_state) {
#        } else {
#          &print($indent++,"else begin\n");
#        }
        if ($language eq "systemverilog") {
          push(@assignments,"$nextstatevar = $state; ${comm} Added because implied_loopback is true\n");
        } elsif ($language eq "verilog") {
          push(@assignments,"$nextstatevar\[$state\] = 1'b1; ${comm} Added because implied_loopback is true\n");
        } elsif ($language eq "vhdl") {
          push(@assignments,"$nextstatevar = $state; ${comm} Added because implied_loopback is true\n");
        }
#        if ($no_transitions_from_this_state) {
#        } else {
#          &print(--$indent,"end\n"); 
#        }

      } elsif ($globals{machine}{default_state_is_x}{value}) {
        # Don't need anything.  If we're doing onehot (see if above), and 
        # we've got default_state_is_x on, we're using the sv XXX format, both
        # in sv and in verilog, so we do nothing.  
        # In vhdl, we add the XXX state
        if ($language eq "vhdl") {
          push(@assignments,sprintf("$nextstatevar = XXX; ${comm} default to x because default_state_is_x is set\n"));
        }

      } else {
        &error("Internal error - cannot get here...");
      }
    }
    push(@ifs,[$equation,@assignments]) if (@assignments);
  }
  &print_ifelse;
  if ($language eq "vhdl") {
    if ($encoding eq "onehot") {
      &print(--$indent,"end if;\n"); # end of case match
      &print(0,"\n"); 
    } else {
      $indent--;
      &print(0,"\n"); 
    }
  } else {
    &print(--$indent,"end\n"); # end of case match
  }
}
if ($use_undefined_goto_state) {
  if ($language eq "vhdl" && ($encoding eq "heros" || $encoding eq "none")) {
    &print($indent++, "when others =>\n");
    $default_inserted = 1;
  } elsif ($language eq "vhdl" && $encoding eq "onehot") {
    # vhdl onehot requires an "if" of all the statebits being 0
    $ifstatement = "if (";
    foreach $state (@allstates) {
      $ifstatement .= "(${statevar}(${state}_BIT) = '0') and ";
    }
    $ifstatement =~ s/ and $//;
    $ifstatement .= ") then\n";
    &print($indent++,$ifstatement);
  } else {
    &print($indent++, "default : begin\n");
  }
  if ($encoding eq "heros" || $encoding eq "none") {
    &print($indent,"$nextstatevar = $undefined_goto_state; ${comm} Added because undefined_states_go_here is set\n");
    &translate_last_line_to_vhdl_if_required("assignment");
  } elsif ($encoding eq "onehot") {
        if ($language eq "systemverilog" || $globals{machine}{default_state_is_x}{value}) {
          &print($indent,"$nextstatevar = $undefined_goto_state; ${comm} Added because undefined_states_go_here is set\n");
          &translate_last_line_to_vhdl_if_required("assignment");
        } elsif ($language eq "verilog") {
          &print($indent,"$nextstatevar\[$undefined_goto_state\] = 1'b1; ${comm} Added because undefined_states_go_here is set\n");
        } elsif ($language eq "vhdl") {
          &print($indent,"$nextstatevar = $undefined_goto_state; ${comm} Added because undefined_states_go_here is set\n");
          &translate_last_line_to_vhdl_if_required("assignment");
        }
  }
  if ($language eq "vhdl" && ($encoding eq "heros" || $encoding eq "none")) {
    $indent--;
  } elsif ($language eq "vhdl" && $encoding eq "onehot") {
    &print(--$indent, "end if;\n");
  } else {
    &print(--$indent, "end\n");
  }
} elsif ($casedefault) {
  &print($indent,"default: ; ${comm} added because -casedefault is set\n");
}
if ($language eq "vhdl") {
  if ($encoding eq "heros" || $encoding eq "none") {
    &print($indent,"when others =>\n") unless ($default_inserted);
    &print(0,"\n");
    &print(--$indent, "end case;\n");
  } else {
    # No default and no case statement in vhdl onehot
    #&print(0,"\n");
  }
  &print(--$indent, "end process;\n");
} else {
  &print(--$indent, "endcase\n");
  &print(--$indent, "end\n");
}

#######################
# State sequential block
#######################
$section = "seq_block";
&print(0, "\n");


unless ($encoding eq "onehot") {
  &print($indent,"${comm} Assign reg'd outputs to state bits\n");
  foreach $regout (sort {
      $reg_outputs{$a}{statebit_lower} <=>
      $reg_outputs{$b}{statebit_lower}
    } keys %reg_outputs) {
    # suppress [0:0] except for vhdl
    if ($language ne "vhdl" && $total_statebits == 1) {
      &print($indent,"assign $regout = $statevar;\n");
    } else {
      &print($indent,"assign $regout = $statevar\[$reg_outputs{$regout}{statebit_range}\];\n");
    }
    &translate_last_line_to_vhdl_if_required("assignment");
  }
  &print(0,"\n");
}

# renames (for vhdl)
if ($language eq "vhdl") {
  if (%port2var) {
    &print($indent,"${comm} Port renames for vhdl\n");
    foreach $port (sort keys %port2var) {
      &print($indent,&translate_range_to_vhdl_if_required($port) . " <= " . &translate_range_to_vhdl_if_required($port2var{$port}) . ";\n");
      #&translate_last_line_to_vhdl_if_required("assignment");
    }
  }
  #&print($indent,"-- vhdl_remap_lines\n");
  foreach $line (@vhdl_remap_lines) {
    &print($indent,$line);
  }
  &print(0,"\n");
}


&print($indent, "${comm} sequential always block\n");

# Create the always @ line using a sub so it can be used by datapath
# as well.
$line = &create_sequential_always;
&print($indent++,"$line");

if ($language eq "vhdl") {

  # figure out edge string
  $vhdl_edge_string = $globals{machine}{clock}{type};
  $vhdl_edge_string =~ s/posedge/rising_edge/;
  $vhdl_edge_string =~ s/negedge/falling_edge/;
  
  if ($sync eq "sync") {
    &print($indent++,"if (${vhdl_edge_string}($clk)) then\n");
  }

  # vhdl always uses the reset clause
  if ($reset_signal) {
    if ($reset_edge =~ /neg/) {
      $reset_val = 0;
    } else {
      $reset_val = 1;
    }
    &print($indent++,"if (${reset_signal}='$reset_val') then"); # no \n in case we add begin/end
  } else {
    &print($indent++,"if ('0') then"); # 
  }
  &print(0,"\n"); # add \n
  
  if ($encoding eq "heros" || $encoding eq "none") {
    &print($indent,"$statevar <= $reset_state;\n");
    &translate_last_line_to_vhdl_if_required("assignment");

  } elsif ($encoding eq "onehot") {
    if ($language eq "systemverilog" || ($language eq "verilog" && $globals{machine}{default_state_is_x}{value})) {
      &print($indent,"$statevar <= $reset_state;\n");
      &translate_last_line_to_vhdl_if_required("assignment");
    } elsif ($language eq "verilog") {
      &print($indent,"$statevar <= $zero_padded_one << $reset_state;\n");
      &translate_last_line_to_vhdl_if_required("assignment");
    } elsif ($language eq "vhdl") {
      &print($indent,"$statevar <= $reset_state;\n");
      &translate_last_line_to_vhdl_if_required("assignment");
    }
  }

  # Add flags
  foreach $flag (sort keys %flags) {
    # If value is defined in resetval, use it
    if ($globals{outputs}{$flag}{resetval} ne "") {
      &print($indent,"assign $flag = $globals{outputs}{$flag}{resetval};\n");
      $pbuf[$#pbuf] = &translate_port2var_in_first_instance($flag,$pbuf[$#pbuf]);
      &translate_last_line_to_vhdl_if_required("assignment");
    # Otherwise, use 0 and issue warning
    } else {
      &warning($indent,"No reset value set for flag $flag - using 0");
      #&print($indent,"assign $flag = $widths{$flag}'d0;\n");
      $temp = &stringzero($flag,"-quote_unless_translate");
      &print($indent,"assign $flag = $temp;\n");
      $pbuf[$#pbuf] = &translate_port2var_in_first_instance($flag,$pbuf[$#pbuf]);
      &translate_last_line_to_vhdl_if_required("assignment");
    }
    if (exists($port2var{$flag})) {
      $pbuf[$#pbuf] =~ s/\b$flag\b/$port2var{$flag}/;
    }

  }

  $indent--;
  
  if ($sync eq "async") {
    &print($indent++,"elsif (${vhdl_edge_string}($clk)) then\n");
  } else {
    &print($indent++,"else\n");
  }
  
  #&print(0,"\n"); # add \n
  #$indent++;
  &print($indent,"$statevar <= $nextstatevar;\n");
  # Add flags
  foreach $flag (sort keys %flags) {
    &print($indent,"$flag <= next_${flag};\n");
    $pbuf[$#pbuf] = &translate_port2var_in_first_instance($flag,$pbuf[$#pbuf]);
    &translate_last_line_to_vhdl_if_required("assignment");
  }

  if ($sync eq "sync") {
    &print(--$indent,"end if;\n"); # end of if rising edge
  }
  
  &print(--$indent,"end if;\n"); # end of if reset edge
  &print(--$indent,"end process;\n"); # end of always

} else {
  if ($reset_signal) {
    $bang = "!" if ($reset_edge =~ /neg/);
    &print($indent++,"if (${bang}${reset_signal})"); # no \n in case we add begin/end
    # Add begin if there are flags
    if (keys %flags) {
      &print(0," begin"); # 
    }
    &print(0,"\n"); # add \n
  
    if ($encoding eq "heros" || $encoding eq "none") {
      &print($indent,"$statevar <= $reset_state;\n");
  
    } elsif ($encoding eq "onehot") {
      if ($language eq "systemverilog" || ($language eq "verilog" && $globals{machine}{default_state_is_x}{value})) {
        &print($indent,"$statevar <= $reset_state;\n");
      } elsif ($language eq "verilog") {
        &print($indent,"$statevar <= $zero_padded_one << $reset_state;\n");
      } elsif ($language eq "vhdl") {
        &print($indent,"$statevar <= $reset_state;\n");
        &translate_last_line_to_vhdl_if_required("assignment");
      }
    }
    # Add flags
    foreach $flag (sort keys %flags) {
      # If value is defined in resetval, use it
      if ($globals{outputs}{$flag}{resetval} ne "") {
        &print($indent,"$flag <= $globals{outputs}{$flag}{resetval};\n");
      # Otherwise, use 0 and issue warning
      } else {
        &warning($indent,"No reset value set for flag $flag - using 0");
        #&print($indent,"$flag <= $widths{$flag}'d0;\n");
        $temp = &stringzero($flag,"-noquote");
        &print($indent,"$flag <= $temp;\n");
      }
    }
    # Add end if there are flags
    if (keys %flags) {
      &print($indent,"end\n"); # 
    }
    $indent--;
  
    &print($indent,"else");
  }
  
  # Else clause
  # Add begin if there are flags
  if (keys %flags) {
    &print(0," begin"); # 
  }
  &print(0,"\n"); # add \n
  $indent++;
  &print($indent,"$statevar <= $nextstatevar;\n");
  # Add flags
  foreach $flag (sort keys %flags) {
    &print($indent,"$flag <= next_${flag};\n");
  }
  # Add end if there are flags
  if (keys %flags) {
    &print($indent,"end\n"); # 
  }
  $indent--;
  
  &print(--$indent,"end\n"); # end of always
}

#######################
# Datapath sequential block
#######################
$section = "dp_seq_block";
if (scalar(%regdp_outputs)) {
  &print(0, "\n");
  
  &print($indent, "${comm} datapath sequential always block\n");

  # Create the always @ line 
  $line = &create_sequential_always;
  &print($indent++,"$line");

  # figure out edge string
  $vhdl_edge_string = $globals{machine}{clock}{type};
  $vhdl_edge_string =~ s/posedge/rising_edge/;
  $vhdl_edge_string =~ s/negedge/falling_edge/;

  if ($reset_signal || $language eq "vhdl") {

    if ($language eq "vhdl" && $sync eq "sync") {
      &print($indent++,"if (${vhdl_edge_string}($clk)) then\n");
    }

    # vhdl always uses the reset clause
    if ($language eq "vhdl") {
      if ($reset_signal) {
        if ($reset_edge =~ /neg/) {
          $reset_val = 0;
        } else {
          $reset_val = 1;
        }
        &print($indent++,"if (${reset_signal}='$reset_val') then\n"); 
      } else {
        &print($indent++,"if ('0') then\n");
      }
    } else {
      $bang = "!" if ($reset_edge =~ /neg/);
      &print($indent++,"if (${bang}${reset_signal}) begin\n");
    }
    foreach $regdp_output (sort keys %regdp_outputs) {
      # Figure out a reset value

      # This is different depending on whether this is a REAL regdp, or
      # a reg being treated as a regdp because we're in onehot or encoding none
      $type = $globals{outputs}{$regdp_output}{type};

      # If value is defined in resetval, use it
      if ($type eq "regdp" && $globals{outputs}{$regdp_output}{resetval} ne "") {
        $value = $globals{outputs}{$regdp_output}{resetval};

      # If not, and a value exists in the reset state, use that and warn 
      # Don't warn for type reg as this is the normal case.
      } elsif (exists $states{$reset_state}{attributes}{$regdp_output}{value}) {
        $value = $states{$reset_state}{attributes}{$regdp_output}{value};
        if ($type ne "reg") {
          &warning($indent,"No reset value set for datapath output $regdp_output.   Assigning a reset value of $value based on value in reset state $reset_state");
        }

      # Otherwise, use default value if there is one, and warn.
      } elsif ($regdp_outputs{$regdp_output}{default_value} ne "") {
        $value = $regdp_outputs{$regdp_output}{default_value};
        &warning($indent,"No reset value set for datapath output $regdp_output.   Assigning a reset value of $value based on default");
      } else {
        $value = "$widths{$regdp_output}'d0;";
        &warning($indent,"No reset value could be determined for datapath output $regdp_output - using $value");
      }
      # Check that resulting value is non-null
      # Doesn't happen anymore due to 0 default above...
      # but maybe someday add a switch to disable reset to zero.
      &error($indent,"Reset value of datapath output $regdp_output cannot be determined ") if ($value eq "");
      &print($indent,"$regdp_output <= $value;\n");
      &translate_last_line_to_vhdl_if_required("assignment");
    }

    if ($language eq "vhdl") {
      $indent--;
      if ($sync eq "async") {
        &print($indent++,"elsif (${vhdl_edge_string}($clk)) then\n");
      } else {
        &print($indent++,"else\n");
      }
  
    } else {
      &print(--$indent,"end\n");
      &print($indent++,"else begin\n");
    }
  } else {
    # not vhdl and no reset set
    &print(--$indent,"begin\n");
  }

  # Output defaults.  
  # To get better synth results "out of the box", use a 0 default if no
  # default is defined.  To get the old "hold value in illegal states" 
  # behavior, it will be necessary for the user to set the default to 
  # be the variable name (ie, if the variable name is "foo", make the default
  # value "foo").
  foreach $regdp_output (sort keys %regdp_outputs) {
    if (exists $regdp_outputs{$regdp_output}{default_value} && ($regdp_outputs{$regdp_output}{default_value} ne "") ) {
      $dp_seq_defaults{$regdp_output} = $regdp_outputs{$regdp_output}{default_value};
      &print($indent,"$regdp_output <= $dp_seq_defaults{$regdp_output}; ${comm} default\n");
    } else {
      $dp_seq_defaults{$regdp_output} = "0"; # Save for later
      $dp_seq_default = &stringzero($regdp_output,"-quote_unless_translate");
      &warning($indent,"Datapath output $regdp_output has no default value - using 0");
      &print($indent,"$regdp_output <= $dp_seq_default; ${comm} default to zero for better synth results (no default set in .fzm file)\n");
      #&print($indent,"$regdp_output <= $widths{$regdp_output}'b$dp_seq_defaults{$regdp_output}; ${comm} default to zero for better synth results (no default set in .fzm file)\n");
    }
    &translate_last_line_to_vhdl_if_required("assignment");
    push(@dp_seq_default_lines,$pbuf[$#pbuf]);
  }

  # State "case" block
  &print_case_statement($nextstatevar);

  $keep_case = 0; # flag to keep case statement (at least one non-default found)
  foreach $state (@allstates) {
    if ($encoding eq "onehot") {
      $keep_state = 1; # always keep all cases in onehot
    } else {
      $keep_state = 0; # flag to keep this state
    }
    # Create state + begin (might be "popped" if $keep_state doesn't get set)
    if ($encoding eq "heros" || $encoding eq "none") {
      #&print($indent++,"$state: begin\n");
      &print($indent++,sprintf("%-${statename_length}s:",$state) . " begin\n");
      &translate_last_line_to_vhdl_if_required("case item");
    } elsif ($encoding eq "onehot") {
      if ($language eq "systemverilog" || ($language eq "verilog" && $globals{machine}{default_state_is_x}{value})) {
        &print($indent++,sprintf("%-${nextstatename_length_onehot}s:","$nextstatevar\[${state}_BIT\]") . " begin\n");
      } elsif ($language eq "verilog") {
        &print($indent++,sprintf("%-${nextstatename_length_onehot}s:","$nextstatevar\[$state\]") . " begin\n");
      } elsif ($language eq "vhdl") {
        &print($indent++,"if ($nextstatevar(${state}_BIT) = '1') then" . "${comment}\n");
      }
    }

    # Search state attributes for regdp outputs.  If a regdp output is found,
    # assign its value here
    foreach $regdp_output (sort keys %regdp_outputs) {
      if (exists $states{$state}{attributes}{$regdp_output}{value}) {
        $value = $states{$state}{attributes}{$regdp_output}{value};
      }
      if (exists($dp_seq_defaults{$regdp_output}) && ($value eq $dp_seq_defaults{$regdp_output})) {
        # skip - covered by default
      } else {
        # Check that resulting value is non-null
        &error($indent,"Value of regdp output $regdp_output cannot be determined in state $state") if ($value eq "");
        &print($indent,"$regdp_output <= $value;\n");
        &translate_last_line_to_vhdl_if_required("assignment");
        $keep_state = 1;
        $keep_case = 1;
      }
    }

    # If we did something here, output the end
    if ($keep_state) {
      # If no assignment, insert ";" line
      if ($pbuf[$#pbug] !~ /=/) {
        &print($indent,"; // case must be complete for onehot\n");
      }
      if ($language eq "vhdl") {
        if ($encoding eq "onehot") {
          &print(--$indent,"end if;\n"); # end of case match
          &print(0,"\n"); 
        } else {
          $indent--;
          &print(0,"\n"); 
        }
      } else {
        &print(--$indent,"end\n"); # end of case match
      }
    # Otherwise, pop the state + begin off the print buffer and clean up the indent.
    } else {
      $indent--;
      pop(@pbuf);
    }
  }

  # Similarly, keep case and output endcase only if we found something no-default (flat is set)
  if ($keep_case) {
    if ($language eq "vhdl" && ($encoding eq "heros" || $encoding eq "none")) {
      &print($indent,"${comm} formality insists on this...\n");
      &print($indent,"when others =>\n");
#      foreach $line (@dp_seq_default_lines) {
#        $line =~ s/^\s+//;
#        &print($indent,$line);
#      }
#      $indent--;
      &print(0,"\n");
      &print(--$indent, "end case;\n");
    } else {
      if ($casedefault) {
        &print($indent,"default: ; ${comm} added because -casedefault is set\n");
      }
      if ($language ne "vhdl") {
        &print(--$indent, "endcase\n");
      }
    }
  } else {
    pop(@pbuf);
    &warning($indent,"Did not find any non-default values for any datapath outputs - suppressing case statement");
    $indent--;
  }
  if ($language eq "vhdl") {
    if ($sync eq "sync") {
      &print(--$indent,"end if;\n"); # end of if rising edge
    }
    &print(--$indent,"end if;\n"); # end of if not reset
    &print(--$indent,"end process;\n"); # end of always
  } else {
    &print(--$indent,"end\n"); # end of if not reset
    &print(--$indent,"end\n"); # end of always
  }
}

#######################
# State name-as-ascii block
#######################
if ($simcode && !($language eq "vhdl")) {
  # not implemented for vhdl (yet)
  $section = "ascii_block";
  &print(0, "\n");
  &print($indent, "${comm} This code allows you to see state names in simulation\n");
  #&print($indent, "${comm} synopsys translate_off\n");
  &print($indent, "`ifndef SYNTHESIS\n");
  &print($indent, "reg [" . ($statename_length * 8 - 1) . ":0] $statenamevar;\n");
  &print($indent++, "always @* begin\n");
  
  # State "case" block
  if ($encoding eq "heros" || $encoding eq "none") {
    &print($indent++,"case ($statevar)\n");
  } elsif ($encoding eq "onehot") {
    &print($indent++,"case (1\'b1)\n");
  }
  foreach $state (@allstates) {
    if ($encoding eq "heros" || $encoding eq "none") {
      &print($indent++,sprintf("%-${statename_length}s:",$state) . "\n");

    } elsif ($encoding eq "onehot") {
      if ($language eq "systemverilog" || ($language eq "verilog" && $globals{machine}{default_state_is_x}{value})) {
        &print($indent++,sprintf("%-${statename_length_onehot}s:","$statevar\[${state}_BIT\]") . "\n");
      } elsif ($language eq "verilog") {
        &print($indent++,sprintf("%-${statename_length_onehot}s:","$statevar\[$state\]") . "\n");
      }
    }
    &print($indent++,"$statenamevar = \"$state\";\n");
    $indent--;
    $indent--;
  }
  # add default for X
  #&print($indent++,"default:\n");
  &print($indent++,sprintf("%-${statename_length}s:","default") . "\n");
  &print($indent++,sprintf("$statenamevar = \"%s\";\n", "X" x ($statename_length)));
  $indent--;
  $indent--;
  
  &print(--$indent, "endcase\n");
  &print(--$indent, "end\n");
  
  #&print($indent, "${comm} synopsys translate_on\n\n");
  &print($indent, "`endif\n\n");
}

# Insert string at bottom of module if requested
&insert_text("insert_at_bottom_of_module");

# endmodule and bottom-of-file 

if ($language eq "vhdl") {
  &print(--$indent,"end fizzim;\n");
} else {
  &print(--$indent,"endmodule\n");
}

# Insert string at bottom of file if requested
&insert_text("insert_at_bottom_of_file");

&print_output;

#######################################################################
# Subroutines
#######################################################################

# Insert text from attribute
sub insert_text {
  my ($attribute) = (@_);
  my $line = "";
  my ($comment_lines,$machine_attribute);
  my (@lines);
  eval "\$myindent = \"$indentstring\" x $indent"; # calculate indent
  foreach $machine_attribute (sort keys %{ $globals{"machine"} }) {
    #&print(0,"machine_attribute: $machine_attribute\n");
    if ($machine_attribute =~ /^$attribute/) { # matches anything STARTING WITH requested attribute
      #&print(0,"Matched $machine_attribute\n");
      &print(0,"\n") if (!$line && $insert_messages); # Print newline first time only
      $line = ${globals}{machine}{$machine_attribute}{value}; # Line to insert
      foreach $splitline (split(/\n\s*/,$line)) {  # split on newline
        # Add comment if requested and insert_messages is on
        if ($insert_messages && ($splitline !~ /^\s*${comm}/)) {
          $splitline .= " ${comm} Inserted from attribute ${machine_attribute}" 
        }
        &print($indent,"$splitline\n");
      }
    }
  }
  &print(0,"\n") if ($line && $insert_messages); # Print newline if anything got printed
}

# Strip the comma from a line in the print buffer (but ignore commas in comments)
sub strip_comma_from_last_line {
  if ($pbuf[$#pbuf] =~ s/^([^\/]*),/\1 /) { # get rid of FIRST "," ahead of comment
    $pbuf[$#pbuf] =~ s/ +$//; # and strip trailing blanks
    return 1;
  } else {
    return 0;
  }
}

# Substitute the port2var for the first instance of port
sub translate_port2var_in_first_instance {
  my ($port,$line) = (@_);
  my ($temp);
  if (exists($port2var{$port})) {
    #print "\nwas: $line\n";
    ($temp = $port) =~ s/([\[\]])/\\\1/g; # escape []
    #print "doing rename of \"$temp\" to \"$port2var{$port}\"\n";
    $line =~ s/\b$temp( |\b|$)/$port2var{$port}\1/;
    #print "is:  $line\n\n";
  }
  return $line

}

# Substitute the port2var for the first instance of port
sub translate_port2var_in_all_instances {
  my ($port,$line) = (@_);
  my ($temp);
  if (exists($port2var{$port})) {
    #print "\nwas: $line\n";
    ($temp = $port) =~ s/([\[\]])/\\\1/g; # escape []
    #print "doing rename of \"$temp\" to \"$port2var{$port}\"\n";
    $line =~ s/\b$temp( |\b|$)/$port2var{$port}\1/g;
    #print "is:  $line\n\n";
  }
  return $line

}

# Print out an if/elseif/else statement
# arrays used (global):
#   ifs : array of arrays, in priority order.  Each entry is [equation,assignment,assignment,...]
#
sub print_ifelse {
  my ($i);
  #print "\ndolnum: \"$#ifs\"\n";
  if ($language eq "verilog" || $language eq "systemverilog") {
    for $i (0 .. $#ifs) {
      $entry = $ifs[$i];
      #print "entry: \"@$entry\" i: \"$i\"\n";
      ($equation,@assignments) = (@$entry);
      if ($i == 0) { # first term
        # If always true, just output "begin" without the if 
        # (need a begin/end block in case there are comb outputs)
        if ($equation eq "1") {
          &print($indent++,"begin\n");
          $eq1seen = 1;
        } else {
          &print($indent++,"if ($equation) begin\n");
        }

      } elsif ($i == $#ifs) { # last term
        if ($equation eq "1") {
          &print($indent++,"else begin\n");
          #$eq1seen = 1;
        } else {
          &print($indent++,"else if ($equation) begin\n");
        }

      } else { # intermediate term
        &print($indent++,"else if ($equation) begin\n");
      }
      foreach $assignment (@assignments) {
        &print($indent,"$assignment");
      }
      &print(--$indent,"end\n");
    }
    #&print(--$indent,"end\n");

  } elsif ($language eq "vhdl") {
    my ($printed_if) = 0;
    for my $i (0 .. $#ifs) {
      $entry = $ifs[$i];
      #print "entry: \"@$entry\"\n";
      ($equation,@assignments) = (@$entry);
      $equation = &translate_equation_to_vhdl_if_required($equation);
      if ($i == 0) {
        if ($equation eq "1") {
        } else {
          $printed_if = 1;
          &print($indent++,"if ($equation) then\n");
        }

      } elsif ($i == $#ifs) { # last term
        if ($equation eq "1") {
          &print($indent++,"else\n");
          #$eq1seen = 1;
        } else {
          &print($indent++,"elsif ($equation) then\n");
        }

      } else {
        &print($indent++,"elsif ($equation) then\n");
      }

      foreach $assignment (@assignments) {
        &print($indent,"$assignment");
        &translate_last_line_to_vhdl_if_required("assignment");
      }
      $indent-- unless ($#ifs == 0 && $equation eq "1"); # end of assignment indent
    }
    &print($indent,"end if;\n") if ($printed_if);

  } else {
  }
}

# Translate an equation to vhdl format (or, try...)
sub translate_equation_to_vhdl_if_required {
  my ($equation) = (@_);
  if ($language eq "vhdl") {
    unless ($equation eq "1") {
      # always translate port2var
      foreach $var (sort keys %port2var) {
        $equation =~ s/\b$var\b/$port2var{$var}/g;
      }
      # only do other translations if cmd line switch is on
      if ($translate2vhdl) {
        $equation =~ s/(\w+)/\1='1'/g; # "go" to "go='1'"
        $equation =~ s/\!\s*(\S+)/not (\1)/g;  # not
        $equation =~ s/\|\|?/ or /g;   # | or || to " or "
        $equation =~ s/\&\&?/ and /g;  # & or && to " and "
        $equation =~ s/\s\s+/ /g;      # clean up spaces
      }
    }
  }
  return $equation;
}

sub translate_signal_statement_to_vector_if_required {
  my ($line) = (@_);
  if ($language eq "vhdl") {
    $line =~ s/(.*)(\[.*\])(\s*:\s*)(\w+)/\1\3\4_VECTOR\2/;
    $line = &translate_range_to_vhdl_if_required($line);
  }
  return $line;
}

sub translate_range_to_vhdl_if_required {
  my ($line) = (@_);
  if ($language eq "vhdl") {
    $line =~ s/\[(\d+):(\d+)\]/(\1 downto \2)/g; # translate bit ranges
    $line =~ s/\[(\d+)\]/(\1)/g; # translate single bit select [0] to (0)
  }
  return $line;
}

sub stringzero {
  my ($var,$options) = (@_);
  if ($language eq "vhdl") {
    my $temp = &dec2bin("0",$widths{&varname_no_range($var)});
    if ($options !~ /-noquotes/ && !($options =~ /-quote_unless_translate/ && $translate2vhdl)) {
      if (length($temp) == 1) {
        $temp = "'$temp'";
      } else {
        $temp = "\"$temp\"";
      }
    }
    return $temp;
  } else {
    return "0";
  }
}

sub varname_no_range {
  my ($var) = (@_);
  $var =~ s/\[.*\]//; # remove [ ] range (verilog/sv)
  $var =~ s/\(.*\)//; # remove ( ) range (vhdl)
  return $var;
}

sub convert_value_from_vhdl_to_verilog_if_required {
  my ($value) = (@_);

  if ($language eq "vhdl") {
    #$value =~ s/([xd])"/'\1"/;
    $value =~ s/([x])"/'\1"/;
    $value =~ s/"//g;
    $value =~ s/'//g;
  }
  return $value;
}

# Translate the last pbuf line to vhdl.  Argument is type of line.
sub translate_last_line_to_vhdl_if_required {
  my ($line_type) = (@_);

  if ($language eq "vhdl") {
    if ($line_type eq "assignment") {
      foreach $output (keys %{ $globals{outputs} }) {
        $pbuf[$#pbuf] = &translate_port2var_in_all_instances($output,$pbuf[$#pbuf]);
      }
      #print "doing assignment translate of \"\n$pbuf[$#pbuf]\"\n";
      $pbuf[$#pbuf] =~ s/assign\s*//; # get rid of "assign " if present
      $pbuf[$#pbuf] =~ s/<?=/<=/; # = and <= both translate to <=
      $pbuf[$#pbuf] = &translate_range_to_vhdl_if_required($pbuf[$#pbuf]); # This MUST be done BEFORE	the $translate2vhdl because that code now expects ranges (8 downto 0)
      if ($translate2vhdl) {
        # handle 'h1ff, 'd23, 'b0010 
        if ($pbuf[$#pbuf] =~ /(.+)(\s*)(<=\s*)(\d*)'([bdh])([\dAaBbCcDdEeFf]+)/) {
          my ($var,$width,$radix,$value) = ($1,$4,$5,$6);
          $var =~ s/^\s+//; # remove leading spaces
          $var =~ s/\s+$//; # remove trailing spaces
          $var =~ s/\(.*\)//; # remove range
          if ($radix eq 'b') {
            $substring = "0" x 100 . $value;
            $substring = substr($substring,length($substring) - $widths{$var},$widths{$var});
            #print "line: \"$pbuf[$#pbuf]\" substring: \"$substring\" widths:\"$widths{$var}\" var:\"$var\"\n";
          } elsif ($radix eq 'd') {
            $substring = &dec2bin($value,$widths{$var});
            #print "line: \"$pbuf[$#pbuf]\" substring: \"$substring\" widths:\"$widths{$var}\" var:\"$var\"\n";
          } elsif ($radix eq 'h') {
            $substring = &hex2bin($value,$widths{$var});
            #$substring = sprintf("%0$widths{$var}s",$substring);
            #print "line: \"$pbuf[$#pbuf]\" substring: \"$substring\" widths:\"$widths{$var}\" var:\"$var\" value: \"$value\"\n";
          }
          $pbuf[$#pbuf] =~ s/(\S+)(\s*)(<=\s*)(\d*)'([bdh])([\dAaBbCcDdEeFf]+)/\1\2\3$substring/;
        }
        # Note: Don't do translate of "foo <= 0" - corrupts onehot settings

        $pbuf[$#pbuf] =~ s/<=(\s*)(\D)(\d)(\D)/<=\1\2\'\3\'\4/; # put single digits in quotes
        $pbuf[$#pbuf] =~ s/<=(\s*)(\D)(\d\d+)(\D)/<=\1\2\"\3\"\4/; # put multiple digits in double quotes
      }
      #print "done with translate of \"$pbuf[$#pbuf]\"\n";
    } elsif ($line_type eq "case item") {
      #print "doing case item translate of \"$pbuf[$#pbuf]\"\n";
      # original:
      #   IDLE: begin
      # change to:
      #   when IDLE =>
      #unless ($encoding eq "onehot") {
        $pbuf[$#pbuf] =~ s/^(\s*)(\S+)(\s*):\s*begin/\1when \2\3 =>/;
      #}
      #print "done with translate of \"$pbuf[$#pbuf]\"\n";
    } elsif ($line_type eq "if else statement") {
      #print "doing translate of \"$pbuf[$#pbuf]\"\n";
      # change begin to end 
      if ($pbuf[$#pbuf] =~ /if\s/) {
        $pbuf[$#pbuf] =~ s/(\s+)begin(\s*)$/\1then\2/;
      } else {
        $pbuf[$#pbuf] =~ s/(\s+)begin(\s*)$/\n/;
      }
      if ($pbuf[$#pbuf] =~ /else\s*$/) {
        $endif = 1;
      }
      #print "done with translate of \"$pbuf[$#pbuf]\"\n";
    } else {
      &error("Internal error - cannot get here...");
    }
  }
}

# Create the wire/reg/logic statement.  Routine is used by module port code
# as well as suppress_portlist code.
# argument is output to create statement for.
# returns statement.  Also sets range info globally.
sub create_wirereg_statement {
  my ($prefix,$output,$suffix,$statement_length) = (@_);
  my $signal = "";

  # Handle comments
  if ($globals{outputs}{$output}{comment}) {
    $comment = "${comm} $globals{outputs}{$output}{comment}";
  } else {
    $comment = "";
  }


  if ($language eq "verilog") {
    # Oddly, the comb outputs are reg's (because they will be assigned to
    # in the comb always block), but reg outputs are wire's (because they
    # will have continuous assignment to state bits).  Regdp (registered
    # datapath) are reg's.
    if ($globals{outputs}{$output}{type} eq "comb") {
      $type = "reg";
    } elsif ($globals{outputs}{$output}{type} eq "reg") {
      if ($encoding eq "onehot") {
        $type = "reg";
      } else {
        $type = "wire";
      }
    } elsif ($globals{outputs}{$output}{type} eq "regdp") {
      $type = "reg";
    } elsif ($globals{outputs}{$output}{type} eq "flag") {
      $type = "reg";
    } else {
      &error($indent,"No type found for output $output");
    }
  } elsif ($language eq "systemverilog") {
    # In sv, everything is type logic
    $type = "logic";
  }

  # Handle multibit outputs
  if ($language eq "vhdl") {
    if ($prefix =~ /put/) {
      $prefix =~ s/put//;
    } else {
      $signal = "signal ";
    }
    $suffix =~ s/,/;/;
  }
  if (($name,$range) = ($output =~ /^([^\[]+)\s*(\[.*\])/)) {
    ($upper,$lower) = $range =~ /(\d+):(\d+)/;
    # now set early in script
    #$widths{$name} = abs($upper - $lower) + 1; 
    if ($language eq "vhdl") {
      $line = sprintf("%-${statement_length}s %-s\n","${signal}$name : ${prefix}${vhdl_vector_type}($upper downto $lower)${suffix}",$comment);
      
    } else {
      $line = sprintf("%-${statement_length}s %-s\n","${prefix}$type $range $name${suffix}",$comment);
    }
  } else {
    if ($language eq "vhdl") {
      $line = sprintf("%-${statement_length}s %-s\n","${signal}$output : ${prefix}${vhdl_bit_type}${suffix}",$comment);
    } else {
      $line = sprintf("%-${statement_length}s %-s\n","${prefix}$type $output${suffix}",$comment);
    }
    #$widths{$output} = 1;
  }
  # Remove trailing blanks
  $line =~ s/\s*\n$/\n/;
  return($line);
}

sub vhdl_range {
  my ($upper,$lower) = $range =~ /(\d+):(\d+)/;
  return "$upper downto $lower";
}

sub print_case_statement {
  # Print the case statement.  
  my ($casevar) = (@_);

  # For heros, it's pretty simple:
  if ($encoding eq "heros" || $encoding eq "none") {
    if ($language eq "vhdl") {
      &print($indent++,"case $casevar is\n");
    } else {
      &print($indent++,"case ($casevar)\n");
    }

  # For onehot, it is based on the onehot pragma and the language
  # In verilog, defaults to "case (1'b1) synopsys parallel_case full_case".
  # In systemverilog, defaults to "unique case (1'b1)".
  # If pragma is set, always use "case (1'b1) ${comm} pragma", regardless
  # of language.
  } elsif ($encoding eq "onehot") {
    if (exists $globals{machine}{onehot_pragma}{value} ) {
      &warning($indent,"Using override value from attribute onehot_pragma");
      $pragma = $globals{machine}{onehot_pragma}{value};
      unless ($language eq "vhdl") {
        &print($indent++,"case (1'b1) ${comm} $pragma\n");
      }
    } elsif ($language eq "verilog") {
      &print($indent++,"case (1'b1) ${comm} synopsys parallel_case full_case\n");
    } elsif ($language eq "systemverilog") {
      &print($indent++,"unique case (1'b1)\n");
    } elsif ($language eq "vhdl") {
      &print(0,"\n"); # do nothing but print blank line
    }
  }
}

sub create_sequential_always {
  if (! exists $globals{inputs}{$clk}) {
    #&warning($indent,"Specified clock signal $clk is not an input");
    &error($indent,"Specified clock signal $clk is not an input");
  }
  if ($globals{machine}{clock}{type} !~ /^(pos)|(neg)edge$/) {
    &error($indent,"Clock type not specified as posedge or negedge");
  }
  # Build clock portion of always @(posedge...)
  if ($language eq "verilog") {
    $line = "always @(";
  } elsif ($language eq "systemverilog") {
    $line = "always_ff @(";
  }
  $line = $line . "$globals{machine}{clock}{type} $clk" ;
  
  # Add reset if given
  if (exists $globals{machine}{reset_signal}{value}) {
    if (! exists $globals{inputs}{$reset_signal}) {
      #&warning($indent,"Specified reset signal $reset_signal is not an input");
      &error($indent,"Specified reset signal $reset_signal is not an input");
    }
    if ($globals{machine}{reset_signal}{type} =~ /^((neg)|(pos))edge$/) {
      $sync = "async";
    } elsif ($globals{machine}{reset_signal}{type} eq "positive") {
      $sync = "sync";
    } elsif ($globals{machine}{reset_signal}{type} eq "negative") {
      $sync = "sync";
    } else {
      &error($indent,"reset_signal $reset_signal not specified as type posedge, negedge, positive or negative");
    }
    $reset_edge = $globals{machine}{reset_signal}{type};
    if ($sync eq "async") {
      $line = $line . " or $reset_edge $reset_signal";
    }
    if (! exists $globals{machine}{reset_state}{value}) {
      &error($indent,"Reset signal given, but no reset state found.");
    } else {
      $reset_state = $globals{machine}{reset_state}{value};
    }
  }
  if ($language ne "vhdl") {
    return $line = $line . ") begin\n";
  } else {
    if ($section eq "seq_block") {
      $line = "FF: process(";
    } elsif ($section eq "dp_seq_block") {
      $line = "DP: process(";
    } else {
      &error("Internal error - cannot get here...");
    }
    $line .= "$clk,$reset_signal,$nextstatevar,";
    if ($section eq "seq_block") {
      foreach $flag (sort keys %flags) {
        $line .= &translate_range_to_vhdl_if_required("next_${flag},");
      }
    }
    $line =~ s/^(.*),/\1/; # get rid of last ,
    $line .= ") begin\n";
    return $line;
  }
}

sub sort_by_priority_then_equation_equal_1 {
  #&debug("a priority: $a -> $transitions{$a}{attributes}{priority}{value}",0,$section);
  #&debug("a equation: $a -> $transitions{$a}{attributes}{equation}{value}",0,$section);
  #&debug("a: $a -> $a",0,$section);
  #&debug("b priority: $b -> $transitions{$b}{attributes}{priority}{value}",0,$section);
  #&debug("b equation: $b -> $transitions{$b}{attributes}{equation}{value}",0,$section);
  #&debug("b: $a -> $b",0,$section);
#  &debug("priority sort result is: " . $transitions{$a}{attributes}{priority}{value} <=> $transitions{$b}{attributes}{priority}{value},0,$section);

  $transitions{$a}{attributes}{priority}{value} <=> 
  $transitions{$b}{attributes}{priority}{value}
    ||
  ($transitions{$a}{attributes}{equation}{value} eq "1") cmp
  ($transitions{$b}{attributes}{equation}{value} eq "1")
    # finally, sort by trans name just so order will be predictable
    ||
   $a cmp $b
}


sub parse_input {

  my %myattributes_forcompare;

  &debug("Start of parse_input\"$_\"",0,"parse_input");

  # Create local version of myattributes with substition done to enable
  # compares
  foreach $entry (keys %myattributes) {
    $entry =~ s/\*[^\*]*\*/[^\"]+/g;
    $entry =~ s/\{/\\{/g;  # required for perl v5.22.0 and above - left brace must be escaped
    $myattributes_forcompare{$entry} = 1;
  }

  while (<>) {
    chomp;
    s/##.*$// ;# Remove comments
    s/^\s*//;  # Remove leading whitespace

    # Toss status/endstatus
    if (/^\s*<status>/) {
      until (/^\s*<\/status>/) {
        $_ = <>;
      }
      next;
    }

    # Ignore drawArea (strips this level entirely from out data
    # structures)
    next if (/drawArea/);

    # Look for endtoken first to allow elsif to handle token case
    if (($endtoken) = (/^\s*<\/(.*)>/)) {
      &debug("endtoken: $endtoken from \"$_\"",0,"parse_input");
      &debug("ptr was \"$ptr\"",0,"parse_input");
      # Found an endtoken.  If this is the array, clear the array value.
      # Otherwise, remove everything from this token to the end from ptr.
      if ($array eq $endtoken) {
        $array = "";
        $ptr = "";
      } else {
        $ptr =~ s/{\"\Q$endtoken\E\"}.*$//;
      }
      &debug("new array is \"${array}\"",0,"parse_input");
      &debug("new ptr is \"${ptr}\"",0,"parse_input");
      &debug("",0,"parse_input");
    } elsif (($token) = (/^\s*<(.*)>/)) {
      &debug("token: $token from \"$_\"",0,"parse_input");
      # Found new token.  If array is blank, this is the new array.
      # Otherwise, append to ptr.  
      if ($array eq "") {
        $array = "$token";
      } else {
        $ptr = $ptr . "{\"$token\"}";
      }
      &debug("new array is \"${array}\"",0,"parse_input");
      &debug("new ptr is \"${ptr}\"",0,"parse_input");
      &debug("",0,"parse_input");
    } else {
      $value = $_;
      &debug("value: $value from \"$_\"",0,"parse_input");
      # Found a value instead of a token.  Use array and ptr to set
      # the value using eval.
      # First, turn state name (or transition name) into index
      &debug("old ptr (in value block) is \"$ptr\"",0,"parse_input");
      if ($ptr =~ s/({\"attributes\"}).*name.*value\"}/{\"$value\"}\1/) {
        # If this already exists, it means we have a duplicate entry!
        if (exists $${array}{"$value"}{"attributes"}) {
          &error($indent,"Error - found duplicate entry for $array $value");
        }
        &debug("new ptr (in value block) is \"$ptr\"",0,"parse_input");
      } else {
        $keep = 0;
        foreach $entry (keys %myattributes_forcompare) {
          #print STDERR "Looking at myatt $entry\n";
          #print STDERR " comparing to ${array}${ptr}\n";
          if ("${array}${ptr}" =~ $entry) {
            &debug("Got match to $entry\n",0,"parse_input");
            $keep = 1;
          }
        }
        #$cmd = "\$${array}${ptr} = q{${value}};";
        $value =~ s/"/\\"/g;  # escape quotes for next line
        $value =~ s/\$/\\\$/g; # escape $ signs (in case code has $time...)

        $cmd = "\$${array}${ptr} = \"$value\";";
        if ($keep) {
          &debug("cmd is \"$cmd\"",0,"parse_input");
          eval $cmd unless (!$array);
        } else {
          &debug("skipped cmd \"$cmd\"",0,"parse_input");
        }
      }
    }
  }

  &debug("End of parse_input\"$_\"\n\n",0,"parse_input");

  # Check some random values
  #&debug("state0 vis is $states{state0}{attributes}{vis}",0,"parse_input");
  #&debug("trans0 startState is $transitions{trans0}{startState}",0,"parse_input");
  #&debug("trans0 endState is $transitions{trans0}{endState}",0,"parse_input");
}

sub assign_states_hero {
  my ($state);
  #  print &dec2bin(1,5) . "\n"; 
  #  print &dec2bin(17,5) . "\n"; 
  #  @statevals = &get_statevals(5);  # create global statevals
  #  print "@statevals\n";
  #  exit;
  
  &debug("Starting assign_states_hero with minbits $minbits and maxbits $maxbits",0,"assign_states");
  if ($regbits > $regbits_warning_threshold) {
    $suppressed = &warning($indent,"$regbits bits specified as type \"reg\".  Type \"reg\" means they will be included in the state encoding.  With so many bits, this might take a very long time and/or consume large amounts of memory.  Consider converting some of them to type \"regdp\" or type \"flag\".  To suppress this message in the future, use \"-nowarn T20\"");
    unless ($suppressed) {
      print STDERR "Sleeping 10 seconds, then continuing...\n";
      sleep 10;
    }
  }
  
  $as_iterations = 0;
  for ($bits = $minbits ; $bits <= $maxbits ; $bits++) {
    &debug("Trying $bits bits for states",0,"bits");
    &debug("Trying $bits bits for states",0,"assign_states");
    &verbose("Trying $bits bits for states",0,"");
    @statevals = &get_statevals($bits);  # create global statevals
    &debug("statevals: @statevals",0,"assign_states");
    #print STDERR "statevals: @statevals\n";
    # Pre-determine legal state values for each state
    # This is much faster than checking all of them.
    %legal_statevals = ();
    foreach $state (@allstates) {
      STATEVAL : foreach $stateval (@statevals) {
        if (&matches_fixedbits($state,$stateval)) {
          push(@{ $legal_statevals{$state} }, $stateval);
        }
      }
      #print "$state @{ $legal_statevals{$state} }\n";
      &debug("legal values for state $state: @{ $legal_statevals{$state} }",0,"assign_states");
    }
    # Call recursive subroutine to assign states
    ($success,$uaref,$v2nref) = &attempt_assign_states( \@allstates, \%state_val2name, 0);
    if ($success) {
      &debug("Found successful assignment with $bits bits",0,"assign_states");
      &verbose("Found successful assignment with $bits bits after $as_iterations iterations",0,"assign_states");
      last;
    }
  }
  
  if (!$success) {
    $maxbitsplusone = $maxbits + 1;
    &error($indent,"No valid state assignment found in range of $minbits to $maxbits bits - try using -minbits $maxbitsplusone -maxbits $maxbitsplusone on the command line or in be_cmd.");
  }

}

sub attempt_assign_states {
  my ($uaref,$v2nref,$depth) = @_;
  my (@unassigned_states);
  my (%state_val2name);
  my ($state,$stateval);

  # Dereference arguments into local data structures
  @unassigned_states = @$uaref;
  %state_val2name = %$v2nref;

  if ($as_iterations >= $max_iterations) {
    if ($bits == $minbits) {
      &error($indent,"No valid state assignment found after $as_iterations iterations.  Try increasing max iterations using -iterations");
    } else {
      &error($indent,"No valid state assignment found after $as_iterations iterations.  Try using -minbits $bits or increase max iterations using -iterations");
    }
  }
  $as_iterations++;

  &debug("attempt_assign_states called with unassigned states \"@unassigned_states\", depth=$depth",$depth,"assign_states");
  #if ($depth > 16) { exit; } # temp
  &debug("attempt_assign_states iterations: $as_iterations",$depth,"assign_states");
  foreach $temp (sort keys %state_val2name) { 
    &debug("$temp: $state_val2name{$temp}",$depth,"assign_states");
  }

  while ($state = shift(@unassigned_states)) {
    &debug("attempt_assign_states working on state $state",$depth,"assign_states");
    STATEVAL : foreach $stateval (@{ $legal_statevals{$state} }) {
      &debug("attempt_assign_states looking at stateval $stateval for uniqueness",$depth,"assign_states");
      next if ($state_val2name{$stateval}); # must be unique
      &debug("attempt_assign_states trying stateval $stateval for state $state",$depth,"assign_states");
      if (!$design_has_graycodes || &matches_graycodes($state,$stateval,%state_val2name)) { 
        # looks good at this level
        &debug("$stateval for state $state passes checks at this level $depth",$depth,"assign_states");
        $state_val2name{$stateval} = $state;
        if (!@unassigned_states) { # if nothing left, we're done
          &debug("attempt_assign_states returning success",$depth,"assign_states");
          return (1, \@unassigned_states, \%state_val2name);
        } else { # otherwise, keep trying recursively
          $depth++;
          ($found_state,$uaref1,$v2nref1) = &attempt_assign_states( \@unassigned_states, \%state_val2name,$depth);
          &debug("Call for $state value $stateval returned $found",$depth,"assign_states");

        }
        if ($found_state) { # good right the way down
          &debug("good right the way down",0,"assign_states"); 
          %state_val2name = %$v2nref1;
          last STATEVAL;
        } else {
          &debug("stateval $stateval for state $state looked good, but failed recursively - trying next",$depth,"assign_states");
          delete $state_val2name{$stateval};
          $depth--;
        }
      } else { 
        $found_state = 0;
        &debug("assignment failed fixedbits check",0,"assign_states");
      } 
    }
    return ($found_state, \@unassigned_states, \%state_val2name);
  }
}

sub matches_fixedbits {
  my ($state,$stateval) = @_;

  # Use a bit index that runs right to left
  for ($bit = 0 ; $bit < length($stateval) ; $bit++) {
    $substrbit = length($stateval) - $bit - 1 ;
    &debug("matches_fixbits looking at bit $bit of $stateval (" . substr($stateval,$substrbit,1) . ") for $state",$depth,"mf");
    if ( (exists $fixedbits{$state}{$bit})
      && ($fixedbits{$state}{$bit} ne (substr($stateval,$substrbit,1))) ) {
        &debug("matches_fixbits found an illegal value at bit $bit of $stateval for $state",$depth,"mf");
        return 0;
    }
  }
  return 1;
}

sub matches_graycodes {
  my ($state,$stateval,%state_val2name) = @_;
  return ( &matches_graycodes_to_this_state($state,$stateval,%state_val2name)
        && &matches_graycodes_from_this_state($state,$stateval,%state_val2name) );
}

sub matches_graycodes_to_this_state {
  my ($state,$stateval,%state_val2name) = @_;
  my ($otherstateval,$graystate);

  # look at each currently defined state (in state_val2name)
  foreach $otherstateval (keys %state_val2name) {
    $otherstate = $state_val2name{$otherstateval}; # get the name
    if (exists $graytransitions{$otherstate}) { # if it has a gray list
      foreach $graystate (@{ $graytransitions{$otherstate} }) { # look through the list
        if ($graystate eq $state) {  #I'm in his list
          &debug("matches_graycodes_to_this_state checking $graystate ($otherstateval) against $state ($stateval)",$depth,"mgto");
          # check to see if it is legal
          if (!&isgraytransition($stateval,$otherstateval)) {
            return 0;
          }
        }
      }
    }
  }
  return 1; # if nothing illegal found, all must be legal
}

sub matches_graycodes_from_this_state {
  my ($state,$stateval,%state_val2name) = @_;
  my ($otherstateval,$graystate);

  # look at each state that should be a gray transition from this state
  return 1 if (!exists $graytransitions{$state});
  foreach $graystate (@{ $graytransitions{$state} }) { 
    &debug("matches_graycodes_from_this_state looking at gray state $graystate for state $state",$depth,"mgfrom");
    # find the encoding for the gray state
    foreach $otherstateval (keys %state_val2name) {
      &debug("matches_graycodes_from_this_state looking at otherstateval $otherstateval",$depth,"mgfrom");
      if ($state_val2name{$otherstateval} eq $graystate) {
        &debug("Checking $graystate ($otherstateval) against $state ($stateval)",$depth,"mgfrom");
        # check to see if it is legal
        if (!&isgraytransition($stateval,$otherstateval)) {
          return 0;
        }
      }
    }
  }
  return 1; # if nothing illegal found, all must be legal
}

sub isgraytransition {
  my ($stateval1,$stateval2) = @_;
  my ($diffs);

  # using perl's normal left to right bit order
  &debug("isgraytransition checking $stateval1 against $stateval2",$depth,"isgraytrans");
  for ($bit = 0 ; $bit < length($stateval1) ; $bit++) {
    if (substr($stateval1,$bit,1) ne substr($stateval2,$bit,1)) {
      $diffs++
    }
  }
  &debug("isgraytransition found $diffs diffs",$depth,"isgraytrans");
  return ($diffs <= 1);
}

sub required_bits {
  my ($n) = @_;
  $div= log($n) / log(2);
  $base = sprintf("%d",$div);
  if (($div - $base) > 0) {$base++;}
  return $base;
}


sub hex2bin {
  my ($hex,$bits) = @_;
  my ($bin,$i);

  %hex2bin = ( 0 => "0000",
               1 => "0001",
               2 => "0010",
               3 => "0011",
               4 => "0100",
               5 => "0101",
               6 => "0110",
               7 => "0111",
               8 => "1000",
               9 => "1001",
               a => "1010",
               b => "1011",
               c => "1100",
               d => "1101",
               e => "1110",
               f => "1111",
               A => "1010",
               B => "1011",
               C => "1100",
               D => "1101",
               E => "1110",
               F => "1111",
  );
  
  #print "hex is \"$hex\"\n";
  #print "length is " . length($hex) . "\n";
  for ($i = 0; $i < length($hex) ; $i++) {
    #print "bin was \"$bin\"\n";
    $bin = $bin . $hex2bin{substr($hex,$i,1)};
    #print "bin is \"$bin\"\n";
  }
  #print "bits is \"$bits\"\n";
  #print "bin was \"$bin\"\n";
  $bin = sprintf("%0${bits}s",$bin);  # pad with zeros
  $bin = substr($bin,-1 * $bits); # truncate to correct length
  #print "bin is \"$bin\"\n";
  return $bin;
}

sub octal2bin {
  my ($octal) = @_;
  my ($bin,$i);

  %octal2bin = ( 0 => "000",
               1 => "001",
               2 => "010",
               3 => "011",
               4 => "100",
               5 => "101",
               6 => "110",
               7 => "111",
  );
  
  for ($i = 0; $i < length($octal) ; $i++) {
    $bin = $bin . $octal2bin{substr($octal,$i,1)};
  }
  return $bin;
}

sub dec2bin {
  my ($dec,$bits) = @_;
  my ($hex,$bin,$i);

  $hex = sprintf("%x",$dec);
  #print "hex is $hex\n";
  $bin = &hex2bin($hex,$bits);
  #print "bin is $bin\n";
  # Strip leading zeros
  $bin =~ s/^0*//;
  $bin = "0" if ($bin eq "");
  # Zero extend according to requested bitlength
  #print "dec2bin bin was $bin for bits $bits\n";
  if ($bits) {
    $bin = "0" x ($bits - length($bin)) . $bin;
    #$bin = sprintf("%0${bits}s",$bin);
  }
  #print "dec2bin extended bin is $bin\n";
  if ($bits) {
    substr($bin,length($bin) - $bits,$bits);
  }
  #print "dec2bin extended bin with bits applied is $bin\n";
  return $bin
}

sub convert2bin {
  my ($value,$check) = @_;
  my ($bits,$val);
  my $bitlen = "";

  #print "Starting convert2bin on value \"$value\n";

  if (($bitlen,$format,$val) = ($value =~ /^(?:(\d*))?'(?:([bdoh]))?([0-9a-fA-F]*)$/)) {
  } elsif (($val) = ($value =~ /^(\d+)$/)) {
    $format = "d" unless ($format);
  } else {
    $format = "b" unless ($format);
    $val = $value;
  }
  #print "value: $value: bitlen: $bitlen format: $format val: $val\n";
  if ($format eq "b") {
    #print "converted a bin $value: bitlen: $bitlen format: $format val: $val\n";
    $bitlen = length($val) unless ($bitlen); # default length
    if ($bitlen != length($val)) {
      &error($indent,"Binary value $value has a size that doesn't match the value");
    }
    $value = substr($val,length($val) - $bitlen,$bitlen);
  } elsif ($format eq "h") {
    #print "converted a hex $value: bitlen: $bitlen format: $format val: $val\n";
    $bitlen = 4 * length($val) unless ($bitlen); # default length
    $value = &hex2bin($val,$bitlen);
    # Zero extend
    #print "value was $value\n";
    #$value = "0" x ($bitlen - length($value)) . $value;
    #print "value extended is $value\n";
    $value = substr($value,length($value) - $bitlen,$bitlen);
  } elsif ($format eq "o") {
    #print "converted a octal $value: bitlen: $bitlen format: $format val: $val\n";
    $bitlen = 3 * length($val) unless ($bitlen); # default length
    $value = &octal2bin($val);
    $value = substr($value,length($value) - $bitlen,$bitlen);
  } elsif ($format eq "d") {
    #print "converted a dec $value: bitlen: $bitlen format: $format val: $val ";
    $value = &dec2bin($val,$bitlen);
    #print "to $value\n";
  } else {
    &error($indent,"Got an unrecognized format $format in convert2bin") unless (!$check);
  }

  #print "returning $value\n";
  #print "\n";
  return $value;
}

sub get_statevals {
  my($bits) = @_;
  my(@statevals);
  my($i);
  my($bin);

  for ($i = 0 ; $i < 2 ** $bits ; $i++) {
    $bin = &dec2bin($i,$bits);
    push(@statevals,$bin);
  }
  return @statevals;
}


sub debug {
  my ($string,$depth,$section) = @_;
  if ($global_debug || $debug_sections{$section}) {
    print STDERR " " x $depth . "<db${section}>: $string\n";
  }
}

sub verbose {
  my ($string,$depth,$section) = @_;
  if ($verbose) {
    print STDERR " " x $depth . "$string\n";
  }
}

sub error {
  my ($indent,$string) = @_;

  # indent is ignored.  It is just there to make it easy to switch btw
  # warnings and errors.

  &print_output;
  print "\n\nError: $string - exiting\n";
  print STDERR "\n\nError: $string - exiting\n";
  exit;
}

sub warning {
  my($indent,$string) = @_;
  my($group,$number) = ();

  # Determine warning number based on string.  It would be cleaner to just
  # have the call use the number, but that makes it difficult to see what
  # the warning is when reading the code.

  # R group - resets
  if ($string =~ /No reset specified/) {$group = "R"; $number = 1;}
#  elsif ($string =~ /No reset value for datapath output \S+ set in reset state \S+ - Assiging a reset value of \S+ based on default/) {$group = "R"; $number = 5;}
  # Now an error:
  #elsif ($string =~ /Specified reset signal \S+ is not an input/) {$group = "R"; $number = 6;} 

  # I group - implied loopback
  # P group - priority
  # C group - combinational outputs
  # D group - defaults
  # O group - overrides
  # F group - flags
  # N group - includes
  # R group - resets
  # T group - data type
  elsif ($string =~ /Neither implied_loopback nor default_state_is_x attribute is set on state machine - defaulting to implied_loopback to avoid latches being inferred/) {$group = "I"; $number = 2;}

  elsif ($string =~ /State \S+ has multiple exit transitions, and transition \S+ has no defined priority/) {$group = "P"; $number = 3;}
  elsif ($string =~ /State \S+ has multiple exit transitions, and transition \S+ has the same priority as transition .*/) {$group = "P" ; $number = "4"}

  elsif ($string =~ /Combinational output \S+ is assigned on transitions, but has a non-default value ".+" in state \S+/) {$group = "C" ; $number = 7;}

  elsif ($string =~ /Neither implied_loopback nor default_state_is_x attribute is set on onehot state machine and there is no loopback arc - this could result in latches being inferred/) {$group = "I"; $number = 8;}

  elsif ($string =~ /Did not find any non-default values for any datapath outputs - suppressing case statement/) {$group = "D"; $number = 9;}

  elsif ($string =~ /Combinational output \S+ has no default value/) {$group = "C" ; $number = 10;}

  elsif ($string =~ /Datapath output \S+ has no default value/) {$group = "D" ; $number = 11;}

  elsif ($string =~ /Using override value from attribute/) {$group = "O" ; $number = 12;}

  elsif ($string =~ /undefined_states_go_here.*onehot/) {$group = "O" ; $number = 13;}

  elsif ($string =~ /No reset value set for flag/) {$group = "R"; $number = 14;}

  elsif ($string =~ /Flag \S+ is assigned on transitions, but is also assigned value ".+" in state \S+/) {$group = "F" ; $number = 17;}

  elsif ($string =~ /No reset value set for datapath.*reset state/) {$group = "R"; $number = 18;}

  elsif ($string =~ /No reset value set for datapath.*default/) {$group = "R"; $number = 18;}

  elsif ($string =~ /No reset value set for datapath.*using 0/) {$group = "R"; $number = 18;}

  elsif ($string =~ /Could not open file/) {$group = "N"; $number = 19;}

  elsif ($string =~ /specified as type "reg"/) {$group = "T"; $number = 20;}


  else {
    print "Unrecognized warning \"$string\" - exiting\n";
    print STDERR "Unrecognized warning \"$string\" - exiting\n";
    exit;
  }

  # Output warning unless suppressed
  unless ($nowarns{"${group}${number}"} || $nowarns{$group}) {
    eval "\$myindent = \"$indentstring\" x $indent";
    if ($warnout eq "stdout" || $warnout eq "both") {
      # warnings are stored in an array whose indeces correspond to the
      # previous line of pbuf.  Use concat in case there are multiple warnings
      # associated with the same line.
      $warnings[$#pbuf] = $warnings[$#pbuf] . "${myindent}${comm} Warning $group$number: $string \n";
    }
    if ($warnout eq "stderr" || $warnout eq "both") {
      print STDERR "\n\nWarning $group$number: $string \n";
    }
    return 0;
  }
  return 1; # signal error was suppressed
}

sub assertion {
  my($indent,$string) = @_;

  eval "\$myindent = \"$indentstring\" x $indent";
  $assertions[$#pbuf] = $assertions[$#pbuf] . "${myindent}${string}\n";
}

sub print {
  my($indent,$string) = @_;
  my($skip,$maxlen,$thislen) = 0;
  my($tag) = "${comm}tersetag";
  my($i,$j);
 
  $savesection = $section;
  $section = "terse"; # for debug
  if ($language eq "vhdl") {
  } else {
    # -terse handling
    # If you plan to follow or modify the -terse code, be sure to have an
    # ample supply of barf bags near to hand.  It is HORRIBLE.
    #
    # The basic idea is that all calls to &print go to an array (pbuf).  This
    # is done even without -terse, just to be consistent.  Warnings and
    # "comments" must go to separate arrays that parallel pbuf.  This makes
    # it easier for the tersify code to find its way around.
    #
    # When -terse is on, it looks for "end" and "endcase" (plus wire statements),
    # and starts poking around in the pbuf to try to get rid of begin/end pairs.
    # When it finds an "endcase", it backs up through pbuf to tag lines as
    # statement and non-statement, and to calculate how much indent will be
    # required to make the assignment statements line up.  It then goes back
    # forward through pbuf and makes these modifications.
    #
    # Yech!
     
    # 1) Get rid of unnecessary wire statements
    if ($terse && ($string =~ /^\s*(in|out)put wire /)) {
      #&debug("line:\n$string\n",0,$section);
      ($temp = $string) =~ s/\/\/.*$//; # temp is string without any comments
      unless ($temp =~ /\[\d+:\d+\]/) { # unless temp has [n:m]..
        $string =~ s/ wire//;           # get rid of the wire statement
        $string =~ s/,/,     /;         # re-pad in case of comment
        $string =~ s/,\s*$/,\n/;          # and strip trailing blanks
      }
      #&debug(" now:\n$string\n",0,$section);
    }
    # Ditto for logic inputs in sv
    if ($language eq "systemverilog" && $terse && ($string =~ /^\s*input logic /)) {
      ($temp = $string) =~ s/\/\/.*$//; # temp is string without any comments
      unless ($temp =~ /\[\d+:\d+\]/) { # unless temp has [n:m]..
        $string =~ s/ logic//;           # get rid of the wire statement
        $string =~ s/,/,      /;         # re-pad in case of comment
        $string =~ s/,\s*$/,\n/;          # and strip trailing blanks
      }
    }
  
    # 2) Get rid of extra begin/end pairs
    #
    if ($terse && ($string =~ /^\s*end\s*$/))  {
      # a) If we're on an "end" line and the next-to-previous line ends in begin,
      # strip the begin and newline and set skip flag.
      # ex: 
      # if (foo) begin
      #   bar <= 0
      # end
      # or:
      # begin
      #   bar <= 0
      # end
      if (!&is_stateline($pbuf[$#pbuf - 1])
       && $pbuf[$#pbuf - 1] =~ s/begin\s*$/\1/
      ) {
        &debug("Doing step a on \"$string\"\n",0,$section);
        $skip = 1;
        # If resulting string is empty, remove it
        if ($pbuf[$#pbuf - 1] =~ /^\s*$/) {
          splice(@pbuf, $#pbuf -1, 1);
        }
        # Unindent
        eval "\$myindent = \"$indentstring\"";
        $pbuf[$#pbuf] =~ s/$myindent//;
  
      # b) If we're on an "end" line and the next-to-previous line looks like
      # a "IDLE:" line, strip the begin and newline and set skip flag.
      # ex: 
      # IDLE: begin
      #   bar <= 0
      # end
      } elsif (&is_stateline($pbuf[$#pbuf - 1])) {
        &debug("Doing step b on \"$string\"\n",0,$section);
        if ($pbuf[$#pbuf - 1] =~ s/(\S+\s*:)\s+begin\s*$/\1/) {
          $skip = 1;
          # Unindent
          #eval "\$myindent = \"$indentstring\"";
          #$pbuf[$#pbuf] =~ s/$myindent//;
        }
  
      # c) If we're on an "end" line and the next-to-previous line is "else", 
      # 2 lines up from that is "if", and
      # 1 line from that is "state:"
      # IDLE: begin	< -4
      # if (foo)		< -3
      #   bar <=1		< -2
      # else		< -1
      #   bar <= 0	<  0
      # end		< curr
      #
      # remove the begin and newline, join the lines and set skip flag
      # (lines are joined explicitly because "case" step needs all "statements"
      # on a single line).
      # Also, scoot the -1 line (else) over by the length to the ":" plus 1
      #
      # Ends up as:
      # IDLE: if (foo) bar <=1   <-2
      # else    bar <= 0         <-1	
      # end		       < curr
      } elsif ($pbuf[$#pbuf -1] =~ /else /) {
        # Look for first if and previous state
        $n = 3;
        while ($pbuf[$#pbuf - $n] =~ /else if/) {
          $n = $n+2;
        }
  
        if ($pbuf[$#pbuf - $n] =~ /^\s*if /
             && $pbuf[$#pbuf - $n - 1] =~ /begin\s*$/  # no comment on state begin
             && (&is_stateline($pbuf[$#pbuf - $n - 1]))
             ) {
  
          &debug("In step c, n is \"$n\"\n",0,$section);
          $stateline = $#pbuf - $n - 1;
          &debug("In step c, stateline is \"$stateline\"\n",0,$section);
  
          &debug("Doing step c on \"$string\"\n",0,$section);
          $pbuf[$stateline] =~ s/(\S+\s*:)\s+begin\s*$/\1/; # strip begin
    
          # Do the "scoot"
          $colonpos = index($pbuf[$stateline],":");
          ($temp = $pbuf[$stateline]) =~ s/^(\s*).*$/\1/;
          $startpos = length($temp);
          $scoot = $colonpos - $startpos;
          &debug("Scoot is \"$scoot\"\n",0,$section);
          for ($i = $#pbuf-1; $i > $stateline+1; $i = $i - 2) {
            &debug("Scooting line \"$pbuf[$i]\"\n",0,$section);
            $pbuf[$i] = " " x $scoot . $pbuf[$i];
          }
          
          # Remove indent on -3 and join -3 and -4 into -3
          # Might not be literally -3 and -4..., make it relative to stateline
          $pbuf[$stateline+1] =~ s/^$level4indent/ /;
          $pbuf[$stateline+1] = $pbuf[$stateline] . $pbuf[$stateline+1];
          # And snuff out -4
          &move_warnings_and_assertions($stateline,$stateline+1); # move warnings/assertions to -3
          splice(@pbuf, $stateline, 1);
          #$pbuf[$#pbuf - 4] = "";
          $skip = 1;
        }
  
      # d) If we're on an "end" line and their are "stacked begin/begin end/end
      # sets, like this:
      # IDLE: begin	< -?
      #   begin           < -?
      #     bar <=1	< -?
      #     bar <= 0	< -1
      #   end             <  0
      # end		< curr
      # remove the inside begin/end pair, but do not set skip flag.
      } elsif ($pbuf[$#pbuf] =~ /^\s*end/) { # previous was ALSO end
        &debug("Doing step d on \"$string\"\n",0,$section);
        # troll through the buffer looking for the previous begin
        $i = $#pbuf-1;
        while ($pbuf[$i] !~ /^[^\/]*begin/ && $pbuf[$i] !~ /^\s*end/ && $i > 0) {
          $i--
        }
        # $i is now pointing at begin (or at end or start of buffer)
        # If it is begin, then do our thing
        if ($pbuf[$i] =~ /^\s*begin/) { # MUST be a PURE begin
          if ($pbuf[$i-1] =~ /^[^\/]*begin/) { # Previous is ALSO a begin
            # Note that pure begin/ends should not have warnings, so it is
            # safe to do the snuff...
            # snuff out $i entry
            splice(@pbuf, $i, 1);
            #$pbuf[$i] = "";
            # snuff out last entry (previous end)
            splice(@pbuf, $#pbuf, 1);
            #$pbuf[$#pbuf] = "";
          }
        }
  
      # e) If we're on an "end" line and the next-to-previous line is "if", 
      # 1 line from that is "state:"
      # IDLE: begin	< -2
      # if (foo)		< -1
      #   bar <=1		< 0
      # end		< curr
      # remove the begin and newline, join the lines and set skip flag
      # (lines are joined explicitly because "case" step needs all "statements"
      # on a single line).
      } elsif ( $pbuf[$#pbuf -1] =~ /if /
             && $pbuf[$#pbuf -2] =~ /begin\s*$/  # no comment on state begin
             && (&is_stateline($pbuf[$#pbuf - 2]))
             ) {
  
        &debug("Doing step e on \"$string\"\n",0,$section);
        $pbuf[$#pbuf - 2] =~ s/(\S+\s*:)\s+begin\s*$/\1/; # strip begin
  
        
        # Remove indent on 2 and join -1 and -2 into -1
        $pbuf[$#pbuf-1] =~ s/^$level4indent/ /;
        $pbuf[$#pbuf-1] = $pbuf[$#pbuf-2] . $pbuf[$#pbuf-1];
        # And snuff out -2
        &move_warnings_and_assertions($#pbuf-2,$#pbuf-1); # move warnings/assertions to -1
        splice(@pbuf, $#pbuf -2, 1);
        #$pbuf[$#pbuf - 2] = "";
        $skip = 1;
  
      } else {
        #print $string;
        #print $pbuf[$#pbuf - 1];
      }
    }
  
    # Change the statename (sim code) section to put state and assignment
    # on the same line
    if ($terse && ($string =~ /$statenamevar\s+=/)) {
      &debug("Found statename line \"$string\"\n",0,$section);
      &debug("$pbuf[$#pbuf]\n",0,$section);
      chomp($pbuf[$#pbuf]);
    }
  
    # 3) At endcase, back up and re-format assignment lines to make them line up
    if ($terse && ($string =~ /^\s*endcase/)) {
      &debug("\nBefore 1st pass:\n",0,$section);
      &debug(join("",@pbuf),0,$section);
      #for ($i=0; $i<=$#pbuf; $i++) {
      #  print "$i : $pbuf[$i]\n";
      #}
      
      # "0th pass": Back up through buffer to the case statement and find
      # the max char location of equations.
      $max_eqpos = 0;
      $i = $#pbuf;
      while ($pbuf[$i] !~ /^\s*(unique )?case/ && $i > 0) {
        $nocomment = &nocomment($pbuf[$i]);
        if ($nocomment =~ /\s(if)|(else if)/)  {
          &debug("Found eq line \"$nocomment\"\n",0,$section);
          $eqpos = index($nocomment,"(");
          $max_eqpos = $eqpos if ($eqpos > $max_eqpos);
          &debug("eqpos is $eqpos max_eqpos is $max_eqpos\n",0,$section);
        }
        $i--;
      }
      
      # 1st pass: Back up through the buffer to the case statement and gather
      # information about the longest statement line.  Also, tag the lines as
      # statement (#) and provide their length if applicable by pre-pending
      # the tag.   This will be used and removed on the 2nd pass.
      $maxlen = 0;
      $i = $#pbuf;
      $thislen = 0;
      while ($pbuf[$i] !~ /^\s*(unique )?case/ && $i > 0) {
        &debug("\n\n1st pass line is:\n",0,$section);
        &debug("\"$pbuf[$i]\"",0,$section);
        if ($pbuf[$i] =~ /^\s*(if)|(else)|(end)|(begin)/ || &is_stateline($pbuf[$i])) {
          # "statement" lines
          &debug("\nIt's a statement line\n",0,$section);
  
          # make sure we don't get fooled by ( in a comment...
          $nocomment = &nocomment($pbuf[$i]);
  
          # Shift equations over
          if ($nocomment =~ /\(/) { # have a line with an equation
            $eqpos = index($nocomment,"(");
            $eq_shift = $max_eqpos - $eqpos;
            $eq_pad = " " x $eq_shift;
            &debug("eqpos is $eqpos eq_shift is $eq_shift eq_pad is \"$eq_pad\"\n",0,$section);
            &debug("unmodified equation line is \"$pbuf[$i]\"\n",0,$section);
            $pbuf[$i] =~ s/\(/$eq_pad(/;  # insert spaces to move equation over
            # Adjust pad, unless pad is full maxlen (full maxlen means that
            # there was a newline)
            &debug("  modified equation line is \"$pbuf[$i]\"\n",0,$section);
            # re-create nocomment line
            $nocomment = &nocomment($pbuf[$i]);
          }
  
          # Get length and set maxlen
          $thislen = length($nocomment);
          &debug("got a match: thislen is $thislen\n",0,$section);
          $maxlen = $thislen if ($maxlen < $thislen);
  
          if ($pbuf[$i] =~ /\n$/) {
            # lines with eol should just to be marked
            $pbuf[$i] =~ s/^/## /;
          } else {
            # Tag with length (will be removed on 2nd pass)
            $pbuf[$i] =~ s/^/#$thislen /;
          }
  
        } else {
          &debug("\nIt's an assignment line\n",0,$section);
        }
        &debug("\n1st pass modified line is:\n",0,$section);
        &debug("\"$pbuf[$i]\"",0,$section);
        $i--;
      }
  
      &debug("\nBefore 2nd pass:\n",0,$section);
      &debug(join("",@pbuf),0,$section);
  
      &debug("\nBefore 2nd pass, in detail:\n",0,$section);
      for ($j=$i+1;$j<$#pbuf;$j++) {
        &debug($pbuf[$j],0,$section)
      }
      &debug("\n**** end of detail ****\n",0,$section);
  
      # 2nd pass: go forward through the case section of the buffer and make the
      # appropriate mods
      $maxlen++;
      &debug("maxlen is $maxlen\n",0,$section);
      &debug("max_eqpos is $max_eqpos\n",0,$section);
      # $i is from above!
      $i++; # go to next after case statement
      while ($i <= $#pbuf) {
        &debug("\n2nd pass line is:\n",0,$section);
        &debug("\"$pbuf[$i]\"",0,$section);
        if ($pbuf[$i] =~ /^#/) { # statement line of some sort
          if ($pbuf[$i] =~ s/^#(\d+) //) { # this is a line without a begin
            $pad = " " x ($maxlen - $1); # calculate pad for later lines
          } else {
            # "begin" lines will have a nl, so space following lines over
            # by the full maxlen
            $pbuf[$i] =~ s/^## //;
            $pad = " " x $maxlen;
          }
  
  
          &debug("taglen is $1 ; pad is \"$pad\"\n",0,$section);
          &debug("modified statement line is \"$pbuf[$i]\"\n",0,$section);
        } else {
          $pbuf[$i] =~ s/^\s*/$pad/;
          &debug("modified assignment line is \n\"$pbuf[$i]\"\n",0,$section);
          &debug("last 2 lines: \n" . $pbuf[$i-1] . $pbuf[$i],0,$section);
        }
        $i++;
      }
      #$max_eqpos = 0;
    }
  }

  #push(@pbuf,$indentstring x $indent . "$string") unless ($skip);
  # Handle indent in a way that makes regexp's work
  eval "\$myindent = \"$indentstring\" x $indent";
  $string = $myindent . $string;
  push(@pbuf,$string) unless ($skip);
  
  &debug("\npbuf is now:\n",0,$section);
  &debug(join("",@pbuf),0,$section);
  &debug("\npbuf done:\n",0,$section);

  $section = $savesection;
}

sub nocomment {
  my ($line) = @_;
  ($nocomment = $line) =~ s/\s*\/\/.*$//;
  $nocomment =~ s/\s*\/\*.*$//; # for /* - not sure if this is necessary
  return $nocomment;
}

sub print_output {
  # Dump print buffer and warnings
  for ($i=0; $i<=$#pbuf; $i++) {
#    print "$i : $pbuf[$i]";
#    if ($warnings[$i]) {
#      print "w$i : $warnings[$i]";
#    }
    #print "$i : $assertions[$i]";
    print $pbuf[$i];
    if ($pbuf[$i] =~ /\n/) {
      print $warnings[$i];
      print $assertions[$i];
    } else {
      # if line doesn't end in nl, append warning/assertion to next line
      $warnings[$i+1] = $warnings[$i] . $warnings[$i+1];
      $assertions[$i+1] = $assertions[$i] . $assertions[$i+1];
    }
  }
}

sub move_warnings_and_assertions {
  my ($from,$to) = @_;
  if ($warnings[$from]) { # move warning to $to
    $warnings[$to] = $warnings[$to] . $warnings[$from];
    $warnings[$from] = "";
  }
  splice(@warnings, $from, 1); # snuff out $from
  if ($assertions[$from]) { # move assertion to $to
    $assertions[$to] = $assertions[$to] . $assertions[$from];
    $assertions[$from] = "";
  }
  splice(@assertions, $from, 1); # snuff out $from
}

sub is_stateline {
  my($line) = @_;

  #print "line was: $line\n";
  $line =~ s/\s*\/\/.*$//; # get rid of comments
  #$line =~ s/\[.*:.*\]//g; # get rid of all bit ranges
  #$line =~ s/\[[^\]]+\]//g; # get rid of all bit ranges
  $line =~ s/\[[^\]]+:[^\]]+\]//g; # get rid of all bit ranges foo[1:0]
  #print "line is: $line\n";

  return
      ($line =~ /$nextstatevar\[\S+\]\s*:/)  # onehot
  ||  ($line =~ /$statevar\[\S+\]\s*:/)      # onehot "state[IDLE_BIT]:"
  ||  ($line =~ /^\s+\S+\s*:/)
  ||  ($line =~ /^\s*default\s*:/)
  ;
   
}

sub remove_from_array {
  my ($delvalue,@array) = @_;
  my @return;
  foreach $value (@array) {
    if (!($value eq $delvalue)) {
      push(@return, $value);
    }
  }
  return @return;
}

sub process_options {

  # Debug stuff
  &debug("orig_argv: " . join(" ",@orig_argv),0,"be_cmd");
  &debug("ARGV: " . join(" ",@ARGV),0,"be_cmd");

  use Getopt::Long;
  # Default non-null options
  $encoding = "heros";
  $statevar = "state";
  $nextstatevar = "nextstate";
  $statenamevar = "statename";
  $warnout = "both";
  $simcode = 2;  # funny default so we can see if it was forced on for SV
  $indentstring = "  ";
  $language = "verilog";
  $indexvar = "index";
  $addversion = "1";
  $max_iterations = 10000000;
  $regbits_warning_threshold = 8;
  $translate2vhdl = 0;
  $vhdl_intvar_suffix = "_internal";
  $insert_messages = 1;

  # Process options
  die unless GetOptions(
    "help" => \$help,
    "verbose!" => \$verbose,
    "debug=s" => \@debug_sections,
    "encoding=s" => \$encoding,
    "simcode!" => \$simcode,
    "maxbits=s" => \$usermaxbits,
    "minbits=s" => \$userminbits,
    "statevar=s" => \$statevar,
    "nextstatevar=s" => \$nextstatevar,
    "statenamevar=s" => \$statenamevar,
    "indexvar=s" => \$indexvar,
    "warnout=s" => \$warnout,
    "nowarn=s" => \@nowarns,
    "version!" => \$version,
    "addversion!" => \$addversion,
    "terse!" => \$terse,
    "sunburst!" => \$terse,
    "indentstring=s" => \$indentstring,
    "language=s" => \$language,
    "comb2reg!" => \$comb2reg,
    #"casedefault!" => \$casedefault, # not yet - need to figure out if/when this is safe (doesn't change logic or cause latches)
    "minversion=s" => \$minversion,
    "inst!" => \$inst,
    "force_undefined_goto_in_onehot!" => \$force_undefined_goto_in_onehot,
    "force_keep_undefined_goto_state!" => \$force_keep_undefined_goto_state,
    "test_flags" => \$test_flags, # temp code
    "iterations=s" => \$max_iterations,
    "translate2vhdl!" => \$translate2vhdl,
    "vhdl_intvar_suffix=s" => \$vhdl_intvar_suffix,
    "insert_messages!" => \$insert_messages,
    );
  
  if ($help) {
    &show_help;
    exit;
  }

  if ($version) {
    print "$me version $scriptversion\n";
    exit;
  }

  # Turn debug array into a hash
  foreach $section (@debug_sections) {
    $debug_sections{$section} = 1;
  }
  
  # Turn nowarn array into a hash
  foreach $nowarn (@nowarns) {
    $nowarns{$nowarn} = 1;
  }

  if ($language =~ /^verilog$/i) {
    $language = "verilog";
  } elsif ($language =~ /^s.*/i) {
      $language = "systemverilog";
  } elsif ($language =~ /^vh.*/i) {
      $language = "vhdl";
  } else {
    die "Unrecognized language \"$language\"\n";
  }

  if ($simcode == 2) { # default still
    if ($language eq "verilog") {
      $simcode = 1; # default on for verilog
    } elsif ($language eq "systemverilog") {
      $simcode = 0; # default off for sv
    }
  }


  # Create reserved words hash
  %reserved_words = (
  $statevar => 1,
  $nextstatevar => 1,
  $statenamevar => 1,
  $indexvar => 1,
  );

  # bit types for vhdl (might become arguments some day)
  $vhdl_bit_type = "STD_LOGIC";
  $vhdl_vector_type = "STD_LOGIC_VECTOR";

}

sub show_help {
 print STDERR "Syntax:
  $me [options] 

  options:
  -help                 Show this help
  -verbose              Be Chatty
  -debug section        Turn on debug messages for section <section>
                        (Can be used multiple times)
  -encoding <encoding>  Use encoding <encoding>
  -simcode              Add code to add ascii state names in simulation
  -maxbits <value>      For heros encoding, set upper limit of search range to <value>
  -minbits <value>      For heros encoding, set lower limit of search range to <value>
  -statevar <name>      Use <name> instead of \"state\" for the state variable
  -nextstatevar <name>  Use <name> instead of \"nextstate\" for the nextstate variable
  -statenamevar <name>     Use <name> instead of \"statename\" for the ASCII state name variable
  -indexvar <name>     Use <name> instead of \"index\" for the ASCII index variable in systemverilog onehot
  -warnout <output>     Control warning output.  <output> = stdout | stderr | both
  -nowarn <number>      Suppress warning number <number>
                        (Can be used multiple times)
  -version              Just echo version and exit
  -addversion           Add version to output as a comment
  -terse/-sunburst      Use Sunburst Design (Cliff Cummings) output format
  -indentstring <string> Use <string> instead of \"$indentstring\" for indenting
  -language <language>  Language options are Verilog, SystemVerilog, vhdl
  -comb2reg             Try to convert any comb outputs with all-constant
                        values to registered outputs. (experimental)
  -minversion <version> Require at least version \"version\" or bail out
  -force_undefined_goto_in_onehot  Don't ignore undefined_states_go_here attribute when doing onehot encoding
  -force_keep_undefined_goto_state  Don't suppress undefined_states_go_here state even if state space is full without it
  -iterations <value>    For heros encoding, set max number of iterations attempted before bailing out (default: $max_iterations)
  -vhdl_intvar_suffix <string> Suffix for vhdl internal variables (remapped port names).  Defaults to $vhdl_intvar_suffix

  Note: boolean switches accept the \"no\" syntax to turn them off 
    (So, -nosimcode turns off simcode)

  Recognized User Attributes:
    suppress_portlist - set on outputs to prevent them from appearing in the portlist
    include_at_top_of_file - read in a file ahead of the rest (useful for copyright statements)
    insert_at_top_of_file* - insert text at top of file
    insert_at_bottom_of_file* - insert text at bottom of file
    insert_in_module_declaraion* - insert in the module declaration section (useful for wires)
    insert_at_top_of_module* - insert text at top of module
    insert_at_bottom_of_module* - insert text at bottom of module
    insert_ahead_of_comb_block* - insert text ahead of comb block

  For details on recognized attributes, use \"-help -verbose\"
  \n";


#  -casedefault          Add an empty \"default: ;\" to end of case statements

  if ($verbose) {
    &set_myattributes;
    print STDERR "Recognized attributes:\n";
    foreach $entry (sort keys %myattributes) {
      $string = $myattributes{$entry};
      unless ($string =~ /^</) { # Skip entries marked with <>
        # Massage the output appearance
        #print "$entry:\n\t$string\n";
        foreach $array ("globals", "state", "transition") {
          $entry =~ s/^($array){"([^"]+)"}{/\1 > \2 > /;
        }
        #$entry =~ s/"//g;
        $entry =~ s/\}\{/ /g;
        $entry =~ s/[{}]//g;
        $entry =~ s/"value"/<value>/;
        $entry =~ s/"vis"/<vis>/;
        $entry =~ s/"type"/<type>/;
        $entry =~ s/ "attributes"//;
        $entry =~ s/ machine / state machine /;
        $entry =~ s/\*(\S+)\*/\$\1\$/g;
        $string =~ s/\*(\S+)\*/\$\1\$/g;
        print STDERR "$entry:\n\t$string\n\n";
      }
    }
  }
}

sub set_myattributes {
  # Cannot set based on encoding because file must be parsed to figure out
  # the encoding.  Cannot parse twice because it comes from STDIN.
  #if ($encoding eq "heros" || $encoding eq "onehot") {
    %myattributes = (
      'globals{"machine"}{"name"}{"value"}' => 'fsm module name',
      'globals{"machine"}{"clock"}{"value"}' => 'clock signal',
      'globals{"machine"}{"clock"}{"type"}' => 'clock signal type ("posedge", "negedge")',
      'globals{"machine"}{"reset_signal"}{"value"}' => 'reset signal',
      'globals{"machine"}{"reset_signal"}{"type"}' => 'reset signal type ("posedge", "negedge", "positive", "negative")',
      'globals{"machine"}{"reset_state"}{"value"}' => 'reset state',
      'globals{"machine"}{"reset_state"}{"type"}' => 'reset state type ("allzeros", "allones", "anyvalue")',
      'globals{"machine"}{"implied_loopback"}{"value"}' => 'Every state has an implied loopback (Note: overrides default_state_is_x)',
      'globals{"machine"}{"default_state_is_x"}{"value"}' => 'If no valid transition occur, set state to x',
      'globals{"machine"}{"undefined_states_go_to"}{"value"}' => 'undefined states got to <value>',
      'globals{"machine"}{"insert_at_top_of_file*"}{"value"}' => 'Text to insert at the top of the file (use \n to get newline, or use suffix to get multiple sorted lines)',
      'globals{"machine"}{"insert_in_module_declaration*"}{"value"}' => 'Text to insert at the top of the module (after module statement) (use \n to get newline, or use suffix to get multiple sorted lines)',
      'globals{"machine"}{"insert_at_top_of_module*"}{"value"}' => 'Text to insert at the top of the module (after module statement) (use \n to get newline, or use suffix to get multiple sorted lines)',
      'globals{"machine"}{"insert_at_bottom_of_file*"}{"value"}' => 'Text to insert at the bottom of the file (use \n to get newline, or use suffix to get multiple sorted lines)',
      'globals{"machine"}{"insert_at_bottom_of_module*"}{"value"}' => 'Text to insert at the bottom of the module (after module statement) (use \n to get newline, or use suffix to get multiple sorted lines)',
      'globals{"machine"}{"insert_ahead_of_comb_block*"}{"value"}' => 'Text to insert ahead of the combinational (always @* block) (usually for wire...) (use \n to get newline, or use suffix to get multiple sorted lines)',
      'globals{"machine"}{"stateout"}{"value"}' => 'Output on which to send out the state vector',
      'globals{"machine"}{"nextstateout"}{"value"}' => 'Output on which to send out the nextstate vector',
      'globals{"machine"}{"include_at_top_of_file"}{"value"}' => 'File to include (read) at the top of the file (value is file name)',
      'globals{"machine"}{"be_cmd"}{"value"}' => 'command to run backend',
      'globals{"machine"}{"onehot_pragma"}{"value"}' => 'Override for synopsys parallel_case full_case on onehot case statements',
      'globals{"inputs"}{"*input*"}{"value"}' => 'input signal *input*',
      'globals{"inputs"}{"*input*"}{"comment"}' => 'Comment for input signal *input*',
      'globals{"outputs"}{"*output*"}{"value"}' => 'output signal *output*',
      'globals{"outputs"}{"*output*"}{"type"}' => 'output signal type ("reg", "regdp" or "comb")',
      'globals{"outputs"}{"*output*"}{"comment"}' => 'Comment for output signal *output*',
      'globals{"outputs"}{"*output*"}{"useratts"}' => 'User attirbutes for output signal *output*',
      'globals{"outputs"}{"*output*"}{"resetval"}' => 'Reset value for output signal *output*',
      'globals{"machine"}{"name"}{"comment"}' => 'FSM name comment',
      'globals{"trans"}{"*output*"}{"value"}' => 'output signal *output* on transitions',
      'globals{"machine"}{".*"}{"value"}' => 'misc (indentified by type)',
      'globals{"machine"}{".*"}{"type"}' => 'misc types',

      'state{"*state*"}{"attributes"}{"*output*"}{"value"}' => 'Value of output signal *state* (if *output* is an output)',
      'state{"*state*"}{"attributes"}{"vis"}' => '<internal - forces state to be parsed even if no outputs',
      'state{"*state*"}{"attributes"}{"comment"}' => 'Comment for state *state*',

      'transition{"*transition*"}{"attributes"}{"equation"}{"value"}' => 'Transition equation for transition *transition*',
      'transition{"*transition*"}{"attributes"}{"*output*"}{"value"}' => 'Value of output signal *output* (if *output* is an output) in transition *transition*',
      'transition{"*transition*"}{"attributes"}{"*output*"}{"type"}' => 'Type of output signal *output* (if *output* is an output, it will be "output") in transition *transition*',
      'transition{"*transition*"}{"attributes"}{"priority"}{"value"}' => 'Priority of transition *transition* relative to other transitions FROM that state',
      'transition{"*transition*"}{"startState"}' => '<Internal>',
      'transition{"*transition*"}{"endState"}' => '<Internal>',
      'transition{"*transition*"}{"attributes"}{"graycode"}{"value"}' => 'If set, transition must be gray-coded',
      'transition{"*transition*"}{"attributes"}{"graytransition"}{"value"}' => '<Internal - backward compatibility>',
      'transition{"*transition*"}{"attributes"}{"comment"}' => 'Comment for transition *transition*',
    );
  #} else {
  #  die "Can't do set_myattributes for encoding \"$encoding\"\n";
  #}
}


# when doing require or use we must return 1
1
