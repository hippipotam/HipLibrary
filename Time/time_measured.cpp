#include <ctime>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <sys/time.h>
#include <unistd.h>
using namespace std;
using namespace std::chrono;

void f1()
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	double dif = duration_cast<nanoseconds>( t2 - t1 ).count();
	printf ("High resolution: Elasped time is %lf nanoseconds.\n", dif );
}

void f2()
{
	timespec ts1,ts2;
	clock_gettime(CLOCK_MONOTONIC, &ts1);
	clock_gettime(CLOCK_MONOTONIC, &ts2);
	double dif = double( ts2.tv_nsec - ts1.tv_nsec );
	printf ("clock_gettime:   Elasped time is %lf nanoseconds.\n", dif );
}

void f3()
{
	struct timeval t1,t0;
	gettimeofday(&t0, 0);
	gettimeofday(&t1, 0);
	double dif = double( (t1.tv_usec-t0.tv_usec)*1000);
	printf ("gettimeofday:    Elasped time is %lf nanoseconds.\n", dif );
}
void f4()
{
	high_resolution_clock::time_point t1 , t2;
	double diff = 0;
	t1 = high_resolution_clock::now() ;
	for(int i = 1; i <= 10 ; i++) {
		t2 = high_resolution_clock::now() ;
		diff += duration_cast<nanoseconds>( t2 - t1 ).count();
		t1 = t2;
	}
	printf ("high_resolution_clock:: Elasped time is %lf nanoseconds.\n", diff/10 );
}

void f5()
{
	timespec ts1,ts2;
	double diff = 0;
	clock_gettime(CLOCK_MONOTONIC, &ts1);
	for(int i = 1; i <= 10 ; i++) {
		clock_gettime(CLOCK_MONOTONIC, &ts2);
		diff+= double( ts2.tv_nsec - ts1.tv_nsec );
		ts1 = ts2;
	}
	printf ("clock_gettime::         Elasped time is %lf nanoseconds.\n", diff/10 );
}

void f6()
{
	struct timeval t1,t2;
	double diff = 0;
	gettimeofday(&t1, 0);
	for(int i = 1; i <= 10 ; i++) {
		gettimeofday(&t2, 0);
		diff+= double( (t2.tv_usec-t1.tv_usec)*1000);
		t1 = t2;
	}
	printf ("gettimeofday::          Elasped time is %lf nanoseconds.\n", diff/10 );
}

void f7()
{
	high_resolution_clock::time_point t1 , t2;
	double diff[10];// = 0;
	duration<double> time_span[10];	
	
	double tp[10];
	
	for(int i = 1; i <= 10 ; i++) {
		t1 = high_resolution_clock::now();
		usleep(1000000);
		t2 = high_resolution_clock::now();
		auto elapsed_mcs = duration_cast<microseconds>(t2 - t1);
		tp[i] = double(elapsed_mcs.count()) / 1000.0;
	}
	double df = 0;
	for (int i = 0; i < 10; i++)
		df += tp[i];
	df /= 10.0;
	printf ("hrc::1sec            Elasped time is %lf microseconds.\n",  df);
}

void f8()
{
	timespec ts1,ts2;
	double diff[10];// = 0;
	clock_gettime(CLOCK_MONOTONIC, &ts1);
	//long diff[10];
	
	for(int i = 1; i <= 10 ; i++) {
		clock_gettime(CLOCK_MONOTONIC, &ts1);
		usleep(1000000);
		clock_gettime(CLOCK_MONOTONIC, &ts2);
		diff[i] = double( ts2.tv_nsec - ts1.tv_nsec ) + (ts2.tv_sec - ts1.tv_sec);
		//ts1 = ts2;
	}
	double df = 0;
	for (int i = 0; i < 10; i++)
		df += diff[i];
	df /= 10.0;
	
	printf ("clock_gettime::1sec  Elasped time is %lf microseconds.\n", df/1000.0 ); //nanoseconds
}

void f9()
{
	struct timeval t1,t2;
	double diff[10];// = 0;
	
	for(int i = 1; i <= 10 ; i++) {
		gettimeofday(&t1, 0);
		usleep(1000000);
		gettimeofday(&t2, 0);
		diff[i] = double( (t2.tv_usec-t1.tv_usec)*1000);
	}
	double df = 0;
	for (int i = 0; i < 10; i++)
		df += diff[i];
	df /= 10;
	printf ("gettimeofday::1sec   Elasped time is %lf microseconds.\n", df/1000.0 );
}

void elps_t()
{
	timespec ts1, ts2;
	for (int i = 0; i < 10; i++) {
		//clock_gettime(CLOCK_MONOTONIC, &ts1);
		usleep(500000);
		clock_gettime(CLOCK_MONOTONIC, &ts2);
		double diff = abs(ts2.tv_nsec - ts1.tv_nsec); // ); // + double(ts2.tv_sec - ts1.tv_sec);
		printf("time %.5lf sec (%ld | %ld)\n", diff / 1000000000.0, ts1.tv_nsec, ts2.tv_nsec);
		ts1 = ts2;
	}
}

int main() {
	//f1();
	//f2();
	//f3();	
	//f4();
	//f5();
	//f6();
	//f7();
	//f8();
	//f9();
	elps_t();
	return 0;
}
