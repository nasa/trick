#ifndef MM_TEST_HH
#define MM_TEST_HH

#include "trick/MemoryManager.hh"

const char* ISO_6429_Restore_Default   = "\x1b[00m";
const char* ISO_6429_Bold              = "\x1b[01m";
const char* ISO_6429_Underline         = "\x1b[04m";
const char* ISO_6429_Black_Foreground  = "\x1b[30m";
const char* ISO_6429_Red_Foreground    = "\x1b[31m";
const char* ISO_6429_Green_Foreground  = "\x1b[32m";
const char* ISO_6429_Yellow_Foreground = "\x1b[33m";
const char* ISO_6429_Blue_Foreground   = "\x1b[34m";
const char* ISO_6429_Purple_Foreground = "\x1b[35m";
const char* ISO_6429_Cyan_Foreground   = "\x1b[36m";
const char* ISO_6429_White_Foreground  = "\x1b[37m";
const char* ISO_6429_Black_Background  = "\x1b[40m";
const char* ISO_6429_Red_Background    = "\x1b[41m";
const char* ISO_6429_Green_Background  = "\x1b[42m";
const char* ISO_6429_Yellow_Background = "\x1b[43m";
const char* ISO_6429_Blue_Background   = "\x1b[44m";
const char* ISO_6429_Purple_Background = "\x1b[45m";
const char* ISO_6429_Cyan_Background   = "\x1b[46m";
const char* ISO_6429_White_Background  = "\x1b[47m";

/*
   This template function validates requirements of any given ALLOC_INFO record.
 */
template <typename T>
void validate_alloc_info(ALLOC_INFO* alloc_info,
                         T *test_var,
                         TRICK_TYPE trick_type,
                         const char *type_name,
                         const char *var_name,
                         int num,
                         int num_index,
                         int *extent ) {

        int ii;

        ASSERT_TRUE(alloc_info != NULL) << "No alloc_info record found.";

        EXPECT_EQ( alloc_info->start, (void*)test_var) <<
                "alloc_info->start should always be equal to the variable pointer." ;

        ASSERT_EQ( alloc_info->num, num);
        EXPECT_EQ( alloc_info->end,   (void*)((char*)test_var + num*sizeof(T) - 1));
        EXPECT_STREQ(var_name, alloc_info->name);
        EXPECT_EQ( (size_t)alloc_info->size, sizeof(T));

        EXPECT_EQ( alloc_info->type, trick_type);

        if ((trick_type == TRICK_STRUCTURED) || (trick_type == TRICK_ENUMERATED)) {

            EXPECT_TRUE( alloc_info->attr != NULL) <<
                "If alloc_info->type is TRICK_STRUCTURED or TRICK_ENUMERATED then" <<
                " alloc_info->attr should not be NULL.";
            EXPECT_TRUE( alloc_info->user_type_name != NULL) <<
                "If alloc_info->type is TRICK_STRUCTURED or TRICK_ENUMERATED then" <<
                " alloc_info->user_type_name should not be NULL.";
            EXPECT_STREQ( alloc_info->user_type_name, type_name) <<
                "If alloc_info->type is TRICK_STRUCTURED or TRICK_ENUMERATED then" <<
                "alloc_info->user_type_name should be equal to the name of the user defined type.";

        } else {

            EXPECT_TRUE( alloc_info->attr == NULL) <<
                "If alloc_info->type is not TRICK_STRUCTURED nor TRICK_ENUMERATED then" <<
                " alloc_info->attr should be NULL.";
            EXPECT_TRUE( alloc_info->user_type_name == NULL) <<
                "If alloc_info->type is not TRICK_STRUCTURED nor TRICK_ENUMERATED then" <<
                " user_type_name should be NULL.";
        }

        EXPECT_EQ( alloc_info->num_index, num_index);
        for (ii=0 ; ii<num_index ; ++ii) {
            EXPECT_EQ( alloc_info->index[ii], extent[ii]);
        }

}

template <typename T>
void validate_alloc_info(Trick::MemoryManager *memmgr,
                T *test_var,
                TRICK_TYPE trick_type,
                const char *type_name,
                const char *var_name,
                int num,
                int num_index,
                int *extent ) {

        ALLOC_INFO* alloc_info = NULL;
        alloc_info = memmgr->get_alloc_info_of(test_var);

        validate_alloc_info(alloc_info, test_var, trick_type, type_name, var_name, num, num_index, extent);

}
#endif
