module main;

integer i, j;

initial begin
	i = 10 + 5 + 1 + 134;
	j = 10 + 5 + -1 + -134;
end

initial begin
`ifdef DUMP
        $dumpfile( "add1.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
