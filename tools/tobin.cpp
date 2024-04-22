//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Jan 28 00:22:15 GMT-0800 1998
// Last Modified: Wed Jan 28 00:22:20 GMT-0800 1998
// Filename:      tools/tobin.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/tobin.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Convert binary numbers to decimal.
//

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;


int    atohd       (const char* aNumber);
void   exitUsage   (const char* command);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	if (argc != 2) exitUsage(argv[0]);

	int aValue;
	aValue = atohd(argv[1]);

	for (int i=0; i<32; i++) {
		if ((aValue >> (31-i)) & 0x01) {
			cout << "1";
		} else {
			cout << "0";
		}
		if (i%4 == 3) cout << " ";
	}
	cout << endl;

	return 0;
}



//////////////////////////////
//
// atohd --
//

int atohd(const char* aNumber) {
	if (strchr(aNumber, 'x') != NULL) {
		return (int)strtol(aNumber, (char**)NULL, 16);
	} else {
		return atoi(aNumber);
	}
}



//////////////////////////////
//
// exitUsage --
//

void exitUsage(const char* command) {
		cout << endl;
		cout << "Converts an integer into binary form.\n";
		cout << endl;
		cout << "Usage: " << command << " integer" << endl;
		cout << endl;
		cout << "   integer = [-2147483648..2147483647] \n";
		cout << "             [-0x80000000..0x7fffffff] \n";
		cout << endl;
		exit(1);
}



