#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string>
#include <iomanip>
#include <chrono>

using namespace std;

vector<vector<double>> lh_pclass, lh_sex;
vector<double> apriori, age_mean, age_sd;

double calcMean(vector<double> v)
{
	double sum = 0;

	for (int i = 0; i < v.size(); i++)
		sum += v[i];

	double mean = sum / v.size();

	return mean;
}

// Standard Deviation
double calcSD(vector<double> v)
{
	double mean = calcMean(v);
	double SD = 0;

	for (int i = 0; i < v.size(); i++)
		SD += pow(v[i] - mean, 2);

	return sqrt(SD / v.size());
}

// Probability using normal distribution
double calcProb(double val, double mean, double sd)
{
	static const double inv_sqrt_2pi = 0.3989422804014327;
	double a = (val - mean) / sd;
	return inv_sqrt_2pi / sd * exp(-0.5f * a * a);
}

vector<double> predict(double pclass, double sex, double age)
{
	// num_s = prob_survived, num_p = prob_perished
	double num_s = lh_pclass[1][pclass - 1] * lh_sex[1][sex] * apriori[1] * calcProb(age, age_mean[1], age_sd[1]);
	double num_p = lh_pclass[0][pclass - 1] * lh_sex[0][sex] * apriori[0] * calcProb(age, age_mean[0], age_sd[0]);
	return { num_p / (num_s + num_p), num_s / (num_s + num_p) };
}

void calcStats(vector<vector<double>> data)
{
	vector<vector<double>> survivedData(3);
	vector<vector<double>> diedData(3);

	vector<double> survivedPclass, survivedSex, survivedAge, diedPclass, diedSex, diedAge;

	for (int i = 0; i < data[1].size(); i++)
	{
		if (data[1][i] == 1)
		{
			survivedPclass.push_back(data[0][i]);
			survivedSex.push_back(data[2][i]);
			survivedAge.push_back(data[3][i]);
		}
		else
		{
			diedPclass.push_back(data[0][i]);
			diedSex.push_back(data[2][i]);
			diedAge.push_back(data[3][i]);
		}
	}

	survivedData = { survivedPclass, survivedSex, survivedAge };
	diedData = { diedPclass, diedSex, diedAge };


	// Probability of survived/died
	apriori.push_back(((double)diedData[0].size() / data[0].size()));
	apriori.push_back(((double)survivedData[0].size() / data[0].size()));
	cout << fixed << setprecision(6);
	cout << "A-priori probabilities:" << endl;
	cout << "       0        1" << endl;
	cout << apriori[0] << " " << apriori[1] << endl << endl;

	// Likelihood p(survived | pclass)
	double died1 = 0; double died2 = 0; double died3 = 0; double survived1 = 0; double survived2 = 0; double survived3 = 0;

	for (int i = 0; i < diedPclass.size(); i++)
	{
		if (diedPclass[i] == 1)
			died1++;
		else if (diedPclass[i] == 2)
			died2++;
		else if (diedPclass[i] == 3)
			died3++;
	}
	for (int i = 0; i < survivedPclass.size(); i++)
	{
		if (survivedPclass[i] == 1)
			survived1++;
		else if (survivedPclass[i] == 2)
			survived2++;
		else if (survivedPclass[i] == 3)
			survived3++;
	}
	lh_pclass.push_back({ (died1 / diedPclass.size()), (died2 / diedPclass.size()), (died3 / diedPclass.size()) });
	lh_pclass.push_back({ (survived1 / survivedPclass.size()), (survived2 / survivedPclass.size()), (survived3 / survivedPclass.size()) });

	// Printing likelihood values for p(survived | pclass)
	cout << "Likelihood values for p(survived | pclass)" << endl << "          1        2        3" << endl;
	cout << "0: " << lh_pclass[0][0] << " " << lh_pclass[0][1] << " " << lh_pclass[0][2] << endl;
	cout << "1: " << lh_pclass[1][0] << " " << lh_pclass[1][1] << " " << lh_pclass[1][2] << endl << endl;


	// Likelihood p(survived | sex)
	double diedFemale = 0; double diedMale = 0; double survivedFemale = 0; double survivedMale = 0;

	for (int i = 0; i < diedSex.size(); i++)
	{
		if (diedSex[i] == 0)
			diedFemale++;
		else
			diedMale++;
	}
	for (int i = 0; i < survivedSex.size(); i++)
	{
		if (survivedSex[i] == 0)
			survivedFemale++;
		else
			survivedMale++;
	}
	lh_sex.push_back({ (diedFemale / diedSex.size()), diedMale / diedSex.size() });
	lh_sex.push_back({ (survivedFemale / survivedSex.size()), survivedMale / survivedSex.size() });

	// Printing likelihood values for p(survived | sex)
	cout << "Likelihood values for p(survived | sex)" << endl << "     Female     Male" << endl;
	cout << "0: " << lh_sex[0][0] << " " << lh_sex[0][1] << endl;
	cout << "1: " << lh_sex[1][0] << " " << lh_sex[1][1] << endl << endl;

	// Calculate mean and standard deviation
	age_mean.push_back(calcMean(diedAge));
	age_mean.push_back(calcMean(survivedAge));
	age_sd.push_back(calcSD(diedAge));
	age_sd.push_back(calcSD(survivedAge));

	// Printing matrix
	cout << "Age matrix" << endl;
	cout << "              0         1" << endl;
	cout << "Mean: " << age_mean[0] << " " << age_mean[1] << endl;
	cout << "SD:   " << age_sd[0] << " " << age_sd[1] << endl << endl;
}

double calcAccuracy(vector<double> predictions, vector<double> data)
{
	double correct = 0;

	for (int i = 0; i < data.size(); i++)
	{
		if (data[i] == predictions[i])
			correct++;
	}

	return correct / data.size();
}

double calcSensitivity(vector<double> predictions, vector<double> data)
{
	double TP = 0;
	double FN = 0;

	for (int i = 0; i < data.size(); i++)
	{
		if (predictions[i] == 1 && data[i] == 1)
			TP++;
		else if (predictions[i] == 0 && data[i] == 1)
			FN++;
	}

	return TP / (TP + FN);
}

double calcSpecificity(vector<double> predictions, vector<double> data)
{
	double TN = 0;
	double FP = 0;

	for (int i = 0; i < data.size(); i++)
	{
		if (predictions[i] == 0 && data[i] == 0)
			TN++;
		else if (predictions[i] == 1 && data[i] == 0)
			FP++;
	}

	return TN / (TN + FP);
}

int main2(int argc, char** argv)
{
	ifstream inFS;
	string line;
	string row_in, pclass_in, survived_in, sex_in, age_in;
	const int MAX_LEN = 2000;
	vector<double> pclass(MAX_LEN);
	vector<double> survived(MAX_LEN);
	vector<double> sex(MAX_LEN);
	vector<double> age(MAX_LEN);

	// cout << "Opening file titanic_project.csv." << endl;

	inFS.open("titanic_project.csv");
	if (!inFS.is_open())
	{
		cout << "Could not open file titanic_project.csv." << endl;
		return 1;
	}

	getline(inFS, line);

	int numObservations = 0;
	while (inFS.good())
	{
		getline(inFS, row_in, ',');
		getline(inFS, pclass_in, ',');
		getline(inFS, survived_in, ',');
		getline(inFS, sex_in, ',');
		getline(inFS, age_in, '\n');

		pclass.at(numObservations) = stof(pclass_in);
		survived.at(numObservations) = stof(survived_in);
		sex.at(numObservations) = stof(sex_in);
		age.at(numObservations) = stof(age_in);

		numObservations++;
	}

	pclass.resize(numObservations);
	survived.resize(numObservations);
	sex.resize(numObservations);
	age.resize(numObservations);

	// cout << "Closing file Boston.csv." << endl << endl;
	inFS.close();

	vector<vector <double>> dataset = { pclass, survived, sex, age };

	// Isolate first 800 observations for train, rest for test
	vector<double> ageTrain(age.begin(), age.begin() + 800);
	vector<double> pclassTrain(pclass.begin(), pclass.begin() + 800);
	vector<double> sexTrain(sex.begin(), sex.begin() + 800);
	vector<double> survivedTrain(survived.begin(), survived.begin() + 800);

	vector<double> ageTest(age.begin() + 800, age.end());
	vector<double> pclassTest(pclass.begin() + 800, pclass.end());
	vector<double> sexTest(sex.begin() + 800, sex.end());
	vector<double> survivedTest(survived.begin() + 800, survived.end());

	vector<vector <double>> trainset = { pclassTrain, survivedTrain, sexTrain, ageTrain};
	vector<vector <double>> testset = { pclassTest, survivedTest, sexTest, ageTest };

	auto start = chrono::high_resolution_clock::now();
	calcStats(trainset);
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

	vector<double> predictions;
	for (int i = 0; i < testset[1].size(); i++)
	{
		vector<double> pred = predict(testset[0][i], testset[2][i], testset[3][i]);
		if (pred[0] > 0.5)
			predictions.push_back(0);
		else
			predictions.push_back(1);
	}

	cout << "Accuracy: " << calcAccuracy(predictions, testset[1]) << endl;
	cout << "Sensitivity: " << calcSensitivity(predictions, testset[1]) << endl;
	cout << "Specificity: " << calcSpecificity(predictions, testset[1]) << endl;
	cout << "Training Time: " << duration.count() << " microseconds" << endl;

	cout << "\nProgram terminated.";
	return 0;
}