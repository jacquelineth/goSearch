@echo off
if not exist bin mkdir bin
go build -o bin\search.exe .\cmd\search
go build -o bin\searchui.exe .\cmd\searchui
echo Built bin\search.exe and bin\searchui.exe
