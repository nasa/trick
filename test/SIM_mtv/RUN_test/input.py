import os

import trick

event_a = None
event_b = None


def main():
    trick.var_allow_all_connections()

    global event_a, event_b
    # Fixed port so the test client knows where to connect
    trick.var_server_set_port(40100)

    # Run fast (no real-time) so CI doesn't time out
    trick.exec_set_software_frame(0.1)
    trick.exec_set_terminate_time(30.0)

    # -------------------------------------------------------
    # Event A: one python condition with a comment,
    #          one python action with a comment.
    # These comment strings are the values we verify via
    # mtv_send_event_data() in the test client.
    # -------------------------------------------------------
    event_a = trick.new_event("event_a")
    event_a.condition(0, "True", "cond_comment_0")
    event_a.action(0, "mobj.action_counter+=1", "act_comment_0")
    event_a.set_cycle(0.1)
    event_a.activate()
    trick.add_event(event_a)

    # -------------------------------------------------------
    # Event B: lives at mtv_list index 1, with TWO conditions
    # and TWO actions so condition_list[1] / action_list[1]
    # exist. Index >= 1 is the regression case from issue
    # #2139: MemoryManager's dynamic-array bounds check
    # rejected mtv_list[1] (and any other index >= 1 on a
    # named TMM pointer array), so every event after the
    # first came back as BAD_REF.
    # condition_var() gets no comment, so its comment
    # defaults to the variable name string.
    # -------------------------------------------------------
    event_b = trick.new_event("event_b")
    event_b.condition_var(0, "mobj.fire_condition")
    event_b.condition(1, "False", "cond_b_comment_1")
    event_b.action(0, "mobj.action_counter+=10", "act_b_comment_0")
    event_b.action(1, "mobj.action_counter+=100", "act_b_comment_1")
    event_b.set_cycle(0.1)
    event_b.activate()
    trick.add_event(event_b)

    # Spawn GTest client 1 second after sim start so all
    # initialization (including MTV registration) is complete.
    varServerPort = trick.var_server_get_port()
    test_output = os.getenv("TRICK_HOME") + "/trick_test/SIM_mtv.xml"
    # Run synchronously (no &) so the sim waits for the test client to finish.
    # The test client calls mobj.success()/fail() + trick.stop() before exiting,
    # which causes the sim to exit with the appropriate return code.
    command = (
        'os.system("./models/test_client/test_client '
        + str(varServerPort)
        + " --gtest_output=xml:"
        + test_output
        + '")'
    )
    trick.add_read(1.0, command)


if __name__ == "__main__":
    main()
