#include <iostream>
#include <string>
using namespace std;

int main() {
    string input;
    while (true) {
	cout << "Welcome to the CaaS hands-on class(week06 Inha univ).\n";
        cout << "Enter the multiplication table to print (enter 'q' to quit): ";
        cin >> input;

        if (input == "q" || input == "Q") {
            cout << "Exiting the program." << endl;
            break;
        }

        try {
            int dan = stoi(input);
            for (int i = 1; i <= 9; ++i) {
                cout << dan << " x " << i << " = " << dan * i << endl;
            }
        }
        catch (...) {
            cout << "Invalid input. Please enter a number or 'q' to quit." << endl;
        }
        cout << endl;
    }
    return 0;
}