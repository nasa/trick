"""
This module contains tools for communicating with a sim's variable
server, primarily via the Variable and VariableServer classes. See
https://github.com/nasa/trick/wiki/Python-Variable-Server-Client for
a tutorial and examples.
"""

from collections import namedtuple
import itertools
import os
import re
import socket
import struct
import threading
import time

class VariableServerError(Exception):
    '''
    Variable Server communication I/O error.
    '''
    pass

class UnitsConversionError(VariableServerError):
    """
    Raised when a units conversion fails.

    Attributes
    ----------
    name : str
        The name of the variable for which the conversion failed.
    units : str
        The units to which the variable could not be converted.
    """
    def __init__(self, name, units):
        super(UnitsConversionError, self).__init__(
          '[{0}] cannot be converted to [{1}]'.format(name, units)
        )
        self.name = name
        self.units = units

class UnexpectedMessageError(VariableServerError):
    """
    Raised when a received message is not of the expected type.

    Attributes
    ----------
    expected_id : int
        The expected message indicator.
    actual_id : int
        The actual message indicator.
    """
    def __init__(self, expected_id, actual_id):
        super(UnexpectedMessageError, self).__init__(
          'Unexpected message received. Expected ID = {0}. Actual ID = {1}'
          .format(expected_id, actual_id)
        )
        self.expected_id = expected_id
        self.actual_id = actual_id

class ValueCountError(VariableServerError):
    """
    Raised when the number of received variable values does not match
    the expected count.

    Attributes
    ----------
    expected : int
        The expected count.
    actual : int
        The actual count.
    """
    def __init__(self, expected, actual):
        super(ValueCountError, self).__init__(
          'Number of values received ({0}) does not match expected ({1})'
          .format(actual, expected)
        )
        self.expected = expected
        self.actual = actual

def _create_enum(name, field_names, ordinal_values=True):
    """
    Create a namedtuple with automatic values.

    Parameters
    ----------
    name : str
        The name of the namedtuple.
    field_names : iterable of str
        The field names.
    ordinal_values : bool
        True to assign each field an ordinal number, starting at 0. This
        creates a classic number-based enum.
        False to use the field names themselves as the values for the
        fields. This creates a string-based enum.
    """
    return namedtuple(name, field_names)(
      *(range(len(field_names)) if ordinal_values else field_names)
    )

class Message(namedtuple('Message', ['indicator', 'data'])):
    """
    A message from the variable server.

    Attributes
    ----------
    indicator : int
        The indicator.
    data : str
        The rest of the message.
    """
    Indicator = _create_enum('Indicator', ['VAR_SEND', 'VAR_EXISTS'])

class Variable(object):
    """
    A variable whose value and units will be updated from the sim. You
    should not directly change any part of this class.

    Attributes
    ----------
    name : str
        The fully-qualified name.
    units : str
        The units. Use 'xx' to specify default units.

    Properties
    ----------
    value : type returned by the type_ parameter of __init__ (or None)
        The value.
    """

    def __init__(self, name, units=None, type_=str):
        """
        Create a new Variable.

        Parameters
        ----------
        name and units are as documented in this class's Attributes
        section.
        type_ : callable
            A callable that accepts one argument and is used to convert
            the stringified value from the variable server to the
            desired type. This can be a builtin like int or float or a
            custom function that does anything you like!
        """
        self.name = name
        self.units = units
        self._type = type_
        self._value = None

    @property
    def value(self):
        '''
        Get the converted value.
        '''
        return self._type(self._value)

    @value.setter
    def value(self, value):
        '''
        Set the value.
        '''
        self._value = value

    def __str__(self):
        return self.name

    def __repr__(self):
        return '{0} = {1}{2}'.format(
          self.name,
          self.value,
          ' {0}'.format(self.units) if self.units is not None else '')

class VariableServer(object):
    """
    Send commands to and receive responses from a simulation's
    variable server.

    You must call close on instances of this class to release resources
    allocated during initialization.
    """

    Channel = _create_enum('Channel', ['ASYNC', 'SYNC', 'BOTH'], False)
    CopyMode = _create_enum('CopyMode', ['ASYNC', 'SCHEDULED', 'TOP_OF_FRAME'])

    def __init__(self, hostname, port):
        """
        Create a connection to the simulation variable server at
        host:port.

        Parameters
        ----------
        hostname : str
            The name of the machine that is running the simulation to
            which you want to connect.
        port : int
            The port on which the simulation's variable server is
            listening.
        """
        self._variables = []
        self._callbacks = {}
        self._error_callbacks = {}
        self._lock = threading.Lock()
        port = int(port)
        self._synchronous_socket = socket.create_connection((hostname, port))
        self._asynchronous_socket = socket.create_connection((hostname, port))
        self._synchronous_file_interface = self._synchronous_socket.makefile()
        self._asynchronous_file_interface = self._asynchronous_socket.makefile()
        self._open = True
        self.pause(channel=self.Channel.SYNC)

        # Define a local function to be used by the sampling thread.
        def update_variables():
            '''
            Continuously update variables.
            '''
            while True:
                try:
                    values = self._read_values(False)
                except Exception as exception:
                    if self._open:
                        for function, args in self._error_callbacks.items():
                            function(*args[0], exception=exception, **args[1])
                    return
                # We must lock here to ensure that variables are not
                # removed while we are processing an update.
                with self._lock:
                    # If there are more values than variables, it must
                    # be that a variable was removed after this message
                    # was sent but before we processed it. Variables can
                    # be removed from any place within the list, so we
                    # don't know which "extra" values to ignore.
                    # We therefore discard the entire message.
                    #
                    # If there are fewer values than variables, it must
                    # be that a variable was added after this message
                    # was sent but before we processed it. Variables
                    # can only be appended to the list, so we know that
                    # any missing values are for variables at the end of
                    # the list. We can therefore still use all the
                    # values to update variables at the front of the
                    # list.
                    #
                    # We could still end up assigning values to the
                    # wrong variables if someone removed AND added
                    # variables after a message was sent but before we
                    # processed it, but the probability of someone
                    # doing that doesn't justify the work at this point.
                    # Besides, it would be corrected with the next
                    # message.
                    if len(values) <= len(self._variables):
                        for variable, value in itertools.izip(
                          self._variables, values):
                            variable.value, variable.units = \
                              _parse_value(value)

                        for function, args in self._callbacks.items():
                            function(*args[0], **args[1])

        # Start a thread that listens for data from the variable server,
        # updates the variables being sampled, and notifies listeners.
        # This thread can only be terminated by calling close. A Python
        # process cannot terminate while non-daemon threads are running,
        # so make this tread a deamon in case the user fails to call
        # close when finished with this instance.
        self._thread = threading.Thread(
          target=update_variables, name='Asynchronous Variable Sampler')
        self._thread.daemon = True
        self._thread.start()

    def __del__(self):
        """
        Call close in case the user forgot. Don't rely on this to clean
        up for you. You should really explicitly call close yourself.
        """
        try:
            self.close()
        except:
            pass

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    def get_value(self, name, units=None, type_=str):
        """
        Get the value of the named variable. If units are specified, the
        value is converted if possible. This function provides a simple
        interface and is most useful when you just want to get the value
        of one variable and don't require periodic sampling. For a more
        powerful but complex function, see get_values. To sample values
        periodically, see add_variables.

        Parameters
        ----------
        name, units, and type_ are as documented in Variable.

        Returns
        -------
        type returned by type_
            The result of calling type_ on the variable's value.

        Raises
        ------
        IOError
            If the remote endpoint has closed the connection.
        UnexpectedMessageError
            If the next message is not a set of variable values.
        ValueCountError
            If more than one value is received.
        UnitsConversionError
            If units are specified and the conversion fails.
        Additional errors may be raised by type_.

        Examples
        --------
        Get value (default type is string):
        >>> from variable_server import VariableServer
        >>> with VariableServer('localhost', 7000) as vs:
        ...     vs.get_value('ball.obj.state.input.position[0]')
        '5'

        Get value as an int:
        >>> from variable_server import VariableServer
        >>> with VariableServer('localhost', 7000) as vs:
        ...     vs.get_value('ball.obj.state.input.position[0]',
        ...     type_=int)
        5

        Get value as a float in millimeters:
        >>> from variable_server import VariableServer
        >>> with VariableServer('localhost', 7000) as vs:
        ...     vs.get_value('ball.obj.state.input.position[0]',
        ...     units='mm', type_=float)
        5000.0

        Convert value with custom function:
        >>> from variable_server import VariableServer
        >>> with VariableServer('localhost', 7000) as vs:
        ...     vs.get_value('ball.obj.state.input.position[0]',
        ...     type_=lambda x: int(x) * 2)
        10
        """

        # add the variable and poll its value
        self._var_add(name, units, self.Channel.SYNC)
        self._var_send()
        self._var_clear(self.Channel.SYNC)

        # make sure we only got one
        values = self._read_values()
        _assert_value_count(1, len(values))

        # check for units conversion
        value, actual_units = _parse_value(values[0])
        _assert_units_conversion(name, units, actual_units)

        return type_(value)

    def set_value(self, name, value, units=None):
        """
        Set the value of the named variable. If units are specified, the
        value is converted if possible. If the convserion fails, the
        value is unchanged, and no error is raised.

        Attributes
        ----------
        name : str
            The fully-qualified name.
        value : any
            The value. This should be of the type expected by the
            variable server.
        units : str
            The units.
        """
        self.send(
          'trick.var_set("{0}", {1}{2})'.format(
            name,
            '"{0}"'.format(value) if isinstance(value, basestring) else value,
            ', "{0}"'.format(units) if units is not None else ''))

    def get_values(self, *variables):
        """
        Get the values of variables. If units are specified, values are
        converted if possible. Each argument is also updated in place,
        so you can ignore the returned list of values if you were just
        going to store them anyway. The list is useful, for example, for
        inlining the results, eliminating the need to update and use
        the variables in separate statements.

        This function is more efficient than calling get_value
        for each variable, but has a steeper learning curve. It is most
        useful when you want to fetch multiple variables' values and
        units and don't require periodic sampling. To sample values
        periodically, see add_variables.

        Parameters
        ----------
        variables : zero or more Variables
            The variables for which to fetch values and units.

        Returns
        -------
        [any]
            A list of the requested values in order. The type of each
            element is that returned by the corresponding Variable's
            type_.

        Raises
        ------
        IOError
            If the remote endpoint has closed the connection.
        UnexpectedMessageError
            If the next message is not a set of variable values.
        ValueCountError
            If the number of received values does not match the number
            of variables.
        UnitsConversionError
            If units are specified and the conversion fails. In this
            case, variables before the error will have been updated,
            but variables after will not. The variable for which the
            error occurred will not be updated.
        Additional errors may be raised by each Variables's type_, but
        they do not prevent the rest of the Variables from being
        updated. Variables whose type_ conversion fails will raise an
        error every time their value property is accessed.

        Example
        -------
        >>> from variable_server import Variable, VariableServer
        >>> position = Variable('ball.obj.state.input.position[0]',
        ... type_=int)
        >>> mass = Variable('ball.obj.state.input.mass', units='g',
        ... type_=float)
        >>> with VariableServer('localhost', 7000) as vs:
        ...     vs.get_values(position, mass)
        [5, 10000.0]
        >>> position
        ball.obj.state.input.position[0] = 5 m
        >>> mass
        ball.obj.state.input.mass = 10000.0 g
        """

        # check for zero arguments
        if not variables:
            return []

        # add all the variables and poll their values
        for variable in variables:
            self._var_add(
              variable.name,
              variable.units if variable.units is not None else 'xx',
              self.Channel.SYNC)
        self._var_send()
        self._var_clear(self.Channel.SYNC)

        # make sure we got as many as expected
        values = self._read_values()
        _assert_value_count(len(variables), len(values))

        # update each Variable, checking units conversions
        for variable, entry in itertools.izip(variables, values):
            value, units = _parse_value(entry)
            if variable.units is not None:
                _assert_units_conversion(variable.name, variable.units, units)
            else:
                variable.units = units
            variable.value = value

        return [variable.value for variable in variables]

    def add_variables(self, *variables):
        """
        Immediately update and begin periodically sampling the given
        variables. This class retains references to all passed-in
        variables and updates their values and units when new values
        arrive from the variable server. All sampling is performed on a
        separate thread, but accessing a variable's fields is always
        safe. Adding variables which are already being sampled has no
        effect. To set the sampling period, see set_period. To register
        a function to be called whenever variables are updated, see
        register_callback.

        Parameters
        ----------
        variables : zero or more Variables
            The variables to begin sampling.

        Raises
        ------
        IOError
            If the remote endpoint has closed the connection.
        UnexpectedMessageError
            If the next message is not a set of variable values.
        ValueCountError
            If the number of received values does not match the number
            of variables.
        UnitsConversionError
            If units are specified and the conversion fails. In this
            case, variables before the error will have been updated,
            but variables after will not. The variable for which the
            error occurred will not be updated.
        Additional errors may be raised by each Variables's type_, but
        they do not prevent the rest of the Variables from being
        updated. Variables whose type_ conversion fails will raise an
        error every time their value property is accessed.

        If any error occurs, no variables are scheduled for sampling.
        """

        # remove existing variables
        variables = [variable for variable in variables
                     if variable not in self._variables]

        # check for type_ and units conversion errors
        self.get_values(*variables)

        for variable in variables:
            # No lock is needed here because:
            # - appending to the variables list while
            #   update_variables is executing does not invalidate
            #   the length check
            # - izip is bounded by the number of values, and the
            #   length check ensures there are at least as many
            #   variables as values
            self._variables.append(variable)
            self._var_add(
              variable.name,
              variable.units if variable.units is not None else 'xx')

    def remove_variables(self, *variables):
        """
        Stop sampling the given variables. Removing variables that are
        not being sampled has no effect.

        Parameters
        ----------
        variables : zero or more Variables
            The variables to stop sampling.
        """
        for variable in variables:
            if variable in self._variables:
                # Variables must not be removed while update_variables
                # is processing an update. See its comment for an
                # explanation.
                with self._lock:
                    self._variables.remove(variable)
                self._var_remove(variable.name)

    def remove_all_variables(self):
        """
        Stop sampling all variables. To merely suspend sampling,
        see pause.
        """
        self._var_clear()
        # No lock is needed here because:
        # - This assignment is atomic.
        # - If update_variables has already called izip, the list it is
        #   ierating over is unchanged as we're assigning a new list
        #   here instead of clearing the shared reference.
        # - If update_variables has not yet called izip, when it does,
        #   izip will return an empty generator, terminating the loop.
        self._variables = []

    def set_units(self, name, units):
        """
        Set the units in which the named variable is sampled. This only
        applies to Variables being periodically sampled.
        See add_variables.

        Parameters
        ----------
        name : str
            The variable's name.
        units : str
            The units to which to convert the sampled value.
        """
        self.send('trick.var_units("{0}", "{1}")'.format(name, units),
                  self.Channel.ASYNC)

    def set_period(self, period):
        """
        Set the sampling period (in seconds).

        Parameters
        ----------
        period : float
            The inverse of the rate (in Hz) at which you want to sample
            variable values.
        """
        self.send('trick.var_cycle({0})'.format(float(period)),
                  self.Channel.ASYNC)

    def register_callback(self, function, args=None, kwargs=None):
        """
        Call function whenever new variable values are sampled.
        Registering an aleady-registered function replaces its existing
        registration. The order in which functions are called is not
        specified. Functions are executed on the asynchronous sampling
        thread.

        Paramaters
        ----------
        function : callable
            The function to call.
        args : tuple
            The positional arguments to be passed to the function.
        kwargs : dict
            The keyword arguments to be passed to the function.
        """
        if args is None:
            args = []
        if kwargs is None:
            kwargs = {}
        self._callbacks[function] = args, kwargs

    def deregister_callback(self, function):
        """
        Do not call function whenever new variable values are sampled.
        Deregistering an unregistered function has no effect.

        Parameters
        ----------
        function : any
            A function previously added via register_callback.
        """
        self._callbacks.pop(function, None)

    def register_error_callback(self, function, args=None, kwargs=None):
        """
        Call function if an error occurs while sampling variable values.
        Registering an aleady-registered function replaces its existing
        registration. The order in which functions are called is not
        specified. Functions are executed on the asynchronous sampling
        thread.

        Paramaters
        ----------
        function : callable
            The function to call. It must accept a keyword argument
            named 'exception' which will contain the error.
        args : tuple
            The positional arguments to be passed to the function.
        kwargs : dict
            The keyword arguments to be passed to the function.
        """
        if args is None:
            args = []
        if kwargs is None:
            kwargs = {}
        self._error_callbacks[function] = args, kwargs

    def deregister_error_callback(self, function):
        """
        Do not call function if an error occurs while sampling variable
        values. Deregistering an unregistered function has no effect.

        Parameters
        ----------
        function : any
            A function previously added via register_error_callback.
        """
        self._error_callbacks.pop(function, None)


    def pause(self, pause=True, channel=Channel.ASYNC):
        """
        Pause or unpause sampling.

        Parameters
        ----------
        pause : bool
            True to pause sampling.
            False to resume sampling.
        channel : Channel
            The channel to affect. You should almost certainly leave
            this as the default.
        """
        self.send('trick.var_{0}pause()'.format('' if pause else 'un'),
                  channel)

    def set_debug(self, level, channel=Channel.BOTH):
        """
        Set the debugging level. This effects how much information the
        sim's variable server outputs to the sim's standard output
        stream. It is not a debugging option for this Python class and
        does not produce output on this process.

        Parameters
        ----------
        level : int
            The debugging level. 0 = no debugging.
        channel : Channel
            The channel to affect.
        """
        self.send('trick.var_debug({0})'.format(int(level)), channel)

    def set_tag(self, tag):
        """
        Set the identifier for this variable server client. The tag is
        used in log files to associate each message with its sender.

        Parameters
        ----------
        tag : str
            An identifier for this client.
        """
        for channel in [self.Channel.SYNC, self.Channel.ASYNC]:
            self.send(
              'trick.var_set_client_tag("{0}_{1}")'.format(tag, channel),
              channel)

    def set_copy_mode(self, mode=CopyMode.ASYNC, channel=Channel.BOTH):
        """
        Set the method by which variable values are copied.

        Parameters
        ----------
        mode : CopyMode
            ASYNC
            Values are copied by an independent thread. This has no
            effect on simulation execution, but values within a set may
            not all be from the same sim frame.

            SCHEDULED
            Values are copied as an automatic_last job in the sim's main
            thread. Values within a set are guaranteed to be from the
            same sim frame.

            TOP_OF_FRAME
            Values are copied at the top of each sim frame in the main
            thread. Values within a set are guaranteed to be from the
            same sim frame.

        channel : Channel
            The channel to affect.
        """
        self.send('trick.var_set_copy_mode({0})'.format(int(mode)), channel)

    def send_on_copy(self, enable=True):
        """
        Set the method by which variable values are sent.

        Parameters
        ----------
        enable : bool
            True to cause the variable server to send variable values
            immediately after copying them, using the same thread, which
            will impact simulation execution if the copy mode is not
            ASYNCHRONOUS.
            False to send values asynchronously on an independent
            thread.
        """
        self.send('trick.var_set_copy_mode({0})'.format(bool(enable)),
                  self.Channel.ASYNC)

    def validate_addresses(self, validate=True, channel=Channel.BOTH):
        """
        Set whether or not addresses are validated.

        When bool(validate) is True, variable addresses will be
        validated against the memory manager before being read. Those
        not known to Trick are considered invalid and their values are
        reported as "BAD_REF". This prevents malformed variable
        requests, such as pointers with invalid offsets, from causing
        segmentation faults.

        Parameters
        ----------
        validate : bool
            The desired validation state.
        channel : Channel
            The channel to affect.
        """
        self.send('trick.var_validate_address({0})'.format(bool(validate)),
                  channel)

    def variable_exists(self, name):
        """
        Determine if name is known to the memory manager.

        Parameters
        ----------
        name : str
            The variable's name.

        Returns
        -------
        bool
            True if name has been registered with the memory manager.
            False if not.

        Raises
        ------
        IOError
            If the remote endpoint has closed the connection.
        UnexpectedMessageError
            If the next message on the synchronous channel is not a
            response to a var_exists inquiry.
        """
        self.send('trick.var_exists("{0}")'.format(name))
        message = self.readline()
        _assert_message_type(message, Message.Indicator.VAR_EXISTS)
        return message.data == '1'

    def freeze(self, freeze=True):
        """
        Freeze or unfreeze the sim.

        Parameters
        ----------
        freeze : bool
            True to freeze the sim.
            False to unfreeze it.
        """
        self.send('trick.exec_{0}()'
                  .format('freeze' if bool(freeze) else 'run'))

    def enable_real_time(self, enable=True):
        """
        Toggle real-time execution.

        Parameters
        ----------
        enable : bool
            True so synchronize sim execution with the real-time clock.
            False to run as quickly as possible.
        """
        self.send('trick.real_time_{0}able()'
                  .format('en' if bool(enable) else 'dis'))

    def send(self, command, channel=Channel.SYNC):
        """
        Append a newline to command and send it, blocking until all data
        has been sent. Calling this directly is only necessary if you
        want to send a command for which there is not already a method
        in this class. You must not send a command on the asynchronous
        channel that produces a response, as the response will be
        consumed by the variable-sampling thread, which will likely
        cause an error. You must ensure the socket is left in a "clean"
        state before calling other methods of this class. If incoming
        data is left on either channel, it is likely to cause other
        methods to fail since they won't get what they're expecting when
        they go to read.

        Parameters
        ----------
        command : str
            The command to send.
        channel : Channel
            The channel on which to send. You should almost certainly
            leave this as the default.
        """
        command = '{0}\n'.format(command)

        for channel in {
          self.Channel.SYNC: [self._synchronous_socket],
          self.Channel.ASYNC: [self._asynchronous_socket],
          self.Channel.BOTH: [self._synchronous_socket,
                              self._asynchronous_socket]
        }[channel]:
            channel.sendall(command)

    def readline(self, synchronous_channel=True):
        """
        Read a newline-terminated line, blocking if necessary. Calling
        this directly is only necessary if you have directly called
        send and expect a response from the variable server. The newline
        character is stripped.

        Returns
        -------
        Message
            The next available message.
        synchronous_channel : bool
            True to read from the synchronous channel.
            False to read from the asynchronous channel.

        Raises
        ------
        IOError
            If the remote endpoint has closed the connection.
        """
        file_interface = (self._synchronous_file_interface
                          if synchronous_channel
                          else self._asynchronous_file_interface)
        line = file_interface.readline()
        if not line:
            raise IOError("The remote endpoint has closed the connection")
        line = line.rstrip(os.linesep).split('\t', 1)
        return Message(int(line[0]), line[1])

    def _var_add(self, name, units=None, channel=Channel.ASYNC):
        """
        Send a var_add command to the variable server.

        Parameters
        ----------
        name : str
            The variable's name.
        units : str
            The units in which to report the variable's value.
        channel : Channel
            The channel to affect.
        """
        self.send(
          'trick.var_add("{0}"{1})'
          .format(name, ', "{0}"'.format(units) if units is not None else ''),
          channel)

    def _var_remove(self, name):
        """
        Send a var_remove command to the variable server.

        Parameters
        ----------
        name : str
            The variable's name.
        channel : Channel
            The channel to affect.
        """
        self.send('trick.var_remove("{0}")'.format(name),
                  self.Channel.ASYNC)

    def _var_send(self, channel=Channel.SYNC):
        """
        Send a var_send command to the variable server.

        Parameters
        ----------
        channel : Channel
            The channel to affect.
        """
        self.send('trick.var_send()', channel)

    def _read_values(self, synchronous_channel=True):
        """
        Read a set of variable values.

        Parameters
        ----------
        synchronous_channel : bool
            True to read from the synchronous channel.
            False to read from the asynchronous channel.

        Returns
        -------
        [str]
            A list of values, which may include units.

        Raises
        ------
        IOError
            If the remote endpoint has closed the connection.
        UnexpectedMessageError
            If the next message is not a set of variable values.
        """
        message = self.readline(synchronous_channel)
        _assert_message_type(message, Message.Indicator.VAR_SEND)
        return message.data.split('\t')

    def _var_clear(self, channel=Channel.ASYNC):
        """
        Send a var_clear command to the variable server.

        Parameters
        ----------
        channel : Channel
            The channel to affect.
        """
        self.send('trick.var_clear()', channel)

    def close(self):
        """
        Close the connection and release resources. No methods can be
        called after this one. A new connection can be established only
        by creating a new instance.
        """
        self._open = False
        self._synchronous_file_interface.close()
        self._asynchronous_file_interface.close()
        self._synchronous_socket.shutdown(socket.SHUT_RDWR)
        self._asynchronous_socket.shutdown(socket.SHUT_RDWR)
        self._synchronous_socket.close()
        self._asynchronous_socket.close()
        self._thread.join()

def from_pid(pid, timeout=None):
    """
    Connect to the simulation at the given pid. This is done by
    listening on the multicast channel over which all sims broadcast
    their existance.

    Parameters
    ----------
    pid : int
        The sim's process ID.
    timeout : positive float or None
        How long to look for the sim before giving up. Pass None to wait
        indefinitely.

    Returns
    -------
    VariableServer
        A VariableServer connected to the sim at pid.

    Raises
    ------
    socket.timeout
        If a timeout occurs.
    """
    clock = time.time()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('', 9265))
    sock.setsockopt(
      socket.IPPROTO_IP,
      socket.IP_ADD_MEMBERSHIP,
      struct.pack("=4sl", socket.inet_aton('224.3.14.15'), socket.INADDR_ANY))
    file_interface = sock.makefile()

    # the socket will clean itself up when it's garbage-collected
    while True:
        if timeout is not None:
            timeout -= (time.time() - clock)
            if timeout < 0:
                raise socket.timeout
            clock = time.time()
            sock.settimeout(timeout)

        host, port, _, process_id = file_interface.readline().split('\t')[:4]
        if int(process_id) == int(pid):
            return VariableServer(host, port)

def _parse_value(text):
    """
    Parse a variable server value with optional units.

    Parameters
    ----------
    text : str
        The variable server text.

    Returns
    -------
    str, str (or None)
        A tuple containing:
            - The value.
            - The units, which may be None.
    """
    match = re.match(r"(?P<value>.*)(?: {(?P<units>.*)})", text)
    if match:
        return match.group('value'), match.group('units')
    return text, None

def _assert_message_type(message, indicator):
    """
    Raise an error if the message indicator does not match the expected
    indicator.

    Parameters
    ----------
    message : Message
        The message to test.
    indicator : Message.Indicator
        The expected indicator.

    Raises
    ------
    UnexpectedMessageError
        If the message indicator does not match the expected indicator.
    """
    if message.indicator != indicator:
        raise UnexpectedMessageError(indicator, message.indicator)

def _assert_units_conversion(name, expected_units, actual_units):
    """
    Raise an error if the actual units do not match the expected units.

    Parameters
    ----------
    name : str
        The name of the variable being tested.
    expected_units : str
        The expected units.
    actual_units : str
        The actual units.

    Raises
    ------
    UnitsConversionError
        If the actual units do not match the expected units.
    """
    if expected_units != actual_units and expected_units != 'xx':
        raise UnitsConversionError(name, expected_units)

def _assert_value_count(expected, actual):
    """
    Raise an error if actual does not match expected.

    Parameters
    ----------
    expected : any
        The expected value.
    actual : any
        The actual value.

    Raises
    ------
    ValueCountError
        If actual does not match expected.
    """
    if expected != actual:
        raise ValueCountError(expected, actual)

if __name__ == '__main__':
    import doctest
    doctest.testmod()
