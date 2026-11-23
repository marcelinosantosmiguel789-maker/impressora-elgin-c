@echo off
setlocal
where gcc >nul 2>nul
if errorlevel 1 (
    echo Erro: GCC/MinGW nao encontrado no PATH.
    exit /b 1
)

echo Arquitetura do GCC:
gcc -dumpmachine

gcc -std=c99 -Wall -Wextra -o impressora.exe main.c
if errorlevel 1 (
    echo Falha na compilacao. Confira a instalacao do MinGW e os avisos acima.
    exit /b 1
)

echo Compilacao concluida: impressora.exe
endlocal
