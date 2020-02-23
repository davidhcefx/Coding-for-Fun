#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;


// #include <fstream>, <cstdlib>
// Transform a path to Format 8.3 (not including the basedir).
string format_8_3(const string& addr);

int main()
{
	cout << format_8_3("C:\\Program Files\\Windows Defender") << "\n";
	// example output: "C:\\Program Files\\WINDOW~1"
	return 0;
}

string format_8_3(const string& addr){
	if (addr[1] != ':' || addr[2] != '\\'){
		cerr << "Error: please provide absolute file path\n";
		return "";
	}
	int basepos, firstpart, i = addr.size() - 1;
	if (addr.back() == '\\'){
		i--;
	}
	basepos = addr.rfind('\\', i) + 1;
	firstpart = addr.find(' ', basepos);
	if (firstpart == -1){  // no need to transform to format 8.3
		return addr;
	}
	// transform using DIR /X command
	system("CD %TEMP%");
	string base {addr.substr(0, basepos)};
	string cmd = "DIR /X \"" +						\
		base +										\
		addr.substr(basepos, firstpart - basepos) +	\
		"\"* > .\\result.fmt83";
	system(cmd.c_str());
	// parse the list to get the transformed result
	ifstream fin(".\\result.fmt83");
	string line, fullname { addr.substr(basepos, addr.size() - basepos) };
	int end = -1;
	while (getline(fin, line)){
		if ((end = line.rfind(fullname)) != -1) break;
	}
	if (end == -1){
		cerr << "Error: can't find the target address. Please check the address: "<<addr<<"\n";
		return "";
	}
	end--;  // at blank
	while (isblank(line[end])) end--;  // find first non-blank character
	basepos = line.rfind(' ', end);
	fin.close();
	system("DEL /Q .\\result.fmt83");
	return base + line.substr(basepos + 1, end + 1 - basepos);
}
