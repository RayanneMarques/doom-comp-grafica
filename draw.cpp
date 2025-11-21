#include <GL/glut.h>
#include <math.h>
#include "scene.h"

#define NUM_TORRES 5
#define RAIO 10.0f // raio das torres ao redor do centro

static void desenhaLosango(float altura)
{
    float h = altura / 2.0f;
    float s = altura / 3.0f;

    float claro[3] = {0.3f, 1.0f, 0.3f};
    float escuro[3] = {0.0f, 0.6f, 0.0f};

    glBegin(GL_TRIANGLES);
    // metade de cima
    glColor3fv(claro);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(-s, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, s);

    glColor3fv(escuro);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(0.0f, 0.0f, s);
    glVertex3f(s, 0.0f, 0.0f);

    glColor3fv(claro);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(s, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -s);

    glColor3fv(escuro);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(0.0f, 0.0f, -s);
    glVertex3f(-s, 0.0f, 0.0f);

    // metade de baixo
    glColor3fv(claro);
    glVertex3f(0.0f, -h, 0.0f);
    glVertex3f(0.0f, 0.0f, s);
    glVertex3f(-s, 0.0f, 0.0f);

    glColor3fv(escuro);
    glVertex3f(0.0f, -h, 0.0f);
    glVertex3f(s, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, s);

    glColor3fv(claro);
    glVertex3f(0.0f, -h, 0.0f);
    glVertex3f(0.0f, 0.0f, -s);
    glVertex3f(s, 0.0f, 0.0f);

    glColor3fv(escuro);
    glVertex3f(0.0f, -h, 0.0f);
    glVertex3f(-s, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -s);
    glEnd();
}

void desenhaChao()
{
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex3f(-80.0f, 0.0f, -80.0f);
    glVertex3f(80.0f, 0.0f, -80.0f);
    glVertex3f(80.0f, 0.0f, 80.0f);
    glVertex3f(-80.0f, 0.0f, 80.0f);
    glEnd();
}

// --- Adicione isso no começo do draw.cpp, logo após os includes ---

static void desenhaBicicleta()
{
    // Pega o tempo em milissegundos e multiplica para ajustar a velocidade
    // O sinal negativo faz girar "para frente"
    float anguloRodas = -glutGet(GLUT_ELAPSED_TIME) * 0.5f; 

    // Cor Amarelo Neon
    glColor3f(1.0f, 1.0f, 0.0f);

    // --- Roda de Trás ---
    glPushMatrix();
    glTranslatef(-0.8f, 0.35f, 0.0f);
    
    // AQUI ESTÁ O TRUQUE: Girar no eixo Z (o eixo do pneu)
    glRotatef(anguloRodas, 0.0f, 0.0f, 1.0f); 
    
    glutSolidTorus(0.05, 0.35, 10, 20);
    glPopMatrix();

    // --- Roda da Frente ---
    glPushMatrix();
    glTranslatef(0.8f, 0.35f, 0.0f);
    
    // A mesma rotação para a roda da frente
    glRotatef(anguloRodas, 0.0f, 0.0f, 1.0f);

    glutSolidTorus(0.05, 0.35, 10, 20);
    glPopMatrix();

    // --- O Resto da Bicicleta (Quadro não gira) ---
    glLineWidth(3.0f); 
    glBegin(GL_LINES);
        // Triângulo traseiro
        glVertex3f(-0.8f, 0.35f, 0.0f); 
        glVertex3f(0.0f, 0.0f, 0.0f);   
        
        glVertex3f(-0.8f, 0.35f, 0.0f); 
        glVertex3f(-0.2f, 0.7f, 0.0f);  

        glVertex3f(0.0f, 0.0f, 0.0f);   
        glVertex3f(-0.2f, 0.7f, 0.0f);  

        // Barra principal
        glVertex3f(-0.2f, 0.7f, 0.0f);
        glVertex3f(0.5f, 0.7f, 0.0f);   

        // Garfo dianteiro
        glVertex3f(0.5f, 0.9f, 0.0f);   
        glVertex3f(0.8f, 0.35f, 0.0f);  

        // Guidão 
        glVertex3f(0.5f, 0.9f, -0.3f);
        glVertex3f(0.5f, 0.9f, 0.3f);
    glEnd();
    glLineWidth(1.0f); 

    // --- Banco ---
    glPushMatrix();
    glTranslatef(-0.2f, 0.75f, 0.0f);
    glScalef(0.3f, 0.1f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void desenhaTorresELosangos()
{
    float alturaTorre = 2.5f;
    float w = 0.7f;
    float ang0 = -M_PI / 2.0f;
    float passo = 2.0f * M_PI / NUM_TORRES;

    for (int i = 0; i < NUM_TORRES; i++)
    {
        float ang = ang0 + passo * i;
        float x = RAIO * cosf(ang);
        float z = RAIO * sinf(ang);

        glPushMatrix();
        glTranslatef(x, 0.0f, z); // Move para a posição da torre

        // ============================================
        // CÓDIGO DA BICICLETA AQUI
        // ============================================
        if (i == 0) // Apenas na primeira torre
        {
            glPushMatrix();
            // Ajusta posição para encostar na base da torre
            glTranslatef(0.8f, 0.0f, 0.8f); 
            
            // Gira a bike para ficar numa posição legal
            glRotatef(120.0f, 0.0f, 1.0f, 0.0f); 
            
            // Inclina ela um pouco (como se estivesse apoiada)
            glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);

            desenhaBicicleta();
            glPopMatrix();
        }
        // ============================================

        // Torre roxa
        glPushMatrix();
        glColor3f(0.6f, 0.0f, 0.8f);
        glTranslatef(0.0f, alturaTorre / 2.0f, 0.0f);
        glScalef(w, alturaTorre, w);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Losango verde girando em cima 
        glPushMatrix();
        glTranslatef(0.0f, alturaTorre + 1.2f, 0.0f);
        glRotatef(anguloPiramide, 0.0f, 1.0f, 0.0f);
        desenhaLosango(1.5f);
        glPopMatrix();

        glPopMatrix();
    }
}

void desenhaPiramideDegraus()
{
    float alturaDegrau = 0.5f;
    float tamanhoBase = 6.0f;
    float reducao = 0.65f;

    glPushMatrix();

    float roxo1[3] = {0.55f, 0.00f, 0.75f};
    float roxo2[3] = {0.65f, 0.10f, 0.85f};
    float roxo3[3] = {0.75f, 0.20f, 0.95f};

    // Degrau 1
    glColor3f(roxo1[0], roxo1[1], roxo1[2]);
    glPushMatrix();
    glTranslatef(0.0f, alturaDegrau / 2.0f, 0.0f);
    glScalef(tamanhoBase, alturaDegrau, tamanhoBase);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Degrau 2
    glColor3f(roxo2[0], roxo2[1], roxo2[2]);
    glPushMatrix();
    glTranslatef(0.0f, alturaDegrau + alturaDegrau / 2.0f, 0.0f);
    glScalef(tamanhoBase * reducao, alturaDegrau, tamanhoBase * reducao);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Degrau 3
    glColor3f(roxo3[0], roxo3[1], roxo3[2]);
    glPushMatrix();
    glTranslatef(0.0f, 2 * alturaDegrau + alturaDegrau / 2.0f, 0.0f);
    glScalef(tamanhoBase * reducao * reducao,
             alturaDegrau,
             tamanhoBase * reducao * reducao);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Esfera flutuando acima
    float topoDegrausY = 5.0f * alturaDegrau;
    float raioEsfera = 3.0f;

    glPushMatrix();
    glTranslatef(0.0f, topoDegrausY + raioEsfera + 0.2f, 0.0f);

    glRotatef(anguloEsfera, 1.0f, 1.5f, 0.0f);

    double eq[4];

    // metade de cima
    glColor3f(0.40f, 0.00f, 0.00f);
    eq[0] = 0;
    eq[1] = -1;
    eq[2] = 0;
    eq[3] = 0;
    glClipPlane(GL_CLIP_PLANE0, eq);
    glEnable(GL_CLIP_PLANE0);
    glutSolidSphere(raioEsfera, 40, 40);
    glDisable(GL_CLIP_PLANE0);

    // metade de baixo
    glColor3f(0.80f, 0.00f, 0.00f);
    eq[0] = 0;
    eq[1] = 1;
    eq[2] = 0;
    eq[3] = 0;
    glClipPlane(GL_CLIP_PLANE1, eq);
    glEnable(GL_CLIP_PLANE1);
    glutSolidSphere(raioEsfera, 40, 40);
    glDisable(GL_CLIP_PLANE1);

    glPopMatrix();

    glPopMatrix();
}
