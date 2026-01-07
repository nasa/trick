
rebuild_trickify: 
	+trick-ify --trick_home "../../../" --trickify_make "../../../share/trick/makefiles/" --include "../models/Core_Model ../models/Support_Code" --s_define "../S_define" --s_define_filter "../models/Support_Code" --s_overrides "../S_overrides.mk" -n "Ball_Lib" -b "a"

spotless_trickify:
	-rm -rf build .trick
	-rm -f S_source.hh
	-rm -f trickify_obj_list
	-rm -f trickify_src_list
	-rm -f trickify_stage_*
	-rm -f trickify.log
	-rm -f get_vars.mk
	-rm -f var_dump.mk
	-rm -f *_list

apocalypse_trickify: spotless_trickify
	-rm -f python
	-rm -f *.a
	-rm -f *.o
	-rm -f *.so

judgement_trickify: apocalypse_trickify
	-rm -f S_overrides_trickify.mk
	-rm -f *.mk
