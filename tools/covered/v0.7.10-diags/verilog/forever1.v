module main;

reg a, b;

initial begin
	a = 1'b0;
	forever #5 a = ~a;
	b = 1'b0;
end

initial begin
`ifdef DUMP
        $dumpfile( "forever1.vcd" );
        $dumpvars( 0, main );
`endif
        #100;
        $finish;
end

endmodule
