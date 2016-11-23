module main;

reg [2:1] a;
reg       b;

always @(a) b = a[1];

initial begin
`ifdef DUMP
	$dumpfile( "always6.vcd" );
	$dumpvars( 0, main );
`endif
	a = 2'b01;
	#5;
	a = 2'b10;
	#5;
	$finish;
end

endmodule
