#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
using namespace std;

int main()
{
	ifstream openF("Open.txt"), closeF("Close.txt");

	string open, close;

	//"today" represents number of times when opening price is closer to this day closing price then yesterday closing price
	//"yesterday" represents number of times when opening price closer to previous day closing price then today closing price
	float yesterday = 0, today = 0, k, l;
	getline(openF, open);
	getline(closeF, close);
	while (getline(openF, open) && today + yesterday < 1262)
	{
		k = stof(close) - stof(open);
		getline(closeF, close);
		l = stof(open) - stof(close);
		if (abs(k / l) > 1) today++;
		else yesterday++;
	}
	cout << yesterday << " " << today;
	return 0;
