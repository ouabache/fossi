module main;

reg [1:0]  a;
reg [31:0] b;

always @*
  begin : foo
    real mem0, mem1;
    a = 2'b0;
    mem0 = 2'b01;
    mem1 = 2'b10;
    #5;
    a = b ? mem1 : mem0;
  end

initial begin
`ifdef DUMP
        $dumpfile( "slist3.3.vcd" );
        $dumpvars( 0, main );
`endif
	b = 1'b0;
        #10;
        $finish;
end

endmodule
