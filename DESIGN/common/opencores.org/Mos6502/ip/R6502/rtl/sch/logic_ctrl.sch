v 20130925 2
C 10300 54000 1 0 0 mux_vector.sym
{
T 11000 54200 5 10 1 1 0 2 1
refdes=U1
T 10700 54000 5 10 1 1 0 2 1
module_name=cde_mux_def 
}
U 10300 54500 9800 54500 10 0
U 10300 54300 9800 54300 10 0
U 9600 54300 9600 54100 10 0
U 9600 54100 9600 54000 10 0
U 9600 54000 9400 53800 10 0
U 9400 53800 8000 53800 10 0
U 12800 54600 11600 54600 10 0
U 9100 54600 8000 54600 10 0
U 7800 51700 7200 51700 10 0
C 6300 50900 1 0 0 in_port.sym
C 6300 50500 1 0 0 in_port.sym
C 6300 50100 1 0 0 in_port.sym
C 6300 49700 1 0 0 in_port.sym
U 10100 51500 10100 49400 10 -1
U 10100 49400 10200 49200 10 0
U 10200 49200 12100 49200 10 0
{
T 10400 49300 5 10 1 1 0 0 1
netname=irq[6:0]
}
U 12100 49200 12200 49300 10 0
C 12400 52100 1 0 0 buf_vector.sym
{
T 13000 52800 5 10 1 1 0 2 1
refdes=U2
}
C 12400 50900 1 0 0 buf_vector.sym
{
T 13000 51600 5 10 1 1 0 2 1
refdes=U3
}
U 13500 52500 14700 52500 10 0
{
T 13500 52700 5 10 1 1 0 0 1
netname=io_module_pic_irq_in[6:0]
}
U 13500 51300 14700 51300 10 0
{
T 13500 51500 5 10 1 1 0 0 1
netname=io_module_vic_irq_n[6:0]
}
U 12200 49300 12200 52300 10 -1
U 12200 52300 12400 52500 10 0
U 12200 51100 12400 51300 10 0
U 9600 54300 9800 54500 10 0
U 9600 54100 9800 54300 10 0
U 9100 54600 9100 55000 10 1
N 10300 54800 9300 54800 4
N 9300 54800 9100 55000 4
N 9900 51000 10100 50800 4
N 9900 50600 10100 50400 4
N 9900 50200 10100 50000 4
N 9900 49800 10100 49600 4
C 7800 51300 1 0 0 buf_vector.sym
{
T 8400 52000 5 10 1 1 0 2 1
refdes=U4
}
U 8900 51700 9900 51700 10 0
U 9900 51700 10100 51500 10 0
C 7800 50600 1 0 0 buf_sml.sym
{
T 8200 51000 5 10 1 1 0 2 1
refdes=U? 
}
C 7800 50200 1 0 0 buf_sml.sym
{
T 8200 50600 5 10 1 1 0 2 1
refdes=U? 
}
C 7800 49800 1 0 0 buf_sml.sym
{
T 8200 50200 5 10 1 1 0 2 1
refdes=U? 
}
C 7800 49400 1 0 0 buf_sml.sym
{
T 8200 49800 5 10 1 1 0 2 1
refdes=U? 
}
N 7800 51000 7200 51000 4
N 7200 50600 7800 50600 4
N 7200 50200 7800 50200 4
N 7200 49800 7800 49800 4
N 8600 51000 9900 51000 4
N 8600 50600 9900 50600 4
N 8600 50200 9900 50200 4
N 8600 49800 9900 49800 4
C 7100 53700 1 0 0 in_port_vector.sym
C 7100 54500 1 0 0 in_port_vector.sym
C 12800 54500 1 0 0 out_port_vector.sym
C 14700 52400 1 0 0 out_port_vector.sym
C 14700 51200 1 0 0 out_port_vector.sym
C 6300 51600 1 0 0 in_port_vector.sym