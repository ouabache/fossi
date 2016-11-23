# Name:     exclude12.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     09/17/2008
# Purpose:  Verify that an excluded line coverage point can be printed properly.

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "exclude12", 1, @ARGV );

# Simulate and get coverage information
if( $SIMULATOR eq "IV" ) {
  system( "iverilog -DDUMP exclude12.v; ./a.out" ) && die;
} elsif( $SIMULATOR eq "CVER" ) {
  system( "cver -q +define+DUMP exclude12.v" ) && die;
} elsif( $SIMULATOR eq "VCS" ) {
  system( "vcs +define+DUMP exclude12.v; ./simv" ) && die;
}

# Perform diagnostic running code here
&runScoreCommand( "-t main -vcd exclude12.vcd -v exclude12.v -o exclude12.cdd" );

# Create temporary file that will contain an exclusion message
&runCommand( "echo This line is not needed > exclude12.excl" );

# Perform exclusion
&runExcludeCommand( "-m L10 exclude12.cdd < exclude12.excl" );

# Now print the exclusion
if( $CHECK_MEM_CMD ne "" ) {
  $check = 1;
  $CHECK_MEM_CMD = "";
}
&runExcludeCommand( "-p L10 exclude12.cdd > exclude12.out" );
if( $check == 1 ) {
  system( "cat exclude12.out | ./check_mem > exclude12.err" ) && die;
} else {
  system( "mv exclude12.out exclude12.err" ) && die;
}
system( "cat exclude12.err" ) && die;

# Remove temporary exclusion reason file
system( "rm -f exclude12.excl" ) && die;

# Perform the file comparison checks
&checkTest( "exclude12", 1, 1 );

exit 0;

