module main;

reg  b;
wire a = b;

initial begin
`ifdef DUMP
	$dumpfile( "err2.0.vcd" );
	$dumpvars( 0, main );
`endif
	b = 1'b1;
	#10;
	$finish;
end

endmodule
