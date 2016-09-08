
1) Install tools

   cd tools/install/Ubuntu16.04
   make install

2) Copy this template for design environment

   cp -r tools/DESIGN  . 


3) source environment
   . ./profile


4) Install fusesoc

  cd tools/fusesoc/v1.5
  pip install fusesoc

5) initialize and test

  cd DESIGN
  fusesoc init
  fusesoc list-systems
  fusesoc list-cores
  fusesoc sim de1 --vcd --elf-load hello_nop.elf --testcase TEST --timeout 12000
  cd build/de1/sim-icarus
  gtkwave -f TEST.vcd
  

