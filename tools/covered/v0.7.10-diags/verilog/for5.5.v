/*
 Name:     for5.5.v
 Author:   Trevor Williams  (phase1geo@gmail.com)
 Date:     11/11/2007
 Purpose:  Verifies local time declaration in for loops.
*/

module main;

initial begin : foo
	reg [1:0] a;
        reg [1:0] i;
	i = 0;
	#5;
	for( time i=0; i<4; i++ ) begin : for1
	  a = i;
        end
	i = a;
end

initial begin
`ifdef DUMP
        $dumpfile( "for5.5.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
