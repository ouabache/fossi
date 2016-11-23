/*
 Name:        fsm13.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        03/24/2010
 Purpose:     Verify that the "trans" parameters can allow for optional numbers/etc.
              after them to avoid warning messages with some compilers.
*/

module main;

parameter STATE_A = 0,
          STATE_B = 1,
          STATE_C = 2,
          STATE_D = 3;

reg       clock;
reg       reset;
reg       go;
reg [1:0] state, nstate;

always @(posedge clock) state <= reset ? STATE_A : nstate;

(* covered_fsm, foobar, is="state", os="nstate",
   trans0="STATE_A->STATE_A",
   trans1="STATE_A->STATE_B",
   transFOO="STATE_B->STATE_B",
   transbuggy="STATE_B->STATE_C",
   transa="STATE_C->STATE_C",
   trans__="STATE_C->STATE_A" *)
always @*
  case( state )
    STATE_A :   nstate = go ? STATE_B : STATE_A;
    STATE_B :   nstate = go ? STATE_C : STATE_B;
    STATE_C :   nstate = go ? STATE_A : STATE_C;
  endcase

initial begin
        clock = 1'b0;
        forever #(2) clock = ~clock;
end

initial begin
`ifdef DUMP
        $dumpfile( "fsm13.vcd" );
        $dumpvars( 0, main );
`endif
        reset <= 1'b1;
        go    <= 1'b0;
        @(posedge clock);
        reset <= 1'b0;
        repeat (2) @(posedge clock);
        go <= 1'b1;
        @(posedge clock);
        go <= 1'b0;
        #10;
        $finish;
end

endmodule
