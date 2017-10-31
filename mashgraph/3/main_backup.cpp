#include <iostream>
#include <vector>

#include "Utility.h"
#include <SOIL.h>
#include "EasyBMP/include/EasyBMP.h"

using namespace std;
GLenum DEFAULT_TEXTURE_WRAP = GL_REPEAT;

const uint GRASS_INSTANCES = 10000; // Количество травинок

GL::Camera camera;               // Мы предоставляем Вам реализацию камеры. В OpenGL камера - это просто 2 матрицы. Модельно-видовая матрица и матрица проекции. // ###
                                 // Задача этого класса только в том чтобы обработать ввод с клавиатуры и правильно сформировать эти матрицы.
                                 // Вы можете просто пользоваться этим классом для расчёта указанных матриц.

int multisampling = 0;
int supersampling = 0;
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
vector<vector<float>> groundHeightMap{};
int groundWidth, groundHeight;
unsigned char* groundImage;

GLuint skyShader; // Шейдер для земли
GLuint skyVAO; // VAO для земли
GLuint skyTexture;
int skyWidth, skyHeight;
unsigned char* skyImage;

// Размеры экрана
uint screenWidth = 800;
uint screenHeight = 600;

// Это для захвата мышки. Вам это не потребуется (это не значит, что нужно удалять эту строку)
bool captureMouse = true;


void loadheightmap() {
    cout << "begin" << endl;
    groundHeightMap.resize(1000);
    for (int i = 0; i < 1000; i++) {
        groundHeightMap[i].resize(1000);
    }
    BMP* image = new BMP();
    image->ReadFromFile("groundHeightMap.bmp");
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            groundHeightMap[i][j] = (image->GetPixel(i, j).Red + image->GetPixel(i, j).Green + image->GetPixel(i, j).Blue) / 768.0;
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
    // glBindVertexArray(groundVAO);                                                CHECK_GL_ERRORS
    for (uint i = 0; i < groundHeightMap.size() - 1; i++) {
        for (uint j = 0; j < groundHeightMap[0].size() - 1; j++) {
            glBegin(GL_TRIANGLE_STRIP);
                glVertex4f(i * 0.1, groundHeightMap[i][j] * 10, j*0.1, 1);
                glVertex4f((i + 1) * 0.1, groundHeightMap[i + 1][j] * 10, j*0.1, 1);
                glVertex4f(i * 0.1, groundHeightMap[i][j + 1] * 10, (j + 1)*0.1, 1);
                glVertex4f((i + 1) * 0.1, groundHeightMap[i + 1][j + 1] * 10, (j + 1)*0.1, 1);
            glEnd();
        }
    }
    // // Рисуем землю: 2 треугольника (6 вершин)
    // glDrawArrays(GL_TRIANGLES, 0, 6);                                            CHECK_GL_ERRORS

    // Отсоединяем VAO
    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    // glBindVertexArray(0);                                                        CHECK_GL_ERRORS
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

// Обновление смещения травинок
int steps = 0;
void UpdateGrassVariance() {
    float delta_t = 1/20.0;
    // Генерация случайных смещений
        for (uint i = 0; i < GRASS_INSTANCES; ++i) {
            grassVarianceAcceleration[i] = (grassVarianceData[i].x - 0.014) / ( 2.0);
            grassVarianceSpeed[i] = grassVarianceSpeed[i] - grassVarianceAcceleration[i] * delta_t - ((float)rand() / RAND_MAX - 0.5) / 3000.0;
            grassVarianceData[i].x += grassVarianceSpeed[i] * delta_t;
            grassVarianceData[i].z += grassVarianceSpeed[i] * delta_t;
        }
    steps++;
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
    DrawGround();
    DrawGrass();
    glutSwapBuffers();
}

// Завершение программы
void FinishProgram() {
    SOIL_free_image_data(grassImage);
    SOIL_free_image_data(groundImage);
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
        supersampling = !supersampling;
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
vector<VM::vec2> GenerateGrassPositions() {
    vector<VM::vec2> grassPositions(GRASS_INSTANCES);
    for (uint i = 0; i < GRASS_INSTANCES; ++i) {
        grassPositions[i] = VM::vec2((i % int(sqrt(GRASS_INSTANCES))) / sqrt(GRASS_INSTANCES) + ((float)rand() / RAND_MAX / 10), (i / int(sqrt(GRASS_INSTANCES))) / sqrt(GRASS_INSTANCES) + ((float)rand() / RAND_MAX / 10));
    }
    return grassPositions;
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

// Создание травы
void CreateGrass() {
    uint LOD = 1;
    grassImage = SOIL_load_image("Texture/grass.jpg", &grassWidth, &grassHeight, 0, SOIL_LOAD_RGB);
    // Создаём меш
    vector<VM::vec4> grassPoints = GenMesh(LOD);
    // Сохраняем количество вершин в меше травы
    grassPointsCount = grassPoints.size();
    // Создаём позиции для травинок
    vector<VM::vec2> grassPositions = GenerateGrassPositions();
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

    // Здесь создаём буфер
    GLuint pointsBuffer;
    // Это генерация одного буфера (в pointsBuffer хранится идентификатор буфера)
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    // Привязываем сгенерированный буфер
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    // Заполняем буфер данными из вектора
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * grassPoints.size(), grassPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    // Создание VAO
    // Генерация VAO
    glGenVertexArrays(1, &grassVAO);                                             CHECK_GL_ERRORS
    // Привязка VAO
    glBindVertexArray(grassVAO);                                                 CHECK_GL_ERRORS

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec2) * grassPositions.size(), grassPositions.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    GLuint positionLocation = glGetAttribLocation(grassShader, "position");      CHECK_GL_ERRORS
    glEnableVertexAttribArray(positionLocation);                                 CHECK_GL_ERRORS
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);        CHECK_GL_ERRORS
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
    loadheightmap();
    if (!supersampling) {
        groundImage = SOIL_load_image("Texture/ground.jpg", &groundWidth, &groundHeight, 0, SOIL_LOAD_RGBA);
        normalgroundWidth = groundWidth;
        normalgroundHeight = groundHeight;
    } else {
        groundImage = SOIL_load_image("Texture/ground_large.jpg", &groundWidth, &groundHeight, 0, SOIL_LOAD_RGBA);
    }
    // Земля состоит из двух треугольников
    vector<VM::vec4> meshPoints = {
        VM::vec4(0, 0, 0, 0.9),
        VM::vec4(1, 0, 0, 0.9),
        VM::vec4(1, 0, 1, 0.9),
        VM::vec4(0, 0, 0, 0.9),
        VM::vec4(1, 0, 1, 0.9),
        VM::vec4(0, 0, 1, 0.9),
    };

    // Подробнее о том, как это работает читайте в функции CreateGrass

    groundShader = GL::CompileShaderProgram("ground");

    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    glGenVertexArrays(1, &groundVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(groundVAO);                                                CHECK_GL_ERRORS

    GLuint index = glGetAttribLocation(groundShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS
    for (uint i = 0; i < groundHeightMap.size() - 1; i++) {
        for (uint j = 0; j < groundHeightMap[0].size() - 1; j++) {
            glBegin(GL_TRIANGLE_STRIP);
                glVertex4f(i * 0.1, groundHeightMap[i][j] * 10, j*0.1, 1);
                glVertex4f((i + 1) * 0.1, groundHeightMap[i + 1][j] * 10, j*0.1, 1);
                glVertex4f(i * 0.1, groundHeightMap[i][j + 1] * 10, (j + 1)*0.1, 1);
                glVertex4f((i + 1) * 0.1, groundHeightMap[i + 1][j + 1] * 10, (j + 1)*0.1, 1);
            glEnd();
        }
    }
    glGenTextures(1, &groundTexture);                                            CHECK_GL_ERRORS
    glBindTexture(GL_TEXTURE_2D, groundTexture);                                 CHECK_GL_ERRORS
    if (supersampling) {
        // unsigned char * newgroundImage;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, groundWidth, groundHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, groundImage);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                CHECK_GL_ERRORS
    glGenerateMipmap(GL_TEXTURE_2D);
    GLuint texture = glGetAttribLocation(groundShader, "texture");               CHECK_GL_ERRORS
    glEnableVertexAttribArray(texture);                                          CHECK_GL_ERRORS
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, 0);                 CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

void CreateSky() {
    skyTexture = SOIL_load_OGL_texture(
        "Texture/sky.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
    // Земля состоит из двух треугольников
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

    // Подробнее о том, как это работает читайте в функции CreateGrass

    skyShader = GL::CompileShaderProgram("sky");

    GLuint pointsBuffer;
    glGenBuffers(1, &pointsBuffer);                                              CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, pointsBuffer);                                 CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, sizeof(VM::vec4) * meshPoints.size(), meshPoints.data(), GL_STATIC_DRAW); CHECK_GL_ERRORS

    glGenVertexArrays(1, &skyVAO);                                            CHECK_GL_ERRORS
    glBindVertexArray(skyVAO);                                                CHECK_GL_ERRORS

    GLuint index = glGetAttribLocation(skyShader, "point");                   CHECK_GL_ERRORS
    glEnableVertexAttribArray(index);                                            CHECK_GL_ERRORS
    glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);                   CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, skyTexture);                                 CHECK_GL_ERRORS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);  // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // when texture area is large, bilinear filter the first mipmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // texture should tile
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glGenerateMipmap(GL_TEXTURE_2D);
    GLuint texture = glGetAttribLocation(skyShader, "texture");               CHECK_GL_ERRORS
    glEnableVertexAttribArray(texture);                                          CHECK_GL_ERRORS
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, 0);                 CHECK_GL_ERRORS

    glBindTexture(GL_TEXTURE_2D, 0);                                             CHECK_GL_ERRORS
    glBindVertexArray(0);                                                        CHECK_GL_ERRORS
    glBindBuffer(GL_ARRAY_BUFFER, 0);                                            CHECK_GL_ERRORS
}

int main(int argc, char **argv)
{
    try {
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
        glutMainLoop();
    } catch (string s) {
        cout << s << endl;
    }
}
