set PATHORG=%PATH%
set PATH=%SPRINGHEAD2%\BIN;%SPRINGHEAD2%\BIN\Graphviz\bin;%PATH%
doxygen springhead.doxy 2> springhead.log
set PATH=%PATHORG%
:END
