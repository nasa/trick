#!/bin/tcsh -f

set er7_docs = $ER7_UTILS_HOME/er7_utils/docs

chdir $ER7_UTILS_HOME

if (-d html) rm -rf html
if (-d doxygen) rm -rf doxygen
mkdir doxygen

set doxygen_version = `doxygen --version | cut -d'.' -f -3`
if (-f $er7_docs/doxygen/html_config.dox.$doxygen_version) then
   set doxygen_config = $er7_docs/doxygen/html_config.dox.$doxygen_version
else
   echo "Configuration file for doxygen verion $doxygen_version not found"
   exit 1
endif

(doxygen $doxygen_config >> doxygen/doxygen.out) >>& doxygen/doxygen.err

mkdir html
mv doxygen/html html/er7_utils

