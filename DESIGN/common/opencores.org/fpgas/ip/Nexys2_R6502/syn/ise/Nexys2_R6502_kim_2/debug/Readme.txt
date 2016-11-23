cp bitstream.svf into this subdirectory

sudo openocd -f olimex_nexys2.cfg&
sudo openocd -f olimex_logipi.cfg&



telnet localhost 4444
scan_chain
svf  bitstream.svf -tap nexys2.fpga
svf  ledtest.svf   -tap nexys2.fpga

