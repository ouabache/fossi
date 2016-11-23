/* 
 * Verifies the case where we have already parsed the needed module
 * and then hit another instance that instantiates the module again.
 */

module main;

reg [3:0] a, b;

foobar foo( a );
barfoo bar( b );

initial begin
`ifdef DUMP
	$dumpfile( "instance6.2.vcd" );
	$dumpvars( 0, main );
`endif
	a = 4'b0000;
        b = 4'b0111;
	#5;
	a = 4'b0001;
	#5;
	a = 4'b0100;
	#5;
	$finish;
end

endmodule


module foobar( b );

parameter bsize = 4;

input [bsize-1:0]  b;

wire [1:0] a;

assign a = b[1:0];

endmodule


module barfoo( b );

input [3:0]  b;

foobar #(2) dude( b[2:1] );

endmodule
