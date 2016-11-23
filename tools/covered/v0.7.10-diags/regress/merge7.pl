# Name:     merge7.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     09/22/2008
# Purpose:  Merges two CDD files with exclusion reasons (no conflicts).

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "merge7", 0, @ARGV );

# Simulate the CDD files to merge
&run( "merge7a" ) && die;
&run( "merge7b" ) && die;

# Exclude variable "a" from toggle coverage in merge7a
&runCommand( "echo The output variable a is not being used > merge7a.excl" );
&runExcludeCommand( "-m T01 merge7a.cdd < merge7a.excl" );
system( "rm -f merge7a.excl" ) && die;

# Allow the timestamps to be different
sleep( 1 );

# Exclude variable "B" from toggle coverage in merge7b
&runCommand( "echo The output variable b is not being used > merge7b.excl" );
&runExcludeCommand( "-m T02 merge7b.cdd < merge7b.excl" );
system( "rm -f merge7b.excl" ) && die;

# Perform the merge (don't specify an exclusion reason resolver)
&runMergeCommand( "-o merge7.cdd merge7a.cdd merge7b.cdd" );

# Generate reports
&runReportCommand( "-d v -e -x -o merge7.rptM merge7.cdd" );
&runReportCommand( "-d v -e -x -i -o merge7.rptI merge7.cdd" );

# Perform the file comparison checks
if( $DUMPTYPE eq "VCD" ) {
  &checkTest( "merge7", 3, 0 ); 
} else {
  &checkTest( "merge7", 3, 5 );
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
    } elsif( $DUMPTYPE eq "FST" ) {
      &convertCfg( "fst", "${bname}.cfg" );
    }

    # Score CDD file
    &runScoreCommand( "-f ${bname}.cfg -D DUMP" );

  }

  return $retval;

}

