module main;

wire [3:0] a;
reg        b;

assign a = {4{1'b0}};

initial begin
`ifdef DUMP
	$dumpfile( "expand4.3.vcd" );
	$dumpvars( 0, main );
`endif
	#5;
	b = 1'b0;
	#5;
	b = 1'bz;
	#5;
	$finish;
end

endmodule
