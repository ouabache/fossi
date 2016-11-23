/*
 Name:        err8.1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        06/21/2008
 Purpose:     See script for details.
*/

module main;

reg a;

initial begin
	a = 1'b0;
	#5;
	a = 1'b1;
end

initial begin
`ifdef DUMP
        $dumpfile( "err8.1.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
