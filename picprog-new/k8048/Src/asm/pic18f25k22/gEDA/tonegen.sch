v 20110115 2
C 40000 40000 0 0 0 title-B.sym
C 46000 42600 1 0 0 pic18f25k22.sym
{
T 46400 49000 5 10 0 0 0 0 1
device=PIC18F25K22
T 46400 49200 5 10 0 0 0 0 1
footprint=DIP28
}
C 44900 47800 1 0 0 resistor-2.sym
{
T 45300 48150 5 10 0 0 0 0 1
device=RESISTOR
T 45200 48100 5 10 1 1 0 0 1
refdes=2K
}
C 44900 47400 1 0 0 resistor-2.sym
{
T 45300 47750 5 10 0 0 0 0 1
device=RESISTOR
T 44700 47600 5 10 1 1 0 0 1
refdes=2K
}
C 44900 47000 1 0 0 resistor-2.sym
{
T 45300 47350 5 10 0 0 0 0 1
device=RESISTOR
T 44700 46900 5 10 1 1 0 0 1
refdes=2K
}
C 44900 46600 1 0 0 resistor-2.sym
{
T 45300 46950 5 10 0 0 0 0 1
device=RESISTOR
T 45200 46300 5 10 1 1 0 0 1
refdes=2K
}
C 51700 48200 1 0 0 resistor-2.sym
{
T 52100 48550 5 10 0 0 0 0 1
device=RESISTOR
T 52500 48400 5 10 1 1 0 0 1
refdes=470R
}
N 51700 48300 51100 48300 4
N 43000 47900 44900 47900 4
N 43300 46700 43300 47900 4
N 43300 47500 44900 47500 4
N 44900 47100 43300 47100 4
N 44900 46700 43300 46700 4
C 40900 47600 1 0 0 beeper-1.sym
{
T 41200 48500 5 10 0 0 0 0 1
device=BEEPER
T 41200 48700 5 10 0 0 0 0 1
symversion=0.1
}
N 40900 47900 40900 46700 4
N 51100 47900 51700 47900 4
N 52600 48300 55200 48300 4
N 46100 47900 45800 47900 4
N 46100 47500 45800 47500 4
N 46100 47100 45800 47100 4
N 45800 46700 46100 46700 4
N 46100 48300 45800 48300 4
N 45800 48300 45800 49100 4
N 45800 49100 51700 49100 4
N 46100 45500 45800 45500 4
N 45800 42300 45800 45500 4
N 45800 42300 51700 42300 4
N 51700 42300 51700 47100 4
N 51100 44700 51700 44700 4
N 51100 45100 52600 45100 4
N 51400 45100 51400 47500 4
N 51400 47500 55200 47500 4
N 51700 47100 55200 47100 4
T 53800 48000 9 10 1 0 0 0 1
PGC
T 53800 48400 9 10 1 0 0 0 1
PGD
T 53800 48800 9 10 1 0 0 0 1
VPP
C 51700 44500 1 0 0 capacitor-1.sym
{
T 51900 45200 5 10 0 0 0 0 1
device=CAPACITOR
T 52000 44200 5 10 1 1 0 0 1
refdes=100nF
T 51900 45400 5 10 0 0 0 0 1
symversion=0.1
}
N 52600 44700 52600 45100 4
T 50000 40700 9 10 1 0 0 0 1
Simple Tone Generator
T 50000 40400 9 10 1 0 0 0 1
tonegen.sch
T 50000 40100 9 10 1 0 0 0 1
1
T 51500 40100 9 10 1 0 0 0 1
1
T 53900 40400 9 10 1 0 0 0 1
1.2
T 53900 40100 9 10 1 0 0 0 1
D M BROAD
C 43000 47700 1 0 1 capacitor-4.sym
{
T 42800 48800 5 10 0 0 0 6 1
device=POLARIZED_CAPACITOR
T 42800 47500 5 10 1 1 0 6 1
refdes=10uF
T 42800 48400 5 10 0 0 0 6 1
symversion=0.1
}
N 41900 47900 42100 47900 4
C 40700 46400 1 0 0 ground.sym
T 55400 48700 9 10 1 0 0 0 1
GPIO 9
T 55400 47900 9 10 1 0 0 0 1
GPIO 10
T 55400 48300 9 10 1 0 0 0 1
GPIO 11
T 55400 47500 9 10 1 0 0 0 1
3V3
T 55400 47100 9 10 1 0 0 0 1
GND
T 55400 49100 9 10 1 0 0 0 1
R-PI
N 45500 43100 46100 43100 4
N 45500 43100 45500 41900 4
N 51100 43100 53100 43100 4
N 53100 43100 53100 46700 4
N 53100 46700 55200 46700 4
N 45500 41900 53400 41900 4
N 53400 41900 53400 46300 4
N 53400 46300 55200 46300 4
T 53800 46800 9 10 1 0 0 0 1
SDA
T 53800 46400 9 10 1 0 0 0 1
SCL
T 55400 46700 9 10 1 0 0 0 1
GPIO 0/2
T 55400 46300 9 10 1 0 0 0 1
GPIO 1/3
N 51400 49100 51400 48700 4
N 51400 48700 51700 48700 4
C 51700 48600 1 0 0 resistor-2.sym
{
T 52100 48950 5 10 0 0 0 0 1
device=RESISTOR
T 52500 48800 5 10 1 1 0 0 1
refdes=470R
}
C 51700 47800 1 0 0 resistor-2.sym
{
T 52100 48150 5 10 0 0 0 0 1
device=RESISTOR
T 52500 48000 5 10 1 1 0 0 1
refdes=470R
}
N 52600 48700 55200 48700 4
N 52600 47900 55200 47900 4
C 51700 49000 1 0 0 resistor-2.sym
{
T 52100 49350 5 10 0 0 0 0 1
device=RESISTOR
T 52500 49200 5 10 1 1 0 0 1
refdes=10K
}
N 52600 49100 53400 49100 4
N 53400 49100 53400 47500 4
