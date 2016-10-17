cp bitstream.svf into this subdirectory

openocd -f olimex_logipi.cfg&



telnet localhost 4444
scan_chain
svf  bitstream.svf -tap logipi.fpga
svf  ledtest.svf   -tap logipi.fpga

