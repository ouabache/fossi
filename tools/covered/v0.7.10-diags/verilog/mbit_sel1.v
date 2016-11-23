module main;

reg     [4:1]    z;
wire    [1:0]    a, b, c, g, h, i, j;
wire    [2:0]    d, e;
wire    [3:0]    f;

assign a = z[2:1];
assign b = z[3:2];
assign c = z[4:3];
assign d = z[3:1];
assign e = z[4:2];
assign f = z[4:1];
assign g = z[6:5];
assign h = z[5:4];
assign i = z[1:0];
assign j = z[-1:-2];

initial begin
`ifdef DUMP
	$dumpfile( "mbit_sel1.vcd" );
	$dumpvars( 0, main );
`endif
	z = 4'h0;
	#5;
	z = 4'h1;
	#5;
	z = 4'h4;
	#5;
	z = 4'h8;
	#5;
	z = 4'h0;
	#5;
	$finish;
end

endmodule
