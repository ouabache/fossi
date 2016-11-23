module main;

reg  [7:0]        chr; 
wire [(8*8)-1:0]  str;

assign str = {"foo", chr};

initial begin
`ifdef DUMP
	$dumpfile( "string1.vcd" );
	$dumpvars( 0, main );
`endif
	chr = "bar";
	#10;
	chr = "doo";
	#10;
	$finish;
end

endmodule
