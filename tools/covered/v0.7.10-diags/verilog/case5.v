module main;

wire      a;
`ifdef RUNTEST
real      mem0;
real      mem1;
`endif
reg [1:0] entry;

always @*
  case( a )
    1'b0    :  entry = mem0;
    1'b1    :  entry = mem1;
    default :  entry = 2'bx;
  endcase

initial begin
`ifdef DUMP
        $dumpfile( "case5.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
