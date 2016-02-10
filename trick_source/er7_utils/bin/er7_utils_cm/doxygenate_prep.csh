#!/bin/tcsh -f

set install_target = $cwd/doxygen
if (! -d $install_target) then
   echo "Missing installation target $install_target"
   exit 1
endif

set er7_scripts = $ER7_UTILS_HOME/er7_utils/bin/er7_utils_cm
set pp = "$er7_scripts/er7_utils_pp -mod -dox -prefix $install_target/er7_utils"

chdir $ER7_UTILS_HOME/er7_utils

set skip = '/(verif|\.svn|unit_test|data)/'

set files = ()
foreach dir (integration interface jeod trick)
   set dirfiles = `find $dir -name '*.[ch][ch]' | grep -v -E $skip`
   set files = ($files $dirfiles)
end

if (-d $install_target/er7_utils) then
   rm -rf $install_target/er7_utils
endif
mkdir $install_target/er7_utils

set missing = ()
foreach file ($files)
   (perl $pp $file >> $install_target/doxygen.out) \
      >>& $install_target/doxygen.err
   if (! -f $install_target/er7_utils/$file) then
      set missing = ($missing $file)
   endif
end

if ($#missing > 0) then
   printf "\nCould not process the following ER7 numerical utilities files:\n" \
      >> $install_target/doxygen.err
   foreach file ($missing)
      printf "$file\n" >> $install_target/doxygen.err
   end
endif

cp docs/doxygen/groups.txt $install_target/er7_utils

printf "End of doxygen processing of ER7 numerical utilities\n\n" \
   >> $install_target/doxygen.err
