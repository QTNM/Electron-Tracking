"""
Electron trace module

Module provides wrappers to various electron tracing routines
in an import-able form, which is required for threading.
"""

import numpy as np
from qtnm_base import QtnmBaseSolver


def trace_theta(theta, solver, v0, nrot=3000):
    if not isinstance(solver, QtnmBaseSolver):
        raise TypeError('Argument solver must be a subclass of QtnmBaseSolver')

    _x0 = np.zeros(3)
    _v0 = np.array([v0 * np.cos(theta), 0.0, v0 * np.sin(theta)])

    return solver.solve(nrot, x0=_x0, v0=_v0, cfl=1e-1)


# A better implementation would be a wrapper to trace_theta
def electron_trace_2d(params, solver, v0, nrot=3000):
    if not isinstance(solver, QtnmBaseSolver):
        raise TypeError('Argument solver must be a subclass of QtnmBaseSolver')

    theta = params[0]
    r0 = params[1]
    print('Processing run: theta = %f, r0 = %f' % (theta, r0))

    _x0 = np.zarray([0.0, r0, 0.0])
    _v0 = np.array([v0 * np.cos(theta), 0.0, v0 * np.sin(theta)])

    return solver.solve(nrot, x0=_x0, v0=_v0, cfl=1e-1)
