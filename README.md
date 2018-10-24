# boost_python_lagrange

C++ better implementation of scipy.interpolate.lagrange.

## problem of scipy.interpolate.lagrange

```python
import scipy.interpolate
ts = (2457995.5, 2457996.5, 2457998.5, 2457999.5)
xs = (-1.252793566234204, -1.26138959806602, -1.278335905926806, -1.286685192684683)
t = 2457997.5
x = -1.26990387693624
scipy.interpolate.lagrange(ts, xs)(t)
#-1398.81689453125
x - -1398.81689453125
#1397.5469906543137
```

## Usage

```python
import lagrange
ts = (2457995.5, 2457996.5, 2457998.5, 2457999.5)
xs = (-1.252793566234204, -1.26138959806602, -1.278335905926806, -1.286685192684683)
t = 2457997.5
x = -1.26990387693624
lagrange.Lagrange(ts, xs)(t)
#-1.2699038761754027
x - -1.2699038761754027
#7.608373930878543e-10
```

## DeltaLagrange

```
import lagrange
ts = (2457995.5, 2457996.5, 2457998.5, 2457999.5)
xs = (-1.252793566234204, -1.26138959806602, -1.278335905926806, -1.286685192684683)
t = 2457997.5
x = -1.26990387693624
l = lagrange.Lagrange(ts, xs)
c = l.coefficients()
c
#[8.243925777214518e-08, -0.6078653443383755, 1494030.4051179832, -1224024832324.8926]
def calc(c, t):
    r = 0
    for i in range(len(c) - 1):
       r += c[i]
       r *= t
    r += c[-1]
    return r

calc(c, t)
#-1.26953125
x - -1.26953125
-0.0003726269362400725

dl = lagrange.DeltaLagrange(ts, xs)
c, t0, x0 = dl.coefficients()
calc(c, t - t0) + x0
#-1.2699038761754027
x - -1.2699038761754027
#-7.608373930878543e-10
l(t) - (calc(c, t - t0) + x0)
#0.0
```

## Requirements

* Boost.Python
* Boost.Multiprecision

## Reference

* <https://gist.github.com/mugwort-rc/1ff6ff1246f529d4870fbc8084520ebd>
* [lagrange.ref.lagrange](https://github.com/mugwort-rc/boost_python_lagrange/blob/master/lagrange/ref/lagrange.py)
