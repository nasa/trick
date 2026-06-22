import trick

def main():
    trick.exec_set_job_onoff("the_object.stlc.addData", 1, True)
    trick.exec_set_job_onoff("the_object.stlc.test",    1, False)
    trick.exec_set_job_onoff("the_object.stlc.print",   1, False)

    # ---------------------------------------------------------------
    # ASCII data recording group for STL primitive containers
    # ---------------------------------------------------------------
    drg = trick.DRAscii("STL_DR")

    # std::vector<double> — expected values: 4.0, 5.0, 6.0
    drg.add_variable("the_object.stlc.double_vector[0]")
    drg.add_variable("the_object.stlc.double_vector[1]")
    drg.add_variable("the_object.stlc.double_vector[2]")

    # std::deque<float> — expected values: 98.7, 65.4, 32.1
    drg.add_variable("the_object.stlc.float_deque[0]")
    drg.add_variable("the_object.stlc.float_deque[1]")
    drg.add_variable("the_object.stlc.float_deque[2]")

    # std::array<int,4> — expected values: 10, 20, 30, 40
    drg.add_variable("the_object.stlc.int_array[0]")
    drg.add_variable("the_object.stlc.int_array[1]")
    drg.add_variable("the_object.stlc.int_array[2]")
    drg.add_variable("the_object.stlc.int_array[3]")

    # STL-element struct member: vec_user_simple is std::vector<UserClass>
    # vec_user_simple[i].a[j] = i+j, vec_user_simple[i].b = 8888888888
    drg.add_variable("the_object.stlc.vec_user_simple[0].b")
    drg.add_variable("the_object.stlc.vec_user_simple[0].a[0]")
    drg.add_variable("the_object.stlc.vec_user_simple[0].a[2]")

    # STL-element struct member: vec_user_defined is std::vector<SimpleWrapper>
    # vec_user_defined[i].a = 888, vec_user_defined[i].vec[j] = i+j
    drg.add_variable("the_object.stlc.vec_user_defined[0].a")

    # Nested STL: outer vector element's inner vector element
    # vec_user_defined[0].vec = {0, 1, 2, ..., 9}
    drg.add_variable("the_object.stlc.vec_user_defined[0].vec[0]")
    drg.add_variable("the_object.stlc.vec_user_defined[0].vec[2]")

    drg.set_cycle(0.1)
    drg.freq = trick.DR_Always
    drg.thisown = 0
    trick.add_data_record_group(drg, trick.DR_Buffer)

    # ---------------------------------------------------------------
    # Binary data recording group — same variables
    # ---------------------------------------------------------------
    drg_bin = trick.DRBinary("STL_DR_bin")
    drg_bin.add_variable("the_object.stlc.double_vector[0]")
    drg_bin.add_variable("the_object.stlc.double_vector[1]")
    drg_bin.add_variable("the_object.stlc.double_vector[2]")
    drg_bin.add_variable("the_object.stlc.float_deque[0]")
    drg_bin.add_variable("the_object.stlc.float_deque[1]")
    drg_bin.add_variable("the_object.stlc.float_deque[2]")
    drg_bin.add_variable("the_object.stlc.int_array[0]")
    drg_bin.add_variable("the_object.stlc.int_array[1]")
    drg_bin.add_variable("the_object.stlc.int_array[2]")
    drg_bin.add_variable("the_object.stlc.int_array[3]")
    drg_bin.add_variable("the_object.stlc.vec_user_simple[0].b")
    drg_bin.add_variable("the_object.stlc.vec_user_simple[0].a[0]")
    drg_bin.add_variable("the_object.stlc.vec_user_simple[0].a[2]")
    drg_bin.add_variable("the_object.stlc.vec_user_defined[0].a")
    drg_bin.add_variable("the_object.stlc.vec_user_defined[0].vec[0]")
    drg_bin.add_variable("the_object.stlc.vec_user_defined[0].vec[2]")
    drg_bin.set_cycle(0.1)
    drg_bin.freq = trick.DR_Always
    drg_bin.thisown = 0
    trick.add_data_record_group(drg_bin, trick.DR_Buffer)

    # ---------------------------------------------------------------
    # HDF5 data recording group — same variables
    # ---------------------------------------------------------------
    drg_hdf5 = trick.DRHDF5("STL_DR_hdf5")
    drg_hdf5.add_variable("the_object.stlc.double_vector[0]")
    drg_hdf5.add_variable("the_object.stlc.double_vector[1]")
    drg_hdf5.add_variable("the_object.stlc.double_vector[2]")
    drg_hdf5.add_variable("the_object.stlc.float_deque[0]")
    drg_hdf5.add_variable("the_object.stlc.float_deque[1]")
    drg_hdf5.add_variable("the_object.stlc.float_deque[2]")
    drg_hdf5.add_variable("the_object.stlc.int_array[0]")
    drg_hdf5.add_variable("the_object.stlc.int_array[1]")
    drg_hdf5.add_variable("the_object.stlc.int_array[2]")
    drg_hdf5.add_variable("the_object.stlc.int_array[3]")
    drg_hdf5.add_variable("the_object.stlc.vec_user_simple[0].b")
    drg_hdf5.add_variable("the_object.stlc.vec_user_simple[0].a[0]")
    drg_hdf5.add_variable("the_object.stlc.vec_user_simple[0].a[2]")
    drg_hdf5.add_variable("the_object.stlc.vec_user_defined[0].a")
    drg_hdf5.add_variable("the_object.stlc.vec_user_defined[0].vec[0]")
    drg_hdf5.add_variable("the_object.stlc.vec_user_defined[0].vec[2]")
    drg_hdf5.set_cycle(0.1)
    drg_hdf5.freq = trick.DR_Always
    drg_hdf5.thisown = 0
    trick.add_data_record_group(drg_hdf5, trick.DR_Buffer)

    trick.exec_set_freeze_frame(0.10)
    trick.stop(1.0)

if __name__ == "__main__":
    main()
