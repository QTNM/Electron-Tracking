"""
Electron trace module

Module provides wrappers to various electron tracing routines
in an import-able form, which is required for threading.
"""

from ford1991 import solve_3d as solve_3d_ford
from lorentz import solve_3d as solve_3d_lorentz
import numpy as np

def electron_trace_ford(theta, B0, v0, bfield, nrot=3000):
    ic = [0, 0, 0, v0 * np.cos(theta), 0, v0 * np.sin(theta), 0.0]
    return solve_3d_ford(nrot, b0=B0, v0=v0, calc_b_field=bfield.evaluate_field, ic=ic, cfl=1e-1)

def electron_trace_lorentz(theta, B0, v0, bfield, nrot=3000):
    ic = [0, 0, 0, v0 * np.cos(theta), 0, v0 * np.sin(theta), 0.0]
    return solve_3d_lorentz(nrot, b0=B0, v0=v0, calc_b_field=bfield.evaluate_field, ic=ic, cfl=1e-1)
