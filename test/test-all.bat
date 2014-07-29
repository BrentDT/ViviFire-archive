@Echo Off
SetLocal

Set fails=0
If Exist test.log Del test.log

For /R %%f In (*.in.txt) Do Call :Test "%%f"

Del test.tmp
Echo %fails% failed. >> test.log
GoTo :EOF

:Test
Set f=%~1
..\VF -v0 %1 > test.tmp
FC test.tmp "%f:~0,-6%v0.txt" > NUL
If ErrorLevel 1 (
  Echo %f% failed. >> test.log
  Set /A fails+=1
)
GoTo :EOF
