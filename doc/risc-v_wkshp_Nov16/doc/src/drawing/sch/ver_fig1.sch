v 20100214 2
C 50500 14600 1 0 0 frame_800x600.sym
B 54300 15300 1200 1600 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
B 52800 15300 800 1600 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
B 51000 15300 1400 1600 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 54700 16100 9 10 1 0 0 0 1
DUT
T 52900 15700 9 10 1 0 0 0 3
 BFM
MODEL

T 51000 15800 9 10 1 0 0 0 2
 TEST_DEFINE

B 56300 15300 800 1600 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
B 57500 15300 1400 1600 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 56400 15700 9 10 1 0 0 0 3
 BFM
MODEL

T 57500 15800 9 10 1 0 0 0 2
 TEST_DEFINE

L 52400 16100 52800 16100 3 0 0 0 -1 -1
L 52400 16000 52800 16000 3 0 0 0 -1 -1
L 52800 16000 52700 15900 3 0 0 0 -1 -1
L 52800 16100 52700 16200 3 0 0 0 -1 -1
L 52400 16100 52500 16200 3 0 0 0 -1 -1
L 52400 16000 52500 15900 3 0 0 0 -1 -1
L 57100 16100 57500 16100 3 0 0 0 -1 -1
L 57100 16000 57500 16000 3 0 0 0 -1 -1
L 57500 16000 57400 15900 3 0 0 0 -1 -1
L 57500 16100 57400 16200 3 0 0 0 -1 -1
L 57100 16100 57200 16200 3 0 0 0 -1 -1
L 57100 16000 57200 15900 3 0 0 0 -1 -1
L 53600 16100 54300 16100 3 0 0 0 -1 -1
L 53600 16000 54300 16000 3 0 0 0 -1 -1
L 55500 16100 56300 16100 3 0 0 0 -1 -1
L 55500 16000 56300 16000 3 0 0 0 -1 -1
L 53600 16100 53700 16200 3 0 0 0 -1 -1
L 53600 16000 53700 15900 3 0 0 0 -1 -1
L 54300 16100 54200 16200 3 0 0 0 -1 -1
L 54300 16000 54200 15900 3 0 0 0 -1 -1
L 55500 16100 55600 16200 3 0 0 0 -1 -1
L 55500 16000 55600 15900 3 0 0 0 -1 -1
L 56300 16100 56200 16200 3 0 0 0 -1 -1
L 56300 16000 56200 15900 3 0 0 0 -1 -1
B 50700 14800 8500 2600 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 51200 17100 9 10 1 0 0 0 1
TEST_CASE
V 54700 19700 1000 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
B 56400 20100 2200 500 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 56900 20300 9 10 1 0 0 0 1
LOG FILE
B 56400 19100 2800 500 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 56500 19300 9 10 1 0 0 0 1
VALUE CHANGE DUMP  FILE
L 55613 20072 56400 20300 3 0 0 0 -1 -1
L 55653 19452 56400 19300 3 0 0 0 -1 -1
L 54700 18700 54700 17400 3 0 0 0 -1 -1
L 54700 18700 54600 18600 3 0 0 0 -1 -1
L 54700 18700 54800 18600 3 0 0 0 -1 -1
T 54200 19600 9 10 1 0 0 0 2
  VERILOG
SIMULATOR
L 56400 20300 56283 20335 3 0 0 0 -1 -1
L 56400 20300 56300 20200 3 0 0 0 -1 -1
L 56400 19300 56300 19400 3 0 0 0 -1 -1
L 56400 19300 56274 19243 3 0 0 0 -1 -1
T 51300 20500 9 10 1 0 0 0 2
  COMMAND 
LINE OPTIONS
T 51400 19600 9 10 1 0 0 0 2
FIRMWARE
BIT IMAGE
T 51400 17800 9 10 1 0 0 0 2
EXPECTED
VALUES
B 51200 20300 1500 800 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
B 51200 19400 1500 700 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
B 51200 18500 1500 700 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
L 52700 20800 53821 20187 3 0 0 0 -1 -1
L 52700 19800 53694 19722 3 0 0 0 -1 -1
L 52700 19000 53757 19362 3 0 0 0 -1 -1
L 53766 19368 53633 19412 3 0 0 0 -1 -1
L 53761 19363 53687 19263 3 0 0 0 -1 -1
L 53816 20196 53747 20331 3 0 0 0 -1 -1
L 53821 20177 53672 20162 3 0 0 0 -1 -1
L 53701 19722 53561 19840 3 0 0 0 -1 -1
L 53694 19715 53554 19641 3 0 0 0 -1 -1
B 56400 18100 2800 500 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
L 55469 19065 56400 18300 3 0 0 0 -1 -1
L 56400 18300 56313 18439 3 0 0 0 -1 -1
L 56400 18300 56254 18325 3 0 0 0 -1 -1
T 56500 18300 9 10 1 0 0 0 1
RECORDED VALUES
B 51200 17600 1500 700 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 51400 18700 9 10 1 0 0 0 2
INPUT
DATA
L 52700 18000 53950 19050 3 0 0 0 -1 -1
L 53933 19058 53816 19050 3 0 0 0 -1 -1
L 53948 19053 53916 18906 3 0 0 0 -1 -1