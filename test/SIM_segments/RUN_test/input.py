
def main():

    drg0 = trick.DRAscii("Ball")
    drg0.set_cycle(0.1)
    drg0.freq = trick.DR_Always
    drg0.thisown = 0
    trick.add_data_record_group(drg0, trick.DR_Buffer)

    # set the recording group to record only in segment "A"
    drg0.add_tag("A")

    # This call will set the segment when the top_of_frame jobs are executed the first time.
    trick.exec_segment_set("A")
    trick.exec_segment_switch()

    # Note: calling exec_segment_switch in the input file may be the norm.
    # If so, segment switching and single stepping through jobs from the sim_control panel
    # are not compatible. We get into a situation where we have the
    # mutex to the input processor locked and stepping jobs need the i.p. to step.
    # Since single stepping jobs is rare, I'm leaving the "feature" there because it
    # would be hard to fix.

    # These segment switches happen immediately
    trick.add_read(1.0 ,"""trick.exec_segment_set("B") ; trick.exec_segment_switch()""")
    trick.add_read(2.0 ,"""trick.exec_segment_set("A") ; trick.exec_segment_switch()""")

    # This segment switch happens at the top of the next software frame.
    trick.add_read(3.0 ,"""trick.exec_segment_set("None")""")

    # This is a bad segment name test
    trick.add_read(3.1 ,"""trick.exec_segment_set("C")""")

    trick.exec_set_software_frame(0.01)

    #trick.real_time_enable()
    #trick.freeze()
    #trick.sim_control_panel_set_enabled(True)

    trick.stop(3.5)

if __name__ == "__main__":
    main()


