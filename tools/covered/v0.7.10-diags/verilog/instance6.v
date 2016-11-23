/* 
 * Verifies the case where we have already parsed the needed module
 * and then hit another instance that instantiates the module again.
 */

module main;

reg [2:0] a, b;

foobar foo( a );
barfoo bar( b );

initial begin
`ifdef DUMP
	$dumpfile( "instance6.vcd" );
	$dumpvars( 0, main );
`endif
	a = 3'b000;
        b = 3'b111;
	#5;
	a = 3'b001;
	#5;
	a = 3'b100;
	#5;
	$finish;
end

endmodule


module foobar( b );

input [2:0]  b;

wire [1:0] a;

assign a = b[2:1];

endmodule


module barfoo( b );

input [2:0]  b;

foobar dude( b );

endmodule
