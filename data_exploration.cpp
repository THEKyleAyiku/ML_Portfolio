#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

// Reading the CSV file into 2 vectors based on the 2 columns of data.
void print_stats (vector <double> vec)
{
    // Printing the sum.
    double sum = rm_medv_sum(vec);
    cout << "The sum is: " << sum << endl;

    // Printing the mean.
    double mean = rm_medv_mean(vec);
    cout << "The mean is: " << mean << endl;
}

// Finding the sum.
double rm_medv_sum (vector <double> vec)
{
    double sum = 0.0;
    for (int num1 = 0; num1 < vec.size(); num1++)
    {
        sum += vec[num1];
    }
    return sum;
}

// Finding the mean.
double rm_medv_mean (vector <double> vec)
{
    double mean = 0.0;
    for (int num2 = 0; num2 < vec.size(); num2++)
    {
        mean == sum / 507; // There are 507 values in the data.
    }
    return mean;
}

// Finding the median.
double rm_medv_median (double <vector> vec)
{
    double median = 0.0;
    int size = vec.size();

    // Using the sort function.
    sort() 
    
}

// Code from PDF "Portfolio Component 1 - Data Exploration."
int main(int argc, char** argv)
{
    ifstream inFS;     // Input file stream.
    string line;
    string rm_in, medv_in;
    const int MAX_LEN = 1000;
    vector <double> rm(MAX_LEN);
    vector <double> medv(MAX_LEN);

    // Opening the file.
    cout << "Opening file Boston.csv." << endl;

    inFS.open("Boston.csv");
    if (!inFS.is_open())
    {
        cout << "Could not open file Boston.csv" << endl;
        return 1; // This is an error.
    } 
    
    // Boston.csv contains two doubles.
    cout << "Reading line 1" << endl;
    getline(inFS, line);

    // For the heading.
    cout << "Heading: " << line << endl;

    int numObservations = 0;
    while (inFS.good())
    {
        getline(inFS, rm_in, ',');
        getline(inFS, medv_in, '\n');

        rm.at(numObservations) = stof(rm_in);
        medv.at(numObservations) = stof(medv_in);

        numObservations++;
    }

    rm.resize(numObservations);
    medv.resize(numObservations);

    cout << "New length " << rm.size() << endl;

    cout << "Closing file Boston.csv." << endl;
    inFS.close(); // This closes the file.

    cout << "Number of records: " << numObservations << endl;

    cout << "\nStats for rm" << endl;
     print_stats(rm);

    cout << "\nStats for medv" << endl;
     print_stats(medv);

    
    cout << "\nProgram terminated.";
    return 0;
}
