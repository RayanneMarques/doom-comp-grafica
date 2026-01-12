#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include "drawlevel.h"
#include "scene.h"
#include "utils/levelmetrics.h"
#include <cstdio>

// --- VARIÁVEIS EXTERNAS (Vêm do main.cpp) ---
extern GLuint texParede;      // Parede 1 (Pedra)
extern GLuint texParedeMetal; // Parede 2 (Metal) - NOVO

extern GLuint texPiso;        // Piso 1 (Comum)
extern GLuint texPisoAlt;     // Piso 2 (Alternativo) - NOVO

extern GLuint texTeto;        // Teto - NOVO

extern GLuint texLava;
extern GLuint texSangue;
extern GLuint progLava;
extern GLuint progSangue;

// Controle de tempo
extern float tempoEsfera;

// Config do grid
static const float TILE = 4.0f;    // tamanho do tile no mundo
static const float WALL_H = 4.0f;  // altura da parede
static const float EPS_Y = 0.001f; // evita z-fighting

static void bindTexture0(GLuint tex)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
}

// Desenha um quadrado plano (usado para chão e teto)
static void desenhaQuadPlano(float x, float y, float z, float tile, float tilesUV)
{
    float half = tile * 0.5f;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x - half, y, z + half);
    glTexCoord2f(tilesUV, 0.0f);
    glVertex3f(x + half, y, z + half);
    glTexCoord2f(tilesUV, tilesUV);
    glVertex3f(x + half, y, z - half);
    glTexCoord2f(0.0f, tilesUV);
    glVertex3f(x - half, y, z - half);
    glEnd();
}

// Agora aceita a textura como parâmetro (para suportar Piso 1 e Piso 2)
static void desenhaTileChao(float x, float z, GLuint texturaID)
{
    glUseProgram(0); // sem shader
    glColor3f(1, 1, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaID);

    // desenha no chão (y = EPS_Y)
    desenhaQuadPlano(x, EPS_Y, z, TILE, 1.0f);
}

// Nova função para desenhar o teto (apenas muda o Y para WALL_H)
static void desenhaTeto(float x, float z)
{
    glUseProgram(0);
    glColor3f(1, 1, 1); // ou uma cor mais escura para simular sombra: glColor3f(0.6f, 0.6f, 0.6f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texTeto);

    // desenha na altura da parede (y = WALL_H)
    // Note: A normal deveria estar invertida para olhar de baixo, 
    // mas sem culling habilitado funciona igual.
    desenhaQuadPlano(x, WALL_H, z, TILE, 1.0f);
}

// Verifica se o caractere representa um ambiente externo (céu aberto)
bool isOutdoor(char c) {
    // '0' = Piso Comum Externo
    // '_' = Piso Alt Externo
    // Adicione outros se houver
    return (c == '0' || c == '_');
}

// Verifica se o caractere é sólido (outra parede)
// Usado para não desenhar faces escondidas (Otimização + Correção Visual)
bool isSolid(char c) {
    return (c == '1' || c == 'M');
}

// Configurações "fakes" de luz para demonstração. 
// SUBSTITUA pelos seus comandos reais de iluminação (ex: glUniform ou glEnable(GL_LIGHT0))
void aplicarLuzSolar() {
    // Exemplo: Luz forte e branca (Sol)
    // Se usar shader: glUniform1i(uLightType, 0); 
    glColor3f(1.0f, 1.0f, 1.0f); 
}

void aplicarLuzLocal() {
    // Exemplo: Luz mais fraca ou amarelada (Interior)
    // Se usar shader: glUniform1i(uLightType, 1);
    glColor3f(0.6f, 0.6f, 0.5f); 
}

// Agora aceita a textura como parâmetro (para suportar Pedra e Metal)
// static void desenhaParede(float x, float z, GLuint texturaID)
// {
//     float half = TILE * 0.5f;

//     glColor3f(1, 1, 1);
//     glBindTexture(GL_TEXTURE_2D, texturaID);

//     // textura repetida ao longo da parede
//     float tilesX = 1.0f;
//     float tilesY = 1.0f;

//     glBegin(GL_QUADS);

//     // Frente (z+)
//     glTexCoord2f(0.0f, 0.0f);   glVertex3f(x - half, 0.0f, z + half);
//     glTexCoord2f(tilesX, 0.0f); glVertex3f(x + half, 0.0f, z + half);
//     glTexCoord2f(tilesX, tilesY); glVertex3f(x + half, WALL_H, z + half);
//     glTexCoord2f(0.0f, tilesY); glVertex3f(x - half, WALL_H, z + half);

//     // Trás (z-)
//     glTexCoord2f(0.0f, 0.0f);   glVertex3f(x + half, 0.0f, z - half);
//     glTexCoord2f(tilesX, 0.0f); glVertex3f(x - half, 0.0f, z - half);
//     glTexCoord2f(tilesX, tilesY); glVertex3f(x - half, WALL_H, z - half);
//     glTexCoord2f(0.0f, tilesY); glVertex3f(x + half, WALL_H, z - half);

//     // Direita (x+)
//     glTexCoord2f(0.0f, 0.0f);   glVertex3f(x + half, 0.0f, z + half);
//     glTexCoord2f(tilesX, 0.0f); glVertex3f(x + half, 0.0f, z - half);
//     glTexCoord2f(tilesX, tilesY); glVertex3f(x + half, WALL_H, z - half);
//     glTexCoord2f(0.0f, tilesY); glVertex3f(x + half, WALL_H, z + half);

//     // Esquerda (x-)
//     glTexCoord2f(0.0f, 0.0f);   glVertex3f(x - half, 0.0f, z - half);
//     glTexCoord2f(tilesX, 0.0f); glVertex3f(x - half, 0.0f, z + half);
//     glTexCoord2f(tilesX, tilesY); glVertex3f(x - half, WALL_H, z + half);
//     glTexCoord2f(0.0f, tilesY); glVertex3f(x - half, WALL_H, z - half);

//     glEnd();
// }

// Nova assinatura: precisa do mapa e coordenadas do grid (ix, iz)
static void desenhaParedeSmart(float wx, float wz, GLuint texturaID, 
                               const std::vector<std::string> &data, int ix, int iz, int W, int H)
{
    float half = TILE * 0.5f;
    float tilesX = 1.0f;
    float tilesY = 1.0f;

    glBindTexture(GL_TEXTURE_2D, texturaID);
    
    // Vizinhos (com verificação de limites do mapa)
    char nNorth = (iz > 0)     ? data[iz - 1][ix] : '0'; // Z-
    char nSouth = (iz < H - 1) ? data[iz + 1][ix] : '0'; // Z+
    char nEast  = (ix < W - 1) ? data[iz][ix + 1] : '0'; // X+
    char nWest  = (ix > 0)     ? data[iz][ix - 1] : '0'; // X-

    glBegin(GL_QUADS);

    // --- FACE NORTE (Z-) ---
    // Só desenha se não tiver outra parede colada (isSolid)
    if (!isSolid(nNorth)) {
        if (isOutdoor(nNorth)) aplicarLuzSolar(); else aplicarLuzLocal();
        glNormal3f(0.0f, 0.0f, -1.0f); // Normal aponta para Norte
        glTexCoord2f(0.0f, 0.0f);    glVertex3f(wx + half, 0.0f, wz - half); // Note a inversão da ordem X para facear corretamente
        glTexCoord2f(tilesX, 0.0f);  glVertex3f(wx - half, 0.0f, wz - half);
        glTexCoord2f(tilesX, tilesY);glVertex3f(wx - half, WALL_H, wz - half);
        glTexCoord2f(0.0f, tilesY);  glVertex3f(wx + half, WALL_H, wz - half);
    }

    // --- FACE SUL (Z+) ---
    if (!isSolid(nSouth)) {
        if (isOutdoor(nSouth)) aplicarLuzSolar(); else aplicarLuzLocal();
        glNormal3f(0.0f, 0.0f, 1.0f); // Normal aponta para Sul
        glTexCoord2f(0.0f, 0.0f);    glVertex3f(wx - half, 0.0f, wz + half);
        glTexCoord2f(tilesX, 0.0f);  glVertex3f(wx + half, 0.0f, wz + half);
        glTexCoord2f(tilesX, tilesY);glVertex3f(wx + half, WALL_H, wz + half);
        glTexCoord2f(0.0f, tilesY);  glVertex3f(wx - half, WALL_H, wz + half);
    }

    // --- FACE LESTE (X+) ---
    if (!isSolid(nEast)) {
        if (isOutdoor(nEast)) aplicarLuzSolar(); else aplicarLuzLocal();
        glNormal3f(1.0f, 0.0f, 0.0f); // Normal aponta para Leste
        glTexCoord2f(0.0f, 0.0f);    glVertex3f(wx + half, 0.0f, wz + half);
        glTexCoord2f(tilesX, 0.0f);  glVertex3f(wx + half, 0.0f, wz - half);
        glTexCoord2f(tilesX, tilesY);glVertex3f(wx + half, WALL_H, wz - half);
        glTexCoord2f(0.0f, tilesY);  glVertex3f(wx + half, WALL_H, wz + half);
    }

    // --- FACE OESTE (X-) ---
    if (!isSolid(nWest)) {
        if (isOutdoor(nWest)) aplicarLuzSolar(); else aplicarLuzLocal();
        glNormal3f(-1.0f, 0.0f, 0.0f); // Normal aponta para Oeste
        glTexCoord2f(0.0f, 0.0f);    glVertex3f(wx - half, 0.0f, wz - half);
        glTexCoord2f(tilesX, 0.0f);  glVertex3f(wx - half, 0.0f, wz + half);
        glTexCoord2f(tilesX, tilesY);glVertex3f(wx - half, WALL_H, wz + half);
        glTexCoord2f(0.0f, tilesY);  glVertex3f(wx - half, WALL_H, wz - half);
    }

    glEnd();
    
    // Reseta cor para branco para não afetar o resto
    glColor3f(1,1,1);
}

static void desenhaTileLava(float x, float z)
{
    glUseProgram(progLava);

    GLint locTime = glGetUniformLocation(progLava, "uTime");
    GLint locStr = glGetUniformLocation(progLava, "uStrength");
    GLint locScr = glGetUniformLocation(progLava, "uScroll");
    GLint locHeat = glGetUniformLocation(progLava, "uHeat");
    GLint locTex = glGetUniformLocation(progLava, "uTexture");

    glUniform1f(locTime, tempoEsfera);
    glUniform1f(locStr, 1.0f);
    glUniform2f(locScr, 0.1f, 0.0f);
    glUniform1f(locHeat, 0.6f);

    bindTexture0(texLava);
    glUniform1i(locTex, 0);

    glColor3f(1, 1, 1);
    desenhaQuadPlano(x, EPS_Y, z, TILE, 2.0f);

    glUseProgram(0);
}

static void desenhaTileSangue(float x, float z)
{
    glUseProgram(progSangue);

    GLint locTime = glGetUniformLocation(progSangue, "uTime");
    GLint locStr = glGetUniformLocation(progSangue, "uStrength");
    GLint locSpd = glGetUniformLocation(progSangue, "uSpeed");
    GLint locTex = glGetUniformLocation(progSangue, "uTexture");

    glUniform1f(locTime, tempoEsfera);
    glUniform1f(locStr, 1.0f);
    glUniform2f(locSpd, 2.0f, 1.3f);

    bindTexture0(texSangue);
    glUniform1i(locTex, 0);

    glColor3f(1, 1, 1);
    desenhaQuadPlano(x, EPS_Y, z, TILE, 2.0f);

    glUseProgram(0);
}

void drawLevel(const MapLoader &map)
{
    const auto &data = map.data();
    int H = map.getHeight();
    int W = map.getWidth();

    // centraliza o mapa no mundo
    LevelMetrics m = LevelMetrics::fromMap(map, TILE);

    for (int z = 0; z < H; z++)
    {
        for (int x = 0; x < (int)data[z].size(); x++)
        {
            float wx, wz;
            m.tileCenter(x, z, wx, wz); // centro do tile

            char c = data[z][x];
            bool temTeto = false;

            // --- 1. LÓGICA DE PAREDES E CHÃO ---
        if (c == '1') {
                // Parede Pedra
                desenhaParedeSmart(wx, wz, texParede, data, x, z, W, H);
                temTeto = true; 
            }
            else if (c == 'M') {
                // Parede Metal
                desenhaParedeSmart(wx, wz, texParedeMetal, data, x, z, W, H);
                temTeto = true;
            }
            // ... MANTENHA O RESTO DOS ELSE IF (Pisos, Lava, Sangue) IGUAL ...
            else if (c == '0') {
                 // ... seu código antigo de chão ...
                 desenhaTileChao(wx, wz, texPiso);
                 temTeto = false;
            }
            else if (c == 'I') {
                // Piso Comum (Com teto - Interno)
                desenhaTileChao(wx, wz, texPiso);
                temTeto = true;
            }
            else if (c == '_') {
                // Piso Alternativo (Sem teto - Externo)
                desenhaTileChao(wx, wz, texPisoAlt);
                temTeto = false;
            }
            else if (c == 'A') {
                // Piso Alternativo (Com teto - Interno)
                desenhaTileChao(wx, wz, texPisoAlt);
                temTeto = true;
            }
            else if (c == 'L') {
                desenhaTileLava(wx, wz);
            }
            else if (c == 'B') {
                desenhaTileSangue(wx, wz);
            }

            // --- 2. DESENHO DO TETO ---
            if (temTeto) {
                desenhaTeto(wx, wz);
            }
        }
    }
}