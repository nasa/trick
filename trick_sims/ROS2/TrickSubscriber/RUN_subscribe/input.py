trick.real_time_enable()
trick.exec_set_software_frame(0.1)
trick.itimer_enable()

simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)

#TrickSubscriber.subscribe_node.subscriptionName = "topic"
TrickSubscriber.subscribe_node.subscriptionName = "chatter"
