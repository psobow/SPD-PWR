// neh-algoritm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <functional> 

std::ofstream output;

clock_t beginTimeC_MAX;
clock_t endTimeC_MAX;

double C_MAXtime = 0;

int temp[501][21]; // tablica wykorzystywana w funkcji calculateC_MAX



class Job {
public:
	int tasks[20]; // 20 - max quantity of machines

	int entireLength = 0;
	int initialIndex = 0;
	Job() {}


	void calculateJobLength(int size) {
		this->entireLength = 0;

		for (int i = 0; i < size; i++) {
			this->entireLength += tasks[i];
		}
	}

	void reset() {
		entireLength = 0;
	}

};



void printArray(Job jobs[], int sizeJob, int sizeMachines) {
	for (int i = 0; i < sizeJob; i++) {
		std::cout << "\nInit index: " << jobs[i].initialIndex + 1 << " entireLength: " << jobs[i].entireLength << "     [";
		for (int j = 0; j < sizeMachines; j++) {
			std::cout << jobs[i].tasks[j] << ", ";
		}
		std::cout << "]\n";
	}
}

void printInitialOrder(Job jobs[], int size) {
	for (int i = 0; i < size; i++) {
		output << jobs[i].initialIndex + 1 << " ";
	}
}


int calculateC_MAX(Job jobs[], const int quantityOfJobs, const int quantityOfMachines) {
	beginTimeC_MAX = clock();
	temp[1][1] = jobs[1].tasks[1];

	int valueCarryForNextRow = 0;

	for (int i = 0; i < quantityOfJobs; i++) {
		temp[i][0] = 0;
	}

	for (int i = 0; i < quantityOfMachines; i++) {
		temp[0][i] = 0;
	}


	for (int row = 1; row <= quantityOfJobs; row++) {
		for (int column = 1; column <= quantityOfMachines; column++) {
			valueCarryForNextRow = jobs[row - 1].tasks[column - 1];
			temp[row][column] = std::max(temp[row - 1][column], temp[row][column - 1]) + valueCarryForNextRow;
		}
	}
	endTimeC_MAX = clock();
	C_MAXtime += double(endTimeC_MAX - beginTimeC_MAX) / (CLOCKS_PER_SEC);
	return temp[quantityOfJobs][quantityOfMachines];
}



int main() {
	std::cout << "Witam. Zapisuje dane do pliku: output-data.txt";

	clock_t begin;
	clock_t end;

	clock_t beginSort;
	clock_t endSort;

	double sortingTime = 0;

	clock_t beginNeh;
	clock_t endNeh;

	double nehTime = 0;

	std::ifstream myfile("neh.data.txt");
	output.open("output-data.txt", std::ofstream::out | std::ofstream::trunc);
	

	int quantityOfJobs = 0;
	int quantityOfMachines = 0;
	int temporaryValue;


	Job jobs[500];

	std::string line;
	std::string dataIndexToString = "";

	Job currentJob;


	if (!myfile.is_open()) throw std::runtime_error("Could not open file");

	begin = clock();

	for (int dataIndex = 0; dataIndex <= 120; dataIndex++) {

		if (dataIndex < 10) {
			dataIndexToString = "data.00" + std::to_string(dataIndex) + ":\r";
		}
		else if (dataIndex >= 10 && dataIndex < 100) {
			dataIndexToString = "data.0" + std::to_string(dataIndex) + ":\r";
		}
		else {
			dataIndexToString = "data." + std::to_string(dataIndex) + ":\r";
		}

		while (getline(myfile, line) && (line != dataIndexToString)); // skip useless data

		myfile >> quantityOfJobs;
		myfile >> quantityOfMachines;

		try 
		{
			if (quantityOfJobs == 0) throw std::runtime_error( std::string("\nCould not read data from file. (probably inappropriate end of line character in variable dataIndexToString)\n") + 
				"Try \\n, \\n\\r, \\r, or none" );
		}
		catch (const std::exception & e)
		{
			std::cerr << e.what() << "\n";
			exit(-1);
		}

		
		// read data set 
		for (int row = 0; row < quantityOfJobs; row++) { // row corespond with quantityOfJobs inside array
			for (int col = 0; col < quantityOfMachines; col++) { // col correspond with quantityOfMachines 
				myfile >> temporaryValue;
				currentJob.tasks[col] = temporaryValue;
			}
			currentJob.initialIndex = row;
			currentJob.calculateJobLength(quantityOfMachines);

			jobs[row] = currentJob;
		}



		/*
		Pierwszy istotny fakt aby uzyskać takie same wyniki jak u pana Doktora:

		Trzeba posortować tak żeby w przypadku zadań o tej samej wartości entireLength
		pierwsze było to które ma mniejszą wartość initialIndex. czyli to zadanie które było pierwsze podane na wejściu powinno być pierwsze w przypadku takich samych entireLength.
		*/


		beginSort = clock();
		// Najpierw sortuje tablice na podstawie wartości entireLength
		std::sort(jobs, jobs + quantityOfJobs, [](const Job & first, const Job & second) { return first.entireLength > second.entireLength; });
		int firstIndexOfSameValue = 0;
		int lastIndexOfSameValue = 0;

		// Teraz szukam takich fragmentów w tablicy gdzie obok siebie są Job'y których wartość entireLength jest taka sama
		// następnie sortuje ten fragment tablicy na podstawie initialIndex
		for (int i = 0; i < quantityOfJobs-1; i++) {
			if (jobs[i].entireLength == jobs[i + 1].entireLength) {
				firstIndexOfSameValue = i;
				while (jobs[i].entireLength == jobs[i + 1].entireLength) {
					i++;
				}
				lastIndexOfSameValue = i;
			}
			std::sort(jobs + firstIndexOfSameValue, jobs + lastIndexOfSameValue + 1, [](const Job & first, const Job & second) { return first.initialIndex < second.initialIndex; });
		}

		endSort = clock();

		sortingTime += double(endSort - beginSort) / (CLOCKS_PER_SEC / 1000);





		 // neh algorithm:

		
		int currentC_MAX = 0;
		int smallestC_MAX = 0;

		int quantityOfSwaps = 0;
		int quantityOfSwapsWithTheBestResult = 0;

		beginNeh = clock();

		for (int currentJobIndex = 0, currentSetSize = 2; currentJobIndex < quantityOfJobs - 1; currentJobIndex++, currentSetSize++) {
			//std::cout << "=====================================\n"; printArray(jobs, currentSetSize, quantityOfMachines);
			currentC_MAX = calculateC_MAX(jobs, currentSetSize, quantityOfMachines); //std::cout << "current C_MAX = " << currentC_MAX << "\n";
			smallestC_MAX = currentC_MAX;
			

			// swap current job from LAST index jobVector.size() - 1 times, inside jobVector in order to obtain the smallestC_MAX for current set
			quantityOfSwaps = 0;
			quantityOfSwapsWithTheBestResult = 0;
			for (int i = currentSetSize - 1; i >= 1; i--) { // start from last index of current set

				quantityOfSwaps++;
				std::swap(jobs[i], jobs[i - 1]); 
				currentC_MAX = calculateC_MAX(jobs, currentSetSize, quantityOfMachines); 
				//std::cout << "\n"; printArray(jobs, currentSetSize, quantityOfMachines); 
				//std::cout << "Swaps = " << quantityOfSwaps << " current C_MAX = " << currentC_MAX << "\n";




// Tutaj jest drugi istotny fragment aby uzyskać takie same wyniki jak u pana doktora:

//Tutaj trzeba brać ostatnie uszeregowanie, które daje najmniejszy czas C_MAX.
//więc jeśli mamy dwa najmniejsze uszeregowania o tym samym czasie to wybieramy uszeregowanie ostatnie znalezione.
// czyli musi być (currentC_MAX <= smallestC_MAX) zamiast (currentC_MAX < smallestC_MAX)
				
				if (currentC_MAX <= smallestC_MAX) {
					smallestC_MAX = currentC_MAX;
					quantityOfSwapsWithTheBestResult = quantityOfSwaps;
				}
			} // after this loop, job which we were swaping is placed at the 0 index in vector
			//std::cout << "smallestC_MAX at swap number: " << quantityOfSwapsWithTheBestResult << "\n";

			// we are swaping job back to the best place
			for (int x = quantityOfSwaps, index = 0; x > quantityOfSwapsWithTheBestResult; x--) {
				std::swap(jobs[index], jobs[index + 1]);
				index++;
			}
			//std::cout << "after swaping back:/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"; printInitialOrder(jobs, currentSetSize);
		}
		endNeh = clock();

		nehTime += double(endNeh - beginNeh) / (CLOCKS_PER_SEC);


		output << dataIndexToString << "\n";
		printInitialOrder(jobs, quantityOfJobs);
		output << "\nresult = " << calculateC_MAX(jobs, quantityOfJobs, quantityOfMachines); output << "\n";
		
	}

	
	
	
	end = clock();
	double elapsedTimeInSeconds = double(end - begin) / (CLOCKS_PER_SEC);
	output << "\nElapsed time: " << elapsedTimeInSeconds << " seconds.\n";
	output << "Elapsed sorting time " << sortingTime << "\n";
	output << "Elapsed neh time: " << nehTime << "\n";

	std::cout << "\nElapsed time: " << elapsedTimeInSeconds << " seconds.\n";
	std::cout << "Elapsed sorting time " << sortingTime << "\n";
	std::cout << "Elapsed neh time: " << nehTime << "\n";

	std::cout << "time inside C_MAX = " << C_MAXtime << "\n";

	myfile.close();
	return 0;
}
