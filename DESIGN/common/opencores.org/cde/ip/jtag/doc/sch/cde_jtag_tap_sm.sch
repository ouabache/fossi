v 20100214 1
C 1500 300 1 0 0 in_port.sym  
{
T 1500 300 5 10 1 1 0 6 1 1 
refdes=trst_n_pad_in
}
C 1500 700 1 0 0 in_port.sym  
{
T 1500 700 5 10 1 1 0 6 1 1 
refdes=tms_pad_in
}
C 1500 1100 1 0 0 in_port.sym  
{
T 1500 1100 5 10 1 1 0 6 1 1 
refdes=tdo_in
}
C 1500 1500 1 0 0 in_port.sym  
{
T 1500 1500 5 10 1 1 0 6 1 1 
refdes=tdi_pad_in
}
C 1500 1900 1 0 0 in_port.sym  
{
T 1500 1900 5 10 1 1 0 6 1 1 
refdes=clk
}
C 4600 300  1 0  0 out_port_vector.sym
{
T 5600 300 5  10 1 1 0 0 1 1 
refdes=instruction[3:0]
}
C 4600 700  1 0 0 out_port.sym
{
T 5600 700 5  10 1 1 0 0 1 1
refdes=update_dr_o
}
C 4600 1100  1 0 0 out_port.sym
{
T 5600 1100 5  10 1 1 0 0 1 1
refdes=update_dr_clk_o
}
C 4600 1500  1 0 0 out_port.sym
{
T 5600 1500 5  10 1 1 0 0 1 1
refdes=test_logic_reset_o
}
C 4600 1900  1 0 0 out_port.sym
{
T 5600 1900 5  10 1 1 0 0 1 1
refdes=tdo_pad_out
}
C 4600 2300  1 0 0 out_port.sym
{
T 5600 2300 5  10 1 1 0 0 1 1
refdes=tdo_pad_oe
}
C 4600 2700  1 0 0 out_port.sym
{
T 5600 2700 5  10 1 1 0 0 1 1
refdes=tap_highz_mode
}
C 4600 3100  1 0 0 out_port.sym
{
T 5600 3100 5  10 1 1 0 0 1 1
refdes=shift_dr_o
}
C 4600 3500  1 0 0 out_port.sym
{
T 5600 3500 5  10 1 1 0 0 1 1
refdes=jtag_reset
}
C 4600 3900  1 0 0 out_port.sym
{
T 5600 3900 5  10 1 1 0 0 1 1
refdes=capture_dr_o
}
C 4600 4300  1 0 0 out_port.sym
{
T 5600 4300 5  10 1 1 0 0 1 1
refdes=bsr_output_mode
}
