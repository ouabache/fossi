/*
 Name:        merge_err2.1a.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        08/05/2008
 Purpose:     See script for details.
*/

module main_a;

reg  a, b;
wire c, z;

adder1 adder (
  .a(a),
  .b(b),
  .c(c),
  .z(z)
);

initial begin
`ifdef DUMP
        $dumpfile( "merge_err2.1a.vcd" );
        $dumpvars( 0, main_a );
`endif
	a = 1'b0;
	b = 1'b1;
        #10;
	a = 1'b1;
        $finish;
end

endmodule
