module main;

reg	clock;
reg	a, b;

always @(posedge clock)
  case( a )
    1'b0 :  b <= 1'b1;
    1'b1 :  b <= 1'b0;
  endcase

initial begin
`ifdef DUMP
	$dumpfile( "case1.2.vcd" );
	$dumpvars( 0, main );
`endif
	clock = 1'b0;
	forever #(5) clock = ~clock;
end

initial begin
	a = 1'b0;
	#20;
	$finish;
end

endmodule
