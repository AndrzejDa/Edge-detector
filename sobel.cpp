#include <iostream>
#include <fstream>

using namespace std;

struct BITMAPFILEHEADER{
	unsigned short int bfType = 0x4D42;          // Sygnatura pliku, ‘BM’ dla prawidłowego pliku BMP
    unsigned int bfSize = 0;               // Długość całego pliku w bajtach
    unsigned short int bfReserved1 = 0;               // Pole zarezerwowane (zwykle zero)
    unsigned short int bfReserved2 = 0;               // Pole zarezerwowane (zwykle zero)
    unsigned int bfOffBits = 0;             // Pozycja danych obrazowych w pliku
}; 

struct BITMAPINFOHEADER{
	unsigned int biSize = 0;                      // Rozmiar nagłówka informacyjnego
	int biWidth = 0;                      // Szerokość obrazu w pikselach
	int biHeight = 0;                     // Wysokość obrazu w pikselach
	unsigned short int biPlanes = 1;                    // Liczba płatów (musi być 1)
	unsigned short int biBitCount = 0;                 // Liczba bitów na piksel: 1, 4, 8, 16, 24 lub 32
	unsigned int biCompression = 0;               // Algorytm kompresji: BI_RGB=0, BI_RLE8=1, BI_RLE4=2, BI_BITFIELDS=3
	unsigned int biSizeImage = 0;                // Rozmiar rysunku
	int biXpelsPerMeter = 0;					//Rozdzielczość pozioma
	int biYpelsPerMeter = 0;					//Rozdzielczość pionowa
	unsigned int biCrlUses = 0;               // Liczba kolorów w palecie
	unsigned int biCrlImportant = 0;          // Liczba ważnych kolorów w palecie
};

short sobel_mask[8][3][3] = {
	{{-1, 0, 1},
	{-2, 0, 2},		//0 stopni
	{-1, 0, 1}},
	
	{{0, 1, 2},
	{-1, 0, 1},		//45 stopni
	{-2, -1, 0}},
	
	{{1, 2, 1},
	{0, 0, 0},		//90 stopni
	{-1, -2, -1}},
	
	{{2, 1, 0},
	{1, 0, -1},		//135 stopni
	{0, -1, -2}},
	
	{{1, 0, -1},
	{2, 0, -2},		//180 stopni
	{1, 0, -1}},
	
	{{0, -1, -2},
	{1, 0, -1},		//225 stopni
	{2, 1, 0}},
	
	{{-1, -2, -1},
	{0, 0, 0},		//270 stopni
	{1, 2, 1}},
	
	{{-2, -1, 0},
	{-1, 0, 1},		//315 stopni
	{0, 1, 2}}	
};


int readBMIH(ifstream &ifs, BITMAPINFOHEADER &bfh);
int readBMFH(ifstream &ifs, BITMAPFILEHEADER &bfh);
int writeBMIH(ofstream &ofs, BITMAPINFOHEADER &bfh);
int writeBMFH(ofstream &ofs, BITMAPFILEHEADER &bfh);

int main(){
	char name[15];
	char name2[15];
	BITMAPFILEHEADER bmp_file_header;	//naglowek pliku wejsciowego
	BITMAPINFOHEADER bmp_info_header;	//naglowek obrazu w pliku wejsciowym
	BITMAPFILEHEADER bmp2_file_header;	//naglowek pliku wyjsciowego
	BITMAPINFOHEADER bmp2_info_header;	//naglowek obrazu w pliku wyjsciowym
	
	cout << "podaj nazwe pliku: ";
	cin >> name;
	cin.ignore(200, '\n');
	cout << endl;
	
	ifstream file(name, ios::in | ios::binary);
	if(!file){
		cout <<"nieudane otwarcie pliku" << endl;
		return 0;
	}
	cout << "plik do odczytu" << endl;
	readBMFH(file, bmp_file_header);
	if(bmp_file_header.bfType != 0x4D42){
		cout << "zla sygnatura" << endl;
		return 0;
	}else{
		cout << "zgodna sygnatura " << endl << endl;
	}
	//wypisanie parametrow pliku wejsciowego
	cout << "dane BMFileHeader:" << endl;
	cout << "sygnatura pliku " << bmp_file_header.bfType << endl;
	cout << "dlugosc calego pliku w bajtach " << bmp_file_header.bfSize << endl;
	cout << "pole zarezerwowane 1 " << bmp_file_header.bfReserved1 << endl;
	cout << "pole zarezerwowane 2 " << bmp_file_header.bfReserved2 << endl;
	cout << "pozycja danych obrazowych pliku " << bmp_file_header.bfOffBits << endl << endl;
	
	readBMIH(file, bmp_info_header);
	cout << "dane BMInfoHeader:" << endl;
	cout << "rozmiar naglowka informacyjnego " << bmp_info_header.biSize << endl;
	cout << "szerokosc obrazu w pikselach " << bmp_info_header.biWidth << endl;
	cout << "wysokosc obrazu w pikselach " << bmp_info_header.biHeight << endl;
	cout << "liczba platow " << bmp_info_header.biPlanes << endl;
	cout << "liczba bitow na piksel " << bmp_info_header.biBitCount << endl;
	cout << "algorytm kompresji " << bmp_info_header.biCompression << endl;
	cout << "rozmiar rysunku " << bmp_info_header.biSizeImage << endl;
	cout << "rozdzielczosc pozioma " << bmp_info_header.biXpelsPerMeter << endl;
	cout << "rozdzielczosc pionowa " << bmp_info_header.biYpelsPerMeter << endl;
	cout << "liczba kolorow w palecie " << bmp_info_header.biCrlUses << endl;
	cout << "liczba waznych kolorow w palecie " << bmp_info_header.biCrlImportant << endl;
		
	
	file.seekg(bmp_file_header.bfOffBits, file.beg);
	
	//odczyt danych obrazowych 
	int y = bmp_info_header.biHeight;
	int x = bmp_info_header.biSizeImage / bmp_info_header.biHeight;
	 
	int **Tab;	
	Tab = new int* [y];
	
	for(int i = 0; i < y; i++){
		Tab[i] =  new int [x];
	}
	
	//czyszczenie wejsciowej tablicy dynamicznej
	for(int i = 0; i < y; i++){
		for(int j = 0; j < x; j++){
			Tab[i][j] = 0;
		}
	}
	
	
	//odczyt
	for(int i = 0; i < y; i++){
		for(int j = 0; j < x; j++){
			file.read(reinterpret_cast<char*>(&Tab[i][j]),1);
			if (Tab[i][j] > 255) Tab[i][j] = 0;
		}
	}
	file.close();	
	
	int **Out;
	Out = new int* [y];
	
	for(int i = 0; i < y; i++){
		Out[i] = new int [x];
	}
	
	int i, a, b, j, sum = 0;
	short max = 255;
	short min = 0;
	
	//czyszczenie wyjsciowej tablicy dynamicznej
	for(i=0; i<y; i++){
		for(j=0; j<x; j++){
			Out[i][j] = 0;
		}
	}
		
			
	
	//operacja konwolucji     niezaleznie dla B G R
	for(i = 1; i < y - 1; i++){
		for(j = 3; j < x - 5; 3*(j++)){
			for(int k = 0; k < 8; k++){
				sum = 0;
				for(a =- 1; a < 2; a++){
					for(b =- 1; b < 2; b++){
						sum = sum + Tab[i + a][j + 3*b] * sobel_mask[k][a + 1][b + 1];
					}
				}
				if(sum > max) sum = max;
				if(sum < 0) sum = 0;
				
				if(sum > Out[i][j])
				Out[i][j] = sum;
			}
		} 
	} 
	
	for(i = 1; i < y - 1; i++){
		for(j = 4; j < x - 4; 3*(j++)){
			for(int k = 0; k < 8; k++){
				sum = 0;
				for(a =- 1; a < 2; a++){
					for(b =- 1; b < 2; b++){
						sum = sum + Tab[i + a][j + 3*b] * sobel_mask[k][a + 1][b + 1];
					}
				}
				if(sum > max) sum = max;
				if(sum < 0) sum = 0;
				
				if(sum > Out[i][j])
				Out[i][j] = sum;
			}
		} 
	}
	
	for(i = 1; i < y - 1; i++){
		for(j = 5; j < x - 3; 3*(j++)){
			for(int k = 0; k < 8; k++){
				sum = 0;
				for(a =- 1; a < 2; a++){
					for(b =- 1; b < 2; b++){
						sum = sum + Tab[i + a][j + 3*b] * sobel_mask[k][a + 1][b + 1];
					}
				}
				if(sum > max) sum = max;
				if(sum < 0) sum = 0;
				
				if(sum > Out[i][j])
				Out[i][j] = sum;
			}
		} 
	}
	
	
	
	cout << "podaj nazwe pliku do zapisu: ";
	cin >> name2;
	cin.ignore(200, '\n');
	ofstream file2(name2, ios::out | ios::binary);
	if(!file2){
		cout <<"nieudane otwarcie pliku" << endl;
		return 0;
	}
	cout << "plik gotowy do zapisu" << endl << endl;
	writeBMFH(file2, bmp_file_header);
	writeBMIH(file2, bmp_info_header);
	for(int i = 0; i < y; i++){
		for(int j = 0; j < x; j++){
			file2.write(reinterpret_cast<char*>(&Out[i][j]),1);
		}
	}
	cout << "plik zapisany pomyslnie" << endl;
	
	for(int i = 0; i < y; i++){
		delete[] Tab[i];
		Tab[i] = NULL;
	}
	delete[] Tab;
	Tab = NULL;
	
	for(int i = 0; i < y; i++){
		delete[] Out[i];
		Out[i] = NULL;
	}
	delete[] Out;
	Out = NULL;

	file2.close();
	return 0;
}

int readBMFH(ifstream &ifs, BITMAPFILEHEADER &bfh){
	ifs.read(reinterpret_cast<char*>(&bfh.bfType), 2);
	ifs.read(reinterpret_cast<char*>(&bfh.bfSize), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.bfReserved1), 2);
	ifs.read(reinterpret_cast<char*>(&bfh.bfReserved2), 2);
	ifs.read(reinterpret_cast<char*>(&bfh.bfOffBits), 4);
	return ifs.tellg();		
}

int readBMIH(ifstream &ifs, BITMAPINFOHEADER &bfh){
	ifs.read(reinterpret_cast<char*>(&bfh.biSize), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.biWidth), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.biHeight), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.biPlanes), 2);
	ifs.read(reinterpret_cast<char*>(&bfh.biBitCount), 2);
	ifs.read(reinterpret_cast<char*>(&bfh.biCompression), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.biSizeImage), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.biXpelsPerMeter), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.biYpelsPerMeter), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.biCrlUses), 4);
	ifs.read(reinterpret_cast<char*>(&bfh.biCrlImportant), 4);
	return ifs.tellg();		
}

int writeBMFH(ofstream &ofs, BITMAPFILEHEADER &bfh){
	ofs.write(reinterpret_cast<char*>(&bfh.bfType), 2);
	ofs.write(reinterpret_cast<char*>(&bfh.bfSize), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.bfReserved1), 2);
	ofs.write(reinterpret_cast<char*>(&bfh.bfReserved2), 2);
	ofs.write(reinterpret_cast<char*>(&bfh.bfOffBits), 4);
	
}

int writeBMIH(ofstream &ofs, BITMAPINFOHEADER &bfh){
	ofs.write(reinterpret_cast<char*>(&bfh.biSize), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.biWidth), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.biHeight), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.biPlanes), 2);
	ofs.write(reinterpret_cast<char*>(&bfh.biBitCount), 2);
	ofs.write(reinterpret_cast<char*>(&bfh.biCompression), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.biSizeImage), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.biXpelsPerMeter), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.biYpelsPerMeter), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.biCrlUses), 4);
	ofs.write(reinterpret_cast<char*>(&bfh.biCrlImportant), 4);
	
}
