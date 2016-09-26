v 20100214 1
C 700 300 1 0 0 in_port.sym  
{
T 700 300 5 10 1 1 0 6 1 1 
refdes=reset
}
C 700 700 1 0 0 in_port.sym  
{
T 700 700 5 10 1 1 0 6 1 1 
refdes=clk
}
C 4000 300  1 0  0 out_port_vector.sym
{
T 5000 300 5  10 1 1 0 0 1 1 
refdes=wdata[7:0]
}
C 4000 700  1 0  0 out_port_vector.sym
{
T 5000 700 5  10 1 1 0 0 1 1 
refdes=addr[addr_width-1:0]
}
C 4000 1100  1 0 0 out_port.sym
{
T 5000 1100 5  10 1 1 0 0 1 1
refdes=wr
}
C 4000 1500  1 0 0 out_port.sym
{
T 5000 1500 5  10 1 1 0 0 1 1
refdes=rd
}
C 4000 1900  1 0 0 out_port.sym
{
T 5000 1900 5  10 1 1 0 0 1 1
refdes=cs
}
C 4000 2300  1 0  0 io_port_vector.sym
{
T 5000 2300 5  10 1 1 0 0 1 1 
refdes=rdata[7:0]
}
