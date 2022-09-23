
"""
This is the ball sim complete with integration loop converted to python
This input file demonstrates the ability to create an entire simulation in an input file.
Do not try this at home!
"""

import math

# BallForce class flattened to remove input and output subclasses
class BallForce:
    def __init__(self):
        self.origin = [ 0.0 , 2.0 ]
        self.input_force = 8.0
        self.output_force = [ 0.0 , 0.0 ]

# BallState class flattened to remove input and output subclasses
class BallState:
    def __init__(self):
        self.mass = 10.0
        self.speed = 3.5
        self.elevation = 0.785398163
        self.input_position = [ 5.0  , 5.0 ]
        self.output_position = [ 0.0  , 0.0 ]
        self.output_velocity = [ 0.0  , 0.0 ]
        self.output_acceleration = [ 0.0  , 0.0 ]
        self.external_force = [ 0.0  , 0.0 ]

# Ball class includes a BallForce and BallState
class Ball:
    def __init__(self):
        self.state = BallState()
        self.force = BallForce()

    # state_init is an initialization job.  It is translated from the C++ version
    def state_init(self):
        #print "here in state_init"
        self.state.output_position = self.state.input_position
        self.state.output_velocity = [ self.state.speed * math.cos(self.state.elevation) , \
                                       self.state.speed * math.sin( self.state.elevation ) ]
        return(0)

    # force_field is a derivative job.  It is translated from the C++ version
    def force_field(self):
        #print "here in force_field"
        rel_pos = [ self.force.origin[0] - float(self.state.output_position[0]) , \
                    self.force.origin[1] - float(self.state.output_position[1]) ]
        mag = math.sqrt( rel_pos[0] * rel_pos[0] + rel_pos[1] * rel_pos[1] )
        unit = [ rel_pos[0] / mag , rel_pos[1] / mag ]
        self.force.output_force = [ self.force.input_force * unit[0] , self.force.input_force * unit[1] ]
        return(0)

    # state_deriv is a derivative job.  It is translated from the C++ version
    def state_deriv(self):
        #print "here in state_deriv"
        self.state.external_force = self.force.output_force
        self.state.output_acceleration = [ self.state.external_force[0] / self.state.mass , \
                                           self.state.external_force[1] / self.state.mass ]
        return(0)

    # state_integ is an integration job.  It is translated from the C++ version
    def state_integ(self):
        #print "here in state_integ"

        trick.load_indexed_state(0 , self.state.output_position[0])
        trick.load_indexed_state(1 , self.state.output_position[1])
        trick.load_indexed_state(2 , self.state.output_velocity[0])
        trick.load_indexed_state(3 , self.state.output_velocity[1])

        trick.load_indexed_deriv(0 , self.state.output_velocity[0])
        trick.load_indexed_deriv(1 , self.state.output_velocity[1])
        trick.load_indexed_deriv(2 , self.state.output_acceleration[0])
        trick.load_indexed_deriv(3 , self.state.output_acceleration[1])

        trick.integrate()

        self.state.output_position[0] = trick.unload_indexed_state(0)
        self.state.output_position[1] = trick.unload_indexed_state(1)
        self.state.output_velocity[0] = trick.unload_indexed_state(2)
        self.state.output_velocity[1] = trick.unload_indexed_state(3)

        return(trick.get_intermediate_step())

    # state_print is a scheduled job.  It is translated from the C++ version
    def state_print(self):
        #print "time = " , trick.exec_get_sim_time() , " , position = " , self.state.output_position
        trick.message_publish(trick.MSG_NORMAL, "time = " + str(trick.exec_get_sim_time()) + " , position = " + str(self.state.output_position) + "\n")
        return(0)

my_ball = Ball()

# ball_call_function is our glue function from the Trick scheduler to our python code above
# The scheduler calls InputProcessSimObject::call_function which in turn calls this function
# with the job id to run.  It is up to the user to keep these ids in this function and the
# add jobs calls below in sync.
def ball_call_function(id):
    if id == 3: ret = my_ball.state_init()
    if id == 4: ret = my_ball.force_field()
    if id == 5: ret = my_ball.state_deriv()
    if id == 6: ret = my_ball.state_integ()
    if id == 7: ret = my_ball.state_print()
    ball_ips.return_value = ret
    return(0)

# declare a new InputProcessSimObject and use ball_call_function as the glue between C++ and python
ball_ips = trick.InputProcessSimObject("ball_call_function")

# create a new Runge Kutta 2 integrator with 4 states
#my_integ = trick.getIntegrator(trick.Runge_Kutta_2, 4 , 1.0)

# The add job call below requires an "Integrator **".  my_integ is an "Integrator *"
# This wrap_ptr function adds the second pointer
#my_integ_ptr = trick.wrap_ptr(my_integ)

# add the ball jobs to the sim_object
ball_ips.add_job(0,3,"initialization",None,1.0,"ball_ips.state_init")
ball_ips.add_job(0,4,"derivative",None,1.0,"ball_ips.force_field")
ball_ips.add_job(0,5,"derivative",None,1.0,"ball_ips.state_deriv")
ball_ips.add_job(0,6,"integration",None,1.0,"ball_ips.state_integ")
ball_ips.add_job(0,7,"scheduled",None,10.0,"ball_ips.state_print")

trick.exec_add_sim_object(ball_ips, "ball_ips")

# declare a new Integrator loop sim object
my_integ_loop = trick.IntegLoopSimObject(0.01, 0, ball_ips, None) ;
# create a new Runge Kutta 2 integrator with 4 states
my_integ_loop.getIntegrator(trick.Runge_Kutta_2, 4)

# add the ball sim_object to the scheduler
# add the integrator sim_object to the scheduler
trick.exec_add_sim_object(my_integ_loop, "my_integ_loop")
my_integ_loop.integ_sched.rebuild_jobs()

# trick system defaults for this sim
#trick.exec_set_enable_freeze(0)
trick.exec_set_terminate_time(trick.exec_get_sim_time() + 300.0)

# This input file can be sent to the simulation through the variable server.
# If the sim is inputted this way, the sim will be in freeze so these
# jobs must be called because we are past the point where they would have been called.
if trick.exec_get_mode() == trick.Freeze:
    my_ball.state_init()
    my_ball.force_field()
    my_ball.state_deriv()
    trick.exec_run()

