`variable_server.py` is a Python module for communicating with a sim's variable server from a Python program. Its primary purpose is to easily get and set variable values and units, but it also includes some additional convenience methods for affecting the sim's state. The code itself is well-commented, so I won't be reproducing the API here. Run `pydoc variable_server` (in the containing directory) for that.

# Release Your Resources!
First things first. Communicating
 with the variable server means opening sockets. Sockets are a resource. Threads are also a resource, and this module uses them as well. The OS gets angry when you leak resources, so you should do your best to dispose of them when you're done. Python doesn't support RAII well, and `__del__` isn't a good place to free resources, so I'm afraid I couldn't automatically clean up after you. You're going to have to be explicit about it, which [Python style](https://www.python.org/dev/peps/pep-0020/) prefers anyway.

## Call Close when You're Done
So how do we release this module's resources? I've provided a handy little function called `close` that takes care of everything for you. All you have to do is remember to call it when you're done. I know, I know, I hate having to remember to call close functions too. They're unassuming, not particularly interesting, and easy to forget about. But there's no way around it, so do your best. In truth, the world won't come crashing down around you if you do forget. You probably won't even notice a difference unless you're leaking hundreds of `VariableServer` instances, in which case you're probably _trying_ to break everything. But call it anyway, ok?

```python
from variable_server import VariableServer
variable_server = VariableServer('localhost', 7000)
# I'm using variable_server here.
# Getting values.
# Setting units.
# Doing other stuff.
# Ok, I'm done.
variable_server.close()  # don't forget to call close!
```

## Or Use a Context Manager
Wait a tick! Python has the concept of context managers, which support automatic finalization within a limited scope. This is perfect if you only need to create an instance for a single block of code. However, it doesn't work if the use is spread over multiple scopes (many different methods sharing the same instance, for example), so you'll have to decide what works best for you.

```python
from variable_server import VariableServer
with VariableServer('localhost', 7000) as variable_server:
    # Hmmm, this syntax is a little strange, but I'll go with it.
    # Actually, the more I look at it, the more I like it.
    # Python is pretty cool!
    # Oh yeah, I'm supposed to be using variable_server here.
    # Ok, I'm done. 
# Look, ma! No need to call close!
```

`close` is automatically called when the `with` block exits, no matter how that occurs: normally, via exception, ~~even if you pull the power cord from your computer!~~

# Just Tell Me How to Get a Frickin' Value
Looking for the TL;DR version, eh? Alright, here you go:

```python
>>> from variable_server import VariableServer
>>> variable_server = VariableServer('localhost', 7000)
>>> variable_server.get_value('ball.obj.state.input.mass')
'10'
```

## What!? That Returned a String. Mass isn't a String!
Well if you weren't in such a rush, we could talk a bit more about your options. What's that? You suddenly have some time to actually read the documentation? Great! Let's dive in.

## Specifying Type
`get_value` has a parameter called `type_` that is used to convert the string value returned by the sim into something more useful. Want an int? Pass `int`. Want a float? Pass `float`. Want a string? Don't pass anything; `str` is the default. Whatever you pass to `type_` is actually called on the string from the sim, so you can pass any function that accepts one argument. Even a custom lambda!

```python
>>> variable_server.get_value('ball.obj.state.input.mass', type_=int)
10
>>> variable_server.get_value('ball.obj.state.input.mass', type_=float)
10.0
>>> variable_server.get_value('ball.obj.state.input.mass', type_=lambda x: int(x) * 2)
20
```

You'll get an error if you try an invalid conversion.

```python
>>> variable_server.get_value('ball.obj.state.input.mass', type_=dict)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "variable_server.py", line 331, in get_value
    return type_(value)
ValueError: dictionary update sequence element #0 has length 1; 2 is required
```

## Specifying Units
`get_value` has a parameter for that too: `units`.

```python
>>> variable_server.get_value('ball.obj.state.input.mass', units='g', type_=int)
10000
```

You'll get an error if you try an invalid conversion.

```python
>>> variable_server.get_value('ball.obj.state.input.mass', units='m')
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "variable_server.py", line 329, in get_value
    _assert_units_conversion(name, units, actualUnits)
  File "variable_server.py", line 927, in _assert_units_conversion
    raise UnitsConversionError(name, expectedUnits)
variable_server.UnitsConversionError: [ball.obj.state.input.mass] cannot be converted to [m]
```

# What About Setting Values?
Of course you can set values! It's even easier than getting them.

```python
>>> variable_server.set_value('ball.obj.state.input.mass', 5)
>>> variable_server.get_value('ball.obj.state.input.mass', type_=int)
5
```

You can specify units when you set variables too.

```python
>>> variable_server.set_value('ball.obj.state.input.mass', 5, units='g')
```

Doing so has no effect on subsequent calls to `get_value`, which continues to use the original units (kg, in this case)

```python
>>> variable_server.get_value('ball.obj.state.input.mass', type_=float)
0.005
```

unless you say otherwise.

```python
>>> variable_server.get_value('ball.obj.state.input.mass', units='g', type_=float)
5.0
```

# Single-Value Fetches are for Chumps. I Want Multiple Values Simultaneously!
To get any fancier, we have to talk about implementation details a bit. Trick's variable server doesn't actually have a "one-shot" value fetching option. Instead, it's designed to periodically send a set of variable values over and over again. If you're familiar with variable server commands, `get_value` actually calls `var_add`, `var_send`, and `var_clear` every time it's called. If we want multiple values, we're better off doing all the `var_adds` together and just calling `var_send` and `var_clear` once. If you don't know what I'm talking about, don't worry about it. All you need to know is that `get_values` is more efficient than calling `get_value` for fetching multiple variables.

It's also a little more complicated. Having a parameter list like `name1, units1, type1, name2, units2, type2` and so on would get ugly fast. So say goodbye to the simple interface! Time to encapsulate that data in a class.

## The `Variable` Class
`Variable` represents a simulation variable. It's constructor takes the same parameters we've been using with `get_value` and `set_value`: `name`, `units`, and `type_`. `Variables` are used with the `get_values` function (note the trailing `s`), which accepts an arbitrary number of them. `get_values` uses the information in each `Variable` in the same way that `get_value` uses its parameters, and the observable behavior is largely the same: you get back a list of values.

```python
>>> from variable_server import Variable
>>> position = Variable('ball.obj.state.input.position[0]', type_=int)
>>> mass = Variable('ball.obj.state.input.mass', units='g', type_=float)
>>> variable_server.get_values(position, mass)
[5, 10000.0]
```

And you get an error if a units or type_ conversion fails.

```python
>>> variable_server.get_values(Variable('ball.obj.state.input.mass', units='m'))
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "variable_server.py", line 430, in get_values
    _assert_units_conversion(variable.name, variable.units, units)
  File "variable_server.py", line 941, in _assert_units_conversion
    raise UnitsConversionError(name, expectedUnits)
variable_server.UnitsConversionError: [ball.obj.state.input.mass] cannot be converted to [m]

>>> variable_server.get_values(Variable('ball.obj.state.input.mass', type_=dict))
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "variable_server.py", line 438, in get_values
    return [variable.value for variable in variables]
  File "variable_server.py", line 145, in value
    return self._type(self._value)
ValueError: dictionary update sequence element #0 has length 1; 2 is required
```

But wait, there's more! Each `Variable` is also updated in place, so you can ignore the returned list and use each `Variable`'s `value` property instead if that's more convenient.

```python
>>> position.value
5
```

Units are also available and are automatically filled in if you didn't specify them when creating the `Variable`.

```python
>>> position.units
'm'
```

You were probably going to save the returned values somewhere anyway, right? Might as well save them with the `Variable`s themselves! However, the returned list can be useful if you want to use the values in the same expression in which they're fetched.

```python
>>> x = Variable('ball.obj.state.output.position[0]')
>>> y = Variable('ball.obj.state.output.position[1]')
>>> print 'The ball is at position ({0}, {1})'.format(*variable_server.get_values(x, y))
The ball is at position (3.069993744436219, -11.04439115432281)
```

Or if you don't want to save the values at all!

```python
>>> print 'The ball is at position ({0}, {1})'.format(*variable_server.get_values(
... Variable('ball.obj.state.output.position[0]'),
... Variable('ball.obj.state.output.position[1]')))
The ball is at position (3.069993744436219, -11.04439115432281)
```

Which are both equivalent to, but more compact than:

```python
>>> x = Variable('ball.obj.state.output.position[0]')
>>> y = Variable('ball.obj.state.output.position[1]')
>>> variable_server.get_values(x, y)
['3.069993744436219', '-11.04439115432281']
>>> print 'The ball is at position ({0}, {1})'.format(x.value, y.value)
The ball is at position (3.069993744436219, -11.04439115432281)
```

### Don't Mess With `Variable` Attributes
You should consider `Variable` read-only. This module ensures that each `Variable`'s state remains consistent. Once you've constructed one, you should not directly set any of its fields, and you shouldn't need to. Of course, this is Python, so there's nothing to stop you from doing:

```python
>>> mass.value = 1337
```

But that's certainly not going to affect the corresponding variable in the sim.

```python
>>> variable_server.get_value('ball.obj.state.input.mass', type_=float)
5.0
```

And changing a `Variable`'s units

```python
>>> mass.units = 'g'
```

is not going to automagically perform a conversion.

```python
>>> mass
ball.obj.state.input.mass = 1337.0 g
```

A `Variable` only reflects the state of its corresponding variable in the sim. It does not manipulate it. Always use  `set_value` to change the value. The units can be specified in `Variable`'s constructor. They can also be changed via `set_units`, but only for `Variable`s that are being periodically sampled.

# Periodic Sampling
Ah, now we're _really_ cooking! This is what the variable server was made for: sending sets of variable values at a specified rate. If you find yourself calling `get_values` over and over again on the same set of variables, perhaps you'd like to step up to the big leagues and take a crack at asynchronous periodic sampling. Don't worry, it's not as scary as it sounds. In fact, we're already familiar with the core data structure: our old friend `Variable`.

## Adding `Variable`s
Periodic sampling uses the same `Variable`s we used with `get_values`. To get started, just call `add_variables`!

```python
>>> position = Variable('ball.obj.state.output.position[0]', type_=float)
>>> variable_server.add_variables(position)
```

After checking for units and type_ conversion errors, this causes the sim to periodically send the value of `ball.obj.state.output.position[0]` to us, which is used to automatically update `position`.

```python
>>> position.value
-7.24269488786
>>> position.value
-9.0757620175
>>> position.value
-9.751339991
```

Look at that! `position` is updating all on its own. Now you can stick your periodic logic in a nice `while` loop and run forever!

```python
>>> import time
>>> while True:
...     position.value
...     time.sleep(1)
-2.065295422179974
1.5358082417288299
4.8450427189593777
```

## Triggering Callbacks
Using a `while` loop with a `sleep` might work for applications that don't care about the "staleness" of the data when it arrives, but we write real-time code around here; I can't suffer unnecessary delays! The problem with the above approach is that there's no synchronization between when the updates occur and when our sleep happens to return. Sure, we could use `set_period` to tell the sim to send data at the same rate that we're sleeping, but we're bound to drift apart over time, and we can't ensure that we start a new cycle at the same time the sim does. Plus, there's network latency. And what if we ask the sim to send as fast as possible? Then we don't even know what the rate _is_!

But wait, it gets worse! If your processing cycle is faster than the sim's update cycle, you'll needlessly reprocess values that haven't been updated since the last time you processed them, which is wasteful. But if your cycle is slower, you'll miss some updates entirely.

For some applications, these issues may truly not matter, and using a simple `while` loop might be sufficient. For the rest of us, there's `register_callback`.

```python
>>> def foo():
...     print position.value
>>> variable_server.register_callback(foo)
0.632962631449
0.598808711027
0.564218072538
```

Now `foo` will be called each and every time there's an update, as soon as it arrives. Huzzah! You can set the period at which updates are sent via `set_period`, which applies to _all_ variables that this instance is tracking, regardless of when they're added. If you want to receive another set at a different rate, you should create another `VariableServer`.

## Concurrency Concerns
Callback functions are executed on the variable sampling thread, which is started when you instantiate `VariableServer` and runs until you call `close` (either explicitly or via a `with` statement). This means that new updates can't be processed until all callback functions have returned. The variable sampling thread spends most of its time blocked, waiting for new updates to arrive, so time consumed by callback functions usually isn't an issue. But if your callback performs a long-running task, you should probably do it in another thread so it doesn't cause the variable sampling thread to fall behind.

# The API
Wikis are great for how-tos and high-level discussions, but if you want to get down to the nuts and bolts, you need to look at the API. You can do so by running `pydoc variable_server` in the directory containing `variable_server.py` or programmatically by calling `help` on the feature in which you're interested.

```python
>>> import variable_server
>>> help(variable_server.Variable)
Help on class Variable in module variable_server:

class Variable(__builtin__.object)
 |  A variable whose value and units will be updated from the sim. You
 |  should not directly change any part of this class.
```

[[Continue to Software Requirements|SRS]]