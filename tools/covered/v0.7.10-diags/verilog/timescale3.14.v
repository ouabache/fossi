/*
 Name:     timescale3.14.v
 Author:   Trevor Williams  (phase1geo@gmail.com)
 Date:     11/25/2006
 Purpose:  Verifies that missing timescale information flags appropriate error.
*/

`timescale

module main;

initial begin
`ifdef DUMP
        $dumpfile( "timescale3.14.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
