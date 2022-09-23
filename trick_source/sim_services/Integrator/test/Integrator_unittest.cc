
#include <gtest/gtest.h>

// We need access to protected stuff for unit tests!
#define protected public

#include "trick/MemoryManager.hh"
#include "trick/Integrator.hh"
#include "trick/IntegLoopScheduler.hh"
#include "trick/Executive.hh"
#include "trick/exec_proto.h"
#include "trick/exec_proto.hh"
#include "trick/SimObject.hh"
//#include "trick/RequirementScribe.hh"
#include <math.h>
#include <iostream>

#define PI 3.141592653589793
#define RAD_PER_DEG (2.0*PI/180.0)

class testSimObject : public Trick::SimObject {
    public:

    int derivative() {
        return 1;
    }

    int integration() {
        return 2;
    }

    int pre_integration() {
        return 3;
    }

    int post_integration() {
        return 4;
    }

    double dynamic_event() {
        return 1.1;
    }
    
    testSimObject() {
        int ii = 0;
        add_job(0, ii++, "derivative", NULL, 1, "derivative", "TRK") ; 
        add_job(0, ii++, "integration", NULL, 1, "integration", "TRK") ; 
        add_job(0, ii++, "dynamic_event", NULL, 1, "dynamic_event", "TRK") ; 
        add_job(0, ii++, "pre_integration", NULL, 1, "pre_integration", "TRK") ; 
        add_job(0, ii++, "post_integration", NULL, 1, "post_integration", "TRK") ; 
    }

    virtual int call_function(Trick::JobData* curr_job);
    virtual double call_function_double( Trick::JobData* curr_job);
}; 

int testSimObject::call_function( Trick::JobData *curr_job) {
    int trick_ret = 0;
    switch (curr_job->id) {
        case 0:
            trick_ret = derivative();
        break;
        case 1:
            trick_ret = integration();
        break;
        case 3:
            trick_ret = pre_integration();
        break;
        case 4:
            trick_ret = post_integration();
        break;
        default:
            trick_ret = -1;
    }        
    return trick_ret ;
}

double testSimObject::call_function_double( Trick::JobData* curr_job) {
    double trick_ret = 0.0;
    switch (curr_job->id) {
        case 3:
            trick_ret = dynamic_event() ;
        break;
        default:
            trick_ret = -1;
    }
    return trick_ret ;
}

class IntegratorTest : public ::testing::Test {
    protected:
    Trick::MemoryManager *memmgr;
    //Trick::RequirementScribe req;
    IntegratorTest() { memmgr = new Trick::MemoryManager;}
    ~IntegratorTest() { delete memmgr;   }
    virtual void SetUp() {} 
    virtual void TearDown() {}

};

class IntegratorLoopTest : public ::testing::Test {
    protected:
    Trick::Executive exec;
    Trick::IntegLoopScheduler* IntegLoop ;
    testSimObject uno; 
    Trick::MemoryManager *memmgr;
    //Trick::RequirementScribe req;
    IntegratorLoopTest() { memmgr = new Trick::MemoryManager;  IntegLoop = new Trick::IntegLoopScheduler(0.01, &uno) ;}
    ~IntegratorLoopTest() { delete memmgr; delete IntegLoop;   }
    virtual void SetUp() {} 
    virtual void TearDown() {}

    Trick::ScheduledJobQueue* get_derivative_queue() { return &IntegLoop->deriv_jobs ; }
    Trick::ScheduledJobQueue* get_integration_queue() { return &IntegLoop->integ_jobs ; }
    Trick::ScheduledJobQueue* get_dynamic_event_queue() { return &IntegLoop->dynamic_event_jobs ; }
    Trick::ScheduledJobQueue* get_post_integration_queue() { return &IntegLoop->post_integ_jobs ; }
    Trick::ScheduledJobQueue* get_pre_integration_queue() { return &IntegLoop->pre_integ_jobs ; }
};

TEST_F(IntegratorLoopTest, Job_Queue_Init) {

    //req.add_requirement("");
    Trick::JobData* curr_job;

    exec_add_sim_object(&uno, "uno");
    IntegLoop->add_integ_jobs_from_sim_object(&uno);

    curr_job = get_derivative_queue()->get_next_job();
    ASSERT_FALSE( curr_job == NULL);
    EXPECT_STREQ( curr_job->name.c_str(), "uno.derivative");

    curr_job = get_integration_queue()->get_next_job();
    ASSERT_FALSE( curr_job == NULL);
    EXPECT_STREQ( curr_job->name.c_str(), "uno.integration");

    curr_job = get_post_integration_queue()->get_next_job();
    ASSERT_FALSE( curr_job == NULL);
    EXPECT_STREQ( curr_job->name.c_str(), "uno.post_integration");

    curr_job = get_pre_integration_queue()->get_next_job();
    ASSERT_FALSE( curr_job == NULL);
    EXPECT_STREQ( curr_job->name.c_str(), "uno.pre_integration");

    curr_job = get_dynamic_event_queue()->get_next_job();
    ASSERT_FALSE( curr_job == NULL);
    EXPECT_STREQ( curr_job->name.c_str(), "uno.dynamic_event");

    curr_job = get_dynamic_event_queue()->get_next_job();
    ASSERT_TRUE( curr_job == NULL);
}

typedef struct {
    double pos[2];
    double vel[2];
    double acc[2];
} BALL;


void deriv(BALL *B) {
    B->acc[0] = -9.81;
    B->acc[1] = 0;
}

void init(BALL *B) {
   
    const double initial_speed = 50.0 ;
    const double initial_angle = 30.0 ;
 
    B->pos[0] = 0.0;
    B->pos[1] = 0.0;
    B->vel[0] = initial_speed * sin(initial_angle * RAD_PER_DEG);
    B->vel[1] = initial_speed * cos(initial_angle * RAD_PER_DEG);
}

BALL Ball_sim( Trick::Integrator *integrator) {

    BALL   ball;
    long   tick = 0;
    double sim_time;

    // Initialization
    init(&ball);

    // Simulation Loop
    do { 
        sim_time = tick * integrator->dt ;
        // Integration Loop
        do { 
            integrator->time = sim_time;
            deriv( &ball);
            integrator->state_in( &ball.pos[0], &ball.pos[1], &ball.vel[0], &ball.vel[1], NULL);
            integrator->deriv_in( &ball.vel[0], &ball.vel[1], &ball.acc[0], &ball.acc[1], NULL);
            integrator->integrate();
            integrator->state_out( &ball.pos[0], &ball.pos[1], &ball.vel[0], &ball.vel[1], NULL);
        } while ( integrator->intermediate_step);
        tick++;
    } while (ball.pos[0] >= 0.0);
    return ball;
}

BALL Ball_eulercromer_sim( Trick::Euler_Cromer_Integrator *integrator ) {

    BALL   ball;
    long   tick = 0;
    double sim_time;

    // Initialization
    init(&ball);

    // Simulation Loop
    do { 
        sim_time = tick * integrator->dt ;
        // Integration Loop
        do {
            integrator->time = sim_time;
            deriv( &ball);
            integrator->state_in( &ball.pos[0], &ball.pos[1], NULL);
            integrator->deriv_in( &ball.vel[0], &ball.vel[1], NULL);
            integrator->deriv2_in( &ball.acc[0], &ball.acc[1], NULL);
            integrator->integrate();
            integrator->state_out( &ball.pos[0], &ball.pos[1], &ball.vel[0], &ball.vel[1], NULL);
        } while ( integrator->intermediate_step);
        tick++;
    } while (ball.pos[0] >= 0.0);
    return ball;
}

void verify_ball_sim_results(BALL *ball, 
                             const double tolerance, 
                             const double pos0, 
                             const double pos1, 
                             const double vel0,
                             const double vel1) 
{
    EXPECT_NEAR( ball->pos[0], pos0, tolerance) ;
    EXPECT_NEAR( ball->pos[1], pos1, tolerance);
    EXPECT_NEAR( ball->vel[0], vel0, tolerance);
    EXPECT_NEAR( ball->vel[1], vel1, tolerance);
}

TEST_F(IntegratorTest, Ball_Euler) {

    //req.add_requirement("2402458648");

    Trick::Integrator *integrator = Trick::getIntegrator( Euler, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0001, -0.087238729171133289, 221.000000, -43.419129, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 0);

    memmgr->delete_var( integrator);
}
  
TEST_F(IntegratorTest, Ball_Euler_Cromer) {

    //req.add_requirement("215930604");

    Trick::Euler_Cromer_Integrator *integrator = 
           (Trick::Euler_Cromer_Integrator *) Trick::getIntegrator( Euler_Cromer, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_eulercromer_sim( integrator);
    verify_ball_sim_results(&ball, 0.0001, -0.087238729171133289, 220.500, -43.222929, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 1);

    memmgr->delete_var( integrator);
}
#if 0  
TEST_F(IntegratorTest, Ball_Nystrom_Lear_2) {
    
    req.add_requirement("2963568341");

    Trick::Integrator *integrator = Trick::getIntegrator( Nystrom_Lear_2, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0001, -0.087238729171133289, 220.75, -43.321029, 25.000000) ;

    integrator->set_first_step_deriv(1);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->get_first_step_deriv(), 1);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 2);

    memmgr->delete_var( integrator);
}
#endif

TEST_F(IntegratorTest, Ball_Runge_Kutta_2) {

    //req.add_requirement("2472195139");

    Trick::Integrator *integrator = Trick::getIntegrator( Runge_Kutta_2, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0000000001, -0.087238729171133289, 220.75, -43.321029810778249, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 3);

    memmgr->delete_var( integrator);
}

TEST_F(IntegratorTest, Ball_Modified_Midpoint_4) {

    //req.add_requirement("2825965576");

    Trick::Integrator *integrator = Trick::getIntegrator( Modified_Midpoint_4, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0000000001, -0.087238729171133289, 220.75, -43.321029810778249, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 4);

    memmgr->delete_var( integrator);
}

TEST_F(IntegratorTest, Ball_Runge_Kutta_4) {

    //req.add_requirement("2472195141");

    Trick::Integrator *integrator = Trick::getIntegrator( Runge_Kutta_4, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0000000001, -0.087238729171133289, 220.75, -43.321029810778249, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 5);

    memmgr->delete_var( integrator);
}

TEST_F(IntegratorTest, Ball_Runge_Kutta_Gill_4) {

    //req.add_requirement("3039955373");

    Trick::Integrator *integrator = Trick::getIntegrator( Runge_Kutta_Gill_4, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0000000001, -0.087238729171133289, 220.75, -43.321029810778249, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 6);

    memmgr->delete_var( integrator);
}

TEST_F(IntegratorTest, Ball_Runge_Kutta_Fehlberg_45) {

    //req.add_requirement("674248205");

    Trick::Integrator *integrator = Trick::getIntegrator( Runge_Kutta_Fehlberg_45, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0000000001, -0.087238729171133289, 220.75, -43.321029810778249, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 7);

    memmgr->delete_var( integrator);
}

TEST_F(IntegratorTest, Ball_Runge_Kutta_Fehlberg_78) {

    //req.add_requirement("674248511");

    Trick::Integrator *integrator = Trick::getIntegrator( Runge_Kutta_Fehlberg_78, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0000000001, -0.087238729171133289, 220.75, -43.321029810778249, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 8);

    memmgr->delete_var( integrator);
}

#if 0
TEST_F(IntegratorTest, Ball_ABM) {

    //req.add_requirement("3643754476");

    Trick::Integrator *integrator = Trick::getIntegrator( ABM_Method, 4, 0.01);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,4); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.01);

    BALL ball = Ball_sim( integrator);
    verify_ball_sim_results(&ball, 0.0001, -0.087238729171133289, 220.75, -43.321029, 25.000000) ;

    integrator->set_first_step_deriv(0);
    EXPECT_EQ(integrator->first_step_deriv,0);
    EXPECT_EQ(integrator->get_first_step_deriv(), 0);
    integrator->set_last_step_deriv(1);
    EXPECT_EQ(integrator->last_step_deriv,1);
    EXPECT_EQ(integrator->get_last_step_deriv(), 1);

    EXPECT_EQ(integrator->get_Integrator_type(), 9);

    memmgr->delete_var( integrator);
}
#endif

namespace Trick {
    class Donna_Integrator : public Integrator {
        public:
            Donna_Integrator() {};
            Donna_Integrator( int State_size, double Dt) {
                initialize(State_size, Dt);
            }

            void initialize(int State_size, double Dt) {
                dt = Dt;
                num_state = State_size;
            }

            int integrate() { return 1;}
    };
}

TEST_F(IntegratorTest, Ball_UserDefined) {

    //req.add_requirement("3491920037");

    Trick::Donna_Integrator *integrator = INTEG_NEW(Trick::Donna_Integrator);
    integrator = new (integrator) Trick::Donna_Integrator(0, 0.1);
    ASSERT_TRUE( (void*)integrator != NULL);
    EXPECT_EQ(integrator->num_state,0); 
    EXPECT_EQ(integrator->intermediate_step,0);
    EXPECT_EQ(integrator->first_step_deriv,1);
    EXPECT_EQ(integrator->last_step_deriv,0);
    EXPECT_EQ(integrator->dt,0.1);

    EXPECT_EQ(integrator->get_Integrator_type(), 10);
}
