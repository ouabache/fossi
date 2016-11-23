# Name:     merge7.6.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     09/22/2008
# Purpose:  Merges two CDD files with exclusion reasons specified for the same coverage point (conflict).
#           Verifies that the "new" resolution value works properly.

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "merge7.6", 0, @ARGV );

# Simulate the CDD files to merge
&run( "merge7.6a" ) && die;
&run( "merge7.6b" ) && die;

# Exclude variable "a" from toggle coverage in merge7.6a
&runCommand( "echo The output variable a is not being used > merge7.6a.excl" );
&runExcludeCommand( "-m T01 merge7.6a.cdd < merge7.6a.excl" );
system( "rm -f merge7.6a.excl" ) && die;

# Allow the timestamps to be different
sleep( 1 );

# Exclude variable "a" from toggle coverage (with different message) in merge7.6b
&runCommand( "echo I dont like the variable a > merge7.6b.excl" );
&runExcludeCommand( "-m T01 merge7.6b.cdd < merge7.6b.excl" );
system( "rm -f merge7.6b.excl" ) && die;

# Perform the merge using the last reason found
&runMergeCommand( "-er new -o merge7.6.cdd merge7.6b.cdd merge7.6a.cdd" );

# Generate reports
&runReportCommand( "-d v -e -x -o merge7.6.rptM merge7.6.cdd" );
&runReportCommand( "-d v -e -x -i -o merge7.6.rptI merge7.6.cdd" );

# Perform the file comparison checks 
if( $DUMPTYPE eq "VCD" ) {
  &checkTest( "merge7.6", 3, 0 );
} else {
  &checkTest( "merge7.6", 3, 5 );
} 

exit 0;


sub run {

  my( $bname )  = $_[0];
  my( $retval ) = 0;
  my( $fmt )    = "";

  # If we are using the VPI, run the score command and add the needed pieces to the simulation runs
  if( $USE_VPI == 1 ) {
    &convertCfg( "vpi", "${bname}.cfg" );
    &runScoreCommand( "-f ${bname}.cfg" );
    $vpi_args = "+covered_cdd=${bname}.cdd";
    if( $COVERED_GFLAGS eq "-D" ) {
      $vpi_args .= " +covered_debug";
    } 
  } 
  
  # Simulate the design
  if( $SIMULATOR eq "IV" ) {
    if( $USE_VPI == 1 ) {
      &runCommand( "iverilog -y lib -m ../../lib/covered.vpi ${bname}.v covered_vpi.v; ./a.out ${vpi_args}" );
    } else {
      &runCommand( "iverilog -DDUMP -y lib ${bname}.v; ./a.out" );
    }
  } elsif( $SIMULATOR eq "CVER" ) {
    if( $USE_VPI == 1 ) {
      &runCommand( "cver -q +libext+.v+ -y lib +loadvpi=../../lib/covered.cver.so:vpi_compat_bootstrap ${bname}.v covered_vpi.v ${vpi_args}" );
    } else {
      &runCommand( "cver -q +define+DUMP +libext+.v+ -y lib ${bname}.v" );
    }
  } elsif( $SIMULATOR eq "VCS" ) {
    if( $USE_VPI == 1 ) {
      &runCommand( "vcs +v2k -sverilog +libext+.v+ -y lib +vpi -load ../../lib/covered.vcs.so:covered_register ${bname}.v covered_vpi.v; ./simv ${vpi_args}" );
    } else {
      &runCommand( "vcs +define+DUMP +v2k -sverilog +libext+.v+ -y lib ${bname}.v; ./simv" );
    }
  } else {
    die "Illegal SIMULATOR value (${SIMULATOR})\n";
  }

  # If we are doing VCD/LXT simulation, run the score command post-process
  if( $USE_VPI == 0 ) {

    # Convert configuration file
    if( $DUMPTYPE eq "VCD" ) {
      &convertCfg( "vcd", "${bname}.cfg" );
    } elsif( $DUMPTYPE eq "LXT" ) {
      &convertCfg( "lxt", "${bname}.cfg" );
    }

    # Score CDD file
    &runScoreCommand( "-f ${bname}.cfg -D DUMP" );

  }

  return $retval;

}

