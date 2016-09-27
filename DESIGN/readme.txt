
1) Install tools

   cd fossi/install/Ubuntu_16.04
   make install

2) Install fusesoc

  cd fossi/tools/fusesoc
  ./install 



3) Copy this template for design environment

   cp -r fossi/DESIGN  {your design directory} 
   cd DESIGN



initialize and test fusesoc

  . ./profile
  fusesoc init
  ./test_fusesoc

OR

initialize and test socgen

  ./test_socgen
  cd work/opencores.org__cde/ip/jtag/sim/icarus/default
  gtkwave -f TestBench.vcd  wave.sav
  cd ../../cov/jtag_tap_tb
  
  check code coverage numbers

