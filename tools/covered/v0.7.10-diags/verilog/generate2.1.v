module main;

localparam a = 2;

generate
  if( a < 1 )
    foobar bar();
endgenerate

initial begin
`ifdef DUMP
        $dumpfile( "generate2.1.vcd" );
        $dumpvars( 0, main );
`endif
	#20;
	$finish;
end

endmodule

//---------------------------------

module foobar;

reg x;

endmodule
