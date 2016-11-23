# Name:     merge_err2.1.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     08/04/2008
# Purpose:  Verifies that an error is displayed if a file is attempted to be merged more than once.

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "merge_err2.1", 0, @ARGV );

# Simulate and get coverage information
if( $SIMULATOR eq "IV" ) {
  system( "iverilog -DDUMP -y lib merge_err2.1a.v; ./a.out" ) && die;
  system( "iverilog -DDUMP -y lib merge_err2.1b.v; ./a.out" ) && die;
  system( "iverilog -DDUMP -y lib merge_err2.1c.v; ./a.out" ) && die;
} elsif( $SIMULATOR eq "CVER" ) {
  system( "cver -q +define+DUMP -y lib merge_err2.1a.v" ) && die;
  system( "cver -q +define+DUMP -y lib merge_err2.1b.v" ) && die;
  system( "cver -q +define+DUMP -y lib merge_err2.1c.v" ) && die;
} elsif( $SIMULATOR eq "VCS" ) {
  system( "vcs +define+DUMP -y lib merge_err2.1a.v; ./simv" ) && die;
  system( "vcs +define+DUMP -y lib merge_err2.1b.v; ./simv" ) && die;
  system( "vcs +define+DUMP -y lib merge_err2.1c.v; ./simv" ) && die;
}

# Perform diagnostic running code here
&runScoreCommand( "-t adder1 -i main_a.adder -v merge_err2.1a.v -y lib -D DUMP -o merge_err2.1.cdd" );
&runScoreCommand( "-t adder1 -i main_a.adder -vcd merge_err2.1a.vcd -v merge_err2.1a.v -y lib -D DUMP -o merge_err2.1a.cdd" );
&runScoreCommand( "-t adder1 -i main_b.adder -vcd merge_err2.1b.vcd -v merge_err2.1b.v -y lib -D DUMP -o merge_err2.1b.cdd" );
&runScoreCommand( "-t adder1 -i main_c.adder -vcd merge_err2.1c.vcd -v merge_err2.1c.v -y lib -D DUMP -o merge_err2.1c.cdd" );

# Perform several merges
&runMergeCommand( "merge_err2.1.cdd merge_err2.1a.cdd" );
&runMergeCommand( "merge_err2.1.cdd merge_err2.1b.cdd" );
&runMergeCommand( "merge_err2.1.cdd merge_err2.1c.cdd" );

# Create reports just to make sure everything is okay here
&runReportCommand( "-d v -o merge_err2.1.rptM merge_err2.1.cdd" );
&runReportCommand( "-d v -i -o merge_err2.1.rptI merge_err2.1.cdd" );

# Perform the file comparison checks
&checkTest( "merge_err2.1", 4, 0 );

exit 0;

