module main;

reg [31:0] i, a, b;

initial begin
	for( i=0; i<10; i++ )
          begin
	   a = i;
          end
end

initial begin
`ifdef DUMP
        $dumpfile( "for3.vcd" );
        $dumpvars( 0, main );
`endif
        #50;
        $finish;
end

endmodule
