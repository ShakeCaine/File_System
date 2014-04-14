#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <algorithm> 

using namespace std;

class File_System
{
public:
	File_System(void);
	~File_System(void);
private:

	class Catalog_Entry
	{
	public:
		string Name;
		int type;
		char flag;
		int location;
		int size;
	};
	//Klasa wpisu w katalogu
	
	class FilePosition_Entry
	{
	private:
		int First_adress;
		int Next_adress;
	public:
		FilePosition_Entry(void){};
		~FilePosition_Entry(void){};
		int returnFirstAdress()
		{
			return First_adress;
		}
		int returnSecondAdress()
		{
			return Next_adress;
		}
		void ChangeAdress(int i, int Adress)
		{
			if(i==0)
				First_adress=Adress;
			else
				Next_adress=Adress;
		}
	};
	//Klasa wpisu w tablicy FAT

	int SizeCatalog; //Wielko�� katalogu. Wi�ksza od 1, jak istnieje wpis.
	ofstream myfile; //Zmienna do odczytywania z pliku tekstowego
	ifstream myfileread; //Zmienna do zapisu do pliku tekstowego
	std::list<int> EmptyBlocksFAT; //Lista wolnych jednostek alokacji
	Catalog_Entry* FilesDescribtionCatalog; //Tablica wpis�w katalogowych
	FilePosition_Entry* FileAllocationTable; //Tablica FAT - 32 wpisy 
	string* AllocatedData; //Tablica jednostek alokacji plik�w- 32 wpisy po 32 bajty
	bool Update()
	{
		UpdateDataToFile();
		UpdateDataFromFile();
		return true;
	}
	//Zapis do pliku i odczyt z pliku
	bool UpdateDataToFile();
	bool UpdateDataFromFile();

	bool AllocateData(string name, int type, char flag, string data, int& error);
	//Rezerwacja i zapis danych do jednostek alokacji

public:

	bool Format();
	//Formatowanie dysku
	bool CheckEmptySpace(int length);
	//Przy podaniu d�ugo�ci danych sprawdza, czy jest wystarczaj�co miejsca

	bool CreateNewFile(string name, int type, char flag, string data, int& error);
	/*Tworzenie nowego pliku:
	name-Nazwa maks. 8 liter/cyfr
	type- rozszerzenie 0=exe, 1=txt
	flag- dwie flagi dost�pu do wyboru R i W (opisane ni�ej)
	data- dane w formie stringa do zapisania(jednostka alokacji po 32 bajty)
	int& error- zwraca numer b��du(opisane poni�ej)*/

	bool ReturnName(int placeInCatalog, int& error, string& name)
	{
		if(SizeCatalog-1<placeInCatalog)
		{
			error=5;
			return false;
		}
		else 
		{
			name=FilesDescribtionCatalog[placeInCatalog].Name;
			return true;
		}
	}
	//Zwraca nazw� pliku w string name
	bool ReturnSize(int placeInCatalog, int& error, int& size)
	{
		if(SizeCatalog<placeInCatalog)
		{
			error=5;
			return false;
		}
		else 
		{
			string datatemp;
			ViewFile(placeInCatalog,datatemp,error);
        for(unsigned int i=0; i<datatemp.length(); i++)
        {
                if(datatemp[i]=='~')
                        {
                                datatemp.resize(i);
                                        break;
				}
		}
		size=datatemp.size();
			return true;
		}
	}
	//Zwraca rozmiar pliku w int size (rozmiar ca�ego stringa)
	bool ReturnSize(string name, int& error, int& size)
	{
		for(int i=0; i<SizeCatalog; i++)
		{
			if(FilesDescribtionCatalog[i].Name==name)
				return ReturnSize(i, error, size);
		}
		return false;
	}
	//Zwraca rozmiar pliku w int size (rozmiar ca�ego stringa)
	bool ReturnFlag(int placeInCatalog, int& error, char& flag)
	{
		error=0;
		if(SizeCatalog-1<placeInCatalog)
		{
			error=5;
			return false;
		}
		else 
		{
			flag=FilesDescribtionCatalog[placeInCatalog].flag;
			return true;
		}
	}
	//Zwraca flag� pliku w char flag, R lub W(opisane poni�ej)
	bool ReturnType(int placeInCatalog, int& error, int& type)
	{
		error=0;
		if(SizeCatalog-1<placeInCatalog)
		{
			error=5;
			return false;
		}
		else 
		{
			type=FilesDescribtionCatalog[placeInCatalog].type;
			return true;
		}
	}
	//Zwraca typ pliku w int type(rozszerzenie), 0=exe, 1=txt

	bool ChangeFlagOfFile(string name, char flag, int& error);
	bool ChangeFlagOfFile(int placeInCatalog, char flag, int& error);
	//Pozwala na zmian� flagi pliku

	bool ViewFile(int placeInCatalog, string& data, int& error);
	bool ViewFile(string name, string& data, int& error);
	//Wy�wietlanie- do data zwraca ca�y string z danymi

	bool ChangeFile(string name, string data, int& error);
	bool ChangeFile(int placeInCatalog, string data, int& error);
	/*Zmiana pliku- data to string, kt�ry ma zmieni� ten istniej�cy
	w przypadku, gdy jest mniejszy, ni� obecny NIE ZOSTANIE NADPISANY,
	lecz usuni�ty i zmieniony.*/

	bool AppendFile(string name, string data, int& error);
	bool AppendFile(int placeInCatalog, string data, int& error);
	//Dopisywanie danych na ko�cu istniej�cego pliku.

	bool RenameFile(string name, string NameReplacement, int& error);
	bool RenameFile(int placeInCatalog, string NameReplacement, int& error);
	//Zmiana nazwy- w przypadku podania tej samej zwraca b��d

	string* ListOfFiles(int& sizeOfCatalog);
	//Zwraca list� plik�w z katalogu wraz z ich flag� w formie stringa

	bool DeleteFile(string name, int& error);
	bool DeleteFile(int placeInCatalog, int& error);
	void File_System::ShowFAT();
	//Usuwanie plik�w
};

/*W przypadku powy�szych funkcji nale�y pami�ta�,
	�e katalog ma numeracj� od 0 do (RozmiarKatlogu-1)
	---------------------------------------------------------------
	Wszystkie zmienne z & oddaj� warto�ci, 
	wi�c powinny mie� zadeklarowane zmienne, do kt�rych moga je odda�
	---------------------------------------------------------------
	FLAGI(char flag):
	'R'-Tylko do odczytywania- nie mo�na zmienia�/dopisywa�
	'W'-Odczytywanie i zmiana
	---------------------------------------------------------------
	ERROR(int& error):
	0-Brak error
	1-Brak miejsca
	2-Zbyt d�uga nazwa
	3-Nazwa ju� istnieje
	4-B��d atrybut�w
	5-Z�e dane(nie ma takiego pliku etc.)
	6-Plik o ograniczonym prawie dost�pu- nie mo�na zapisa�/zmienia�
	7-Crash, dysk zostaje wyczyszczony w celu zapobiegni�cia innym b��dom
	---------------------------------------------------------------
	------------------------*UWAGA*--------------------------------
	Plik FAT.txt do��czony do archiwum musi znajdowa� si� tam, 
	gdzie jest wykonywalny lub debugowany .exe programu.
	W innym przypadku system plik�w nie b�dzie sprawnie dzia�a�.
	*/
