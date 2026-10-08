"""Curated signatures for the most common Trick input-file calls.

Hand-maintained, unlike the rest of this stub (which is scraped from a built
Trick's share/trick/swig/sim_services.py). Anything declared here takes
priority over a same-named symbol scraped from that file.
See vscode-trick/src/pythonStubs.ts.
"""
from typing import Any

def var_allow_connections() -> None:
    """Allow external variable-server clients (Trick View, etc.) to connect."""
    ...

def var_resolve_hostname() -> None:
    """Resolve and print the host/IP the variable server is listening on."""
    ...

def var_server_get_port() -> int:
    """Return the TCP port the variable server is listening on."""
    ...

def var_set_copy_mode(mode: int) -> None:
    """Set the variable server's data-copy mode, e.g. trick.VS_COPY_ASYNC."""
    ...

def exec_set_terminate_time(time_value: float) -> None:
    """Stop the sim at the given simulation time, in seconds."""
    ...

def exec_set_software_frame(seconds: float) -> None:
    """Set the top-level executive's software frame (major cycle) length."""
    ...

def exec_set_freeze_frame(seconds: float) -> None:
    """Set how often freeze-phase jobs run while the sim is frozen."""
    ...

def exec_get_sim_time() -> float:
    """Return the current simulation time, in seconds."""
    ...

def real_time_enable() -> None:
    """Enable real-time (wall-clock-paced) execution."""
    ...

def itimer_enable() -> None:
    """Enable the interval timer used to pace real-time execution."""
    ...

def stop(time: float) -> None:
    """Shortcut for exec_set_terminate_time(time)."""
    ...

def add_read(time: float, code: str) -> int:
    """Schedule `code` (a Python snippet) to run once, at `time` seconds."""
    ...

def attach_units(units: str, value: float) -> float:
    """Tag a literal with engineering units Trick will convert, e.g.
    trick.attach_units("degrees", 45.0)."""
    ...

def checkpoint(path: str) -> None:
    """Write a checkpoint file to `path`."""
    ...

def set_job_onoff(job_name: str, instance_or_phase: int, on_off: bool) -> None:
    """Enable or disable a scheduled job by name."""
    ...

def TMM_declare_var_1d(type_name: str, count: int) -> Any:
    """Allocate a 1-D array of `type_name` through Trick's memory manager."""
    ...

def add_data_record_group(group: Any, dr_type: int = ...) -> None:
    """Register a DataRecordGroup (e.g. trick.DRAscii(...)) for recording."""
    ...

def add_external_application(app: Any) -> None:
    """Register an external application, e.g. trick.SimControlPanel()."""
    ...

class DataRecordGroup:
    def set_cycle(self, cycle: float) -> None: ...
    def set_freq(self, freq: int) -> None: ...
    def set_single_prec_only(self, single_prec: bool) -> None: ...
    def add_variable(self, var_name: str) -> None: ...
    def set_max_file_size(self, num_bytes: int) -> None: ...
    def enable(self) -> None: ...
    def disable(self) -> None: ...

class DRAscii(DataRecordGroup):
    def __init__(self, group_name: str) -> None: ...

class DRBinary(DataRecordGroup):
    def __init__(self, group_name: str) -> None: ...

class SimControlPanel:
    def __init__(self) -> None: ...

class TrickView:
    def __init__(self) -> None: ...

DR_Always: int
DR_Changed: int
DR_Buffer: int
DR_Depend: int
Runge_Kutta_4: int
Euler: int
