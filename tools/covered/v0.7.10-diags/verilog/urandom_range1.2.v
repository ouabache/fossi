/*
 Name:        urandom_range1.2.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        10/04/2008
 Purpose:     Verifies that if max value is less than min, the two values are swapped before
              generating an unsigned random value via $urandom_range.
*/

module main;

integer i, a;

initial begin
	for( i=0; i<4; i=i+1 ) begin
	  a = $urandom_range( 1, 7 );
	  #5;
        end
end

initial begin
`ifdef DUMP
        $dumpfile( "urandom_range1.2.vcd" );
        $dumpvars( 0, main );
`endif
        #50;
        $finish;
end

endmodule
