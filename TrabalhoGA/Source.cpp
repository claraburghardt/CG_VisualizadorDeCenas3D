/* Código adaptado de https://github.com/fellowsheep/CG2024-2/tree/6a149c7a3ac80d20722317d1084002780cfe750c/Hello3D-VS2022
 *
 * Adaptado por Clara Burghardt
 * para a disciplina de Processamento Gráfico - Unisinos
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Common/include/Shader.h"

using namespace std;

//STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include <C:\Users\I555002\Source\Repos\VisualizadorDeCenas3D\Dependencies\stb_image\stb_image.h>

// Variáveis globais da câmera
glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp;

// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Configuração do controle do mouse
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;
float sensitivity = 0.1f;
float zoomSpeed = 1.0f;

// Estrutura para os objetos 3D
struct Object {
    GLuint VAO;
    GLuint texID;
    int nVertices;
    glm::mat4 model;
    float ka, kd, ks;
    float rotationX;
    float rotationY;
    float rotationZ;
    glm::vec3 translation;
    float scale;
};

// Declara os objetos 3D
Object obj1, obj2;
int selectedObject = 1;

// Protótipos das funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
int loadSimpleOBJ(string filePATH, int& nVertices);
void applyTransformations(Object& obj);
GLuint loadTexture(string filePath, int& width, int& height);
glm::vec3 bezierCurve(const std::vector<glm::vec3>& controlPoints, float t);
void loadSceneConfig(Shader shader, string filePath);

int main() {
    // Inicialização do GLFW
    glfwInit();

    // Criação da janela
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, " Visualizador de cenas 3D", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Configuração das funções de callback e mouse
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Inicializa o GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
        std::cout << "Failed to initialize GLAD" << std::endl;

    // Obtem informações do renderizador e versão do OpenGL
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Configura a viewport com as mesmas dimensões da janela
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compila e builda o programa de shader
    Shader shader("C:\\Users\\I555002\\Source\\Repos\\VisualizadorDeCenas3D\\TrabalhoGA\\phong.vs", "C:\\Users\\I555002\\Source\\Repos\\VisualizadorDeCenas3D\\TrabalhoGA\\phong.fs");
    glUseProgram(shader.ID);

    loadSceneConfig(shader, "C:\\Users\\I555002\\Source\\Repos\\VisualizadorDeCenas3D\\TrabalhoGA\\sceneConfig.txt");

    glm::mat4 model = glm::mat4(1);

    // Localização da matriz de modelo
    GLint modelLoc = glGetUniformLocation(shader.ID, "model");

    // Configuração da matriz de visualização
    glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // Configuração da matriz de projeção
    glm::mat4 projection = glm::perspective(glm::radians(39.6f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    //Buffer de textura no shader
    glUniform1i(glGetUniformLocation(shader.ID, "texBuffer"), 0);

    // Habilita o teste de profundidade
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

    // Configura propriedades da superfície
    shader.setFloat("ka", 0.2);
    shader.setFloat("ks", 0.5);
    shader.setFloat("kd", 0.5);
    shader.setFloat("q", 10.0);

    // Definir pontos de controle para a curva de Bezier
    std::vector<glm::vec3> controlPoints = {
        obj2.translation + glm::vec3(-1.0f, 0.0f, -1.0f),
        obj2.translation + glm::vec3(-0.5f, 1.0f, 0.0f),
        obj2.translation + glm::vec3(0.5f, 1.0f, 0.0f),
        obj2.translation + glm::vec3(1.0f, 0.0f, -1.0f)
    };

    float t = 0.0f; // Parâmetro para a Curva de Bezier
    float speed = 0.001f; // Velocidade do movimento

    // Loop principal da aplicação
    while (!glfwWindowShouldClose(window)) {
        // Verifica eventos de input (teclado, mouse, etc.)
        glfwPollEvents();

        // Limpa o buffer de cor e profundidade
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        // Atualiza a posição de obj1 usando a curva de Bezier
        obj1.translation = bezierCurve(controlPoints, t);
        t += speed;
        if (t > 1.0f) t = 0.0f; // Retorne ao início da curva

        // Aplica transformações a cada objeto
        applyTransformations(obj1);
        applyTransformations(obj2);

        // Desenha o objeto 1
        glBindVertexArray(obj1.VAO);
        glBindTexture(GL_TEXTURE_2D, obj1.texID);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obj1.model));
        glDrawArrays(GL_TRIANGLES, 0, obj1.nVertices);

        // Desenha o objeto 2
        glBindVertexArray(obj2.VAO);
        glBindTexture(GL_TEXTURE_2D, obj2.texID);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obj2.model));
        glDrawArrays(GL_TRIANGLES, 0, obj2.nVertices);

        // Atualiza a matriz de visualização com a posição da câmera
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Atualiza a posição da câmera no shader
        shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

        // Troca os buffers e exibe o próximo frame
        glfwSwapBuffers(window);
    }
    // Libera os recursos
    glDeleteVertexArrays(1, &obj1.VAO);
    glDeleteVertexArrays(1, &obj2.VAO);
    glfwTerminate(); // Finaliza o GLFW
    return 0; // Finaliza o programa
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Verifica se é a primeira vez que o mouse é movido
    if (firstMouse) {
        lastX = xpos; // Armazena a posição inicial do mouse em X
        lastY = ypos; // Armazena a posição inicial do mouse em Y
        firstMouse = false; // Atualiza a variável para não entrar mais aqui
    }

    // Calcula o deslocamento do mouse e atualiza posição
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // Aplica a sensibilidade do mouse ao deslocamento
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Atualiza os ângulos de visão (yaw e pitch)
    yaw += xoffset;
    pitch += yoffset;

    // Limita o pitch para evitar uma rotação excessiva
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Calcula a direção para onde a câmera está olhando
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Normaliza o vetor de direção
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Ajusta a posição da câmera com base na rolagem do mouse
    cameraPos += cameraFront * (float)yoffset * zoomSpeed;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // Fecha a janela se a tecla ESC for pressionada
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Seleciona o objeto baseado na tecla pressionada
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        selectedObject = 1;
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        selectedObject = 2;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        selectedObject = 3;

    // Ponto de referência do objeto selecionado
    Object* currentObj = (selectedObject == 1) ? &obj1 : &obj2;

    // Controle de rotação do objeto em X, Y e Z
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        currentObj->rotationX += glm::radians(15.0f);

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
        currentObj->rotationY += glm::radians(15.0f);

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        currentObj->rotationZ += glm::radians(15.0f);

    // Controle de translação do objeto em X, Y e Z
    float translationSpeed = 0.1f;
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para cima
        currentObj->translation.y += translationSpeed;

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para baixo
        currentObj->translation.y -= translationSpeed;

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para a esquerda
        currentObj->translation.x -= translationSpeed;

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para a direita
        currentObj->translation.x += translationSpeed;

    if (key == GLFW_KEY_K && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para frente
        currentObj->translation.z -= translationSpeed;

    if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para trás
        currentObj->translation.z += translationSpeed;

    // Controle de escala
    float scaleSpeed = 0.1f;
    if (key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Aumenta escala
        currentObj->scale += scaleSpeed;

    if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Diminui escala
        currentObj->scale -= scaleSpeed;
}

void applyTransformations(Object& obj) {
    // Inicializa a matriz do modelo como a matriz identidade
    obj.model = glm::mat4(1.0f);

    // Aplica rotação nos eixos X, Y e Z
    obj.model = glm::rotate(obj.model, obj.rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
    obj.model = glm::rotate(obj.model, obj.rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    obj.model = glm::rotate(obj.model, obj.rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

    // Aplica translação 
    obj.model = glm::translate(obj.model, obj.translation);

    // Aplica escala
    obj.model = glm::scale(obj.model, glm::vec3(obj.scale, obj.scale, obj.scale));
}

int loadSimpleOBJ(string filePath, int& nVertices) {
    // Vetores para armazenar os dados do objeto
    vector <glm::vec3> vertices;
    vector <glm::vec2> texCoords;
    vector <glm::vec3> normals;
    vector <GLfloat> vBuffer;

    // Define uma cor padrão
    glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);

    // Abre o arquivo OBJ
    ifstream arqEntrada;

    arqEntrada.open(filePath.c_str());
    if (arqEntrada.is_open()) {
        // Faz o parsing
        string line;
        // Lê o arquivo linha por linha
        while (!arqEntrada.eof()) {
            getline(arqEntrada, line);
            istringstream ssline(line);
            string word;
            ssline >> word;

            // Processa os vértices
            if (word == "v") {
                glm::vec3 vertice;
                ssline >> vertice.x >> vertice.y >> vertice.z;
                vertices.push_back(vertice);
            }

            // Processa as coordenadas de textura
            else if (word == "vt") {
                glm::vec2 vt;
                ssline >> vt.s >> vt.t;
                texCoords.push_back(vt);
            }

            // Processa as normais
            else if (word == "vn") {
                glm::vec3 normal;
                ssline >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            }

            // Processa as faces
            else if (word == "f") {
                while (ssline >> word) {
                    int vi, ti, ni;
                    istringstream ss(word);
                    std::string index;

                    // Pega o índice do vértice
                    std::getline(ss, index, '/');
                    vi = std::stoi(index) - 1;

                    // Pega o índice da coordenada de textura
                    std::getline(ss, index, '/');
                    ti = std::stoi(index) - 1;

                    // Pega o índice da normal
                    std::getline(ss, index);
                    ni = std::stoi(index) - 1;

                    //Recupera os vértices do indice lido
                    vBuffer.push_back(vertices[vi].x);
                    vBuffer.push_back(vertices[vi].y);
                    vBuffer.push_back(vertices[vi].z);

                    //Atributo cor
                    vBuffer.push_back(color.r);
                    vBuffer.push_back(color.g);
                    vBuffer.push_back(color.b);

                    //Atributo coordenada de textura
                    vBuffer.push_back(texCoords[ti].s);
                    vBuffer.push_back(texCoords[ti].t);

                    //Atributo vetor normal
                    vBuffer.push_back(normals[ni].x);
                    vBuffer.push_back(normals[ni].y);
                    vBuffer.push_back(normals[ni].z);
                }
            }
        }
        // Fecha o arquivo após leitura
        arqEntrada.close();

        // Gera o buffer de geometria
        cout << "Gerando o buffer de geometria..." << endl;
        GLuint VBO, VAO;

        // Gera o identificador do VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);

        // Gera o identificador do VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Configura os atributos (posição, cor, coordenada de textura, vetor normal) de vértices 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);

        // Desvincula o buffer e o VAO após configuração
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        nVertices = vBuffer.size() / 11;
        return VAO;
    }
    else {
        // Apresenta erro ao abrir o arquivo
        cout << "Erro ao tentar ler o arquivo " << filePath << endl;
        return -1;
    }
}

GLuint loadTexture(string filePath, int& width, int& height)
{
    GLuint texID; // id da textura a ser carregada

    // Gera o identificador da textura na memória
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Ajuste dos parâmetros de wrapping e filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carregamento da imagem usando a função stbi_load da biblioteca stb_image
    int nrChannels;

    // Carrega a imagem do arquivo especificado
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        // Verifica se a imagem tem 3 canais (RGB) ou 4 canais (RGBA)
        if (nrChannels == 3) // jpg, bmp
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else // assume que é 4 canais png
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cout << "Failed to load texture " << filePath << std::endl;

    // Libera a memória alocada pela função stbi_load
    stbi_image_free(data);
    // Desvincula a textura após o carregamento
    glBindTexture(GL_TEXTURE_2D, 0);
    // Retorna o ID da textura gerada
    return texID;
}

glm::vec3 bezierCurve(const std::vector<glm::vec3>& controlPoints, float t) {
    int n = controlPoints.size() - 1; // Número de pontos de controle menos 1
    glm::vec3 point(0.0f); // Ponto resultante da interpolação de Bezier

    // Calcula o ponto na curva de Bezier para um valor t
    for (int i = 0; i <= n; ++i) {
        // Calcula o coeficiente binomial para a interpolação de Bezier
        float binomialCoeff = glm::pow(1 - t, n - i) * glm::pow(t, i);
        // Acumula a contribuição de cada ponto de controle
        point += binomialCoeff * controlPoints[i];
    }
    // Retorna o ponto final da curva
    return point;
}

void loadSceneConfig(Shader shader, string filePath) {
    // Carrega os objetos 3D a partir do arquivo de configuração
    int texWidth, texHeight;

    // Abre o arquivo de configuração
    std::ifstream configFile(filePath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file" << std::endl;
        return;
    }

    std::string line;
    // Lê o arquivo linha por linha
    while (std::getline(configFile, line)) {
        std::istringstream ss(line);
        std::string key;
        ss >> key;

        // Processa a configuração de um objeto 3D
        if (key == "object") {
            int objId;
            std::string modelFile, textureFile;
            float rotX, rotY, rotZ, transX, transY, transZ, scale;

            // Lê os dados do objeto do arquivo de configuração
            ss >> objId >> modelFile >> textureFile;
            ss >> rotX >> rotY >> rotZ;
            ss >> transX >> transY >> transZ;
            ss >> scale;

            // Seleciona o objeto correspondente
            Object& obj = (objId == 1) ? obj1 : obj2;

            // Carrega o modelo 3D e a textura do objeto
            obj.VAO = loadSimpleOBJ(modelFile, obj.nVertices);
            obj.texID = loadTexture(textureFile, texWidth, texHeight);

            // Configura as transformações do objeto (rotação, translação e escala)
            obj.rotationX = glm::radians(rotX);
            obj.rotationY = glm::radians(rotY);
            obj.rotationZ = glm::radians(rotZ);
            obj.translation = glm::vec3(transX, transY, transZ);
            obj.scale = scale;
        }

        // Processa a configuração da luz
        if (key == "light") {
            float lightPosX, lightPosY, lightPosZ;
            float lightColorR, lightColorG, lightColorB;
            ss >> lightPosX >> lightPosY >> lightPosZ;
            ss >> lightColorR >> lightColorG >> lightColorB;

            // Configura a posição e cor da luz no shader
            shader.setVec3("lightPos", lightPosX, lightPosY, lightPosZ);
            shader.setVec3("lightColor", lightColorR, lightColorG, lightColorB);
        }

        // Processa a configuração da câmera
        if (key == "camera") {
            float camPosX, camPosY, camPosZ;
            float camFrontX, camFrontY, camFrontZ;
            float camUpX, camUpY, camUpZ;
            ss >> camPosX >> camPosY >> camPosZ;
            ss >> camFrontX >> camFrontY >> camFrontZ;
            ss >> camUpX >> camUpY >> camUpZ;

            // Configura a posição, direção e orientação da câmera
            cameraPos = glm::vec3(camPosX, camPosY, camPosZ);
            cameraFront = glm::vec3(camFrontX, camFrontY, camFrontZ);
            cameraUp = glm::vec3(camUpX, camUpY, camUpZ);
        }
    }

    // Fecha o arquivo de configuração
    configFile.close();
}