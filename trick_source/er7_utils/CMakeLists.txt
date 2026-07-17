# liber7_utils.a. Source list cross-checked against a real `make` build's
# lib/liber7_utils.a member set (verified 2026-07-06) — matches exactly.
# io_src objects come from TrickICG.cmake's ER7_IO_SOURCES partition (see
# cmake/scripts/GenerateIOSrcList.cmake).
set( ER7_UTILS_SRC
  integration/abm4/src/abm4_first_order_ode_integrator.cc
  integration/abm4/src/abm4_integrator_constructor.cc
  integration/abm4/src/abm4_second_order_ode_integrator.cc
  integration/beeman/src/beeman_integrator_constructor.cc
  integration/beeman/src/beeman_second_order_ode_integrator.cc
  integration/core/src/base_integration_group.cc
  integration/core/src/bogus_integration_controls.cc
  integration/core/src/first_order_ode_integrator.cc
  integration/core/src/integration_controls.cc
  integration/core/src/integration_messages.cc
  integration/core/src/integrator_constructor.cc
  integration/core/src/integrator_constructor_factory.cc
  integration/core/src/integrator_result_merger.cc
  integration/core/src/integrator_result_merger_container.cc
  integration/core/src/left_quaternion_functions.cc
  integration/core/src/priming_first_order_ode_integrator.cc
  integration/core/src/priming_integration_controls.cc
  integration/core/src/priming_integrator_constructor.cc
  integration/core/src/priming_second_order_ode_integrator.cc
  integration/core/src/second_order_ode_integrator.cc
  integration/core/src/single_cycle_integration_controls.cc
  integration/core/src/standard_integration_controls.cc
  integration/euler/src/euler_first_order_ode_integrator.cc
  integration/euler/src/euler_integrator_constructor.cc
  integration/euler/src/euler_second_order_ode_integrator.cc
  integration/mm4/src/mm4_integrator_constructor.cc
  integration/mm4/src/mm4_second_order_ode_integrator.cc
  integration/nl2/src/nl2_integrator_constructor.cc
  integration/nl2/src/nl2_second_order_ode_integrator.cc
  integration/position_verlet/src/position_verlet_integrator_constructor.cc
  integration/position_verlet/src/position_verlet_second_order_ode_integrator.cc
  integration/rk2_heun/src/rk2_heun_first_order_ode_integrator.cc
  integration/rk2_heun/src/rk2_heun_integrator_constructor.cc
  integration/rk2_heun/src/rk2_heun_second_order_ode_integrator.cc
  integration/rk2_midpoint/src/rk2_midpoint_first_order_ode_integrator.cc
  integration/rk2_midpoint/src/rk2_midpoint_integrator_constructor.cc
  integration/rk2_midpoint/src/rk2_midpoint_second_order_ode_integrator.cc
  integration/rk4/src/rk4_first_order_ode_integrator.cc
  integration/rk4/src/rk4_integrator_constructor.cc
  integration/rk4/src/rk4_second_order_ode_integrator.cc
  integration/rk4/src/rk4_second_order_ode_integrator_base.cc
  integration/rkf45/src/rkf45_butcher_tableau.cc
  integration/rkf45/src/rkf45_first_order_ode_integrator.cc
  integration/rkf45/src/rkf45_integrator_constructor.cc
  integration/rkf45/src/rkf45_second_order_ode_integrator.cc
  integration/rkf78/src/rkf78_butcher_tableau.cc
  integration/rkf78/src/rkf78_first_order_ode_integrator.cc
  integration/rkf78/src/rkf78_integrator_constructor.cc
  integration/rkf78/src/rkf78_second_order_ode_integrator.cc
  integration/rkg4/src/rkg4_butcher_tableau.cc
  integration/rkg4/src/rkg4_first_order_ode_integrator.cc
  integration/rkg4/src/rkg4_integrator_constructor.cc
  integration/rkg4/src/rkg4_second_order_ode_integrator.cc
  integration/rkn4/src/rkn4_integrator_constructor.cc
  integration/rkn4/src/rkn4_second_order_ode_integrator.cc
  integration/symplectic_euler/src/symplectic_euler_integrator_constructor.cc
  integration/symplectic_euler/src/symplectic_euler_second_order_ode_integrator.cc
  integration/velocity_verlet/src/velocity_verlet_integrator_constructor.cc
  integration/velocity_verlet/src/velocity_verlet_second_order_ode_integrator.cc
  interface/src/alloc.cc
  interface/src/deletable.cc
  interface/src/message_handler.cc
  math/src/n_choose_m.cc
  math/src/ratio128.cc
  math/src/uint128.cc
  trick/integration/src/trick_integrator.cc
)

add_library( er7_utils STATIC ${ER7_UTILS_SRC} ${ER7_IO_SOURCES} )
target_link_libraries( er7_utils PUBLIC trick_build_flags )
target_compile_options( er7_utils PRIVATE -Wno-unused-parameter )
set_target_properties( er7_utils PROPERTIES ARCHIVE_OUTPUT_NAME er7_utils)
add_dependencies( er7_utils trick_io_src_gen )

