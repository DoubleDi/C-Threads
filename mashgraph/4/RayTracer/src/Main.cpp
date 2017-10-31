#include "Tracer.h"
#include "stdio.h"

int main(int argc, char **argv)
{
    CTracer tracer;
    CScene scene;

    int xRes = 1024;
    int yRes = 768;
    int omp = 0;

    if(argc == 2) {
        FILE* file = fopen(argv[1], "r");

        if(file) {
            int xResFromFile = 0;
            int yResFromFile = 0;
            int ompFile = 0;

            if(fscanf(file, "%d %d %d", &xResFromFile, &yResFromFile, &omp) == 2) {
                xRes = xResFromFile;
                yRes = yResFromFile;
                omp = ompFile;
            } else
                printf("Invalid config format! Using default parameters.\r\n");

            fclose(file);
        } else
            printf("Invalid config path! Using default parameters.\r\n");
    } else
        printf("No config! Using default parameters.\r\n");

    tracer.m_pScene = &scene;
    tracer.RenderImage(xRes, yRes, omp);
    tracer.SaveImageToFile("Result.bmp");
}
