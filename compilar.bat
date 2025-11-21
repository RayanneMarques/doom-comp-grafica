@echo off
cls
echo ==========================================
echo Compilando projeto Doom CG...
echo ==========================================

:: Comando de compilação
g++ main.cpp scene.cpp input.cpp draw.cpp -o doom.exe -lopengl32 -lglu32 -lfreeglut

:: Verifica se houve erro na compilação (Erro 0 = Sucesso)
if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCESSO] Compilacao concluida. Iniciando o jogo...
    echo ------------------------------------------
    :: Executa o programa gerado
    doom.exe
) else (
    echo.
    echo [ERRO] Falha na compilacao. Verifique as mensagens acima.
)

echo.
pause