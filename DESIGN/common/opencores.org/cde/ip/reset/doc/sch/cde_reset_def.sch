v 20100214 1
C 2300 300 1 0 0 in_port_vector.sym   
{
T 2300 300 5 10 1 1 0 6 1 1
refdes=sync_reset[WIDTH-1:0]
}
C 2300 700 1 0 0 in_port.sym  
{
T 2300 700 5 10 1 1 0 6 1 1 
refdes=clk
}
C 2300 1100 1 0 0 in_port.sym  
{
T 2300 1100 5 10 1 1 0 6 1 1 
refdes=atg_asyncdisable
}
C 2300 1500 1 0 0 in_port.sym  
{
T 2300 1500 5 10 1 1 0 6 1 1 
refdes=async_reset_n
}
C 5800 300  1 0  0 out_port_vector.sym
{
T 6800 300 5  10 1 1 0 0 1 1 
refdes=reset_out[WIDTH-1:0]
}
C 5800 700  1 0  0 out_port_vector.sym
{
T 6800 700 5  10 1 1 0 0 1 1 
refdes=reset_n_out[WIDTH-1:0]
}
