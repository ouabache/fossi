module main;

parameter STATE_IDLE = 1'b0,
          STATE_SEND = 1'b1;

reg            clock;
reg            reset;
reg            state;
reg  [1:0]     next_state;
wire           msg_ip;
reg            head;
reg            valid;

always @(posedge clock) state <= reset ? STATE_IDLE : next_state[1];

(* covered_fsm, channel, is="{reset,state}", os="next_state[1]" *)
always @(state or head or valid)
  begin
   casez( {reset, state} )
     {1'b1, 1'b?}       :  next_state = STATE_IDLE;
     {1'b0, STATE_IDLE} :  next_state = (valid & head) ? {STATE_SEND,1'b0} : {STATE_IDLE,1'b1};
     {1'b0, STATE_SEND} :  next_state =  valid         ? {STATE_SEND,1'b0} : {STATE_IDLE,1'b1};
   endcase
  end

assign msg_ip = ~next_state[0];

initial begin
`ifdef DUMP
	$dumpfile( "fsm8.3.vcd" );
	$dumpvars( 0, main );
`endif
	reset = 1'b1;
	valid = 1'b0;
	head  = 1'b0;
	#20;
	reset = 1'b0;
	@(posedge clock);
        head  <= 1'b1;
        valid <= 1'b1;
	@(posedge clock);
        head  <= 1'b0;
	valid <= 1'b0;
	#20;
	$finish;
end

initial begin
	clock = 1'b0;
	forever #(1) clock = ~clock;
end

endmodule
