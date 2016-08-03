
#include "DPC/DPC_UnitConvDataStream.hh"

// CONSTRUCTOR
DPC_UnitConvDataStream::DPC_UnitConvDataStream(DataStream* ds, const char *ToUnits, const char *FromUnitsHint ) {
    Unit *to_unit = NULL;
    Unit *from_unit = NULL;
    std::string from_unit_string;

    const char * recorded_units = ds->getUnit().c_str();

    source_ds = ds;

    if ((ToUnits != NULL) && (strcmp(ToUnits,"") != 0) && (strcmp(ToUnits,"--") != 0)) {
        try {
            to_unit = new Unit(ToUnits);
        } catch (Unit::CONVERSION_ERROR) {
            delete to_unit;
            to_unit = NULL;
            std::cerr << "ERROR: Invalid units: \"" << ToUnits << "\"." << std::endl;
        }
    }
    // If the user has specified a units conversion and those units are valid ...
    if ( to_unit != NULL ) {
        // If the recorded data file doesn't contain the units in which the data is recorded ... 
        if ((recorded_units == NULL) || (strcmp(recorded_units,"") == 0)) {
            // If the user didn't give us a hint as to what the units are (using var@from_units) ...
            if ((FromUnitsHint == NULL) || (strcmp(FromUnitsHint,"") == 0)) {
                cf = new UCFn("--", "--", 1.0, 0.0);
                std::cerr << "ERROR: Unable to to perform units conversion"
                          << " because the recorded data doesn't indicate it's"
                          << " units and no @from_units hint is provided."
                          << std::endl;
                std::cerr.flush();
            } else { // the user did give us a hint.
                try { 
                    from_unit = new Unit(FromUnitsHint);
                } catch (Unit::CONVERSION_ERROR) {
                    delete from_unit;
                    from_unit = NULL;
                    std::cerr << "ERROR: Unable to to perform units conversion"
                              << " because the recorded data doesn't indicate it's"
                              << " units and although a @from_units hint is provided ("
                              << "(\"" <<  FromUnitsHint << "\"), they are invalid."
                              << std::endl;
                    std::cerr.flush();
                }
            }
        } else { // the recorded data file does "know" the units in which the data was recorded,
            // so those will be the units that we convert from.
            try { 
                from_unit = new Unit(recorded_units);
            } catch (Unit::CONVERSION_ERROR) {
                delete from_unit;
                from_unit = NULL;
                std::cerr << "ERROR: Unable to to perform units conversion because the"
                          << " units in the data recording file appear to be corrupt."
                          << std::endl;
                std::cerr.flush();
            }
        }

        // If we know what units the data was recorded in ...
        if ( from_unit != NULL ) {

            // Then we should be able to convert to the units requested by the user.
            try {
                cf = from_unit->Conversion_to(to_unit);
            
            } catch (Unit::CONVERSION_ERROR) {
                cf = new UCFn(from_unit_string.c_str(),from_unit_string.c_str(), 1.0, 0.0);
                std::cerr << "ERROR: Unable to convert from \"" << from_unit << "\" to \""
                          << to_unit << "\" because they are incompatible." << std::endl;
                std::cerr.flush();
            }
        } else {
            std::cerr << "ERROR: Unable to perform units conversion becuase the units"
                      << " that the data is recorded in is unknown." << std::endl;
            cf = new UCFn("--", "--", 1.0, 0.0);
        }
    } else { // The user has not specified a units conversion or the units were not valid.
        // If the recorded data file doesn't contain the units in which the data is recorded ... 
        if ((recorded_units == NULL) || (strcmp(recorded_units,"") == 0)) {
            // If the user didn't give us a hint as to what the units are (using var@from_units) ...
            if ((FromUnitsHint == NULL) || (strcmp(FromUnitsHint,"") == 0)) {
                cf = new UCFn("--", "--", 1.0, 0.0);
            } else { // the user did give us a hint.
                cf = new UCFn(FromUnitsHint, FromUnitsHint, 1.0, 0.0);
            }
        } else { // the recorded data file does "know" the units in which the data was recorded,
            cf = new UCFn(recorded_units, recorded_units, 1.0, 0.0);
        }
    }

    if (to_unit) delete to_unit;
    if (from_unit) delete from_unit;

    this->begin();
}

// DESTRUCTOR
DPC_UnitConvDataStream::~DPC_UnitConvDataStream() {

    delete cf;
    delete source_ds;
}

// MEMBER FUNCTION
int DPC_UnitConvDataStream::get(double* timestamp, double* paramValue) {
    double time, value;
    int ret ;

    ret = source_ds->get(&time, &value) ;
    *timestamp  = time;
    *paramValue = cf->eval(value);
    return ret ;
}

// MEMBER FUNCTION
int DPC_UnitConvDataStream::peek(double* timestamp, double* paramValue) {
    double time, value;

    if (! source_ds->peek(&time, &value) ) {
        *timestamp  = time;
        *paramValue = cf->eval(value);
        return (0);
    } else {
        return (-1);
    }
}

// MEMBER FUNCTION
std::string DPC_UnitConvDataStream::getFileName() {
    return( source_ds->getFileName());
}

// MEMBER FUNCTION
std::string DPC_UnitConvDataStream::getUnit() {
        std::string unitstr(cf->t_name);
    return( unitstr);
}

// MEMBER FUNCTION
std::string DPC_UnitConvDataStream::getTimeUnit() {
    return( source_ds->getTimeUnit());
}

// MEMBER FUNCTION
void DPC_UnitConvDataStream::begin() {
    source_ds->begin();
}

// MEMBER FUNCTION
int DPC_UnitConvDataStream::end() {
    return( source_ds->end());
}

// MEMBER FUNCTION
int DPC_UnitConvDataStream::step() {
    return( source_ds->step());
}
