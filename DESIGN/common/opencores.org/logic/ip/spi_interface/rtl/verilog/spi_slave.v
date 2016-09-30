module spi_slave(


		 input  wire 	    clk,
		 input  wire 	    reset,
		 input  wire 	    SCK,
		 input  wire 	    MOSI,
		 output wire 	    MISO, 
		 input  wire 	    SSEL,
		 output reg   [15:0] rx_data,
		 input  wire  [15:0] tx_data
);




   
// sync SCK to the FPGA clock using a 3-bits shift register
reg [2:0] SCKr;  always @(posedge clk) SCKr <= {SCKr[1:0], SCK};

wire SCK_risingedge  = (SCKr[2:1]==2'b01);  // now we can detect SCK rising edges

// same thing for SSEL
reg [2:0] SSELr;  always @(posedge clk) SSELr <= {SSELr[1:0], SSEL};
wire SSEL_active = ~SSELr[1];  // SSEL is active low

   

// and for MOSI
reg [1:0] MOSIr;  always @(posedge clk) MOSIr <= {MOSIr[0], MOSI};
wire MOSI_data = MOSIr[1];
// we handle SPI in 8-bits format, so we need a 3 bits counter to count the bits as they come in
reg [3:0] bitcnt;

reg byte_received;  // high when a byte has been received
reg [31:0] byte_data_received;
reg [15:0] byte_data_sent;

   
always @(posedge clk)
begin
  if(~SSEL_active)
    bitcnt <= 4'b0000;
  else
  if(SCK_risingedge)
  begin
    bitcnt <= bitcnt + 4'b0001;
  end
end



always @(posedge clk)
begin
  if(reset)
    byte_data_received <= 32'h00000000;
  else
  if(SCK_risingedge)
  begin
    // implement a shift-left register (since we receive the data MSB first)
    byte_data_received <= {byte_data_received[30:0], MOSI_data};
  end
  else
  begin
    // implement a shift-left register (since we receive the data MSB first)
    byte_data_received <= byte_data_received;
  end
end






   


   
always @(posedge clk) byte_received <= SSEL_active && SCK_risingedge && (bitcnt==4'b1111);

// we use the LSB of the data received to control an LED

always @(posedge clk)
  if(reset )         rx_data <= 16'h0000;

  else
    begin
   if(byte_received) rx_data <= byte_data_received[15:0];
   else              rx_data <= rx_data;
   end




always @(posedge clk)
begin
  if(reset)
    byte_data_sent <= 16'h0000;
  else
  if(SCK_risingedge)
  begin
    // implement a shift-left register (since we receive the data MSB first)
    byte_data_sent <= {byte_data_sent[14:0], 1'b0};
  end
  else
    begin
 if(bitcnt == 4'b0000)

    
  begin
    // implement a shift-left register (since we receive the data MSB first)
    byte_data_sent <= tx_data;
  end
  else
    
  begin
    // implement a shift-left register (since we receive the data MSB first)
    byte_data_sent <= byte_data_sent;
  end

 end
   
end


   

   

   
assign MISO = byte_data_sent[15];  // send MSB first

// we assume that there is only one slave on the SPI bus
// so we don't bother with a tri-state buffer for MISO
// otherwise we would need to tri-state MISO when SSEL is inactive

endmodule
