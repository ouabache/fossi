v 20100214 1
C 1700 300 1 0 0 in_port_vector.sym   
{
T 1700 300 5 10 1 1 0 6 1 1
refdes=rx_data[7:0]
}
C 1700 700 1 0 0 in_port.sym  
{
T 1700 700 5 10 1 1 0 6 1 1 
refdes=tx_ack_error
}
C 1700 1100 1 0 0 in_port.sym  
{
T 1700 1100 5 10 1 1 0 6 1 1 
refdes=rx_read
}
C 1700 1500 1 0 0 in_port.sym  
{
T 1700 1500 5 10 1 1 0 6 1 1 
refdes=rx_parity_rcv
}
C 1700 1900 1 0 0 in_port.sym  
{
T 1700 1900 5 10 1 1 0 6 1 1 
refdes=rx_parity_error
}
C 1700 2300 1 0 0 in_port.sym  
{
T 1700 2300 5 10 1 1 0 6 1 1 
refdes=rx_parity_cal
}
C 1700 2700 1 0 0 in_port.sym  
{
T 1700 2700 5 10 1 1 0 6 1 1 
refdes=rx_full
}
C 1700 3100 1 0 0 in_port.sym  
{
T 1700 3100 5 10 1 1 0 6 1 1 
refdes=rx_frame_error
}
C 1700 3500 1 0 0 in_port.sym  
{
T 1700 3500 5 10 1 1 0 6 1 1 
refdes=reset
}
C 1700 3900 1 0 0 in_port.sym  
{
T 1700 3900 5 10 1 1 0 6 1 1 
refdes=clk
}
C 1700 4300 1 0 0 in_port.sym  
{
T 1700 4300 5 10 1 1 0 6 1 1 
refdes=busy
}
C 4200 300  1 0  0 out_port_vector.sym
{
T 5200 300 5  10 1 1 0 0 1 1 
refdes=tx_data[7:0]
}
C 4200 700  1 0 0 out_port.sym
{
T 5200 700 5  10 1 1 0 0 1 1
refdes=tx_write
}
C 4200 1100  1 0 0 out_port.sym
{
T 5200 1100 5  10 1 1 0 0 1 1
refdes=rx_clr
}
