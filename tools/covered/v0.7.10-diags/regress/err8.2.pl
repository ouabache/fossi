# Name:     err8.2.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     06/21/2008
# Purpose:  

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "err8.2", 1, @ARGV );

# Simulate and get coverage information
if( $SIMULATOR eq "IV" ) {
  system( "iverilog -DDUMP err8.2.v; ./a.out" ) && die;
} elsif( $SIMULATOR eq "CVER" ) {
  system( "cver -q +define+DUMP err8.2.v" ) && die;
} elsif( $SIMULATOR eq "VCS" ) {
  system( "vcs +define+DUMP err8.2.v; ./simv" ) && die;
}

# Create CDD file that we will modify the version of
&runScoreCommand( "-t main -vcd err8.2.vcd -v err8.2.v -o err8.2.tmp.cdd" );

# Modify the version to something which is different
open( OLD_CDD, "err8.2.tmp.cdd" ) || die "Can't open err8.2.tmp.cdd for reading: $!\n";
open( NEW_CDD, ">err8.2.cdd" ) || die "Can't open err8.2.cdd for writing: $!\n";
while( <OLD_CDD> ) {
  chomp;
  if( /^8\s+(.*)$/ ) {
    print NEW_CDD "8\n";
  } else {
    print NEW_CDD "$_\n";
  }
}
close( NEW_CDD );
close( OLD_CDD );
system( "rm -f err8.2.tmp.cdd" ) && die;

&runReportCommand( "-d v -m ltcfam err8.2.cdd 2> err8.2.err" );
system( "cat err8.2.err" ) && die;

# Perform the file comparison checks
&checkTest( "err8.2", 1, 1 );

exit 0;

