/*
    PURPOSE: (ExternalApplication C interface.)
*/

#ifndef EXTERNAL_APPLICATION_C_INTF_H
#define EXTERNAL_APPLICATION_C_INTF_H

#ifdef __cplusplus
extern "C" {
#endif

// Convenience functions for backwards compatibility that, dolphins willing, will one day be removed.

void sim_control_panel_set_enabled(int enabled);
int sim_control_panel_get_enabled();
void sim_control_panel_auto_exit_set_enabled(int enabled);
void trick_view_set_enabled(int enabled);
int trick_view_get_enabled();
void malfunctions_trick_view_set_enabled(int enabled);
int malfunctions_trick_view_get_enabled();
void monte_monitor_set_enabled(int enabled);
int monte_monitor_get_enabled();
void stripchart_set_enabled(int enabled);
int stripchart_get_enabled();
void sim_control_panel_set_startup_command(const char *command);
const char *sim_control_panel_get_startup_command();
void trick_view_set_startup_command(const char *command);
const char *trick_view_get_startup_command();
void malfunctions_trick_view_set_startup_command(const char *command);
const char *malfunctions_trick_view_get_startup_command();
void monte_monitor_set_startup_command(const char *command);
const char *monte_monitor_get_startup_command();
void stripchart_set_startup_command(const char *command);
const char *stripchart_get_startup_command();
void sim_control_panel_launch();
void trick_view_launch();
void malfunctions_trick_view_launch();
void monte_monitor_launch();
void stripchart_launch();
void trick_view_set_cycle_period(double period);
void trick_view_add_auto_load_file(const char *file);

#ifdef __cplusplus
}
#endif
#endif
