@echo off
echo =============================
echo Compilando projeto Doom CG...
echo =============================

g++ main.cpp scene.cpp drawlevel.cpp draw.cpp texture.cpp input.cpp shader.cpp utils/maploader.cpp utils/levelmetrics.cpp -o Doom.exe -lfreeglut -lglew32 -lopengl32 -lglu32

if %errorlevel% neq 0 (
    echo.
    echo [ERRO] Falha na compilacao. Verifique as mensagens acima.
    pause
    exit /b %errorlevel%
)

echo.
echo Compilacao concluida! Executando...
echo =============================
.\Doom.exe