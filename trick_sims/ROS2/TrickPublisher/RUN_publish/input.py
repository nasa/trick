trick.real_time_enable()
trick.exec_set_software_frame(0.1)
trick.itimer_enable()

trick.var_allow_all_connections()

simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)

TrickPublisher.publish_node.publicationName = "Trick_says"
