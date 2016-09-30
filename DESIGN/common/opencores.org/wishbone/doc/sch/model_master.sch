v 20100214 1
C 1700 300 1 0 0 in_port_vector.sym   
{
T 1700 300 5 10 1 1 0 6 1 1
refdes=din[dwidth-1:0]
}
C 1700 700 1 0 0 in_port.sym  
{
T 1700 700 5 10 1 1 0 6 1 1 
refdes=rty
}
C 1700 1100 1 0 0 in_port.sym  
{
T 1700 1100 5 10 1 1 0 6 1 1 
refdes=reset
}
C 1700 1500 1 0 0 in_port.sym  
{
T 1700 1500 5 10 1 1 0 6 1 1 
refdes=err
}
C 1700 1900 1 0 0 in_port.sym  
{
T 1700 1900 5 10 1 1 0 6 1 1 
refdes=clk
}
C 1700 2300 1 0 0 in_port.sym  
{
T 1700 2300 5 10 1 1 0 6 1 1 
refdes=ack
}
C 4700 300  1 0  0 out_port_vector.sym
{
T 5700 300 5  10 1 1 0 0 1 1 
refdes=sel[dwidth/8-1:0]
}
C 4700 700  1 0  0 out_port_vector.sym
{
T 5700 700 5  10 1 1 0 0 1 1 
refdes=dout[dwidth-1:0]
}
C 4700 1100  1 0  0 out_port_vector.sym
{
T 5700 1100 5  10 1 1 0 0 1 1 
refdes=adr[awidth-1:0]
}
C 4700 1500  1 0 0 out_port.sym
{
T 5700 1500 5  10 1 1 0 0 1 1
refdes=we
}
C 4700 1900  1 0 0 out_port.sym
{
T 5700 1900 5  10 1 1 0 0 1 1
refdes=stb
}
C 4700 2300  1 0 0 out_port.sym
{
T 5700 2300 5  10 1 1 0 0 1 1
refdes=cyc
}
