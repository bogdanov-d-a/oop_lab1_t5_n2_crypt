@echo off

set loopIter=1
:LoopStart


if %loopIter%==1 (
	set key=13
)
if %loopIter%==2 (
	set key=75
)

del %loopIter%.tmp >nul 2>&1

Crypt.exe crypt %loopIter%.open %loopIter%.tmp %key%
fc %loopIter%.tmp %loopIter%.crypt

del %loopIter%.tmp

Crypt.exe decrypt %loopIter%.crypt %loopIter%.tmp %key%
fc %loopIter%.tmp %loopIter%.open

del %loopIter%.tmp


if %loopIter% lss 2 (
	set /a loopIter=%loopIter%+1
	goto LoopStart
)

pause
