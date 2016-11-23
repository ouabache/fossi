/*
 Name:     timescale3.11.v
 Author:   Trevor Williams  (phase1geo@gmail.com)
 Date:     11/25/2006
 Purpose:  Verifies that a missing time precision flags the appropriate error.
*/

`timescale 1 s / 

module main;

initial begin
`ifdef DUMP
        $dumpfile( "timescale3.11.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
