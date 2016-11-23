/*
 Name:        exclude15a.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        08/27/2010
 Purpose:     See regress file for details.
*/

module main;

reg  a, b;
wire z;

dut_and d( z, a, b );

initial begin
`ifdef DUMP
        $dumpfile( "exclude15a.vcd" );
        $dumpvars( 0, main );
`endif
	a = 1'b0;
	b = 1'b0;
        #10;
        $finish;
end

endmodule
