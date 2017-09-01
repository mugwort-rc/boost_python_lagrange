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

## Requirements

* Boost.Python
* Boost.Multiprecision

## Reference

* <https://gist.github.com/mugwort-rc/1ff6ff1246f529d4870fbc8084520ebd>
* lagrange.ref.lagrange
