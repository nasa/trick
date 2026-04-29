exec(open("RUN_test/common.py").read())

# Add 100, 200, 400 and 500 Hz integration rates
dyn_integloop.add_rate(1.0/100)
dyn_integloop.add_rate(1.0/200)
dyn_integloop.add_rate(1.0/400)
dyn_integloop.add_rate(1.0/500)


# Add data logging rates to match the integ rates
drg[DR_GROUP_ID].add_cycle(1.0/100)
drg[DR_GROUP_ID].add_cycle(1.0/200)
drg[DR_GROUP_ID].add_cycle(1.0/400)
drg[DR_GROUP_ID].add_cycle(1.0/500)
