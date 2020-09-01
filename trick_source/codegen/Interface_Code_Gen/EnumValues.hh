
#ifndef ENUMVALUES_HH
#define ENUMVALUES_HH

#include <string>
#include <vector>
#include <utility>

#include "ConstructValues.hh"

/**

  EnumValues holds information describing a class found with ICG.  The
  information includes the name and location of the class, properties of the
  class, and a vector list of the member data contained in the class.

  The class also provides print routines for each function in the io_src.

  @author Alexander S. Lin

  @date July 2012

 */

class EnumValues : public ConstructValues {

    public:

        typedef std::pair< std::string , long long > NameValuePair ;

        EnumValues() ;

        void addEnum(std::string in_name , long long in_val) ;
        void addFullyQualifiedEnum(std::string in_name , long long in_val) ;

        void setHasDefinition( bool in ) ;
        bool getHasDefinition() ;

        const std::vector<NameValuePair>& getPairs() {
            return enum_values;
        }

        const std::vector<NameValuePair>& getFullyQualifiedPairs() {
            return fully_qualified_enum_values;
        }

        friend std::ostream & operator << (std::ostream & os , EnumValues & ev ) ;

    private:

        /** List of enums and their values */
        std::vector< NameValuePair > enum_values ;

        /** List of fully qualified enums and their values
            This is used to generate the S_sie.resource file. */
        std::vector< NameValuePair > fully_qualified_enum_values ;

        bool has_definition ;

} ;

#endif
