v 20100214 1
C 1900 300 1 0 0 in_port_vector.sym   
{
T 1900 300 5 10 1 1 0 6 1 1
refdes=wdata[ WIDTH-1:0]
}
C 1900 700 1 0 0 in_port_vector.sym   
{
T 1900 700 5 10 1 1 0 6 1 1
refdes=addr[ ADDR-1:0]
}
C 1900 1100 1 0 0 in_port.sym  
{
T 1900 1100 5 10 1 1 0 6 1 1 
refdes=wr
}
C 1900 1500 1 0 0 in_port.sym  
{
T 1900 1500 5 10 1 1 0 6 1 1 
refdes=rd
}
C 1900 1900 1 0 0 in_port.sym  
{
T 1900 1900 5 10 1 1 0 6 1 1 
refdes=cs
}
C 1900 2300 1 0 0 in_port.sym  
{
T 1900 2300 5 10 1 1 0 6 1 1 
refdes=clk
}
C 1900 2700 1 0 0 in_port.sym  
{
T 1900 2700 5 10 1 1 0 6 1 1 
refdes=be
}
C 4900 300  1 0  0 out_port_vector.sym
{
T 5900 300 5  10 1 1 0 0 1 1 
refdes=rdata[ WIDTH-1:0]
}
