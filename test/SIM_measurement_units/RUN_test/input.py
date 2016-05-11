
def main():

    unit.s = 1
    unit.ps = unit.s
    unit.ns = unit.s
    unit.us = unit.s
    unit.ms = unit.s
    unit.cs = unit.s
    unit.ds = unit.s
    unit.das = unit.s
    unit.hs = unit.s
    unit.ks = unit.s
    unit.Ms = unit.s
    unit.Gs = unit.s
    unit.Ts = unit.s
    unit.min = unit.s
    unit.hr = unit.s
    unit.day = unit.s
    print unit.s
    print unit.ps
    print unit.ns
    print unit.us
    print unit.ms
    print unit.cs
    print unit.ds
    print unit.das
    print unit.hs
    print unit.ks
    print unit.Ms
    print unit.Gs
    print unit.Ts
    print unit.min
    print unit.hr
    print unit.day

    unit.m = 1
    unit.ft = unit.m
    unit.kft = unit.m
    unit.inch = unit.m
    unit.yd = unit.m
    unit.mi = unit.m
    unit.nmile = unit.m
    print unit.m
    print unit.ft
    print unit.kft
    print unit.inch
    print unit.yd
    print unit.mi
    print unit.nmile

    unit.rad = trick.attach_units("r ", 10)

    unit.rad = 1
    unit.degree = unit.rad
    unit.arcsecond = unit.rad
    unit.arcminute = unit.rad
    unit.revolution = unit.rad
    print unit.rad
    print unit.degree
    print unit.arcsecond
    print unit.arcminute
    print unit.revolution

    unit.g = 1000
    unit.kg = unit.g
    unit.t = unit.g
    unit.slug = unit.g
    unit.lb = unit.g
    print unit.g
    print unit.kg
    print unit.t
    print unit.slug
    print unit.lb

    unit.N = 1
    unit.ozf = unit.N
    unit.lbf = unit.N
    print unit.N
    print unit.ozf
    print unit.lbf

    unit.V = 1
    unit.A = 1
    unit.ohm = 1
    print unit.V
    print unit.A
    print unit.ohm

    unit.degC = 0
    unit.degF = unit.degC
    unit.K = unit.degC
    unit.degR = unit.degC
    print unit.degC
    print unit.degF
    print unit.K
    print unit.degR

    unit.J = 1
    unit.BTU = unit.J
    unit.ton_TNT = unit.J
    unit.Wh = unit.J
    unit.kWh = unit.J
    print unit.J
    print unit.BTU
    print unit.ton_TNT
    print unit.Wh
    print unit.kWh

    unit.W = 1
    unit.hp = unit.W
    print unit.W
    print unit.hp

    unit.Pa = 1
    unit.psi = unit.Pa
    unit.atm = unit.Pa
    unit.mmHg = unit.Pa
    unit.inHg = unit.Pa
    unit.cmH2O = unit.Pa
    unit.inch_H2O_39F = unit.Pa
    print unit.Pa
    print unit.psi
    print unit.atm
    print unit.mmHg
    print unit.inHg
    print unit.cmH2O
    print unit.inch_H2O_39F

    unit.l = 1
    unit.gallon = unit.l
    unit.floz = unit.l
    print unit.l
    print unit.gallon
    print unit.floz

    unit.Hz = 1
    print unit.Hz

    unit.one = 1
    unit.count = unit.one
    print unit.one
    print unit.count

    unit.g = 1
    unit.mps = 3e8
    unit.J = unit.g * unit.mps * unit.mps
    print unit.J

    unit.one = 10
    unit.s = 0.01
    unit.Hz = unit.one / unit.s
    print unit.Hz

    unit.fpf = 10
    unit.mps = unit.fpf
    print unit.fpf, "=", unit.mps

    unit.furlong = 100
    unit.fortnight = 10
    unit.mps = unit.furlong / unit.fortnight
    print unit.furlong, "/", unit.fortnight, "=", unit.mps

    unit.m = 3
    temp = unit.m**3
    unit.m3 = unit.m**3
    print temp
    print unit.m3

    unit.s = 0.05
    temp = 1 / unit.s
    unit.Hz = 1 / unit.s
    print temp
    print unit.Hz

    unit.dashdash = 3
    temp = 5 ** unit.dashdash
    print temp

    unit.dashdash = 2.5
    temp = 5 ** unit.dashdash
    print temp

    unit.s = 5
    unit.dashdash = 2.5
    unit.s *= unit.dashdash
    print unit.s

    trick.stop(1.0)

if __name__ == "__main__":
    main()


