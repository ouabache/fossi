
1) Install tools

   cd install/Ubuntu_16.04
   make install

2) Copy this template for design environment

   cp -r DESIGN  {your design directory} 


3) source environment
   . ./profile


4) Install fusesoc

  cd tools/fusesoc
  ./install 

5) initialize and test

  cd DESIGN
  fusesoc init
  fusesoc list-systems
  fusesoc list-cores
  fusesoc sim de1 --vcd --elf-load hello_nop.elf --testcase TEST --timeout 12000
  gtkwave -f build/de1/sim-icarus/TEST.vcd gtkwave.gtkw
