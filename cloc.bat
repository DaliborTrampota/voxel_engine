cloc-2.06.exe --vcs=git ^
 --exclude-list-file=dependencies\include ^
 --exclude-list-file=dependencies\seuil\dependencies ^
 --exclude-dir=archive ^
 --include-lang="C++,GLSL,C/C++ Header" ^
 --counted=tmp.txt ^
 . dependencies\lwgl dependencies\lwgl dependencies\seuil dependencies\seuil