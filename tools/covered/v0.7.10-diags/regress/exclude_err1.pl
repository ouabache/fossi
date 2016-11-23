# Name:     exclude_err1.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     09/17/2008
# Purpose:  Verifies that the appropriate error message is output if an invalid
#           exclude command operation is specified.

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "exclude_err1", 1, @ARGV );

# Perform diagnostic running code here
&runExcludeCommand( "-z 2> exclude_err1.err" );
system( "cat exclude_err1.err" ) && die;

# Perform the file comparison checks
&checkTest( "exclude_err1", 1, 1 );

exit 0;

