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
N 55900 43900 61400 43900 4
T 53900 45400 9 25 1 0 0 0 1
Soft Reset
N 55100 42300 55100 41900 4
N 55100 41900 53700 41900 4
T 60900 42100 9 25 1 0 0 0 2
Reset Style #4
Sync with Backup
C 59200 46200 1 0 0 reg.sym
{
T 61000 48000 5 10 0 0 0 0 1
device=REGISTER
T 60500 48200 5 10 1 1 0 6 1
refdes=U?
}
C 57200 47500 1 0 0 and2-1.sym
{
T 57600 47400 5 10 1 1 0 2 1
refdes=U?
T 57600 47600 5 8 0 0 0 0 1
device=and
}
C 61400 47300 1 0 0 and2-1.sym
{
T 61800 47200 5 10 1 1 0 2 1
refdes=U?
T 61800 47400 5 8 0 0 0 0 1
device=and
}
N 60800 47800 61400 47800 4
N 59200 47800 58500 47800 4
N 57200 47600 57100 47600 4
N 57100 43900 57100 47600 4
N 61400 47400 61400 43900 4