module main;

foo #(10,20) bar();

initial begin
`ifdef DUMP
        $dumpfile( "param9.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule


module foo;

parameter p1 = 5;
parameter p2 = 5;

wire [(p1-1):0] a;
wire [(p2-1):0] b;

endmodule
