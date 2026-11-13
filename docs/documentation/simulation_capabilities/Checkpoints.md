| [Home](/trick) → [Documentation Home](../Documentation-Home) → [Simulation Capabilities](Simulation-Capabilities) → Checkpoints |
|------------------------------------------------------------------|

The Trick CheckPointWriter is a C++ Class that implements checkpointing.

## Checkpointing
```python
# Save a checkpoint before the initialization jobs execute. default False
trick.checkpoint_pre_init(True|False)
# Save a checkpoint after the initialization jobs execute. default False
trick.checkpoint_post_init(True|False)
# Save a checkpoint when the simulation ends. default False
trick.checkpoint_end(True|False)
# Save a checkpoint at a time in the future
trick.checkpoint(<time>)
# Save a checkpoint now
trick.checkpoint()

# Set the CPU to use for checkpoints
trick.checkpoint_cpu(<cpu_num>)

# Save a checkpoint periodically during simulation execution. default False
trick.checkpoint_safestore_set_enabled(True|False)
# Set the safestore checkpoint period. default 9x10e18
trick.checkpoint_safestore(<period>)

# Load a checkpoint
trick.load_checkpoint(<filename>)
# Load a checkpoint without restoring STLs
trick.load_checkpoint(<filename>, False)

```

When `trick.checkpoint_cpu(<cpu_num>)` is set to a value greater than 0, checkpoints are written by a forked process, and the simulation continues without waiting for the write to finish. `trick.TMM_get_checkpoint_write_state()` waits for that process if it is still writing and then returns its result. Starting another checkpoint also waits for the previous forked write to finish first. When the simulation shuts down, it also waits for a pending forked checkpoint to finish, so a large final checkpoint can delay exit, but the file is complete once the simulation has exited.

[Continue to Memory Manager](memory_manager/MemoryManager)
