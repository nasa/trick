
rebuild_trickify: 
	+trick-ify --trick_home "/home/plherrin/trick" --trickify_make "/home/plherrin/trick/share/trick/makefiles/" --include "/home/plherrin/trick/trick_sims/SIM_trickified_demo/models/Core_Model /home/plherrin/trick/trick_sims/SIM_trickified_demo/models/Support_Code" --s_define "/home/plherrin/trick/trick_sims/SIM_trickified_demo/S_define" --s_define_filter "/home/plherrin/trick/trick_sims/SIM_trickified_demo/models/Support_Code" --build "/home/plherrin/trick/trick_sims/SIM_trickified_demo/trickified" --s_overrides "/home/plherrin/trick/trick_sims/SIM_trickified_demo/S_overrides.mk" -n "Ball_Lib" -b "a"

clean_trickify:
	-rm -rf build .trick
	-rm -f S_source.hh
	-rm -f trickify_obj_list
	-rm -f trickify_src_list
	-rm -f trickify_stage_*
	-rm -f trickify.log
	-rm -f get_vars.mk
	-rm -f var_dump.mk
	-rm -f *_list

spotless_trickify: clean_trickify
	-rm -f python
	-rm -f *.a
	-rm -f *.o
	-rm -f *.so

nuke_trickify: spotless_trickify
	-rm -f S_overrides_trickify.mk
	-rm -f *.mk
