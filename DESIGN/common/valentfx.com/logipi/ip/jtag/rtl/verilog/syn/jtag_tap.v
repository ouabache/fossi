/**********************************************************************/
/*                                                                    */
/*             -------                                                */
/*            /   SOC  \                                              */
/*           /    GEN   \                                             */
/*          /    TARGET  \                                            */
/*          ==============                                            */
/*          |            |                                            */
/*          |____________|                                            */
/*                                                                    */
/*  Jtag tap controller for xilinx spartan 3e fpga                    */
/*                                                                    */
/*                                                                    */
/*  Author(s):                                                        */
/*      - John Eaton, jt_eaton@opencores.org                          */
/*                                                                    */
/**********************************************************************/
/*                                                                    */
/*    Copyright (C) <2010>  <Ouabache Design Works>                   */
/*                                                                    */
/*  This source file may be used and distributed without              */
/*  restriction provided that this copyright statement is not         */
/*  removed from the file and that any derivative work contains       */
/*  the original copyright notice and the associated disclaimer.      */
/*                                                                    */
/*  This source file is free software; you can redistribute it        */
/*  and/or modify it under the terms of the GNU Lesser General        */
/*  Public License as published by the Free Software Foundation;      */
/*  either version 2.1 of the License, or (at your option) any        */
/*  later version.                                                    */
/*                                                                    */
/*  This source is distributed in the hope that it will be            */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied        */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR           */
/*  PURPOSE.  See the GNU Lesser General Public License for more      */
/*  details.                                                          */
/*                                                                    */
/*  You should have received a copy of the GNU Lesser General         */
/*  Public License along with this source; if not, download it        */
/*  from http://www.opencores.org/lgpl.shtml                          */
/*                                                                    */
/**********************************************************************/

//////////////////////////////////////////////////////////////////////
//                                                                  //
// This file is a wrapper for the various Xilinx internal BSCAN     //
// TAP devices.  It is designed to take the place of a separate TAP //
// controller in Xilinx systems, to allow a user to access a CPU    //
// debug module (such as that of the OR1200) through the FPGA's     //
// dedicated JTAG / configuration port.                             //
//                                                                  //
//////////////////////////////////////////////////////////////////////
//

// Note that the SPARTAN BSCAN controllers have more than one channel.
// This implementation always uses channel 1, this is not configurable.
// If you want to use another channel, then it is probably because you
// want to attach multiple devices to the BSCAN device, which means
// you'll be making changes to this file anyway.


module cde_jtag_tap 
#(parameter  CHIP_ID_VAL=32'h00000000 ) 
(
		 
input   wire           tdo_i,
output  wire           shiftcapture_dr_clk_o,
output  wire           tdi_o,
output  wire           test_logic_reset_o,
output  wire           shift_dr_o,
output  wire           capture_dr_o,
output  wire           update_dr_clk_o,
output  wire           select_o,

input   wire           aux_tdo_i,
output  wire           aux_shiftcapture_dr_clk_o,
output  wire           aux_tdi_o,
output  wire           aux_test_logic_reset_o,
output  wire           aux_shift_dr_o,
output  wire           aux_capture_dr_o,
output  wire           aux_update_dr_clk_o,
output  wire           aux_select_o
 	 
);

wire       update_dr_i;
wire       aux_update_dr_i;
   
BSCAN_SPARTAN6 
#(1)
BSCAN_SPARTAN6_inst1  (
     .CAPTURE (capture_dr_o),         // CAPTURE output from TAP controller
     .DRCK    (user1_clk_i),          // shiftcapture clk for USER1 functions
     .RESET   (test_logic_reset_o),   // Reset output from TAP controller
     .RUNTEST (                  ),
     .SEL     (select_o),             // USER1 active output
     .SHIFT   (shift_dr_o),           // SHIFT output from TAP controller
     .TCK     (),
     .TDI     (tdi_o),                // TDI output from TAP controller
     .TMS     (),
     .UPDATE  (update_dr_i),          // UPDATE output from TAP controller
     .TDO     (tdo_i)                // Data input for USER1 function
);


BSCAN_SPARTAN6 
#(2)
BSCAN_SPARTAN6_inst2 (
     .CAPTURE (aux_capture_dr_o),         // CAPTURE output from TAP controller
     .DRCK    (user2_clk_i),              // shiftcapture clk for USER1 functions
     .RESET   (aux_test_logic_reset_o),   // Reset output from TAP controller
     .RUNTEST (                  ),
     .SEL     (aux_select_o),             // USER1 active output
     .SHIFT   (aux_shift_dr_o),           // SHIFT output from TAP controller
     .TCK     (),
     .TDI     (aux_tdi_o),                // TDI output from TAP controller
     .TMS     (),
     .UPDATE  (aux_update_dr_i),          // UPDATE output from TAP controller
     .TDO     (aux_tdo_i)                 // Data input for USER2 function
);

   

BUFG 
update_buf (
   .O       (update_dr_clk_o),        // Clock buffer output
   .I       (update_dr_i)             // Clock buffer input
            );


BUFG 
aux_update_buf (
   .O       (aux_update_dr_clk_o),        // Clock buffer output
   .I       (aux_update_dr_i)             // Clock buffer input
            );

   

   

BUFG 
user1_clk_buf (
   .O       (shiftcapture_dr_clk_o),              // Clock buffer output
   .I       (user1_clk_i   )             // Clock buffer input
            );


BUFG 
user2_clk_buf (
   .O       (aux_shiftcapture_dr_clk_o),              // Clock buffer output
   .I       (user2_clk_i )             // Clock buffer input
            );  


   
   
   

endmodule
