setlocal enabledelayedexpansion
@echo off
call .\EmbPythonSwig.bat Base
call .\EmbPythonSwig.bat Foundation
call .\EmbPythonSwig.bat FileIO Foundation
call .\EmbPythonSwig.bat Collision Foundation
call .\EmbPythonSwig.bat Physics Collision Foundation
call .\EmbPythonSwig.bat Graphics Physics Foundation
call .\EmbPythonSwig.bat Creature Graphics Physics Foundation
call .\EmbPythonSwig.bat HumanInterface Graphics Foundation
call .\EmbPythonSwig.bat Framework FileIO HumanInterface Graphics Physics Foundation

exit 0
