#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <fstream>

struct Edge {
    int u, v;
};

struct Vertex {
    int x, y;
};

const std::vector<std::vector<std::vector<bool>>> charTemplates = {
    {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0}
    },
    {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0}
    },
    {
        {1, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0}
    },
    {
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0}
    },
    {
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0}
    },
    {
        {0, 0, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 1, 0}
    },
    {
        {0, 1, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0}
    },
    {
        {0, 1, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0}
    },
    {
        {0, 1, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0}
    },
    {
        {0, 1, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0}
    },
};

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;

    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

void setPixel(uint8_t* image, int width, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || y < 0 || x >= width || y >= width) return; // avoid out of bounds
    int index = (y * width + x) * 3;
    image[index + 0] = b;
    image[index + 1] = g;
    image[index + 2] = r;
}

void drawLine(uint8_t* image, int width, int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {
        setPixel(image, width, x0, y0, r, g, b);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void drawCircle(uint8_t* image, int width, int xc, int yc, int r, uint8_t cr, uint8_t cg, uint8_t cb) {
    int x = r, y = 0;
    int radiusError = 1 - x;

    while (x >= y) {
        for (int i = -x; i <= x; i++) {
            setPixel(image, width, xc + i, yc + y, cr, cg, cb);
            setPixel(image, width, xc + i, yc - y, cr, cg, cb);
            setPixel(image, width, xc + y, yc + i, cr, cg, cb);
            setPixel(image, width, xc - y, yc + i, cr, cg, cb);
        }
        y++;
        if (radiusError < 0) {
            radiusError += 2 * y + 1;
        }
        else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

void saveBMP(const char* filename, uint8_t* image, int width, int height) {
    BMPHeader header;
    std::memset(&header, 0, sizeof(header));

    header.bfType = 0x4D42;
    header.bfSize = sizeof(BMPHeader) + width * height * 3;
    header.bfOffBits = sizeof(BMPHeader);

    header.biSize = 40;
    header.biWidth = width;
    header.biHeight = -height;
    header.biPlanes = 1;
    header.biBitCount = 24;
    header.biSizeImage = width * height * 3;

    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<char*>(&header), sizeof(header));
    file.write(reinterpret_cast<char*>(image), width * height * 3);
    file.close();
}

void drawDigit(uint8_t* image, int width, int x, int y, int digit, uint8_t r, uint8_t g, uint8_t b) {

    const std::vector<std::vector<bool>>& charTemplate = charTemplates[digit];

    for (size_t i = 0; i < charTemplate.size(); ++i) {
        for (size_t j = 0; j < charTemplate[i].size(); ++j) {
            if (charTemplate[i][j]) { // If the pixel is set in the template
                setPixel(image, width, x + j, y + i, r, g, b); // Draw the pixel
            }
        }
    }
}

void drawCircleWithNumber(uint8_t* image, int width, int xc, int yc, int r, int vertexNumber, uint8_t cr, uint8_t cg, uint8_t cb) {
    drawCircle(image, width, xc, yc, r, cr, cg, cb);

    char numberStr[5];
    sprintf_s(numberStr, "%d", vertexNumber);
    int digitCount = strlen(numberStr);
    int digitWidth = 9; 
    int totalWidth = digitWidth * digitCount;
    int startX = xc - totalWidth / 2;
    int startY = yc - 8;

    for (int i = 0; i < digitCount; ++i) {
        int digit = numberStr[i] - '0';
        drawDigit(image, width, startX + i * digitWidth, startY, digit, 0, 0, 0);
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    const int grid_size = 20;  
    const int num_vertices = grid_size * grid_size;
    const int width = 2560, height = 1480;
    const int cell_width = width / grid_size;
    const int cell_height = height / grid_size;

    std::vector<Vertex> vertices(num_vertices);
    int index = 0;
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            vertices[index] = { j * cell_width + cell_width / 2, i * cell_height + cell_height / 2 };
            ++index;
        }
    }

    std::vector<Edge> edges;
    std::ifstream edgeFile("graph_edges.txt");
    if (!edgeFile) {
        std::cerr << "Ошибка при открытии файла graph_edges.txt" << std::endl;
        return EXIT_FAILURE;
    }

    int u, v;
    while (edgeFile >> u >> v) {
        if (u < num_vertices && v < num_vertices) {
            edges.push_back({ u, v });
        }
    }

    uint8_t* image = new uint8_t[width * height * 3];
    std::memset(image, 255, width * height * 3); 

    for (int i = 0; i < num_vertices; ++i) {
        drawCircleWithNumber(image, width, vertices[i].x, vertices[i].y, 5, i, 255, 0, 0);
    }

    for (const auto& edge : edges) {
        drawLine(image, width, vertices[edge.u].x, vertices[edge.u].y, vertices[edge.v].x, vertices[edge.v].y, 0, 0, 0);
    }

    saveBMP("output_graph.bmp", image, width, height);

    delete[] image;

    std::cout << "Граф успешно записан в файл output_graph.bmp" << std::endl;
    return 0;
}
