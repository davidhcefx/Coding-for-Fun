#include <iostream>
#include <fstream>
using namespace std;

// Please #include <fstream>
string format_8_3(const string& addr);

int main()
{
	cout << format_8_3("C:\\Program Files\\Windows Defender") << "\n";
	// example output: "C:\\Program Files\\WINDOW~1"
	return 0;
}

string format_8_3(const string& addr){
	/*
		Format 8.3 Converter (v2) by davidhcefx, 2020.2.24.
	*/
	system("CD %TEMP%");
	ofstream fout("fmt83.bat");
	fout << "@echo off\n";
	fout << "echo %~s1%\n";
	fout.close();
	system(("fmt83.bat \"" + addr + "\"> result.fmt83").c_str());

	string line;
	ifstream fin("result.fmt83");
	getline(fin, line);
	fin.close();
	return line;
}
