/*
PURPOSE:
    (Data Record Binary class.)
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
*/

#ifndef DRBINARY_HH
#define DRBINARY_HH

#include <stdio.h>
#include <string>

#include "trick/DataRecordGroup.hh"

#ifdef SWIG
%feature("compactdefaultargs","0") ;
%feature("shadow") Trick::DRBinary::DRBinary(std::string in_name) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}
#endif

namespace Trick {

    /**
      The DRBinary recording format is a Trick simulation specific format.  Files written in this format are named
      log_<group_name>.trk.  The contents of this file type are readable by the Trick Data Products packages from
      Trick 07 to the current version.  The format of the file follows.

      <center>
      <table>
      <tr><th>Value</th><th>Description</th><th>Type</th><th>Bytes</th></tr>
      <tr><td colspan=4 align=center>START OF HEADER</td></tr>
      <tr><td>Trick-\<vv\>-\<e\></td><td>\<vv\> is trick version, 2 characters (e.g. 07)
      \<e\> is endianness, 1 character: L for little endian, B for big endian</td><td>string</td><td>10</td></tr>
      <tr><td>\<numparms\></td><td>Number of parameters recorded</td><td>int</td><td>4</td></tr>
      <tr><td>17</td><td>parameter \#1 Name string length</td><td>int</td><td>4</td></tr>
      <tr><td>sys.exec.out.time</td><td>parameter \#1 Name (1st parameter is always the system time)</td><td>string</td><td>17</td></tr>
      <tr><td>1</td><td>parameter \#1 Units Name string length</td><td>int</td><td>4</td></tr>
      <tr><td>s</td><td>parameter \#1 Units Name</td><td>string</td><td>1</td></tr>
      <tr><td>10</td><td>parameter \#1 Type (see Table 15)</td><td>int</td><td>4</td></tr>
      <tr><td>8</td><td>parameter \#1 Size (number of bytes the value occupies)</td><td>int</td><td>4</td></tr>
      <tr><td>\<namelen\></td><td>parameter \#2 Name string length</td><td>int</td><td>4</td></tr>
      <tr><td>\<name\></td><td>parameter \#2 Name</td><td>string</td><td>\<namelen\></td></tr>
      <tr><td>\<unitlen\></td><td>parameter \#2 Units Name string length</td><td>int</td><td>4</td></tr>
      <tr><td>\<unit\></td><td>parameter \#2 Units Name</td><td>string</td><td>\<unitlen\></td></tr>
      <tr><td>\<type\></td><td>parameter \#2 Type</td><td>int</td><td>4</td></tr>
      <tr><td><size></td><td>parameter \#2 Size</td><td>int</td><td>4</td></tr>
      <tr><td></td><td>.</td><td></td><td></td></tr>
      <tr><td></td><td>.</td><td></td><td></td></tr>
      <tr><td></td><td>.</td><td></td><td></td></tr>
      <tr><td>\<namelen\></td><td>parameter \#n Name string length</td><td>int</td><td>4</td></tr>
      <tr><td>\<name\></td><td>parameter \#n Name</td><td>string</td><td>\<namelen\></td></tr>
      <tr><td>\<unitlen\></td><td>parameter \#n Units Name string length</td><td>int</td><td>4</td></tr>
      <tr><td>\<unit\></td><td>parameter \#n Units Name</td><td>string</td><td>\<unitlen\></td></tr>
      <tr><td>\<type\></td><td>parameter \#n Type</td><td>int</td><td>4</td></tr>
      <tr><td><size></td><td>parameter \#n Size</td><td>int</td><td>4</td></tr>
      <tr><td colspan=4 align=center>END OF HEADER, START OF RECORDED DATA</td></tr>
      <tr><td>\<value\></td><td>parameter \#1 Value</td><td>10</td><td>8</td></tr>
      <tr><td>\<value\></td><td>parameter \#2 Value</td><td>\<type\></td><td>\<size\></td></tr>
      <tr><td></td><td>.</td><td></td><td></td></tr>
      <tr><td></td><td>.</td><td></td><td></td></tr>
      <tr><td></td><td>.</td><td></td><td></td></tr>
      <tr><td>\<value\></td><td>parameter \#n Value</td><td>\<type\></td><td>\<size\></td></tr>
      <tr><td colspan=4 align=center>REPEAT RECORDED DATA FOR EACH CYCLE</td></tr>
      <tr><td colspan=4 align=center>END OF RECORDED DATA</td></tr>
      </table>
      <b>Binary Data Format</b>
      </center>

      See Trick::MemoryManager::TRICK_TYPE for a definition of the Trick data <type> values used in the above table.
    */
    class DRBinary : public Trick::DataRecordGroup {

        public:

            #ifndef SWIG
            /**
             @brief DRBinary default constructor.
             */
            DRBinary() {}
            #endif
            ~DRBinary() {}

            /**
             @brief @userdesc Create a new Binary data recording group.
             @par Python Usage:
             @code <my_drg> = trick.DRBinary("<in_name>") @endcode
             @copydoc Trick::DataRecordGroup::DataRecordGroup(string in_name)
             */
            DRBinary( std::string in_name, bool register_group = true ) ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_header
             */
            virtual int format_specific_header(std::fstream & outstream) ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_init
             */
            virtual int format_specific_init() ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_write_data
             */
            virtual int format_specific_write_data(unsigned int writer_offset) ;

            /**
             @copybrief Trick::DataRecordGroup::shutdown
             */
            virtual int format_specific_shutdown() ;

        private:
            /** The log file.\n */
            int fd ;             /**< trick_io(**) trick_units(--) */

    } ;

} ;

#ifdef SWIG
%feature("compactdefaultargs","1") ;
#endif

#endif
