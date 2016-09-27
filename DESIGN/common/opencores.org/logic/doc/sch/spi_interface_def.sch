v 20100214 1
C 1800 300 1 0 0 in_port_vector.sym   
{
T 1800 300 5 10 1 1 0 6 1 1
refdes=tx_data[15:0]
}
C 1800 700 1 0 0 in_port.sym  
{
T 1800 700 5 10 1 1 0 6 1 1 
refdes=spi_sel_n_pad_in
}
C 1800 1100 1 0 0 in_port.sym  
{
T 1800 1100 5 10 1 1 0 6 1 1 
refdes=spi_mosi_pad_in
}
C 1800 1500 1 0 0 in_port.sym  
{
T 1800 1500 5 10 1 1 0 6 1 1 
refdes=spi_clk_pad_in
}
C 1800 1900 1 0 0 in_port.sym  
{
T 1800 1900 5 10 1 1 0 6 1 1 
refdes=reset
}
C 1800 2300 1 0 0 in_port.sym  
{
T 1800 2300 5 10 1 1 0 6 1 1 
refdes=clk
}
C 4700 300  1 0  0 out_port_vector.sym
{
T 5700 300 5  10 1 1 0 0 1 1 
refdes=rx_data[15:0]
}
C 4700 700  1 0 0 out_port.sym
{
T 5700 700 5  10 1 1 0 0 1 1
refdes=spi_miso_pad_out
}
C 4700 1100  1 0 0 out_port.sym
{
T 5700 1100 5  10 1 1 0 0 1 1
refdes=busy
}
