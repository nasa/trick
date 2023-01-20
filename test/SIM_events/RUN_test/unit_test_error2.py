trick.terminate_on_event_parse_error(True)

# Error in condition
event1 = trick.new_event("event1")
event1.condition(0, "this is a syntax error")
event1.action(0, "print (\"event1\");")
event1.action(1, "event1.activate()")
event1.set_cycle(1.0)
event1.activate() 
trick.add_event(event1)

trick.stop(10)