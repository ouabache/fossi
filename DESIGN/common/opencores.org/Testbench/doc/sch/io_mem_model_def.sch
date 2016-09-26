v 20100214 1
C 3400 300 1 0 0 in_port_vector.sym   
{
T 3400 300 5 10 1 1 0 6 1 1
refdes=io_mem_req_data_bits_data[127:0]
}
C 3400 700 1 0 0 in_port_vector.sym   
{
T 3400 700 5 10 1 1 0 6 1 1
refdes=io_mem_req_cmd_bits_tag[4:0]
}
C 3400 1100 1 0 0 in_port_vector.sym   
{
T 3400 1100 5 10 1 1 0 6 1 1
refdes=io_mem_req_cmd_bits_addr[25:0]
}
C 3400 1500 1 0 0 in_port.sym  
{
T 3400 1500 5 10 1 1 0 6 1 1 
refdes=reset
}
C 3400 1900 1 0 0 in_port.sym  
{
T 3400 1900 5 10 1 1 0 6 1 1 
refdes=io_mem_req_data_valid
}
C 3400 2300 1 0 0 in_port.sym  
{
T 3400 2300 5 10 1 1 0 6 1 1 
refdes=io_mem_req_cmd_valid
}
C 3400 2700 1 0 0 in_port.sym  
{
T 3400 2700 5 10 1 1 0 6 1 1 
refdes=io_mem_req_cmd_bits_rw
}
C 3400 3100 1 0 0 in_port.sym  
{
T 3400 3100 5 10 1 1 0 6 1 1 
refdes=clk
}
C 7500 300  1 0  0 out_port_vector.sym
{
T 8500 300 5  10 1 1 0 0 1 1 
refdes=io_mem_resp_bits_tag[4:0]
}
C 7500 700  1 0  0 out_port_vector.sym
{
T 8500 700 5  10 1 1 0 0 1 1 
refdes=io_mem_resp_bits_data[127:0]
}
C 7500 1100  1 0 0 out_port.sym
{
T 8500 1100 5  10 1 1 0 0 1 1
refdes=io_out_mem_valid
}
C 7500 1500  1 0 0 out_port.sym
{
T 8500 1500 5  10 1 1 0 0 1 1
refdes=io_out_mem_ready
}
C 7500 1900  1 0 0 out_port.sym
{
T 8500 1900 5  10 1 1 0 0 1 1
refdes=io_mem_resp_valid
}
C 7500 2300  1 0 0 out_port.sym
{
T 8500 2300 5  10 1 1 0 0 1 1
refdes=io_mem_resp_ready
}
C 7500 2700  1 0 0 out_port.sym
{
T 8500 2700 5  10 1 1 0 0 1 1
refdes=io_mem_req_data_ready
}
C 7500 3100  1 0 0 out_port.sym
{
T 8500 3100 5  10 1 1 0 0 1 1
refdes=io_mem_req_cmd_ready
}
C 7500 3500  1 0 0 out_port.sym
{
T 8500 3500 5  10 1 1 0 0 1 1
refdes=io_mem_backup_en
}
C 7500 3900  1 0 0 out_port.sym
{
T 8500 3900 5  10 1 1 0 0 1 1
refdes=io_in_mem_valid
}
C 7500 4300  1 0 0 out_port.sym
{
T 8500 4300 5  10 1 1 0 0 1 1
refdes=io_in_mem_ready
}
