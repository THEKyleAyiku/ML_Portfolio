#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string>
#include <iomanip>
#include <chrono>
#include <eigen3/Eigen/Dense>
#include <map>
#include <list>

using namespace std;

Eigen::MatrixXd Sigmoid(Eigen::MatrixXd Z)
{
	return 1 / (1 + (-Z.array()).exp());
}

Eigen::MatrixXd Predict(Eigen::MatrixXd w, double b, Eigen::MatrixXd x)
{
	int m = x.rows();

	Eigen::MatrixXd pred = Eigen::VectorXd::Zero(m).transpose();

	Eigen::MatrixXd z = (w.transpose() * x.transpose()).array() + b;
	Eigen::MatrixXd a = Sigmoid(z);

	for (int i = 0; i < a.cols(); i++)
	{
		if (a(0, i) <= 0.5)
			pred(0, i) = 0;
		else
			pred(0, i) = 1;
	}

	return pred.transpose();
}

tuple<Eigen::MatrixXd, double, double> Propogate(Eigen::MatrixXd w, double b, Eigen::MatrixXd x, Eigen::MatrixXd y, double lambda)
{
	int m = y.rows();

	Eigen::MatrixXd z = (w.transpose() * x.transpose()).array() + b;
	Eigen::MatrixXd a = Sigmoid(z);

	auto cross_entropy = -(y.transpose() * (Eigen::VectorXd)a.array().log().transpose() + ((Eigen::VectorXd)(1 - y.array())).transpose() * (Eigen::VectorXd)(1 - a.array()).log().transpose()) / m;

	double l2_reg_cost = w.array().pow(2).sum() * (lambda / (2 * m));

	double cost = static_cast<const double>((cross_entropy.array()[0])) + l2_reg_cost;

	Eigen::MatrixXd dw = (Eigen::MatrixXd)(((Eigen::MatrixXd)(a - y.transpose()) * x) / m) + ((Eigen::MatrixXd)(lambda / m * w)).transpose();

	double db = (a - y.transpose()).array().sum() / m;

	return make_tuple(dw, db, cost);
}

tuple<Eigen::MatrixXd, double, Eigen::MatrixXd, double, list<double>> Optimize(Eigen::MatrixXd w, double b, Eigen::MatrixXd x, Eigen::MatrixXd y, int iterator, double learning_rate, double lambda, bool log_cost)
{
	list<double> costsList;

	Eigen::MatrixXd dw;
	double db, cost;

	for (int i = 0; i < iterator; i++)
	{
		tuple<Eigen::MatrixXd, double, double> propogate = Propogate(w, b, x, y, lambda);
		tie(dw, db, cost) = propogate;

		w = w - (learning_rate * dw).transpose();
		b = b - (learning_rate * db);

		if (i % 100 == 0)
			costsList.push_back(cost);

		if (log_cost && i % 100 == 0)
			cout << "Cost after iteration " << i << ": " << cost << endl;
	}

	return make_tuple(w, b, dw, db, costsList);
}

int main(int argc, char** argv)
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

	vector<vector <double>> trainset = { pclassTrain, survivedTrain, sexTrain, ageTrain };
	vector<vector <double>> testset = { pclassTest, survivedTest, sexTest, ageTest };

	int row = dataset.size();
	int col = dataset[0].size();

	//Eigen::MatrixXd dataMatrix = Eigen::MatrixXd::Map(dataset.data(), row, col);

	
	// Magic conversion
	Eigen::MatrixXd x_train = Eigen::MatrixXd::Map(sexTrain.data(), sexTrain.size());
	Eigen::MatrixXd x_test = Eigen::MatrixXd::Map(sexTest.data(), sexTest.size());
	Eigen::MatrixXd y_train = Eigen::MatrixXd::Map(survivedTrain.data(), survivedTrain.size());
	Eigen::MatrixXd y_test = Eigen::MatrixXd::Map(survivedTest.data(), survivedTest.size());
	
	int dims = x_train.cols();
	Eigen::MatrixXd w = Eigen::VectorXd::Zero(dims);
	double b = 0.0;
	double lambda = 0.0;
	bool log_cost = true;
	double learning_rate = 0.01;
	int iterator = 10000;

	Eigen::MatrixXd dw;
	double db;
	list<double> costs;
	tuple<Eigen::MatrixXd, double, Eigen::MatrixXd, double, list<double>> optimize = Optimize(w, b, x_train, y_train, iterator, learning_rate, lambda, log_cost);
	tie(w, b, dw, db, costs) = optimize;

	Eigen::MatrixXd pred_test = Predict(w, b, x_test);
	Eigen::MatrixXd pred_train = Predict(w, b, x_train);

	auto train_accuracy = (100 - (pred_train - y_train).cwiseAbs().mean() * 100);
	auto test_accuracy = (100 - (pred_test - y_test).cwiseAbs().mean() * 100);

	cout << "Train Accuracy: " << train_accuracy << endl;
	cout << "Test Accuracy: " << test_accuracy << endl;
	
	cout << "\nProgram terminated.";
	return 0;
}