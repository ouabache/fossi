v 20100214 1
C 2000 300 1 0 0 in_port_vector.sym   
{
T 2000 300 5 10 1 1 0 6 1 1
refdes=instruction[3:0]
}
C 2000 700 1 0 0 in_port.sym  
{
T 2000 700 5 10 1 1 0 6 1 1 
refdes=update_dr_o
}
C 2000 1100 1 0 0 in_port.sym  
{
T 2000 1100 5 10 1 1 0 6 1 1 
refdes=update_dr_clk_o
}
C 2000 1500 1 0 0 in_port.sym  
{
T 2000 1500 5 10 1 1 0 6 1 1 
refdes=test_logic_reset_o
}
C 2000 1900 1 0 0 in_port.sym  
{
T 2000 1900 5 10 1 1 0 6 1 1 
refdes=tdo_i
}
C 2000 2300 1 0 0 in_port.sym  
{
T 2000 2300 5 10 1 1 0 6 1 1 
refdes=tdi_pad_in
}
C 2000 2700 1 0 0 in_port.sym  
{
T 2000 2700 5 10 1 1 0 6 1 1 
refdes=shift_dr_o
}
C 2000 3100 1 0 0 in_port.sym  
{
T 2000 3100 5 10 1 1 0 6 1 1 
refdes=jtag_shift_clk
}
C 2000 3500 1 0 0 in_port.sym  
{
T 2000 3500 5 10 1 1 0 6 1 1 
refdes=chip_id_tdo
}
C 2000 3900 1 0 0 in_port.sym  
{
T 2000 3900 5 10 1 1 0 6 1 1 
refdes=capture_dr_o
}
C 2000 4300 1 0 0 in_port.sym  
{
T 2000 4300 5 10 1 1 0 6 1 1 
refdes=aux_tdo_i
}
C 5800 300  1 0  0 out_port_vector.sym
{
T 6800 300 5  10 1 1 0 0 1 1 
refdes=chip_id_value[31:0]
}
C 5800 700  1 0 0 out_port.sym
{
T 6800 700 5  10 1 1 0 0 1 1
refdes=tdi_o
}
C 5800 1100  1 0 0 out_port.sym
{
T 6800 1100 5  10 1 1 0 0 1 1
refdes=shiftcapture_dr_o
}
C 5800 1500  1 0 0 out_port.sym
{
T 6800 1500 5  10 1 1 0 0 1 1
refdes=shiftcapture_dr_clk_o
}
C 5800 1900  1 0 0 out_port.sym
{
T 6800 1900 5  10 1 1 0 0 1 1
refdes=select_o
}
C 5800 2300  1 0 0 out_port.sym
{
T 6800 2300 5  10 1 1 0 0 1 1
refdes=sample
}
C 5800 2700  1 0 0 out_port.sym
{
T 6800 2700 5  10 1 1 0 0 1 1
refdes=next_tdo
}
C 5800 3100  1 0 0 out_port.sym
{
T 6800 3100 5  10 1 1 0 0 1 1
refdes=jtag_reset
}
C 5800 3500  1 0 0 out_port.sym
{
T 6800 3500 5  10 1 1 0 0 1 1
refdes=jtag_clk
}
C 5800 3900  1 0 0 out_port.sym
{
T 6800 3900 5  10 1 1 0 0 1 1
refdes=extest
}
C 5800 4300  1 0 0 out_port.sym
{
T 6800 4300 5  10 1 1 0 0 1 1
refdes=clamp
}
C 5800 4700  1 0 0 out_port.sym
{
T 6800 4700 5  10 1 1 0 0 1 1
refdes=chip_id_select
}
C 5800 5100  1 0 0 out_port.sym
{
T 6800 5100 5  10 1 1 0 0 1 1
refdes=bsr_select_o
}
C 5800 5500  1 0 0 out_port.sym
{
T 6800 5500 5  10 1 1 0 0 1 1
refdes=aux_update_dr_o
}
C 5800 5900  1 0 0 out_port.sym
{
T 6800 5900 5  10 1 1 0 0 1 1
refdes=aux_update_dr_clk_o
}
C 5800 6300  1 0 0 out_port.sym
{
T 6800 6300 5  10 1 1 0 0 1 1
refdes=aux_test_logic_reset_o
}
C 5800 6700  1 0 0 out_port.sym
{
T 6800 6700 5  10 1 1 0 0 1 1
refdes=aux_tdi_o
}
C 5800 7100  1 0 0 out_port.sym
{
T 6800 7100 5  10 1 1 0 0 1 1
refdes=aux_shiftcapture_dr_clk_o
}
C 5800 7500  1 0 0 out_port.sym
{
T 6800 7500 5  10 1 1 0 0 1 1
refdes=aux_shift_dr_o
}
C 5800 7900  1 0 0 out_port.sym
{
T 6800 7900 5  10 1 1 0 0 1 1
refdes=aux_select_o
}
C 5800 8300  1 0 0 out_port.sym
{
T 6800 8300 5  10 1 1 0 0 1 1
refdes=aux_jtag_clk
}
C 5800 8700  1 0 0 out_port.sym
{
T 6800 8700 5  10 1 1 0 0 1 1
refdes=aux_capture_dr_o
}
