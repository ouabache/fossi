module main;

reg a;

initial begin
	a = foo_func( {1'b0, 1'b1} );
end

initial begin
`ifdef DUMP
        $dumpfile( "port3.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

function foo_func;
input [1:0] b;
foo_func = |b;
endfunction

endmodule
