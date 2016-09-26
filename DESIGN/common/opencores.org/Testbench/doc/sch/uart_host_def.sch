v 20100214 1
C 1900 300 1 0 0 in_port_vector.sym   
{
T 1900 300 5 10 1 1 0 6 1 1
refdes=rxd_data_out[7:0]
}
C 1900 700 1 0 0 in_port.sym  
{
T 1900 700 5 10 1 1 0 6 1 1 
refdes=txd_buffer_empty
}
C 1900 1100 1 0 0 in_port.sym  
{
T 1900 1100 5 10 1 1 0 6 1 1 
refdes=rxd_stop_error
}
C 1900 1500 1 0 0 in_port.sym  
{
T 1900 1500 5 10 1 1 0 6 1 1 
refdes=rxd_parity_error
}
C 1900 1900 1 0 0 in_port.sym  
{
T 1900 1900 5 10 1 1 0 6 1 1 
refdes=rxd_data_avail
}
C 1900 2300 1 0 0 in_port.sym  
{
T 1900 2300 5 10 1 1 0 6 1 1 
refdes=reset
}
C 1900 2700 1 0 0 in_port.sym  
{
T 1900 2700 5 10 1 1 0 6 1 1 
refdes=clk
}
C 5000 300  1 0  0 out_port_vector.sym
{
T 6000 300 5  10 1 1 0 0 1 1 
refdes=txd_data_in[7:0]
}
C 5000 700  1 0 0 out_port.sym
{
T 6000 700 5  10 1 1 0 0 1 1
refdes=txd_parity
}
C 5000 1100  1 0 0 out_port.sym
{
T 6000 1100 5  10 1 1 0 0 1 1
refdes=txd_load
}
C 5000 1500  1 0 0 out_port.sym
{
T 6000 1500 5  10 1 1 0 0 1 1
refdes=txd_force_parity
}
C 5000 1900  1 0 0 out_port.sym
{
T 6000 1900 5  10 1 1 0 0 1 1
refdes=txd_break
}
C 5000 2300  1 0 0 out_port.sym
{
T 6000 2300 5  10 1 1 0 0 1 1
refdes=rxd_parity
}
C 5000 2700  1 0 0 out_port.sym
{
T 6000 2700 5  10 1 1 0 0 1 1
refdes=rxd_force_parity
}
C 5000 3100  1 0 0 out_port.sym
{
T 6000 3100 5  10 1 1 0 0 1 1
refdes=rxd_data_avail_stb
}
C 5000 3500  1 0 0 out_port.sym
{
T 6000 3500 5  10 1 1 0 0 1 1
refdes=parity_enable
}
