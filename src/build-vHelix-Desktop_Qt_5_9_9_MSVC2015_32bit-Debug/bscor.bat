@echo off
set arg=%1
REM for /F %%i in ("%arg%") do set file=%%~ni
set file=%~dpn1
echo %file%
set scale=1
if "%2"=="" (
set scale=1 
) else (
set scale=%2
)
if exist %arg% (
@echo on
echo *****************Processing file %file% **************************
echo ***********Converting file %file%.ply to %file%.dimacs ...********
ply_to_dimacs %file%.ply %file%.dimacs
echo *****************Done converting!*********************************
echo ***********Converting file %file%.ply to embedding %file%.vcode ...********
ply_to_embedding.exe %file%.ply %file%.vcode
echo *****************Done converting!*********************************
echo ******Adding multiedges to %file%.dimacs to make it Eulerian...***
postman_tour %file%.dimacs %file%multi.dimacs 
echo **********Done with multiedge addition!****************************
echo **********Making the embedding Eulerian...***********************
make_embedding_eulerian.exe %file%.vcode %file%multi.dimacs
echo **********Done making the embedding Eulerian*****************************
echo **********Finding the Atrail ...***********************************
Atrail_search %file%.ecode
echo **********Done finding of an Atrail!*******************************
echo **********Verifying the trail is valid ...*************************
Atrail_verify %file%.ecode %file%.trail
echo **********Done verifying the trail*********************************
echo **********Relaxing a spring system with duplex cylinders***********
scaffold-routing-rectification --input=%file%.ply --output=%file%.rpoly --scaling=%scale%
IF NOT errorlevel 1 echo You can now import %file%.rpoly into Maya using the vHelix plugin.
echo **********Done with the spring relaxation**************************
)