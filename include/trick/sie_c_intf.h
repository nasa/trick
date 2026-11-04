#ifndef SIE_C_INTF_H
#define SIE_C_INTF_H

#ifdef __cplusplus
extern "C" {
#endif

    void sie_print_xml(void);
    void sie_class_attr_map_print_xml(void);
    void sie_enum_attr_map_print_xml(void);
    void sie_top_level_objects_print_xml(void);
    void sie_append_runtime_objs(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <string>
std::string sie_get_runtime_sie_dir(void);
#endif

#endif // SIE_C_INTF_H
