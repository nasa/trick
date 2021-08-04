import inspect
import os
import sys
import unittest

# TODO: Get rid of this and use automatic discovery when Trick requires Python 2.7
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0))), '..')))
from variable_server import *

class TestVariableServer(unittest.TestCase):

    def setUp(self):
        self.variable_server = VariableServer('localhost', 7000)
        self.variables = [
            Variable('ball.obj.state.input.position[0]', type_=int),
            Variable('ball.obj.state.input.mass', units='g', type_=float)
        ]

    def tearDown(self):
        self.variable_server.close()

    def test_get_value(self):
        variable = 'ball.obj.state.input.mass'

        self.assertEqual('10',
          self.variable_server.get_value(variable))

        self.assertEqual(10,
          self.variable_server.get_value(variable, type_=int))

        self.assertEqual(10000.0,
          self.variable_server.get_value(variable, units='g', type_=float))

        self.assertEqual(20,
          self.variable_server.get_value(variable, type_=lambda x: int(x) * 2))

        # bad units
        self.assertRaises(
          UnitsConversionError,
          self.variable_server.get_value,
          variable,
          units='fjarnskaggl')

        # bad type
        self.assertRaises(
          ValueError,
          self.variable_server.get_value,
          variable,
          type_=dict)

    def test_set_value(self):
        variable = 'ball.obj.state.input.position[1]'
        self.variable_server.set_value(variable, 1337)
        self.assertEqual('1337', self.variable_server.get_value(variable))
        self.variable_server.set_value(variable, 1337, 'km')
        self.assertEqual('1337000', self.variable_server.get_value(variable))

    def test_get_values(self):
        # empty call
        self.assertEqual([], self.variable_server.get_values())
        # multi-arg vall
        self.assertEqual(self.variable_server.get_values(*self.variables),
                         [variable.value for variable in self.variables])
        self.assertEqual(5, self.variables[0].value)
        self.assertEqual('m', self.variables[0].units)
        self.assertEqual(10000.0, self.variables[1].value)
        self.assertEqual('g', self.variables[1].units)

    def test_add_variables(self):
        # empty call
        self.variable_server.add_variables()

        # repeated call
        for _ in range(2):
            self.variable_server.add_variables(*self.variables)
            self.assertEqual(self.variables, self.variable_server._variables)

        # bad units
        self.assertRaises(
          UnitsConversionError,
          self.variable_server.add_variables,
          Variable('ball.obj.state.input.mass', units='fjarnskaggl'))

        # bad type
        self.assertRaises(
          ValueError,
          self.variable_server.add_variables,
          Variable('ball.obj.state.input.mass', type_=dict))


    def test_remove_variables(self):
        self.variable_server.add_variables(*self.variables)
        # empty call
        self.variable_server.remove_variables()
        self.assertEqual(self.variables, self.variable_server._variables)
        # repeated call
        for _ in range(2):
            self.variable_server.remove_variables(*self.variables)
            self.assertFalse(self.variable_server._variables)

    def test_remove_all_variables(self):
        self.variable_server.add_variables(*self.variables)
        # repeated call
        for _ in range(2):
            self.variable_server.remove_all_variables()
            self.assertFalse(self.variable_server._variables)

    def test_set_units(self):
        self.variable_server.set_units(self.variables[0], 'cm')

    def test_set_period(self):
        self.variable_server.set_period(10)
        # We would like to verify that VariableServerThread::update_rate
        # was modified, but variable server threads are not registered
        # with the memory manager, so we can't.

    def test_register_callback(self):
        def foo():
            pass
        self.variable_server.register_callback(foo)
        self.assertEqual({foo: ([], {})}, self.variable_server._callbacks)
        args = 1, 2
        kwargs = {'a': 3, 'b': 4}
        self.variable_server.register_callback(foo, args, kwargs)
        self.assertEqual({foo: (args, kwargs)},
                         self.variable_server._callbacks)

    def test_deregister_callback(self):
        def foo():
            pass
        self.variable_server.register_callback(foo)
        # repeated call
        for _ in range(2):
            self.variable_server.deregister_callback(foo)
            self.assertFalse(self.variable_server._callbacks)

    def test_pause(self):
        self.variable_server.pause(True)
        self.variable_server.pause(False)
        # We would like to verify that VariableServerThread::pause_cmd
        # was modified, but variable server threads are not registered
        # with the memory manager, so we can't.

    def test_set_debug(self):
        self.variable_server.set_debug(3)
        # We would like to verify that VariableServerThread::debug
        # was modified, but variable server threads are not registered
        # with the memory manager, so we can't.

    def test_set_tag(self):
        self.variable_server.set_tag('test')
        # We would like to verify that
        # VariableServerThread::connection.client_tag was modified, but
        # variable server threads are not registered with the memory
        # manager, so we can't.

    def test_set_copy_mode(self):
        self.variable_server.set_copy_mode()
        # We would like to verify that VariableServerThread::copy_mode
        # was modified, but variable server threads are not registered
        # with the memory manager, so we can't.

    def test_send_on_copy(self):
        self.variable_server.send_on_copy()
        # We would like to verify that VariableServerThread::write_mode
        # was modified, but variable server threads are not registered
        # with the memory manager, so we can't.

    def test_validate_addresses(self):
        self.variable_server.validate_addresses()
        # We would like to verify that VariableServerThread::validate_addresses
        # was modified, but variable server threads are not registered
        # with the memory manager, so we can't.

    def test_variable_exists(self):
        self.assertTrue(
          self.variable_server.variable_exists(self.variables[0].name))
        self.assertFalse(
          self.variable_server.variable_exists('fjarnskaggl'))

    def test_freeze(self):
        self.variable_server.freeze(True)
        self.variable_server.freeze(False)
        # We would like to verify that Executive::async_freeze_command
        # was modified, but it's **'d out, so we can't.

    def test_enable_real_time(self):
        self.variable_server.enable_real_time(False)
        self.assertTrue(self.variable_server.get_value(
          'trick_real_time.rt_sync.disable_flag', type_=bool))

        self.variable_server.enable_real_time(True)
        self.assertTrue(self.variable_server.get_value(
          'trick_real_time.rt_sync.enable_flag', type_=bool))

# TODO: Get rid of this and use automatic discovery when Trick requires Python 2.7
if __name__ == '__main__':
    unittest.main()
