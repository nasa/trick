set TRICK_HOME=%~dp0..
java -cp %TRICK_HOME%\dist\*;%TRICK_HOME%\lib\* trick.simcontrol.SimControlApplication %~1 %~2