 Synthesis for iCE40 FPGAs can be done with Yosys. Place-and-route can be done with arachne-pnr. Here is an example script for implementing and programming the rot example from arachne-pnr (this example targets the iCEstick development board):

yosys -p "synth_ice40 -blif rot.blif" rot.v
arachne-pnr -d 1k -p rot.pcf rot.blif -o rot.asc
icepack rot.asc rot.bin
iceprog rot.bin

A simple timing analysis report can be generated using the icetime utility:

icetime -tmd hx1k rot.asc


