import trick

def setup_checkpoint_event(file_name="chkpnt_in"):
    chkpnt_event = trick.new_event()
    chkpnt_event.condition(0, "True")
    action_index = 0
    if trick.exec_get_mode() == 5:
        chkpnt_event.action(action_index, 'trick.freeze()')
        action_index += 1
    chkpnt_event.action(action_index, f'trick.checkpoint("{file_name}")')
    chkpnt_event.activate()

    trick.add_event_after(chkpnt_event, "the_object.stlc.addData")

def main():
    trick.exec_set_job_onoff("the_object.stlc.addData", 1, True)
    trick.exec_set_job_onoff("the_object.stlc.test", 1, False)
    trick.exec_set_job_onoff("the_object.stlc.print", 1, False)

    # trick.add_read( 0.5, 'trick.checkpoint("chkpnt_in")')
    setup_checkpoint_event()

    trick.exec_set_freeze_frame(0.10)
    trick.stop(1.0)

if __name__ == "__main__":
    main()

