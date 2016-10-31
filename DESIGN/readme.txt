
1) Install Ubuntu app store tools

   cd fossi/install/Ubuntu
   make install

2) Install rest of tools

  cd fossi/tools
  ./install_all


3) Copy this template for design environment

   cp -r fossi/DESIGN  ~ 
   cd ~/DESIGN



initialize and test fusesoc
  cp fusesoc.xxx fusesoc.conf
  . ./profile
  fusesoc init  (hit return)
  ./test_fusesoc

OR

initialize and test socgen

  ./test_socgen
  cd work/opencores.org__cde/ip/jtag/sim/icarus/default
  gtkwave -f TestBench.vcd  wave.sav
  cd ../../cov/jtag_tap_tb
  
  check code coverage numbers

  
