module main;

event  a;
event  b, c;

reg    d;

always @(a)
  begin
   ->b;
   #5;
   ->c;
  end

always @(b) d <= ~d;
always @(c) d <= ~d;

initial begin
`ifdef DUMP
	$dumpfile( "event1.vcd" );
	$dumpvars( 0, main );
`endif
	d = 1'b0;
	#5;
	->a;
	#20;
	$finish;
end

endmodule
