
drg0 = trick.DRBinary("dyn") 
drg0.set_freq(trick.DR_Always)
drg0.enable()
drg0.set_cycle(0.1)

drg0.add_variable("dyn.contact.balls[0][0].vecPos[0]", "ball[0].pos[0]")
drg0.add_variable("dyn.contact.balls[0][0].vecPos[1]", "ball[0].pos[1]")
drg0.add_variable("dyn.contact.balls[0][0].vecVel[0]", "ball[0].vel[0]")
drg0.add_variable("dyn.contact.balls[0][0].vecVel[1]", "ball[0].vel[1]")
                                                 
drg0.add_variable("dyn.contact.balls[1][0].vecPos[0]", "ball[1].pos[0]")
drg0.add_variable("dyn.contact.balls[1][0].vecPos[1]", "ball[1].pos[1]")
drg0.add_variable("dyn.contact.balls[1][0].vecVel[0]", "ball[1].vel[0]")
drg0.add_variable("dyn.contact.balls[1][0].vecVel[1]", "ball[1].vel[1]")
                                                 
drg0.add_variable("dyn.contact.balls[2][0].vecPos[0]", "ball[2].pos[0]")
drg0.add_variable("dyn.contact.balls[2][0].vecPos[1]", "ball[2].pos[1]")
drg0.add_variable("dyn.contact.balls[2][0].vecVel[0]", "ball[2].vel[0]")
drg0.add_variable("dyn.contact.balls[2][0].vecVel[1]", "ball[2].vel[1]")
                                                 
drg0.add_variable("dyn.contact.balls[3][0].vecPos[0]", "ball[3].pos[0]")
drg0.add_variable("dyn.contact.balls[3][0].vecPos[1]", "ball[3].pos[1]")
drg0.add_variable("dyn.contact.balls[3][0].vecVel[0]", "ball[3].vel[0]")
drg0.add_variable("dyn.contact.balls[3][0].vecVel[1]", "ball[3].vel[1]")
                                                 
drg0.add_variable("dyn.contact.balls[4][0].vecPos[0]", "ball[4].pos[0]")
drg0.add_variable("dyn.contact.balls[4][0].vecPos[1]", "ball[4].pos[1]")
drg0.add_variable("dyn.contact.balls[4][0].vecVel[0]", "ball[4].vel[0]")
drg0.add_variable("dyn.contact.balls[4][0].vecVel[1]", "ball[4].vel[1]")

trick.add_data_record_group( drg0 )

