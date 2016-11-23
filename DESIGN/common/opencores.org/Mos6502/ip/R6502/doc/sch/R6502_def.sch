v 20100214 1
C 1700 300 1 0 0 in_port_vector.sym   
{
T 1700 300 5 10 1 1 0 6 1 1
refdes=gpio_1_in[7:0]
}
C 1700 700 1 0 0 in_port_vector.sym   
{
T 1700 700 5 10 1 1 0 6 1 1
refdes=gpio_0_in[7:0]
}
C 1700 1100 1 0 0 in_port.sym  
{
T 1700 1100 5 10 1 1 0 6 1 1 
refdes=uart_rxd_pad_in
}
C 1700 1500 1 0 0 in_port.sym  
{
T 1700 1500 5 10 1 1 0 6 1 1 
refdes=reset
}
C 1700 1900 1 0 0 in_port.sym  
{
T 1700 1900 5 10 1 1 0 6 1 1 
refdes=cts_pad_in
}
C 1700 2300 1 0 0 in_port.sym  
{
T 1700 2300 5 10 1 1 0 6 1 1 
refdes=clk
}
C 4600 300  1 0  0 out_port_vector.sym
{
T 5600 300 5  10 1 1 0 0 1 1 
refdes=gpio_1_out[7:0]
}
C 4600 700  1 0  0 out_port_vector.sym
{
T 5600 700 5  10 1 1 0 0 1 1 
refdes=gpio_1_oe[7:0]
}
C 4600 1100  1 0  0 out_port_vector.sym
{
T 5600 1100 5  10 1 1 0 0 1 1 
refdes=gpio_0_out[7:0]
}
C 4600 1500  1 0  0 out_port_vector.sym
{
T 5600 1500 5  10 1 1 0 0 1 1 
refdes=gpio_0_oe[7:0]
}
C 4600 1900  1 0  0 out_port_vector.sym
{
T 5600 1900 5  10 1 1 0 0 1 1 
refdes=alu_status[7:0]
}
C 4600 2300  1 0 0 out_port.sym
{
T 5600 2300 5  10 1 1 0 0 1 1
refdes=uart_txd_pad_out
}
C 4600 2700  1 0 0 out_port.sym
{
T 5600 2700 5  10 1 1 0 0 1 1
refdes=rts_pad_out
}
