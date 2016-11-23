module main;

parameter mod = 0;

generate
  if( mod < 1 )
    foo a();
  else
    bar a();
endgenerate

initial begin
`ifdef DUMP
        $dumpfile( "generate3.vcd" );
        $dumpvars( 0, main );
`endif
        #20;
        $finish;
end

initial begin
	a.x = 1'b0;
	#10;
	a.x = 1'b1;
end

endmodule

//--------------------------------

module foo;

reg x;

endmodule

//--------------------------------

module bar;
 
reg x;

endmodule 
