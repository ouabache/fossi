v 20100214 1
C 2400 300 1 0 0 in_port_vector.sym   
{
T 2400 300 5 10 1 1 0 6 1 1
refdes=io_host_out_bits[15:0]
}
C 2400 700 1 0 0 in_port.sym  
{
T 2400 700 5 10 1 1 0 6 1 1 
refdes=reset
}
C 2400 1100 1 0 0 in_port.sym  
{
T 2400 1100 5 10 1 1 0 6 1 1 
refdes=io_host_out_valid
}
C 2400 1500 1 0 0 in_port.sym  
{
T 2400 1500 5 10 1 1 0 6 1 1 
refdes=io_host_in_ready
}
C 2400 1900 1 0 0 in_port.sym  
{
T 2400 1900 5 10 1 1 0 6 1 1 
refdes=clk
}
C 6000 300  1 0  0 out_port_vector.sym
{
T 7000 300 5  10 1 1 0 0 1 1 
refdes=io_host_in_bits[15:0]
}
C 6000 700  1 0 0 out_port.sym
{
T 7000 700 5  10 1 1 0 0 1 1
refdes=io_host_out_ready
}
C 6000 1100  1 0 0 out_port.sym
{
T 7000 1100 5  10 1 1 0 0 1 1
refdes=io_host_in_valid
}
C 6000 1500  1 0 0 out_port.sym
{
T 7000 1500 5  10 1 1 0 0 1 1
refdes=io_host_debug_stats_pcr
}
C 6000 1900  1 0 0 out_port.sym
{
T 7000 1900 5  10 1 1 0 0 1 1
refdes=io_host_clk_edge
}
C 6000 2300  1 0 0 out_port.sym
{
T 7000 2300 5  10 1 1 0 0 1 1
refdes=io_host_clk
}
