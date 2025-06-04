// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <fstream>

using namespace std;

Mat_<uchar> binarizare(Mat_<uchar> img) {
	Mat_<uchar> dest(img.rows, img.cols);
	int prag = 100;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) < prag) {
				dest(i, j) = 0;
			}
			else if (img(i, j) >= prag) {
				dest(i, j) = 255;
			}
		}
	}
	//imshow("Imaginea binarizata", dest);
	//waitKey();
	return dest;
}

void compresie(Mat_<uchar> img) {
	ofstream file("Images/output.bin", ios::binary);
	if (!file.is_open()) {
		cerr << "Nu s-a putut deschide fisierul!\n";
		return;
	}
	file.write((char*)(&img.rows), sizeof(img.rows));
	file.write((char*)(&img.cols), sizeof(img.cols));
	long contor = 0;
	uchar pixel_anterior = img(0, 0);
	char culoare = 'N'; //initializez cu o valoare random
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) == pixel_anterior) {
				contor++;
				pixel_anterior = img(i, j);
			}
			else {
				//s-a detectat un pixel de culoare diferita
				if (pixel_anterior == 0) {
					culoare = 'B';
				}
				else if (pixel_anterior == 255) {
					culoare = 'W';
				}
				file.write((char*)(&contor), sizeof(contor));
				file.write(&culoare, sizeof(culoare));
				contor = 1; //se aduna si pixelul nou de alta culoare
				pixel_anterior = img(i, j);
			}
		}
	}
	if (pixel_anterior == 0) {
		culoare = 'B';
	}
	else if (pixel_anterior == 255) {
		culoare = 'W';
	}
	file.write((char*)(&contor), sizeof(contor));
	file.write(&culoare, sizeof(culoare));

	file.close();
}

void decompresie() {
	//deschid fisierul binar
	ifstream file("Images/output.bin", ios::binary);
	if (!file.is_open()) {
		cerr << "Nu s-a putut deschide fisierul!\n";
		return;
	}

	int rows = 0, cols = 0;
	file.read((char*)(&rows), sizeof(rows));
	file.read((char*)(&cols), sizeof(cols));
	//cout << rows<<" "<<cols;
	Mat_<uchar> dest(rows, cols);
	char culoare = 'N'; //initializez cu o valoare random
	long contor = 0;
    auto iterator = dest.begin(); //pointer la fiecare pixel al imaginii destinatie
	while (file.read((char *)&contor, sizeof(contor))) {
		file.read(&culoare, sizeof(culoare));
		while (contor > 0) {
			if (culoare == 'W') {
				*iterator = 255;
			}
			else if (culoare == 'B') {
				*iterator = 0;
			}
			iterator++;
			contor--;
		}
	}

	file.close();
	imshow("Imaginea dupa decompresie", dest);
	waitKey();
}

void rata_de_compresie(string path) {
	//aflarea dimensiunii fisierului imaginii originale
	ifstream file_img_org(path, ios::binary);
	if (!file_img_org.is_open()) {
		cout << "Nu s - a putut deschide fisierul!\n";
		return;
	}

	// setez pozitia cursorului la sfarsitul fisierului
	file_img_org.seekg(0, ios::end);

	// dimensiunea efectiva a fisierului in bytes
	std::streampos fileSizeImgOrg = file_img_org.tellg();
	cout << "Dimensiunea fisierului bmp al imaginii orgininale: " << fileSizeImgOrg << endl;
	file_img_org.close();

	//aflarea dimensiunii fisierului obtinut dupa compresie
	ifstream file_img_comprimata("Images/output.bin", ios::binary);
	if (!file_img_comprimata.is_open()) {
		cout << "Nu s - a putut deschide fisierul!\n";
		return;
	}

	// setez pozitia cursorului la sfarsitul fisierului
	file_img_comprimata.seekg(0, ios::end);

	// dimensiunea efectiva a fisierului in bytes
	std::streampos fileSizeImgComprimata = file_img_comprimata.tellg();
	cout << "Dimensiunea fisierului bin al imaginii comprimate: " << fileSizeImgComprimata << endl;
	file_img_comprimata.close();

	//rata
	cout << "Rata de compresie: " << fileSizeImgOrg / fileSizeImgComprimata;

}

int main() {
	string path = "Images/img_4.bmp";
	Mat_<uchar> img1 = imread(path, 0);
	Mat_<uchar> img_binarizata = binarizare(img1);
	compresie(img_binarizata);
	imshow("Imaginea originala", img1);
	decompresie();
	rata_de_compresie(path);

	return 0;

}