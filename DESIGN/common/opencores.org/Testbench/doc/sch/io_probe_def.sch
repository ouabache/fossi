v 20100214 1
C 2700 300 1 0 0 in_port_vector.sym   
{
T 2700 300 5 10 1 1 0 6 1 1
refdes=mask[WIDTH-1:0]
}
C 2700 700 1 0 0 in_port_vector.sym   
{
T 2700 700 5 10 1 1 0 6 1 1
refdes=expected_value[WIDTH-1:0]
}
C 2700 1100 1 0 0 in_port_vector.sym   
{
T 2700 1100 5 10 1 1 0 6 1 1
refdes=drive_value[WIDTH-1:0]
}
C 2700 1500 1 0 0 in_port.sym  
{
T 2700 1500 5 10 1 1 0 6 1 1 
refdes=clk
}
C 5700 300  1 0  0 io_port_vector.sym
{
T 6700 300 5  10 1 1 0 0 1 1 
refdes=signal[WIDTH-1:0]
}
