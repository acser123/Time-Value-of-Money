// C++ program to calculate Time Value of Money (TVM)
// 
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Usage:
// tvm [-b] N_Number_of_periods i_annual_interest_rate_percent PV_Present_value PMT_periodic_payment FV_future_value
// Use a question mark '?' to specify the computed value from the above list.
// -b: Compounding at the beginning of the periods. If not specified, the default is compounding at the end of the periods.
// Example:
// For 30 years (12*30=360 periods or months), taking out a loan of $100000 making a payment of $415 every month, paying off the loan to $0 at the end, what is the effective rate if compounding happens at the end of the month?
// $ tvm 360 ? 100000 -415 0
// i=2.87701143497542943805
//
// For 30 years (12*30=360 periods or months), taking out a loan of $100000 making a payment of $415 every month, paying off the loan to $0 at the end, what is the effective rate if compounding happens at the beginning of the month?
// $ tvm -b 360 ? 100000 -415 0
// i=2.89573142690642271191
//
// For 30 years (12*30=360 periods or months), taking out a loan of $100000, at 2.9% annual percentage rate, what is the monthly payment if payng off the loan to $0 at the end?
// $ tvm -b 360 2.9 100000 ? 0
// PMT=-415.23

 


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <algorithm>

// tolerance, do not decreaase, this is all 'double' in C can do
#define EPSILON 0.000000000001 

class Tvm {
	double N=0; // Number of periods
	double i=0; // Periodic interest rate
	double PV=0; // Present Value
	double PMT=0; // Payment per period
	double FV=0; // Future value
	// 1 if compounding is at the beginning of the period, 0 if it's at the ned
        int is_begin=0;  
	
	// Simple bisection algorithm using the func function parameter
	double find_root(double (Tvm::* func) (double), double from, double to) {
		double a = from;
		double b = to;
		double c;
 		double fc;


		// Ensuring convergence, if the beginning is smaller than the end, switch them
 		if ((this->*func)(a)>(this->*func)(b)) {
  			c=a; a=b; b=c;
 		}

 		int j = 0;

 		// Bisection algorithm
 		do {
  			c = (a+b)/2;
			// call the function only once
 			fc = (this->*func)(c);

			if (fc<=0) {
 				a=c; 
			} else {
 				b=c;
			}
  			j++;

  			//printf("*debug* j=%d;  a=%.20f;  b=%.20f;  c=%.20f;  fc=%.20f\n", j, a, b, c, fc);

		} while (fabs(a-b)>EPSILON);
 		return c;
	}

	// This is the same formula as with the calc_FV() that needs to be zero
 	// Need to find a root for this with N 		 
 	double calc_N_internal (double N_h) {
		double helper;
		if (is_begin) {
			helper =(FV - PV*pow((1+i), N_h)-PMT*((pow((1+i), N_h)-1)/i*(i+1)));
		} else {
			helper =(FV - PV*pow((1+i), N_h)-PMT*((pow((1+i), N_h)-1)/i));
		}
		//printf("*debug* N_h=%f; calc_N_internal=%f;\n", N_h, helper);
		return helper;
	}

	// This is the same formula as with the calc_FV() that needs to be zero
 	// Need to find a root for this with i 		 
	double calc_i_internal (double i_h) {
		double helper;
		if (is_begin) {
			helper = (FV - PV*pow((1+i_h), N)-PMT*((pow((1+i_h), N)-1)/i_h*(i_h+1)));
		} else {
			helper = (FV - PV*pow((1+i_h), N)-PMT*((pow((1+i_h), N)-1)/i_h));
		}
//	printf("*debug* calc_i_internal(): N=%f; i_h=%.20f; PV=%f; PMT=%f; FV=%f; calc_i_internal=%f;\n", N, i_h, PV, PMT, FV, helper);
		return helper;
	}

	public:
        void test_init() { 
			N=360;
			i=2.9/12/100;
			PV=100000;
			PMT=-416.5;
			FV=0;
	}
	//
	void set_begin() {
		is_begin=1;
	}	
	int get_begin() {
		return is_begin;
	}	
	// Number of periods
        void set_N(double n) {
		N=n;
	}

	void calc_N() {
		// You have to declare this, otherwise you get invalid use of non-static member function
		// You have to declare this calculate variable pointing to the Tvm::calc_N_internal function that you want to pass to find_root(), otherwise you get invalid use of non-static member function
		// From https://stackoverflow.com/questions/15756331/function-pointers-generate-invalid-use-of-non-static-member-function-error
		// You have to define a pointer variable with the function pointer
		double (Tvm::*calculate)(double) = &Tvm::calc_N_internal;
		// Hoping that the N will be less than 100000 months, which is ~8330 years...
		N = find_root ( calculate, 0, 100000);
	}

	double get_N() {
		return N;
	}

	// Interest rate per period, percentage divided by 100
        void set_i(double i_h) {
		i=i_h; 
	}

	void calc_i() {
		// You have to declare this calculate variable pointing to the Tvm::calc_i_internal function that you want to pass to find_root(), otherwise you get invalid use of non-static member function
		// You have to declare this, otherwise you get invalid use of non-static member function
		double (Tvm::*calculate)(double) = &Tvm::calc_i_internal;
		// can't try from 0 as it will not converge for the calc_i_internal function


		// From https://en.wikipedia.org/wiki/Internal_rate_of_return
		double A = N*PMT;
		double p = log(A/fabs(PMT))/log(A/PV);
		double r1 = pow(A/fabs(PMT), (2/(N+1)))-1; 
		double r2 = pow((1+r1), p) - 1;
		i = find_root ( calculate, r1, r2);


		//i = find_root ( calculate, -1, 1);
		i = find_root ( calculate, 0.002, 1);
	}

	double get_i() {
		return i;
	}

	// Present Value (PV)

	void set_PV(double PV_h) {
		PV=PV_h;
	}

	void calc_PV() {
		// This is the closed formula for the FV calculation from calc_FV()
		if(is_begin) {
			PV = (FV - (PMT*(pow((1+i), N)-1))/i*(1+i))/pow((1+i), N);
		} else {
			PV = (FV - (PMT*(pow((1+i), N)-1))/i)/pow((1+i), N);
		}
			
	}

 	double get_PV() {
		return PV;
	}

	// Payment (PMT)

	void set_PMT(double PMT_h) {
		PMT=PMT_h;
	}

	void calc_PMT() {
		// This is the closed formula for the PMT calculation from calc_FV()
		if(is_begin) {
			PMT = (FV - PV*pow((1+i),N)) / ((pow((1+i), N)-1)*(1+i)) * i;
		} else {
			PMT = (FV - PV*pow((1+i),N)) / ((pow((1+i), N)-1)) * i;
		}
	}

	double get_PMT() {
		return PMT;
	}

	// Future Value (FV)

	void set_FV(double FV_h) {
		FV=FV_h;
	}

	void calc_FV() {
		// From https://keisan.casio.com/exec/system/1234231998 
		if(is_begin) {
			FV = PV*pow((1+i), N)+PMT*((pow((1+i), N)-1))/i*(1+i);
		} else {
			FV = PV*pow((1+i), N)+PMT*((pow((1+i), N)-1))/i;
		}
	}

	double get_FV() {
		return FV;
	}
}; // end Twm

// Command line processing DIY 
// From: https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end) {
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int main (int argc, char *argv[]) {
	Tvm tvm;

	int flag;
	// -b means that we use compounding at the beginning, otherwise
	// compounding is at the end.
	// If no -b flag given, use cmd line arguments 
	// 2 to 6 to fill up the variables.

	int k=0;

	if (argc!=6 && argc!=7){
	 	printf("Command line incorrect.\n\
Usage:\n\
tvm [-b] N_Number_of_periods i_annual_interest_rate_percent PV_Present_value PMT_periodic_payment FV_future_value\n\
Use a question mark ('?') to specify the computed value\n\
from the above list.\n\n\
[-b]: Compounding at the beginning of the periods. If not specified, the default is compounding at the end of the periods.\n\
\n\
Examples:\n\
Example #1: For 30 years (12*30=360 periods or months), taking out a loan of $100000, making a payment of $415 every month, paying off the loan to $0 at the end, what is the effective rate if compounding happens at the end of the month?\n\
$ tvm 360 ? 100000 -415 0\n\
i=2.87701143497542943805\n\
\n\
Example #2: For 30 years (12*30=360 periods or months), taking out a loan of $100000, making a payment of $415 every month, paying off the loan to $0 at the end, what is the effective rate if compounding happens at the beginning of the month?\n\
$ tvm -b 360 ? 100000 -415 0\n\
i=2.89573142690642271191\n\
\n\
Example #3: For 30 years (12*30=360 periods or months), at 2.9%% annual percentage rate, taking out a loan of $100000, what is the monthly payment if paing off the loan to $0 at the end?\n\
$ tvm -b 360 2.9 100000 ? 0\n\
PMT=-415.23\n\
");

		return 1;
	}


	// Compounding at the beginning of the period specified by -b
	if(cmdOptionExists(argv, argv+argc, "-b")) {
		tvm.set_begin();
		flag=1;
		// Use cmd line arguments 2 to 6 to fill up the variables
		k=1;
	}


	tvm.set_N(strtod(argv[++k],NULL));
	tvm.set_i(strtod(argv[++k],NULL)/100/12);
	tvm.set_PV(strtod(argv[++k],NULL));
	tvm.set_PMT(strtod(argv[++k],NULL));
	tvm.set_FV(strtod(argv[++k],NULL));

	// By default, no -b passed
	k=0;	
	if (argc==7 && flag) {
	// We need to start from the second argument if -b was specified
		k=1;
	}
	// N is the unknown
	if (*argv[++k]=='?') {
		// N calculation
		tvm.set_N(0);
		tvm.calc_N();		
        	printf("N=%.2f\n", tvm.get_N()); 
		return 0;
	}

	// i is the unknown
	if (*argv[++k]=='?') {
		// i calculation
		tvm.set_i(EPSILON);
		tvm.calc_i();		
        	printf("i=%.20f\n", tvm.get_i()*12*100); 
		return 0;
	}

	// PV is the unknown
	if (*argv[++k]=='?') {
		// PV calculation
		tvm.set_PV(0);
		tvm.calc_PV();		
        	printf("PV=%.2f\n", tvm.get_PV()); 
		return 0;
	}

	// PMT is the unknown
	if (*argv[++k]=='?') {
		// PV calculation
		tvm.set_PMT(0);
		tvm.calc_PMT();		
        	printf("PMT=%.2f\n", tvm.get_PMT()); 
		return 0;
	}

	// FV is the unknown
	if (*argv[++k]=='?') {
		// FV calculation
		tvm.set_FV(0);
		tvm.calc_FV();		
        	printf("FV=%.2f\n", tvm.get_FV()); 
		return 0;
	}


}

