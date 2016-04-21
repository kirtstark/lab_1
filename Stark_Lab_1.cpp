/***********************************************************
* Author:				Kirt Stark
* Date Created:			4/8/16
* Last Modification Date:	4/12/16
* Lab Number:			CST 352 Lab 1
* Filename:				Stark_Lab_1.cpp
*
* Overview:
*   This program uses a variable number of threads to 
*   calculate all prime numbers from 2 to a given
*   endpoint. The maximum attempted has been
*   6 threads calculating the first 5000 prime numbers
*
* Input:
*	There is no input other than the hardcoded variables for
*   the number of threads and the number of prime numbers
*   to be calculated
*
* Output:
*   The time elaped, number of prime numbers found, the number
*   of prime numbers found for each thread, the number of threads,
*   and a full list of the prime numbers found.
************************************************************/
#include "stdafx.h"
#include <conio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <math.h>
#include <set>
#include<vector>
#include <algorithm>

/* Compute successive prime numbers (now more efficiently). */

int numThreads; // change this value in the main method to match current threads
int min_candidate = 0;
std::set<int> prime_buffer;
std::set<int> prime_final;
std::vector<int> primes_found;
int current_low_thread = 1;
std::mutex mtx;
bool done = false;
int total_primes = 0;
int master_list = 1;
int watcher = 0;

void PressAnyKey();


/**************************************************************
*	  Purpose:  adds a prime number to the list of primes
*
*         Entry:  needs the threadID and the prime number
*
*          Exit:  NA
****************************************************************/

bool addToList(int threadID, int num)
{
	std::unique_lock<std::mutex> lck(mtx);
	prime_final.insert(num);

	if (prime_final.size() >= total_primes)
	{
		done = true;
	}

	return true;
}

/**************************************************************
*	  Purpose:  Used by each thread to access the next
*				number to test for primeness, and increment
*
*         Entry:  NA
*
*          Exit:  Returns the number on the list
****************************************************************/

int get_next_number()
{
	std::unique_lock<std::mutex> lck(mtx);
	return master_list += 2;
}

/**************************************************************
*	  Purpose:  Used to check each successive number for each thread
*				for primeness, and if found will send it to 
*				a function to add it to the list of primes.
*
*         Entry:  requires only the threadID
*
*          Exit:  Returns to main program when it
*				is alerted that the goal has been reached.
****************************************************************/

int compute_prime(const int threadID)
{
	int candidate = get_next_number();
	int sqrCandidate = (int)sqrt(candidate) + 1;
	int count_found = 0;

	while (!done) {
		int factor;
		int is_prime = 1;
		int is_sqrt = 1;

		/* Test primality by successive division.  */
		for (factor = 2; factor <= sqrCandidate; ++factor)
		{
			if (candidate % factor == 0 && candidate > 2) {
				is_prime = 0;
				break;
			}
		}

		if (is_prime) {
			addToList(threadID, candidate);
			++count_found;
		}
		candidate = get_next_number();
		
		while (is_sqrt)
		{
			if ((double)(sqrCandidate = (int)sqrt(candidate)) == sqrt(candidate))
			{
				candidate = get_next_number();
			}
			else
				is_sqrt = 0;
		}

		sqrCandidate++;
	}
	primes_found[threadID - 1] = count_found;
	return count_found;
}

int main()
{	
	std::vector<std::thread> my_threads;
	int thread_count = 0;
	std::ofstream myfile;
	std::string file_name = "result.txt";

	prime_final.insert(2);

	// update this number to change the number of primes to search for
	total_primes = 5000;
	// update this line any time the number of threads changes
	numThreads = 7;
	
	auto t1 = std::chrono::high_resolution_clock::now();

	for (unsigned i = 0; i < numThreads; ++i)
	{
		// spawn threads
		primes_found.push_back(0);
		my_threads.push_back(std::thread(compute_prime, ++thread_count));
	}

	// call join() on each thread in turn
	for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&(std::thread::join)));

	// calculate the time elapsed for the entire program and print the results
	std::cout << "Program took "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t1).count()
		<< " milliseconds" << std::endl;

	std::cout << "number of primes found : " << prime_final.size() << std::endl << std::endl;

	PressAnyKey();

	myfile.open(file_name, std::ios::out);
	myfile << "Results using " << numThreads << " threads\n\n";
	myfile.close();
	myfile.open(file_name, std::ios::out | std::ios::app);
	std::cout << std::endl << std::endl;
	for each(int i in prime_final)
	{
		std::cout << i << std::endl;
		myfile << "\n" << i;
	}

	std::cout << "\n\nhow many did each one find:" << std::endl;
	myfile << "\n\nhow many did each one find:\n";

	int num = 0;
	for each(int i in primes_found)
	{
		std::cout << ++num << ": " << i << std::endl;
		myfile << num << ": " << i << "\n";
	}

	myfile.close();

	PressAnyKey();

	return 0;
}

/**************************************************************
*	  Purpose:  Provides a reliable, standard Press Any Key method
*
*         Entry:  none
*
*          Exit:  Clears the keyboard input buffer, then waits until
*		      user presses another key
*                 Again clears the keyboard input buffer
****************************************************************/

void PressAnyKey()
{
	while (_kbhit())
	{
		_getch();
	}
	std::cout << std::endl << std::endl << "Press any key to continue" << std::endl << std::endl;
	std::cin.clear();

	while (1)
	{
		if (_kbhit())
		{
			while (_kbhit())
			{
				_getch();
			}
			break;
		}
	}
}