v 20110115 2
B 53200 41500 11500 8400 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
C 54300 42300 1 0 0 reg_rst.sym
{
T 56100 44100 5 10 0 0 0 0 1
device=REGISTER_RST
T 55600 44300 5 10 1 1 0 6 1
refdes=U?
}
T 54400 44700 9 10 1 0 0 0 3
ACTIVE LOW
RESET TO
COMPONENT
N 55900 43900 56300 43900 4
T 53900 45400 9 25 1 0 0 0 1
Soft Reset
C 57800 46000 1 0 0 reg_rst.sym
{
T 59600 47800 5 10 0 0 0 0 1
device=REGISTER_RST
T 59100 48000 5 10 1 1 0 6 1
refdes=U?
}
N 55100 42300 55100 41900 4
N 53700 41900 58600 41900 4
N 58600 46000 58600 41900 4
T 61100 43200 9 25 1 0 0 0 2
Reset Style #3
Both
C 56300 47300 1 0 0 and2-1.sym
{
T 56700 47200 5 10 1 1 0 2 1
refdes=U?
T 56700 47400 5 8 0 0 0 0 1
device=and
}
N 57800 47600 57600 47600 4
N 56300 47400 56300 43900 4