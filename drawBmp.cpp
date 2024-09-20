#include <iostream>
#include <fstream>
#include <string>
#include <windows.h> // Для BITMAPFILEHEADER и BITMAPINFOHEADER

using namespace std;

class BMPReader {
private:
    ifstream bmpFile;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    unsigned char* pixelData = nullptr;

public:
    // Открываем BMP файл
    bool openBMP(const string& fileName) {
        bmpFile.open(fileName, ios::binary);
        if (!bmpFile.is_open()) {
            cerr << "Error: Could not open file " << fileName << endl;
            return false;
        }

        // Чтение заголовков BMP файла
        bmpFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(BITMAPFILEHEADER));
        bmpFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER));

        // Проверка корректности формата BMP
        if (fileHeader.bfType != 0x4D42) {
            cerr << "Error: This is not a valid BMP file." << endl;
            return false;
        }

        // Поддерживаются только 24 и 32-битные BMP файлы
        if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
            cerr << "Error: Only 24 or 32-bit BMP files are supported." << endl;
            return false;
        }

        // Выделяем память для пикселей
        pixelData = new unsigned char[infoHeader.biSizeImage];
        bmpFile.seekg(fileHeader.bfOffBits, ios::beg);
        bmpFile.read(reinterpret_cast<char*>(pixelData), infoHeader.biSizeImage);

        return true;
    }

    // Отображаем BMP файл в консоли
    void displayBMP() {
        if (!pixelData) {
            cerr << "Error: No BMP data to display." << endl;
            return;
        }

        // Переходим по строкам с конца, так как BMP хранит их в обратном порядке
        int rowSize = ((infoHeader.biWidth * infoHeader.biBitCount + 31) / 32) * 4;
        for (int y = 0; y < infoHeader.biHeight; ++y) {
            for (int x = 0; x < infoHeader.biWidth; ++x) {
                // Находим начало текущего пикселя
                int pixelIndex = (infoHeader.biHeight - 1 - y) * rowSize + x * (infoHeader.biBitCount / 8);
                unsigned char b = pixelData[pixelIndex];
                unsigned char g = pixelData[pixelIndex + 1];
                unsigned char r = pixelData[pixelIndex + 2];

                // Выводим символ в зависимости от цвета пикселя
                if (r == 0 && g == 0 && b == 0) {
                    cout << "#"; // Черный цвет
                } else if (r == 255 && g == 255 && b == 255) {
                    cout << " "; // Белый цвет (фон)
                } else {
                    cout << "?"; // Неизвестный цвет
                }
            }
            cout << endl; // Переход на новую строку после вывода строки пикселей
        }
    }

    // Закрываем BMP файл и освобождаем ресурсы
    void closeBMP() {
        if (bmpFile.is_open()) {
            bmpFile.close();
        }
        if (pixelData) {
            delete[] pixelData;
            pixelData = nullptr;
        }
    }

    ~BMPReader() {
        closeBMP(); // Освобождение ресурсов при разрушении объекта
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: drawBmp.exe <file_path>" << endl;
        return 1;
    }

    string fileName = argv[1];
    BMPReader bmpReader;

    if (bmpReader.openBMP(fileName)) {
        bmpReader.displayBMP();
    }

    bmpReader.closeBMP();
    return 0;
}
