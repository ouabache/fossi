v 20100214 1
C 1800 300 1 0 0 in_port_vector.sym   
{
T 1800 300 5 10 1 1 0 6 1 1
refdes=axi_wstrb[3:0]
}
C 1800 700 1 0 0 in_port_vector.sym   
{
T 1800 700 5 10 1 1 0 6 1 1
refdes=axi_wdata[31:0]
}
C 1800 1100 1 0 0 in_port_vector.sym   
{
T 1800 1100 5 10 1 1 0 6 1 1
refdes=axi_awsize[2:0]
}
C 1800 1500 1 0 0 in_port_vector.sym   
{
T 1800 1500 5 10 1 1 0 6 1 1
refdes=axi_awlen[7:0]
}
C 1800 1900 1 0 0 in_port_vector.sym   
{
T 1800 1900 5 10 1 1 0 6 1 1
refdes=axi_awid[11:0]
}
C 1800 2300 1 0 0 in_port_vector.sym   
{
T 1800 2300 5 10 1 1 0 6 1 1
refdes=axi_awburst[1:0]
}
C 1800 2700 1 0 0 in_port_vector.sym   
{
T 1800 2700 5 10 1 1 0 6 1 1
refdes=axi_awaddr[31:0]
}
C 1800 3100 1 0 0 in_port_vector.sym   
{
T 1800 3100 5 10 1 1 0 6 1 1
refdes=axi_arsize[2:0]
}
C 1800 3500 1 0 0 in_port_vector.sym   
{
T 1800 3500 5 10 1 1 0 6 1 1
refdes=axi_arlen[7:0]
}
C 1800 3900 1 0 0 in_port_vector.sym   
{
T 1800 3900 5 10 1 1 0 6 1 1
refdes=axi_arid[11:0]
}
C 1800 4300 1 0 0 in_port_vector.sym   
{
T 1800 4300 5 10 1 1 0 6 1 1
refdes=axi_arburst[1:0]
}
C 1800 4700 1 0 0 in_port_vector.sym   
{
T 1800 4700 5 10 1 1 0 6 1 1
refdes=axi_araddr[31:0]
}
C 1800 5100 1 0 0 in_port.sym  
{
T 1800 5100 5 10 1 1 0 6 1 1 
refdes=reset
}
C 1800 5500 1 0 0 in_port.sym  
{
T 1800 5500 5 10 1 1 0 6 1 1 
refdes=clk
}
C 1800 5900 1 0 0 in_port.sym  
{
T 1800 5900 5 10 1 1 0 6 1 1 
refdes=axi_wvalid
}
C 1800 6300 1 0 0 in_port.sym  
{
T 1800 6300 5 10 1 1 0 6 1 1 
refdes=axi_wlast
}
C 1800 6700 1 0 0 in_port.sym  
{
T 1800 6700 5 10 1 1 0 6 1 1 
refdes=axi_rready
}
C 1800 7100 1 0 0 in_port.sym  
{
T 1800 7100 5 10 1 1 0 6 1 1 
refdes=axi_bready
}
C 1800 7500 1 0 0 in_port.sym  
{
T 1800 7500 5 10 1 1 0 6 1 1 
refdes=axi_awvalid
}
C 1800 7900 1 0 0 in_port.sym  
{
T 1800 7900 5 10 1 1 0 6 1 1 
refdes=axi_arvalid
}
C 4600 300  1 0  0 out_port_vector.sym
{
T 5600 300 5  10 1 1 0 0 1 1 
refdes=axi_rid[11:0]
}
C 4600 700  1 0  0 out_port_vector.sym
{
T 5600 700 5  10 1 1 0 0 1 1 
refdes=axi_rdata[31:0]
}
C 4600 1100  1 0  0 out_port_vector.sym
{
T 5600 1100 5  10 1 1 0 0 1 1 
refdes=axi_bresp[1:0]
}
C 4600 1500  1 0  0 out_port_vector.sym
{
T 5600 1500 5  10 1 1 0 0 1 1 
refdes=axi_bid[11:0]
}
C 4600 1900  1 0 0 out_port.sym
{
T 5600 1900 5  10 1 1 0 0 1 1
refdes=axi_wready
}
C 4600 2300  1 0 0 out_port.sym
{
T 5600 2300 5  10 1 1 0 0 1 1
refdes=axi_rvalid
}
C 4600 2700  1 0 0 out_port.sym
{
T 5600 2700 5  10 1 1 0 0 1 1
refdes=axi_rlast
}
C 4600 3100  1 0 0 out_port.sym
{
T 5600 3100 5  10 1 1 0 0 1 1
refdes=axi_bvalid
}
C 4600 3500  1 0 0 out_port.sym
{
T 5600 3500 5  10 1 1 0 0 1 1
refdes=axi_awready
}
C 4600 3900  1 0 0 out_port.sym
{
T 5600 3900 5  10 1 1 0 0 1 1
refdes=axi_arready
}
