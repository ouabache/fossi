module main;

reg a, c;

initial begin
	delay_for_awhile;
	a = 1'b0;
	#10;
end

initial begin
	c = 1'b0;
	#10;
	c = 1'b1;
end

initial begin
`ifdef DUMP
        $dumpfile( "task2.2.vcd" );
        $dumpvars( 0, main );
`endif
	#100;
	$finish;
end

task delay_for_awhile;

reg b;

begin
  b = 1'b0;
  @(posedge c);
  b = 1'b1;
  @(posedge c);
  b = 1'b0;
end

endtask

endmodule
