from decimal import Decimal
import numpy as np


class Lagrange:
    def __init__(self, x, w):
        M = len(x)
        p = [Decimal(0.0)]
        for j in range(M):
            pt = [Decimal(w[j])]
            for k in range(M):
                if k == j:
                    continue
                fac = Decimal(x[j] - x[k])
                pt = np.convolve(pt, [Decimal(1.0) / fac, Decimal(-x[k]) / fac])
            p = self.add(p, pt)
        self.lag = p

    def __call__(self, x):
        if not isinstance(x, Decimal):
            x = Decimal(x)
        ret = Decimal(0.0)
        for i, n in enumerate(reversed(self.lag)):
            ret += x ** i * n
        return ret

    def add(self, a, b):
        diff = len(a) - len(b)
        if diff == 0:
            return [a[i] + b[i] for i in range(len(a))]
        elif diff > 0:
            return self.add(a, [Decimal(0.0) for x in range(diff)] + b)
        else:
            return self.add([Decimal(0.0) for x in range(abs(diff))] + a, b)
