#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
using namespace std;

const double cut = 0.9975;
const double budget = 100000;


int findMaxIndex(const vector<double> &v)
{
	int j = 0;
	cout << "miv" << v.size();
	for (int i = 1; i < v.size(); i++)
	{
		if (v[j] < v[i]) j = i;
	}
	return 0;
}


double tail(const vector<double> &close, int k, int d)
{
	double s = 0;
	for (int i = d - k + 1; i <= d; ++i)
	{
		s += close[i];
	}
	return s / k;
}

//The next function holds the idea
bool doTrade(bool mode, int tailLength, int startDay, int endDay, vector<double> &tailMean,
	const vector<double> &open, const vector<double> &close, vector<double> &invested, vector<double> &held)
{
	bool buyMode = mode;
	int higherCount = 0;

	tailMean[startDay - 1] = tail(close, tailLength, startDay - 1);

	//Every iteration is analysis of data (means and closing prices), ending with "today", and trading the next day (opening price).
	for (int i = startDay; i < endDay; i++)
	{
		tailMean[i] = tail(close, tailLength, i);

		for (int j = 0; j < tailLength; j++)
		{
			if (close[i - j] > tailMean[i]) higherCount++;
		}

		//If I am in "buy mode", I am looking for next local minimum.
		if (buyMode)
		{
			//	cout << "buy ";

			//If "today" mean is lower then "yesterday" mean, I am still behind or around the local minimum.
			if (tailMean[i] < tailMean[i - 1])
			{

				//If "today" price is higher then mean, it is reasonble to assume that I am around the local minimum
				//and to expect that prices go up soon. I trade with respect to probability of that happening.
				if (close[i] > tailMean[i])
				{
					invested[i + 1] = invested[i] + cut * ((double)higherCount / (double)tailLength) * held[i] / open[i + 1];
					held[i + 1] = (1.0 - ((double)higherCount / (double)tailLength)) * held[i];
				}

				//If today price is still lower then mean, it is reasonable to assume that I have still not reached the local minimum.
				//Therefore no action for the next day.
				else
				{
					invested[i + 1] = invested[i];
					held[i + 1] = held[i];
				}
			}

			//If today mean is higher then yesterday, I am probably expecting higher prices soon, so I have passed the local minimum.
			//I invest whtever I have left and proceed to wait for next local maximum to sell.
			//This could be a false alarm.
			else
			{
				invested[i + 1] = invested[i] + cut * held[i] / open[i + 1];
				held[i + 1] = 0;
				buyMode = false;
			}

		}

		//If I am in "sell mode", I am looking for next local maximum.
		else
		{
			//	cout << "sell ";

			//If "today" mean is higher then "yesterday" mean, I am still behind or around the local maximum.
			if (tailMean[i] > tailMean[i - 1])
			{

				//If "today" price is lower then mean, it is reasonble to assume that I am around the local maximum
				//and to expect that prices go down soon. I trade with respect to probability of that happening.
				if (close[i] < tailMean[i])
				{
					invested[i + 1] = ((1.0 - (double)higherCount / (double)tailLength)) * invested[i];
					held[i + 1] = held[i] + cut * ((double)higherCount / (double)tailLength) * invested[i] * open[i + 1];
				}

				//If today price is still higher then mean, it is reasonable to assume that I have still not reached the local maximum.
				//Therefore no action for the next day.
				else
				{
					invested[i + 1] = invested[i];
					held[i + 1] = held[i];
				}
			}

			//If today mean is lower then yesterday, I am probably expecting lower prices soon, so I have passed the local maximum.
			//I sell whtever I have left and proceed to wait for next local minimum to buy.
			//This could be a false alarm.
			else
			{
				held[i + 1] = held[i] + cut*invested[i] * open[i + 1];
				invested[i + 1] = 0;
				buyMode = true;
			}
		}

		//cout << held[i + 1] + invested[i + 1] * open[i + 1] << "\n";
		higherCount = 0;
	}

	return buyMode;
}

int testMean(int endDay, const vector<double> &open, const vector<double> &close,
	vector<double> helpTailMean, vector<double> helpHeld, vector<double> helpInvested)
{
	vector<double> capital(49);
	int n = open.size();
	for (int i = 2; i <= 50; i++)
	{
		cout << "labela\n";

		helpHeld[endDay - 29] = budget;
		helpInvested[endDay - 29] = 0;
		cout << "labela1\n";
		bool neb = doTrade(true, i, endDay - 29, endDay - 1, helpTailMean, open, close, helpInvested, helpHeld);
		cout << neb;
		capital[i] = helpHeld[endDay - 1] + cut * open[endDay - 1] * helpInvested[endDay - 1];
	}
	return findMaxIndex(capital);
}

int main()
{
	ifstream openF("Open.txt"), closeF("Close.txt"), dayF("Day.txt");
	fstream heldF, investedF, totalByDay, meanByDay;

	vector<string> day(0);
	vector<double> open(0);
	vector<double> close(0);

	string line;
	int n = 0;

	while (getline(openF, line))
	{
		n++;
		open.push_back(stod(line));
	}

	n = open.size();

	int start = 0;

	while (getline(closeF, line))
	{
		close.push_back(stod(line));
	}

	int zlj = 0;

	while (getline(dayF, line))
	{
		day.push_back(line);
		if (line.substr(0, line.length() - 1) == "1/3/2000")  //For testing, I downloaded more data then I needed for the trading, so I need to find the strating point.
		{
			start = zlj;
		}
		zlj++;
	}

	vector<double> tailMean(n);
	vector<double> invested(n);
	vector<double> held(n);
	//vector<double> helpTailMean(n), helpInvested(n), helpHeld(n);

	held[start] = budget;
	invested[start] = 0;

	bool buyMode = true; //I enter the mrket willing to buy first.

	int i = start;

	//int tailLength = testMean(i, open, close,helpTailMean,helpHeld,helpInvested);

	while ((i + 10) < (n - 1))
	{
		buyMode = doTrade(buyMode, 18, i, i + 10, tailMean, open, close, invested, held);
		i += 10;
		//tailLength = testMean(i, open, close, helpTailMean,helpHeld,helpInvested);
	}

	while (i < n - 1)
	{
		buyMode = doTrade(buyMode, 18, i, i + 1, tailMean, open, close, invested, held);
		i++;
	}


	openF.close();
	closeF.close();

	meanByDay.open("Mean.txt", ios_base::app);
	heldF.open("Held.txt", ios_base::app);
	investedF.open("Invested.txt", ios_base::app);
	totalByDay.open("Total.txt", ios_base::app);

	for (int l = start; l < n; l++)
	{
		meanByDay << tailMean[l] << ", " << day[l] << "\n";
		heldF << held[l] << "\n";
		investedF << invested[l] << "\n";
		totalByDay << held[l] + cut * open[l] * invested[l] << " " << day[l] << "\n";
	}

	totalByDay.close();
	heldF.close();
	investedF.close();

	cout << "Final capital is: " << held[i] + cut * invested[i] * open[n - 1] << "\n";

	return 0;
}
