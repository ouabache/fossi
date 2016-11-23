module main;

reg	clock;
reg	a, b;

always @(posedge clock)
  case( a )
    1'b0    :  b <= 1'b1;
    1'b1    :  b <= 1'b0;
    default :  b <= 1'bx;
  endcase

initial begin
`ifdef DUMP
	$dumpfile( "case1.1.vcd" );
	$dumpvars( 0, main );
`endif
	clock = 1'b0;
	forever #(5) clock = ~clock;
end

initial begin
	a = 1'b0;
	#22;
	a = 1'b1;
	#22;
	a = 1'b0;
	#7;
	$finish;
end

endmodule
