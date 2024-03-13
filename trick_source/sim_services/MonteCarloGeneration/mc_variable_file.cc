/*******************************TRICK HEADER******************************
PURPOSE: ( Implementation of a file-lookup  assignment

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2019) (Antares) (Initial)))
  (((Isaac Reaves) (NASA) (November 2022) (Integration into Trick Core)))
**********************************************************************/

#include <algorithm> // all_of
#include <sstream>   // istringstream
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/mc_variable_file.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
MonteCarloVariableFile::MonteCarloVariableFile(
    const std::string & var_name,
    const std::string & filename_,
    size_t column_number_,
    size_t first_column_number_)
  :
  MonteCarloVariable(var_name),
  max_skip(0),
  is_dependent(false),
  rand_gen(0),
  filename(filename_),
  column_number(column_number_),
  first_column_number(first_column_number_),
  dependents(),
  file()
{
  // make this a dependent of itself so that when it reads the data file, it
  // populates its own "assignment" variable.
  dependents.push_back(this);
  type = MonteCarloVariable::Prescribed;
}

/*****************************************************************************
initialize_file
Purpose:(Opens the file identified by filename as an ifstream)
*****************************************************************************/
void
MonteCarloVariableFile::initialize_file()
{
  // At this time, the list of dependencies has been finalized.  We can sort
  // this list by column number of each of the dependencies, in increasing
  // order.
  if (dependents.size() > 1) {
    dependents.sort(sort_by_col_num);
  }
  // Check that the specified first_column_number is no larger than the
  // smallest column number:
  MonteCarloVariableFile * first_var =dependents.front();
  if (first_column_number > first_var->get_column_number()) {
    std::string message = 
      std::string("File: ") + __FILE__ + ", Line: " + 
      std::to_string(__LINE__) + ",  Configuration Error\nIn configuring " +
      "the file for variable " + first_var->get_variable_name().c_str() + 
      ", it was identified that\nit was specified to draw data from column " +
      std::to_string(first_var->get_column_number()) + ", but that the " +
      "first\ncolumn was identified as having index " + 
      std::to_string(first_column_number) + ".\n";
    message_publish(MSG_ERROR, message.c_str());
    exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
  }

  // Now we can get to reading the file.
  file.open(filename);

  if (file.fail()) {
    std::string message = 
      std::string("File: ") + __FILE__ + ",  Line: " + 
      std::to_string(__LINE__) + ", I/O error\nUnable to open file " +
      filename.c_str() + " for reading.\nRequired for variable " +
      variable_name.c_str() + ".\n";
    message_publish(MSG_ERROR, message.c_str());
    exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
  }
  // Sanity check -- make sure the file has at least 1 line of data:
  std::string line;
  do {
    // if reached the end of the file, not found anything good.  Fail out.
    if (file.eof()) {
      std::string message = 
        std::string("File: ") + __FILE__ + ", Line: " + 
        std::to_string(__LINE__) + " Invalid data file\nData file " + 
        filename.c_str() + " contains no recognized lines of data\n" +
        "Required for variable " + variable_name.c_str() + ".\n";
      message_publish(MSG_ERROR, message.c_str());
      exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
    }
    std::getline( file, line);
    // keep looking if the line is empty, starts with a "#" character or
    // "/" character or is completely whitespace.
  } while (line.empty() ||
           line.front() == '#' ||
           line.front() == '/' ||
           std::all_of( line.begin(), line.end(), isspace));
  // Rewind the file
  file.seekg(0, file.beg);

}

/*****************************************************************************
generate_assignment
Purpose:(generates the command line that is to be embedded in the monte-input
 file currently being generated.)
*****************************************************************************/
void
MonteCarloVariableFile::generate_assignment()
{
  // if this instance is not dependent on another, need to read the file.
  if (!is_dependent) {
    process_line(); // provides "assignment"
  }
  generate_command();
  insert_units();
}


/*****************************************************************************
register_dependent
Purpose:(Registers another MonteCarloVariableFile instance with this one,
allowing this instance to read the data for the other.)
*****************************************************************************/
void
MonteCarloVariableFile::register_dependent(
    MonteCarloVariableFile * new_var)
{
  if (new_var == NULL) {
    std::string message = 
      std::string("File: ") + __FILE__ + ", line " + 
      std::to_string(__LINE__) + ",  Invalid call\nAttempted to register " +
      "a dependent identified with NULL pointer with \nthe " + 
      "MonteCarloVariableFile for variable " + variable_name.c_str() + 
      ".\nThis is not a valid action.\nRegistration failed, exiting " +
      "without action.\n";
      message_publish(MSG_ERROR, message.c_str());
    return;
  }
  if (new_var->has_dependents()) {
    std::string message = 
      std::string("File: ") + __FILE__ + ", Line: " +
      std::to_string(__LINE__) + ", Invalid configuration\nError in " +
      "attempting to make " + new_var->get_variable_name().c_str() + 
      " be dependent on " + variable_name.c_str() + ".\n" + 
      new_var->get_variable_name().c_str() + " cannot be marked as " + 
      "dependent when it has dependencies of its own.\nThe dependency " +
      "hierarchy can only be one level deep.";
    message_publish(MSG_ERROR, message.c_str());
    exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
  }
  if (new_var->max_skip != max_skip) {
    std::string message = 
      std::string("File: ") + __FILE__ + ", Line: " + 
      std::to_string(__LINE__) + ", Invalid configuration\nIt is not " +
      "permissible for two variables looking at the same file to\noperate " +
      "under different line-selection criteria.\n" + 
      new_var->get_variable_name().c_str() + "\nwill be switched to the " +
      "behavior of\n" + variable_name.c_str() + ",\nwhich " +
      "as a setting for the maximum number of lines to skip of " + 
      std::to_string(max_skip) + "\n";
    message_publish(MSG_ERROR, message.c_str());
  }
  dependents.push_back(new_var);
  new_var->is_dependent = true;
}


/*****************************************************************************
process_line
Purpose:(extract and process a line of data from the file, breaking it
         into words and extracting the approriate word number.)
*****************************************************************************/
void
MonteCarloVariableFile::process_line()
{
  size_t skip_count = 0;
  if (max_skip > 0) {
    std::uniform_int_distribution<int> skip_distrib(0,max_skip);
    skip_count = skip_distrib( rand_gen);
  }

  std::string line;

  for (size_t ii = 0; ii <= skip_count; ++ii) {
    // keep reading the next line until a "good" line is found
    do {
      // read the next line
      std::getline( file, line);

      // if reached the end of the file, clear the error flag and go back
      // to the beginning.
      if (file.eof()) {
        file.clear();
        file.seekg(0, file.beg);
      }
      // keep looking if the line is empty, starts with a "#" character or
      // "/" character or is completely whitespace.
    } while ( line.empty() ||
              line.front() == '#' ||
              line.front() == '/' ||
              std::all_of( line.begin(), line.end(), isspace));

    // A good line was found; return to the top of the for loop to see if
    // we have skipped over enough good lines yet.
  }

  // Have the line containing the data.  Need to assign some subset of the
  // words in this line to a set of variables, knowing the column number for
  // each variable.
  // Capture the first word, and associate it with the user-specified
  // column number of the first word.
  std::istringstream word(line);
  std::string scratch_assignment;
  word >> scratch_assignment;
  size_t current_column_number = first_column_number;

  // Now for each dependent (including itself) find and assign the appropriate
  // word
  for (auto it = dependents.begin(); it != dependents.end(); ++it) {
    // The dependents have already been sorted according to their column
    // number, so the next column we need data from is the column number of
    // this next variable.
    size_t next_column_needed = (*it)->get_column_number();

    // This next-needed column can be no earlier than
    // the current column, but it could be the current column -- could be
    // the first column, or we could have multiple variables collecting data
    // from the same column.  So check whether we need to advance the
    // string-stream.
    while (next_column_needed > current_column_number && word) {
      // for as long as the next needed column is to the right of where we
      // currently are, and there is another word to the right, extract
      // the next word and advance the current column number.
      word >> scratch_assignment;
      current_column_number++;
    }
    // There are two ways to get past the while loop -- we ran out of
    // words, or the current column number reached the target next-needed
    // column number (including the case where it was already there).
    // If we ran out of words before reaching a specified column, we have
    // a problem
    if (current_column_number < next_column_needed) {
      std::string message =
        std::string("File: ") + __FILE__ + ", Line: " + 
        std::to_string(__LINE__) + ", Malformed data file\nData file " +
        "for variable " +  variable_name.c_str() +  " includes this line:\n" + 
        line.c_str() + "\nWhich has only " + 
        std::to_string(current_column_number-1) + " values.\nVariable " +
        variable_name.c_str() + "  uses the value from position " +
        std::to_string(column_number) + 
        ", which does not exist in this line\n";
      exec_terminate_with_return(1, __FILE__, __LINE__, message.c_str());
    }
    // and if we found the desired column, send its value to the variable:
    (*it)->assignment = scratch_assignment;
  }
}

/*****************************************************************************
sort_by_col_num
Purpose:(sorts the dependent list by column number
*****************************************************************************/
bool MonteCarloVariableFile::sort_by_col_num(
       MonteCarloVariableFile * left,
       MonteCarloVariableFile * right)
{
  return left->get_column_number() < right->get_column_number();
}

/*****************************************************************************
summarize_variable
Purpose:(Provide a string summarizing the attributes of this MonteCarloVariable)
*****************************************************************************/
std::string MonteCarloVariableFile::summarize_variable() const
{
  std::ostringstream ss;
  ss << MonteCarloVariable::summarize_variable()
     << ", max_skip=" << max_skip << ", is_dependent=" << is_dependent
     << ", filename=" << filename << ", column_number=" << column_number
     << ", first_column_number=" << first_column_number;
  return (ss.str());
}
