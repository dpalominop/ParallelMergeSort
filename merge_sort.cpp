/*
 * merge_sort.cpp
 *
 *  Created on: Jun 22, 2018
 *      Author: Daniel Palomino
 *      eMail: dapalominop@gmail.com
 */


#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define UMBRAL 32

void insertion_sort (double list[], int size);
void merge (double list[], int size, double temp[]);
void merge_sort_serial (double list[], int size, double temp[]);
void merge_sort_parallel (double list[], int size, double temp[], int threads);


int main (int argc, char *argv[])
{

	if (argc != 4){
		cout << "Uso: "<<argv[0]<<" array-size numero-de-hilos\n";
		return 1;
	}

	int size = atoi (argv[1]);
	int threads = atoi (argv[2]);
	char* input_file = argv[3];

	int processors = omp_get_num_procs ();
	cout<<"Array size = "<<size<<"\nThreads = "<<threads<<"\nProcessors = "<<processors<<endl;

	//omp_set_num_threads (threads);

	double *list = new double[size];
	double *list_bk = new double[size];
	double *temp = new double[size];

	ifstream infile(input_file);
	string linebuffer;

	double start_f = omp_get_wtime ();
	for (int i = 0; i < size; i++) {
	  getline(infile, linebuffer);
	  istringstream iss(linebuffer);
	  if (!(iss >> list_bk[i])) {
		  break;
	  }
	}
	double end_f = omp_get_wtime ();
	cout<<"Upload Time = "<<end_f - start_f<<endl;

	omp_set_nested (1);

	double stime = 10.0;

	//Repetir M veces para obtener el menor tiempo
	for(int i=0;i<20;i++){
		memcpy (list, list_bk, size * sizeof (double));

		double start = omp_get_wtime ();
		merge_sort_parallel (list, size, temp, threads);
		double end = omp_get_wtime ();

		if(stime > (end - start)){
			stime = (end - start);
		}
	}

	cout<<"Sorting Time = "<<stime<<endl;

	for (int i = 1; i < size; i++){
		if (!(list[i - 1] <= list[i])){
			cout<<"Error: list["<<i-1<<"]="<<list[i - 1]<<" > list["<<i<<"]="<<list[i]<<endl;
			return 1;
		}
	}
	cout<<"-Success-"<<endl;

	delete[] list;
	delete[] list_bk;
	delete[] temp;

	return 0;
}


void merge_sort_parallel (double list[], int size, double temp[], int threads)
{
	if (threads == 1){
		merge_sort_serial (list, size, temp);
	}
	else if (threads > 1){
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				merge_sort_parallel (list, size / 2, temp, threads / 2);
			}
			#pragma omp section
			{
				merge_sort_parallel (list + size / 2, size - size / 2, temp + size / 2, threads - threads / 2);
			}
		}

		merge (list, size, temp);
	}
}

void merge_sort_serial (double list[], int size, double temp[])
{
	if (size <= UMBRAL){
		insertion_sort (list, size);
		return;
	}
	merge_sort_serial (list, size / 2, temp);
	merge_sort_serial (list + size / 2, size - size / 2, temp);

	merge (list, size, temp);
}

void merge (double list[], int size, double temp[])
{
	int i1 = 0;
	int i2 = size / 2;
	int tempi = 0;
	while (i1 < size / 2 && i2 < size)
	{
		if (list[i1] < list[i2])
		{
			temp[tempi] = list[i1];
			i1++;
		}
		else
		{
			temp[tempi] = list[i2];
			i2++;
		}
		tempi++;
	}
	while (i1 < size / 2)
	{
		temp[tempi] = list[i1];
		i1++;
		tempi++;
	}
	while (i2 < size)
	{
		temp[tempi] = list[i2];
		i2++;
		tempi++;
	}

	memcpy (list, temp, size * sizeof (double));
}

void insertion_sort (double list[], int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		int j, v = list[i];
		for (j = i - 1; j >= 0; j--)
		{
			if (list[j] <= v)
				break;
			list[j + 1] = list[j];
		}
		list[j + 1] = v;
	}
}
