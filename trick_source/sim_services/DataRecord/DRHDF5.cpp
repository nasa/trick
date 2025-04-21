/*
PURPOSE:
    (Data record to disk in HDF5 format.)
PROGRAMMERS:
     ((Warwick Woodard) (NASA) (February 2010) (--) (Initial version))
*/

#include <cstring>
#include <iostream>
#include <stdlib.h>

#include "trick/DRHDF5.hh"
#include "trick/parameter_types.h"
#include "trick/command_line_protos.h"
#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h"
#include "trick/bitfield_proto.h"

Trick::DRHDF5::DRHDF5( std::string in_name, Trick::DR_Type dr_type ) : Trick::DataRecordGroup(in_name, dr_type) {
    register_group_with_mm(this, "Trick::DRHDF5") ;
}

int Trick::DRHDF5::format_specific_header( std::fstream & out_stream ) {
    out_stream << " byte_order is HDF5" << std::endl ;
    return(0) ;
}

/**
@details
-# Set the file extension to ".h5"
-# Open the log file
-# Create the root directory in the HDF5 file
-# For each variable to be recorded
   -# Create a fixed length packet table
   -# Associate the packet table with the temporary memory buffer storing the simulation data
-# Declare the recording group to the memory manager so that the group can be checkpointed
   and restored.
*/
int Trick::DRHDF5::format_specific_init() {

#ifdef HDF5
    unsigned int ii ;
    hsize_t chunk_size = 1024;
    hid_t byte_id ;
    hid_t file_names_id, param_types_id, param_units_id, param_names_id ;
    hid_t datatype ;
    hid_t s256 ;
    std::string buf;

    file_name.append(".h5") ;

    s256 = H5Tcopy(H5T_C_S1);
    H5Tset_size(s256, 256);

    // Create a new HDF5 file with the specified name; overwrite if it exists.
    if ((file = H5Fcreate(file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) {
        message_publish(MSG_ERROR, "Can't open Data Record file %s.\n", file_name.c_str()) ;
        record = false ;
        return -1 ;
    }

    // Check file validity first
    if (H5Iis_valid(file) <= 0) {
        message_publish(MSG_ERROR, "File handle invalid, id=%lld\n", (long long)file);
        return -1;
    }
    // All HDF5 objects live in the top-level "/" (root) group.
    root_group = H5Gopen(file, "/", H5P_DEFAULT);

    // Create a new group named "header" at the root ("/") level.
    header_group = H5Gcreate(file, "/header", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    // Validate header group
    if (H5Iis_valid(header_group) <= 0) {
        message_publish(MSG_ERROR, "Header group invalid, id=%lld\n", (long long)header_group);
        return -1;
    }
    // Create a packet table (PT) that stores byte order.
    byte_id = H5PTcreate_fl(header_group, "byte_order", s256, chunk_size, 1) ;
    // Add the byte order value to the byte packet table.
    H5PTappend( byte_id, 1, byte_order.c_str() );
    // Create a packet table (PT) that stores each parameter's file location.
    file_names_id = H5PTcreate_fl(header_group, "file_names", s256, chunk_size, 1) ;
    // Create a packet table (PT) that stores each parameter's type.
    param_types_id = H5PTcreate_fl(header_group, "param_types", s256, chunk_size, 1) ;
    // Create a packet table (PT) that stores each parameter's unit.
    param_units_id = H5PTcreate_fl(header_group, "param_units", s256, chunk_size, 1) ;
    // Create a packet table (PT) that stores each parameter's name.
    param_names_id =  H5PTcreate_fl(header_group, "param_names", s256, chunk_size, 1) ;

    // Allocate memory for the parameter names
    param_names = new char*[rec_buffer.size()];
    // Allocate memory for the dataset ids
    param_dataset_ids = new hid_t[rec_buffer.size()];

    // Create a table for each requested parameter.
    for (ii = 0; ii < rec_buffer.size(); ii++) {

        /* Case statements taken from "parameter_types.h."
         *  HDF5 Native types found in "H5Tpublic.h." */
        switch (rec_buffer[ii]->ref->attr->type) {
            case TRICK_CHARACTER:
                datatype = H5T_NATIVE_CHAR;
                break;
            case TRICK_UNSIGNED_CHARACTER:
                datatype = H5T_NATIVE_UCHAR;
                break;
            case TRICK_STRING:
                datatype = s256;
                break;
            case TRICK_SHORT:
                datatype = H5T_NATIVE_SHORT;
                break;
            case TRICK_UNSIGNED_SHORT:
                datatype = H5T_NATIVE_USHORT;
                break;
            case TRICK_ENUMERATED:
            case TRICK_INTEGER:
                datatype = H5T_NATIVE_INT;
                break;
            case TRICK_UNSIGNED_INTEGER:
                datatype = H5T_NATIVE_UINT;
                break;
            case TRICK_LONG:
                datatype = H5T_NATIVE_LONG;
                break;
            case TRICK_UNSIGNED_LONG:
                datatype = H5T_NATIVE_ULONG;
                break;
            case TRICK_FLOAT:
                datatype = H5T_NATIVE_FLOAT;
                break;
            case TRICK_DOUBLE:
                datatype = H5T_NATIVE_DOUBLE;
                break;
            case TRICK_BITFIELD:
                if (rec_buffer[ii]->ref->attr->size == sizeof(int)) {
                    datatype = H5T_NATIVE_INT;
                } else if (rec_buffer[ii]->ref->attr->size == sizeof(short)) {
                    datatype = H5T_NATIVE_SHORT;
                } else {
                    datatype = H5T_NATIVE_CHAR;
                }
                break;
            case TRICK_UNSIGNED_BITFIELD:
                if (rec_buffer[ii]->ref->attr->size == sizeof(unsigned int)) {
                    datatype = H5T_NATIVE_UINT;
                } else if (rec_buffer[ii]->ref->attr->size == sizeof(unsigned short)) {
                    datatype = H5T_NATIVE_USHORT;
                } else {
                    datatype = H5T_NATIVE_UCHAR;
                }
                break;
            case TRICK_LONG_LONG:
                datatype = H5T_NATIVE_LLONG;
                break;
            case TRICK_UNSIGNED_LONG_LONG:
                datatype = H5T_NATIVE_ULLONG;
                break;
            case TRICK_BOOLEAN:
#if ( __sun | __APPLE__ )
                datatype = H5T_NATIVE_INT;
#else
                datatype = H5T_NATIVE_UCHAR;
#endif
                break;
            default:
                continue;
        }

        /* Create packet table(s) to store "fixed-length" packets.
         *  A separate packet table (PT) is created for each variable.
         * PARAMETERS:
         *     IN: Identifier of the file or group to create the table within.
         *     IN: The name of the dataset (i.e. variable).
         *     IN: The datatype of a packet.
         *     IN: The packet table uses HDF5 chunked storage to allow it to
         *         grow. This value allows the user to set the size of a chunk.
         *         The chunk size affects performance.
         *     IN: Compression level, a value of 0 through 9. Level 0 is faster
         *         but offers the least compression; level 9 is slower but
         *         offers maximum compression. A setting of -1 indicates that
         *         no compression is desired.
         * RETURN:
         *     Returns an identifier for the new packet table, or H5I_BADID on error.
         */
        // Allocate memory for the parameter names
        param_names[ii] = (char *)malloc(strlen(rec_buffer[ii]->ref->reference) + 1);
        // Copy the parameter name to the list
        strcpy(param_names[ii], rec_buffer[ii]->ref->reference);
        // Create a packet table for each parameter
        param_dataset_ids[ii] = H5PTcreate_fl(root_group, param_names[ii], datatype, chunk_size, 1) ;
        // Validate the dataset
        if ( param_dataset_ids[ii] == H5I_BADID ) {
            message_publish(MSG_ERROR, "An error occured in data record group \"%s\" when adding \"%s\".\n",
             group_name.c_str() , param_names[ii]) ;
            continue;
        }

        // As a bonus, add a header entry for each parameter.
        /* File Name */
        buf = "log_" + group_name ;
        H5PTappend( file_names_id, 1, buf.c_str() );
        /* Param Type */
        buf = type_string(rec_buffer[ii]->ref->attr->type, rec_buffer[ii]->ref->attr->size );
        H5PTappend( param_types_id, 1, buf.c_str() );
        /* Param Units */
        if ( rec_buffer[ii]->ref->attr->mods & TRICK_MODS_UNITSDASHDASH ) {
            H5PTappend( param_units_id, 1, "--" );
        } else {
            H5PTappend( param_units_id, 1, rec_buffer[ii]->ref->attr->units );
        }
        /* Param Name */
        H5PTappend( param_names_id, 1, rec_buffer[ii]->ref->reference );

    }
    H5PTclose( byte_id );
    H5PTclose( file_names_id );
    H5PTclose( param_types_id );
    H5PTclose( param_units_id );
    H5PTclose( param_names_id );
    H5Gclose( header_group );
#endif

    return(0);
}

#ifdef HDF5
/**
 * Helper function to append specified data records for one variable to its dataset(packet table).
 */
void append_var_packet_table(Trick::DataRecordBuffer *drb, char* buf, size_t records, hid_t param_ds) {
    // Data records to be appended to the packet table
    void* data = 0;
    int bf;

    switch (drb->ref->attr->type) {
        case TRICK_CHARACTER:
        case TRICK_UNSIGNED_CHARACTER:
        case TRICK_STRING:
        case TRICK_SHORT:
        case TRICK_UNSIGNED_SHORT:
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
        case TRICK_UNSIGNED_INTEGER:
        case TRICK_LONG:
        case TRICK_UNSIGNED_LONG:
        case TRICK_FLOAT:
        case TRICK_DOUBLE:
            H5PTappend(param_ds, records , buf);
            break;
        case TRICK_BITFIELD:
            bf = GET_BITFIELD(buf, drb->ref->attr->size, drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
            data = malloc(records * sizeof(bf));
            
            // Extract bitfield for each record from different segments of buf
            for (size_t j = 0; j < records; j++) {               
                // Calculate the correct offset in buf for each record
                // Each record in buf has size of rec_buffer[ii]->ref->attr->size
                size_t offset = j * drb->ref->attr->size;

                if (drb->ref->attr->size == sizeof(int)) {
                    ((int *)data)[j] = extract_bitfield_any(
                                                    *(int *)(buf+offset), drb->ref->attr->size,
                                                    drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
                } else if (drb->ref->attr->size == sizeof(short)) {
                    ((short *)data)[j] = extract_bitfield_any(
                                                    *(short *)(buf+offset), drb->ref->attr->size,
                                                    drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
                } else if (drb->ref->attr->size == sizeof(char)) {
                    ((char *)data)[j] = extract_bitfield_any(
                                                    *(char *)(buf+offset), drb->ref->attr->size,
                                                    drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
                } else {
                    ((int*)data)[j] = extract_bitfield_any(
                                                    *(int *)(buf+offset), drb->ref->attr->size,
                                                    drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
                }
            }
            H5PTappend(param_ds, records, data);
            break;
        case TRICK_UNSIGNED_BITFIELD:
            bf = GET_UNSIGNED_BITFIELD(buf, drb->ref->attr->size, drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
            data = malloc(records * sizeof(bf));

            // Extract bitfield for each record from different segments of buf
            for (size_t j = 0; j < records; j++) {               
                // Calculate the correct offset in buf for each record
                // Each record in buf has size of rec_buffer[ii]->ref->attr->size
                size_t offset = j * drb->ref->attr->size;  // record_size would be the size of one record in buf

                if (drb->ref->attr->size == sizeof(int)) {
                    ((unsigned int *)data)[j] = extract_unsigned_bitfield_any(
                                                    *(unsigned int *)(buf+offset), drb->ref->attr->size,
                                                    drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
                } else if (drb->ref->attr->size == sizeof(short)) {
                    ((unsigned short *)data)[j] = extract_unsigned_bitfield_any(
                                                    *(unsigned short *)(buf+offset), drb->ref->attr->size,
                                                    drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
                } else if (drb->ref->attr->size == sizeof(char)) {
                    ((unsigned char *)data)[j] = extract_unsigned_bitfield_any(
                                                    *(unsigned char *)(buf+offset), drb->ref->attr->size,
                                                    drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
                } else {
                    ((int *)data)[j] = extract_unsigned_bitfield_any(
                                                    *(int *)(buf+offset), drb->ref->attr->size,
                                                    drb->ref->attr->index[0].start, drb->ref->attr->index[0].size);
                }
            }
            H5PTappend(param_ds, records, data);
            break;
        case TRICK_LONG_LONG:
        case TRICK_UNSIGNED_LONG_LONG:
        case TRICK_BOOLEAN:
        default:
            H5PTappend(param_ds, records , buf);
            break;

        if (data != 0) {
            free(data);
            data = 0;
        }
    }
}
#endif

/*
   HDF5 logging is done on a per variable basis instead of per time step like the
   other recording methods.  This write_data routine overrides the default in
   DataRecordGroup.  This routine writes out all of the buffered data of a variable
   in one or two HDF5 calls.
*/
int Trick::DRHDF5::write_data(bool must_write) {

#ifdef HDF5
    unsigned int local_buffer_num ;
    unsigned int num_to_write ;
    unsigned int ii;
    char *buf = 0;
    size_t ds_records1;
    size_t ds_records2;

    if ( record and inited and (buffer_type == DR_No_Buffer or must_write)) {

        // buffer_mutex is used in this one place to prevent forced calls of write_data
        // to not overwrite data being written by the asynchronous thread.
        pthread_mutex_lock(&buffer_mutex) ;
        local_buffer_num = buffer_num ;
        if ( (local_buffer_num - writer_num) > max_num ) {
            num_to_write = max_num ;
        } else {
            num_to_write = (local_buffer_num - writer_num) ;
        }
        writer_num = local_buffer_num - num_to_write ;

        if ( writer_num != local_buffer_num ) {
            unsigned int writer_offset = writer_num % max_num ;
            // Test if the writer pointer to the right of the buffer pointer in the ring
            if ( (writer_num % max_num) > (local_buffer_num % max_num) ) {
                ds_records1 = max_num - writer_offset;
                ds_records2 = local_buffer_num % max_num;

                // we have 2 segments to write per variable
                for (ii = 0; ii < rec_buffer.size(); ii++) {
                    //unsigned int writer_offset = writer_num % max_num ;
                    buf = rec_buffer[ii]->buffer + (writer_offset * rec_buffer[ii]->ref->attr->size) ;
                    append_var_packet_table(rec_buffer[ii], buf, ds_records1, param_dataset_ids[ii]);

                    buf = rec_buffer[ii]->buffer ;
                    append_var_packet_table(rec_buffer[ii], buf, ds_records2, param_dataset_ids[ii]);
                }
            }  else {
                ds_records1 = local_buffer_num - writer_num;
                // we have 1 continous segment to write per variable
                for (ii = 0; ii < rec_buffer.size(); ii++) {
                    //unsigned int writer_offset = writer_num % max_num ;
                    buf = rec_buffer[ii]->buffer + (writer_offset * rec_buffer[ii]->ref->attr->size) ;
                    append_var_packet_table(rec_buffer[ii], buf, ds_records1, param_dataset_ids[ii]);
               }
            }
            writer_num = local_buffer_num ;
        }
        pthread_mutex_unlock(&buffer_mutex) ;

    }
#else
    (void)must_write;
#endif
    return 0 ;
}

/**
@details
-# Snapshot the index of the most recent temporary memory buffer to write to disk to curr_buffer_num
-# For each parameter to be recorded
   -# Point a pointer to the beginning of the data in the memory buffer to be written to disk
   -# Append one packet to the packet table.
*/
int Trick::DRHDF5::format_specific_write_data(unsigned int writer_offset __attribute__((unused))) {

#ifdef HDF5
    unsigned int ii;
    char *buf = 0;

    /* Loop through each parameter. */
    for (ii = 0; ii < rec_buffer.size(); ii++) {

        /* Each parameters[] element contains a DataRecordBuffer class.
         * So there is a seperate DataRecordBuffer per variable.
         * Point to the value to be recorded. */
        buf = rec_buffer[ii]->buffer + (writer_offset * rec_buffer[ii]->ref->attr->size) ;
        append_var_packet_table(rec_buffer[ii], buf, 1, param_dataset_ids[ii]);

    }
#endif

    return(0);
}

/**
@details
-# For each parameter being recorded
   -# Close the HDF5 packet table
-# Close the HDF5 root
-# Close the HDF5 file
*/
int Trick::DRHDF5::format_specific_shutdown() {

#ifdef HDF5
    unsigned int ii ;

    if ( inited ) {
        for (ii = 0; ii < rec_buffer.size(); ii++) {
            // Free parameter names memory
            free(param_names[ii]);
            // Close the parameter dataset
            if (param_dataset_ids[ii] != H5I_BADID) {
                H5PTclose(param_dataset_ids[ii]);
            }
        }
        // Free the parameter names array
        delete[] param_names;
        // Set the pointer to NULL
        param_names = nullptr;
        // Free the dataset ids array
        delete[] param_dataset_ids;
        // Set the pointer to NULL
        param_dataset_ids = nullptr;

        // Close root group
        H5Gclose(root_group);
        // Close file handle
        H5Fclose(file);

    }
#endif
    return(0);
}

