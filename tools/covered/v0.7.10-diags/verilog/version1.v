/*
 Name:        version1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        09/21/2008
 Purpose:     Verifies that version information at the top of the file causes this information
              to be associated with each functional unit in the file.
 Version:     $Id: version1.v,v 1.2 2008/09/21 13:09:32 phase1geo Exp $
*/

module main;

reg a, b;

foo_mod mod( a, b );

initial begin
	b = 1'b1;
	a = b & foo_func( 1'b0 );
end

initial begin
`ifdef DUMP
        $dumpfile( "version1.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

function foo_func;
  input a;
  begin
    foo_func = ~a;
  end
endfunction

endmodule

module foo_mod(
  input a,
  input b
);

wire c = a | b;

endmodule
