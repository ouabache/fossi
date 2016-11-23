`define ONE 1

module main;

reg a, b;

initial begin
`ifdef FOO
`ifdef ZERO2ONE
	a = 1'b0;
	a = 1'b1;
`elsif ONE2ZERO
	a = 1'b1;
	a = 1'b0;
`else
	$display( "Hello world" );
`endif
`endif
	b = 1'b0;
	b = 1'b1;
end

initial begin
`ifdef DUMP
        $dumpfile( "elsif2.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
