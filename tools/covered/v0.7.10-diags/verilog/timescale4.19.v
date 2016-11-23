/*
 Name:        timescale4.19.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        10/24/2008
 Purpose:     Verifies that real delays work properly when timescale is 10 s / 10 ms
*/

`timescale 10 s / 10 ms

module main;

reg a, b;

inner foo( b );

initial begin
	a = 1'b0;
	b = 1'b0;
	#(2.123_456_789_987_654);
	b = 1'b1;
	a = ($time == 2);
end

initial begin
`ifdef DUMP
        $dumpfile( "timescale4.19.vcd" );
        $dumpvars( 0, main );
`endif
        #(10);
        $finish;
end

endmodule

`timescale 10 ms / 10 ms

module inner(
  input b
);

reg a;

initial begin
        a = 1'b0;
        @(posedge b);
        a = ($time == 64'd2123);
end

endmodule

