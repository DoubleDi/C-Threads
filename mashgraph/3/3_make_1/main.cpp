#include <iostream>
#include <vector>
#include <SOIL.h>
#include <time.h>
#include <fstream>

#include "Utility.h"
#include "EasyBMP/include/EasyBMP.h"

using namespace std;

GLenum DEFAULT_TEXTURE_WRAP = GL_REPEAT;

const uint GRASS_INSTANCES = 10000; // Количество травинок
const uint GROUND_INSTANCES = 1000000;

GL::Camera camera;               // Мы предоставляем Вам реализацию камеры. В OpenGL камера - это просто 2 матрицы. Модельно-видовая матрица и матрица проекции. // ###
                                 // Задача этого класса только в том чтобы обработать ввод с клавиатуры и правильно сформировать эти матрицы.
                                 // Вы можете просто пользоваться этим классом для расчёта указанных матриц.

int multisampling = 0;
GLuint grassPointsCount; // Количество вершин у модели травинки
GLuint grassShader;      // Шейдер, рисующий траву
GLuint grassVAO;         // VAO для травы (что такое VAO почитайте в доках)
GLuint grassVariance;    // Буфер для смещения координат травинок
GLuint grassRotate;
GLuint grassTexture;
vector<VM::vec4> grassVarianceData(GRASS_INSTANCES); // Вектор со смещениями для координат травинок
vector<float> grassVarianceSpeed(GRASS_INSTANCES);
vector<float> grassVarianceAcceleration(GRASS_INSTANCES);
vector<VM::vec4> grassRotateData(GRASS_INSTANCES);
int grassWidth, grassHeight, normalgroundWidth, normalgroundHeight;
unsigned char* grassImage;

GLuint groundShader; // Шейдер для земли
GLuint groundVAO; // VAO для земли
GLuint groundTexture;
vector<vector<float>> groundHeightMap;
int groundWidth, groundHeight;
unsigned char* groundImage;

GLuint skyShader;
GLuint skyVAO;
GLuint skyTexture;
int skyWidth, skyHeight;
unsigned char* skyImage;

GLuint waterShader;
GLuint waterVAO;
GLuint waterTexture;
int waterWidth, waterHeight;
unsigned char* waterImage;

GLuint rockShader;
GLuint rockVAO;
GLuint rockTexture;
int rockWidth, rockHeight;
unsigned char* rockImage;

GLuint treeShader;
GLuint treeVAO;
GLuint treeTexture;
GLuint treePointsCount;
int treeWidth, treeHeight;
unsigned char* treeImage;

// Размеры экрана
uint screenWidth = 800;
uint screenHeight = 600;

// Это для захвата мышки. Вам это не потребуется (это не значит, что нужно удалять эту строку)
bool captureMouse = true;

void loadheightmap() {
    BMP* image = new BMP();
    groundHeightMap.resize(sqrt(GROUND_INSTANCES));
    for (int i = 0; i < sqrt(GROUND_INSTANCES); i++) {
        groundHeightMap[i].resize(sqrt(GROUND_INSTANCES));
    }
    image->ReadFromFile("groundHeightMap.bmp");
    for (int i = 0; i < sqrt(GROUND_INSTANCES); i++) {
        for (int j = 0; j < sqrt(GROUND_INSTANCES); j++) {
            groundHeightMap[i][j] = (((image->GetPixel(i, j).Red + image->GetPixel(i, j).Green + image->GetPixel(i, j).Blue) - 768.0 / 2) / 3.0);
        }
    }
}

// Функция, рисующая замлю
void DrawGround() {
    // Используем шейдер для земли
    glUseProgram(groundShader);                                                  CHECK_GL_ERRORS

    // Устанавливаем юниформ для шейдера. В данном случае передадим перспективную матрицу камеры
    // Находим локацию юниформа 'camera' в шейдере
    GLint cameraLocation = glGetUniformLocation(groundShader, "camera");         CHECK_GL_ERRORS
    // Устанавливаем юниформ (загружаем на GPU матрицу проекции?)                                                     // ###
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, groundTexture);                                 CHECK_GL_ERRORS

    // Подключаем VAO, который содержит буферы, необходимые для отрисовки земли
    glBindVertexArray(groundVAO);                                                CHECK_GL_ERRORS

    // Рисуем землю: 2 треугольника (6 вершин)
    glDrawArrays(GL_TRIANGLES, 0, 6 * GROUND_INSTANCES);                                            CHECK_GL_ERRORS

    // Отсоединяем VAO
    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    // Отключаем шейдер
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void DrawSky() {
    glUseProgram(skyShader);                                                  CHECK_GL_ERRORS
    GLint cameraLocation = glGetUniformLocation(skyShader, "camera");         CHECK_GL_ERRORS
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, skyTexture);                                 CHECK_GL_ERRORS

    glBindVertexArray(skyVAO);                                                CHECK_GL_ERRORS
    glDrawArrays(GL_TRIANGLES, 0, 36);                                            CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void DrawWater() {
    glUseProgram(waterShader);                                                  CHECK_GL_ERRORS
    GLint cameraLocation = glGetUniformLocation(waterShader, "camera");         CHECK_GL_ERRORS
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, waterTexture);                                 CHECK_GL_ERRORS

    glBindVertexArray(waterVAO);                                                CHECK_GL_ERRORS
    glDrawArrays(GL_TRIANGLES, 0, 6);                                            CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void DrawRock() {
    glUseProgram(rockShader);                                                  CHECK_GL_ERRORS
    GLint cameraLocation = glGetUniformLocation(rockShader, "camera");         CHECK_GL_ERRORS
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, rockTexture);                                 CHECK_GL_ERRORS

    glBindVertexArray(rockVAO);                                                CHECK_GL_ERRORS
    glDrawArraysInstanced(GL_TRIANGLES, 0, 300, 3);                               CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

void DrawTree() {
    glUseProgram(treeShader);                                                  CHECK_GL_ERRORS
    GLint cameraLocation = glGetUniformLocation(treeShader, "camera");         CHECK_GL_ERRORS
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, treeTexture);                                 CHECK_GL_ERRORS

    glBindVertexArray(treeVAO);                                                CHECK_GL_ERRORS
    glDrawArraysInstanced(GL_TRIANGLES, 0, treePointsCount, 2);                               CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

// Обновление смещения травинок
void UpdateGrassVariance() {
    float delta_t = 1/20.0;
    // Генерация случайных смещений
        for (uint i = 0; i < GRASS_INSTANCES; ++i) {
            grassVarianceAcceleration[i] = (grassVarianceData[i].x - 0.014) / ( 2.0);
            grassVarianceSpeed[i] = grassVarianceSpeed[i] - grassVarianceAcceleration[i] * delta_t - ((float)rand() / RAND_MAX - 0.5) / 3000.0;
            grassVarianceData[i].x += grassVarianceSpeed[i] * delta_t;
            grassVarianceData[i].z += grassVarianceSpeed[i] * delta_t;
        }
    // Привязываем буфер, с 10одержащий смещения
    glBindBuffer(GL_ARRAY_BUFFER, grassVariance);                                CHECK_GL_ERRORS
    // Загружаем данные в видеопамять
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * GRASS_INSTANCES, grassVarianceData.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    // Отвязываем буфер
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

// Рисование травы
void DrawGrass() {
    // Тут то же самое, что и в рисовании земли
    glUseProgram(grassShader);                                                   CHECK_GL_ERRORS
    GLint cameraLocation = glGetUniformLocation(grassShader, "camera");          CHECK_GL_ERRORS
    glUniformMatrix4fv(cameraLocation, 1, GL_TRUE, camera.getMatrix().data().data()); CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, grassTexture);                                  CHECK_GL_ERRORS

    glBindVertexArray(grassVAO);                                                 CHECK_GL_ERRORS
    // Обновляем смещения для травы
    UpdateGrassVariance();
    // Отрисовка травинок в количестве GRASS_INSTANCES
    glDrawArraysInstanced(GL_TRIANGLES, 0, grassPointsCount, GRASS_INSTANCES);   CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glUseProgram(0);                                                             CHECK_GL_ERRORS
}

// Эта функция вызывается для обновления экрана
void RenderLayouts() {
    // Включение буфера глубины
    glEnable(GL_DEPTH_TEST);
    // Очистка буфера глубины и цветового буфера
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Рисуем меши
    DrawSky();
    DrawWater();
    DrawGround();
    DrawRock();
    DrawTree();
    DrawGrass();
    glutSwapBuffers();
}

// Завершение программы
void FinishProgram() {
    SOIL_free_image_data(grassImage);
    SOIL_free_image_data(groundImage);
    SOIL_free_image_data(waterImage);
    SOIL_free_image_data(skyImage);
    glutDestroyWindow(glutGetWindow());
}

void enableMultisample(int msaa)
{
    if (msaa)
    {
        glEnable(GL_MULTISAMPLE);
        glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

        GLint iMultiSample = 0;
        GLint iNumSamples = 0;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
        glGetIntegerv(GL_SAMPLES, &iNumSamples);

        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);
    }
    else
    {
        glDisable(GL_MULTISAMPLE);

        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);

        printf("MSAA off\n");
    }
}

// Обработка события нажатия клавиши (специальные клавиши обрабатываются в функции SpecialButtons)
void KeyboardEvents(unsigned char key, int x, int y) {
    if (key == 27) {
        FinishProgram();
    } else if (key == 'w') {
        camera.goForward();
    } else if (key == 's') {
        camera.goBack();
    } else if (key == 'm') {
        captureMouse = !captureMouse;
        if (captureMouse) {
            glutWarpPointer(screenWidth / 2, screenHeight / 2);
            glutSetCursor(GLUT_CURSOR_NONE);
        } else {
            glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
        }
    } else if (key == 'A') {
        multisampling = !multisampling;
        enableMultisample(multisampling);
    }
}

// Обработка события нажатия специальных клавиш
void SpecialButtons(int key, int x, int y) {
    if (key == GLUT_KEY_RIGHT) {
        camera.rotateY(0.02);
    } else if (key == GLUT_KEY_LEFT) {
        camera.rotateY(-0.02);
    } else if (key == GLUT_KEY_UP) {
        camera.rotateTop(-0.02);
    } else if (key == GLUT_KEY_DOWN) {
        camera.rotateTop(0.02);
    }
}

void IdleFunc() {
    glutPostRedisplay();
}

// Обработка события движения мыши
void MouseMove(int x, int y) {
    if (captureMouse) {
        int centerX = screenWidth / 2,
            centerY = screenHeight / 2;
        if (x != centerX || y != centerY) {
            camera.rotateY((x - centerX) / 1000.0f);
            camera.rotateTop((y - centerY) / 1000.0f);
            glutWarpPointer(centerX, centerY);
        }
    }
}

// Обработка нажатия кнопки мыши
void MouseClick(int button, int state, int x, int y) {
}

// Событие изменение размера окна
void windowReshapeFunc(GLint newWidth, GLint newHeight) {
    glViewport(0, 0, newWidth, newHeight);
    screenWidth = newWidth;
    screenHeight = newHeight;

    camera.screenRatio = (float)screenWidth / screenHeight;
}

// Инициализация окна
void InitializeGLUT(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitContextVersion(3, 0);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowPosition(-1, -1);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Computer Graphics 3");
    glutWarpPointer(400, 300);
    glutSetCursor(GLUT_CURSOR_NONE);

    glutDisplayFunc(RenderLayouts);
    glutKeyboardFunc(KeyboardEvents);
    glutSpecialFunc(SpecialButtons);
    glutIdleFunc(IdleFunc);
    glutPassiveMotionFunc(MouseMove);
    glutMouseFunc(MouseClick);
    glutReshapeFunc(windowReshapeFunc);
}

// Генерация позиций травинок (эту функцию вам придётся переписать)
vector<VM::vec3> GenerateGrassPositions() {
    vector<VM::vec3> grassPositions(GRASS_INSTANCES);
    uint j = 0,k = 0;
    for (uint i = 0; i < GRASS_INSTANCES; ++i) {
        // cout << groundHeightMap[sqrt(float(i)) * 10][sqrt(float(i)) * 10] << endl;
        grassPositions[i] = VM::vec3(
            (i % int(sqrt(GRASS_INSTANCES))) / sqrt(GRASS_INSTANCES) + ((float)rand() / RAND_MAX / 10),
            (i / int(sqrt(GRASS_INSTANCES))) / sqrt(GRASS_INSTANCES) + ((float)rand() / RAND_MAX / 10),
            (groundHeightMap[j * sqrt(GROUND_INSTANCES / GRASS_INSTANCES)][k * sqrt(GROUND_INSTANCES / GRASS_INSTANCES)] / 90.0)
        );
        k++;
        if (k >= sqrt(GRASS_INSTANCES)) { k = 0; j++;}
    }
    return grassPositions;
}

vector<VM::vec3> GenerateRockPositions() {
    vector<VM::vec3> rockPositions;
    rockPositions.push_back(VM::vec3(80, -3, 40));
    rockPositions.push_back(VM::vec3(70, -4, 38));
    rockPositions.push_back(VM::vec3(60, -0.5, 40));
    return rockPositions;
}


vector<VM::vec3> GenerateTreePositions() {
    vector<VM::vec3> treePositions;
    treePositions.push_back(VM::vec3(7, 3.3, 21.6));
    treePositions.push_back(VM::vec3(1, 0, 1));
    return treePositions;
}

// Здесь вам нужно будет генерировать меш
vector<VM::vec4> GenMesh(uint n) {
    return {
        VM::vec4(0, 0, 0, 1),
        VM::vec4(0.5, 0, 0, 1),
        VM::vec4(0, 0.3, 0, 1),

        VM::vec4(0.5, 0, 0, 1),
        VM::vec4(0, 0.3, 0, 1),
        VM::vec4(0.5, 0.3, 0, 1),

        VM::vec4(0, 0.3, 0, 1),
        VM::vec4(0.5, 0.3, 0, 1),
        VM::vec4(0.05, 0.6, 0, 1),

        VM::vec4(0.5, 0.3, 0, 1),
        VM::vec4(0.05, 0.6, 0, 1),
        VM::vec4(0.45, 0.6, 0, 1),

        VM::vec4(0.05, 0.6, 0, 1),
        VM::vec4(0.45, 0.6, 0, 1),
        VM::vec4(0.25, 1, 0, 1),
    };
}

vector<VM::vec4> GenMeshGround() {
    vector<VM::vec4> v;
    for (int i = 0; i < 999; i++) {
        for (int j = 0; j < 999; j++) {
            v.push_back(VM::vec4(i,groundHeightMap[i][j], j, 900));
            v.push_back(VM::vec4(i,groundHeightMap[i][j + 1], j + 1, 900));
            v.push_back(VM::vec4(i + 1,groundHeightMap[i + 1][j + 1], j + 1, 900));
            v.push_back(VM::vec4(i,groundHeightMap[i][j], j, 900));
            v.push_back(VM::vec4(i + 1,groundHeightMap[i + 1][j], j, 900));
            v.push_back(VM::vec4(i + 1,groundHeightMap[i + 1][j + 1], j + 1, 900));
        }
    }
    return v;
}

vector<VM::vec4> GenMeshRock() {
    vector<VM::vec4> v;
    ifstream f;
    float x, y, z;
    float x1, y1, z1;
    float x2, y2, z2;
    f.open("rockcoords");
    f >> x2 >> y2 >> z2;
    f >> x1 >> y1 >> z1;
    while(f >> x >> y >> z) {
        v.push_back(VM::vec4(x, y, z, 90));
        v.push_back(VM::vec4(x1, y1, z1, 90));
        v.push_back(VM::vec4(x2, y2, z2, 90));
        x2 = x1;
        y2 = y1;
        z2 = z1;
        x1 = x;
        y1 = y;
        z1 = z;
    }
    return v;
}

vector<VM::vec4> GenMeshTree() {
    vector<VM::vec4> coords;
    vector<VM::vec4> v;
    ifstream f;
    float x, y, z;
    f.open("treecoords");
    while(f >> x >> y >> z) {
        coords.push_back(VM::vec4(x, y, z, 26));
    }
    f.close();
    f.open("treeindexes");
    while(f >> x >> y >> z) {
        v.push_back(coords[x - 1]);
        v.push_back(coords[y - 1]);
        v.push_back(coords[z - 1]);
    }
    f.close();
    return v;
}

vector<VM::vec2> GenTextureCoords() {
    vector<VM::vec2> v;
    for (uint i = 0; i < GROUND_INSTANCES; i++) {
            v.push_back(VM::vec2(0, 0));
            v.push_back(VM::vec2(0, 1));
            v.push_back(VM::vec2(1, 1));
            v.push_back(VM::vec2(0, 0));
            v.push_back(VM::vec2(1, 0));
            v.push_back(VM::vec2(1, 1));
    }
    return v;
}

// Создание травы
void CreateGrass() {
    uint LOD = 1;
    grassImage = SOIL_load_image("Texture/grass.jpg", &grassWidth, &grassHeight, 0, SOIL_LOAD_RGB);
    // Создаём меш
    vector<VM::vec4> grassPoints = GenMesh(LOD);
    // Сохраняем количество вершин в меше травы
    grassPointsCount = grassPoints.size();
    // Создаём позиции для травинок
    vector<VM::vec3> grassPositions = GenerateGrassPositions();
    // Инициализация смещений для травинок
    for (uint i = 0; i < GRASS_INSTANCES; ++i) {
        grassVarianceData[i] = VM::vec4(0, 0, 0, 0);
        grassVarianceSpeed[i] = 0.005;
        grassVarianceAcceleration[i] = 0;
    }

    for (uint i = 0; i < GRASS_INSTANCES; ++i) {
        float angle = ((float)rand() / RAND_MAX * 2 * M_PI);
        grassRotateData[i] = VM::vec4(cosf(angle), -sinf(angle), sinf(angle), cosf(angle));
    }

    /* Компилируем шейдеры
    Эта функция принимает на вход название шейдера 'shaderName',
    читает файлы shaders/{shaderName}.vert - вершинный шейдер
    и shaders/{shaderName}.frag - фрагментный шейдер,
    компилирует их и линкует.
    */
    grassShader = GL::CompileShaderProgram("grass");
    // Создание VAO
    // Генерация VAO
    glGenVertexArrays(1, &grassVAO);                                             CHECK_GL_ERRORS
    // Привязка VAO
    glBindVertexArray(grassVAO);                                                 CHECK_GL_ERRORS

    // Здесь создаём буфер
    GLuint pointsBuffer;
    // Это генерация одного буфера (в pointsBuffer хранится идентификатор буфера)
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    // Привязываем сгенерированный буфер
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    // Заполняем буфер данными из вектора
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * grassPoints.size(), grassPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    // Получение локации параметра 'point' в шейдере
    GLuint pointsLocation = glGetAttribLocation(grassShader, "point");           CHECK_GL_ERRORS
    // Подключаем массив атрибутов к данной локации
    glEnableVertexAttribArray(pointsLocation);                                   CHECK_GL_ERRORS
    // Устанавливаем параметры для получения данных из массива (по 4 значение типа float на одну вершину)
    glVertexAttribPointer(pointsLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);          CHECK_GL_ERRORS


    glGenTextures(1, &grassTexture);                                             CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, grassTexture);                                  CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassWidth, grassHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, grassImage);
    glGenerateMipmap(GL_TEXTURE_2D);                                             CHECK_GL_ERRORS
    GLuint textureLocation = glGetAttribLocation(grassShader, "texture");        CHECK_GL_ERRORS
    glEnableVertexAttribArray(textureLocation);                                  CHECK_GL_ERRORS
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);         CHECK_GL_ERRORS


    // Создаём буфер для позиций травинок
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);                                            CHECK_GL_ERRORS
    // Здесь мы привязываем новый буфер, так что дальше вся работа будет с ним до следующего вызова glBindBuffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec3) * grassPositions.size(), grassPositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint positionLocation = glGetAttribLocation(grassShader, "position");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(positionLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    // Здесь мы указываем, что нужно брать новое значение из этого буфера для каждого инстанса (для каждой травинки)
    glVertexAttribDivisor(positionLocation, 1);                                  CHECK_GL_ERRORS


    // Создаём буфер для смещения травинок
    glGenBuffers(1, &grassVariance);                                            CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, grassVariance);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * GRASS_INSTANCES, grassVarianceData.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint varianceLocation = glGetAttribLocation(grassShader, "variance");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(varianceLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(varianceLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    glVertexAttribDivisor(varianceLocation, 1);                                  CHECK_GL_ERRORS


    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    // Отвязываем VAO
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    // Отвязываем буфер
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

// Создаём камеру (Если шаблонная камера вам не нравится, то можете переделать, но я бы не стал)
void CreateCamera() {
    camera.angle = 45.0f / 180.0f * M_PI;
    camera.direction = VM::vec3(0, 0.3, -1);
    camera.position = VM::vec3(0.5, 0.2, 0);
    camera.screenRatio = (float)screenWidth / screenHeight;
    camera.up = VM::vec3(0, 1, 0);
    camera.zfar = 50.0f;
    camera.znear = 0.05f;
}

// Создаём замлю
void CreateGround() {
    groundImage = SOIL_load_image("Texture/ground_small.png", &groundWidth, &groundHeight, 0, SOIL_LOAD_RGBA);

    vector<VM::vec4> meshPoints = GenMeshGround();
    vector<VM::vec2> textureCoords = GenTextureCoords();
    groundShader = GL::CompileShaderProgram("ground");
    glGenVertexArrays(1, &groundVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(groundVAO);                                                CHECK_GL_ERRORS


    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint index = glGetAttribLocation(groundShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS


    glGenTextures(1, &groundTexture);                                             CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, groundTexture);                                  CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, groundWidth, groundHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, groundImage);
    glGenerateMipmap(GL_TEXTURE_2D);                                             CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);  CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                CHECK_GL_ERRORS


    GLuint textureBuffer;
    glGenBuffers(1, &textureBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint textureLocation = glGetAttribLocation(groundShader, "texture");        CHECK_GL_ERRORS
    glEnableVertexAttribArray(textureLocation);                                  CHECK_GL_ERRORS
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);         CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

void CreateSky() {
    skyImage = SOIL_load_image("Texture/sky.png", &skyWidth, &skyHeight, 0, SOIL_LOAD_RGBA);
    vector<VM::vec4> meshPoints = {
        VM::vec4(-1, -1, -1, 0.9),
        VM::vec4(2, -1, -1, 0.9),
        VM::vec4(2, -1, 2, 0.9),
        VM::vec4(-1, -1, -1, 0.9),
        VM::vec4(2, -1, 2, 0.9),
        VM::vec4(-1, -1, 2, 0.9),

        VM::vec4(-1, 1, -1, 0.9),
        VM::vec4(2, 1, -1, 0.9),
        VM::vec4(2, 1, 2, 0.9),
        VM::vec4(-1, 1, -1, 0.9),
        VM::vec4(2, 1, 2, 0.9),
        VM::vec4(-1, 1, 2, 0.9),

        VM::vec4(-1, -1, -1, 0.9),
        VM::vec4(-1, 1, -1, 0.9),
        VM::vec4(2, 1, -1, 0.9),
        VM::vec4(-1, -1, -1, 0.9),
        VM::vec4(2, 1, -1, 0.9),
        VM::vec4(2, -1, -1, 0.9),

        VM::vec4(-1, -1, 2, 0.9),
        VM::vec4(-1, 1, 2, 0.9),
        VM::vec4(2, 1, 2, 0.9),
        VM::vec4(-1, -1, 2, 0.9),
        VM::vec4(2, 1, 2, 0.9),
        VM::vec4(2, -1, 2, 0.9),

        VM::vec4(-1, -1, -1, 0.9),
        VM::vec4(-1, 1, -1, 0.9),
        VM::vec4(-1, 1, 2, 0.9),
        VM::vec4(-1, -1, -1, 0.9),
        VM::vec4(-1, 1, 2, 0.9),
        VM::vec4(-1, -1, 2, 0.9),

        VM::vec4(2, -1, -1, 0.9),
        VM::vec4(2, 1, -1, 0.9),
        VM::vec4(2, 1, 2, 0.9),
        VM::vec4(2, -1, -1, 0.9),
        VM::vec4(2, 1, 2, 0.9),
        VM::vec4(2, -1, 2, 0.9),
    };

    vector<VM::vec2> textureCoords = {
        VM::vec2(0, 0),
        VM::vec2(0, 1),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(1, 0),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(0, 1),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(1, 0),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(0, 1),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(1, 0),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(0, 1),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(1, 0),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(0, 1),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(1, 0),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(0, 1),
        VM::vec2(1, 1),
        VM::vec2(0, 0),
        VM::vec2(1, 0),
        VM::vec2(1, 1),
    };
    skyShader = GL::CompileShaderProgram("sky");
    glGenVertexArrays(1, &skyVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(skyVAO);                                                CHECK_GL_ERRORS


    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint index = glGetAttribLocation(skyShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS


    glGenTextures(1, &skyTexture);                                             CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, skyTexture);                                  CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skyWidth, skyHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, skyImage);
    glGenerateMipmap(GL_TEXTURE_2D);                                             CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);  CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                CHECK_GL_ERRORS


    GLuint textureBuffer;
    glGenBuffers(1, &textureBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint textureLocation = glGetAttribLocation(skyShader, "texture");        CHECK_GL_ERRORS
    glEnableVertexAttribArray(textureLocation);                                  CHECK_GL_ERRORS
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);         CHECK_GL_ERRORS


    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

void CreateWater() {
    waterImage = SOIL_load_image("Texture/water.jpg", &waterWidth, &waterHeight, 0, SOIL_LOAD_RGBA);
    vector<VM::vec4> meshPoints = {
        VM::vec4(5.8, -0.5, 0.6, 9),
        VM::vec4(5.8, -0.5, 4, 9),
        VM::vec4(9.2, -0.5, 4, 9),
        VM::vec4(5.8, -0.5, 0.6, 9),
        VM::vec4(9.2, -0.5, 0.6, 9),
        VM::vec4(9.2, -0.5, 4, 9),
    };
    vector<VM::vec2> textureCoords = {
        VM::vec2(5.8, 0.6),
        VM::vec2(5.8, 4),
        VM::vec2(9.2, 4),
        VM::vec2(5.8, 4),
        VM::vec2(9.2, 0.6),
        VM::vec2(9.2, 4),
    };

    waterShader = GL::CompileShaderProgram("water");
    glGenVertexArrays(1, &waterVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(waterVAO);                                                CHECK_GL_ERRORS


    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint index = glGetAttribLocation(waterShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS


    glGenTextures(1, &waterTexture);                                             CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, waterTexture);                                  CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, waterWidth, waterHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, waterImage);
    glGenerateMipmap(GL_TEXTURE_2D);                                             CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);  CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                CHECK_GL_ERRORS


    GLuint textureBuffer;
    glGenBuffers(1, &textureBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint textureLocation = glGetAttribLocation(waterShader, "texture");        CHECK_GL_ERRORS
    glEnableVertexAttribArray(textureLocation);                                  CHECK_GL_ERRORS
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);         CHECK_GL_ERRORS


    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

void CreateRock() {
    rockImage = SOIL_load_image("Texture/rock.jpg", &rockWidth, &rockHeight, 0, SOIL_LOAD_RGBA);
    vector<VM::vec4> meshPoints = GenMeshRock();
    vector<VM::vec3> rockPositions = GenerateRockPositions();
    vector<VM::vec2> textureCoords;
    for (int i = 0; i < 100 * 3; i++) {
        textureCoords.push_back(VM::vec2(0, 0));
        textureCoords.push_back(VM::vec2(0, 1));
        textureCoords.push_back(VM::vec2(1, 1));
        textureCoords.push_back(VM::vec2(0, 0));
        textureCoords.push_back(VM::vec2(1, 0));
        textureCoords.push_back(VM::vec2(1, 1));
    }
    rockShader = GL::CompileShaderProgram("rock");
    glGenVertexArrays(1, &rockVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(rockVAO);                                                CHECK_GL_ERRORS


    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint index = glGetAttribLocation(rockShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS


    glGenTextures(1, &rockTexture);                                             CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, rockTexture);                                  CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rockWidth, rockHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, rockImage);
    glGenerateMipmap(GL_TEXTURE_2D);                                             CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);  CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                CHECK_GL_ERRORS


    GLuint textureBuffer;
    glGenBuffers(1, &textureBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint textureLocation = glGetAttribLocation(rockShader, "texture");        CHECK_GL_ERRORS
    glEnableVertexAttribArray(textureLocation);                                  CHECK_GL_ERRORS
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);         CHECK_GL_ERRORS


    // Создаём буфер для позиций травинок
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);                                            CHECK_GL_ERRORS
    // Здесь мы привязываем новый буфер, так что дальше вся работа будет с ним до следующего вызова glBindBuffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec3) * rockPositions.size(), rockPositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint positionLocation = glGetAttribLocation(rockShader, "position");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(positionLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    // Здесь мы указываем, что нужно брать новое значение из этого буфера для каждого инстанса (для каждой травинки)
    glVertexAttribDivisor(positionLocation, 1);                                  CHECK_GL_ERRORS


    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

void CreateTree() {
    treeImage = SOIL_load_image("Texture/tree.jpg", &treeWidth, &treeHeight, 0, SOIL_LOAD_RGBA);
    vector<VM::vec4> meshPoints = GenMeshTree();
    treePointsCount = meshPoints.size();
    vector<VM::vec3> treePositions = GenerateTreePositions();
    vector<VM::vec2> textureCoords;
    for (uint i = 0; i < treePointsCount; i++) {
        textureCoords.push_back(VM::vec2(0, 0));
        textureCoords.push_back(VM::vec2(0, 1));
        textureCoords.push_back(VM::vec2(1, 1));
        textureCoords.push_back(VM::vec2(0, 0));
        textureCoords.push_back(VM::vec2(1, 0));
        textureCoords.push_back(VM::vec2(1, 1));
    }
    treeShader = GL::CompileShaderProgram("tree");
    glGenVertexArrays(1, &treeVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(treeVAO);                                                CHECK_GL_ERRORS


    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint index = glGetAttribLocation(treeShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS


    glGenTextures(1, &treeTexture);                                             CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, treeTexture);                                  CHECK_GL_ERRORS
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, treeWidth, treeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, treeImage);
    glGenerateMipmap(GL_TEXTURE_2D);                                             CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);  CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                CHECK_GL_ERRORS
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                CHECK_GL_ERRORS


    GLuint textureBuffer;
    glGenBuffers(1, &textureBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint textureLocation = glGetAttribLocation(treeShader, "texture");        CHECK_GL_ERRORS
    glEnableVertexAttribArray(textureLocation);                                  CHECK_GL_ERRORS
    glVertexAttribPointer(textureLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);         CHECK_GL_ERRORS


    // Создаём буфер для позиций травинок
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);                                            CHECK_GL_ERRORS
    // Здесь мы привязываем новый буфер, так что дальше вся работа будет с ним до следующего вызова glBindBuffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);                               CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec3) * treePositions.size(), treePositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS
    GLuint positionLocation = glGetAttribLocation(treeShader, "position");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(positionLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
    // Здесь мы указываем, что нужно брать новое значение из этого буфера для каждого инстанса (для каждой травинки)
    glVertexAttribDivisor(positionLocation, 1);                                  CHECK_GL_ERRORS


    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

int main(int argc, char **argv)
{
    try {
        loadheightmap();
        cout << "Start" << endl;
        InitializeGLUT(argc, argv);
        cout << "GLUT inited" << endl;
        glewInit();
        cout << "glew inited" << endl;
        CreateCamera();
        cout << "Camera created" << endl;
        CreateGrass();
        cout << "Grass created" << endl;
        CreateGround();
        cout << "Ground created" << endl;
        CreateSky();
        cout << "Sky created" << endl;
        CreateWater();
        cout << "Water created" << endl;
        CreateRock();
        cout << "Rock created" << endl;
        CreateTree();
        cout << "Tree created" << endl;
        glutMainLoop();
    } catch (string s) {
        cout << s << endl;
    }
}
