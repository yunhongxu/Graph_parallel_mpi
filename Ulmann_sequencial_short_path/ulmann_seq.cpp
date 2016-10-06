//
//  utility.cpp
//  ullmann_mpi
//
//  Created by xu on 3/15/15.
//  Copyright (c) 2015 ljh. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <iterator>
#include "unistd.h"
#include "ulmann.h"
using namespace std;

int main(int argc, char * argv[])
{

    struct timeval gen_start, gen_end; /* gettimeofday stuff */
    struct timeval c_start, c_end;         /* gettimeofday stuff */
    struct timezone tzp;

    vector<bool> used_columns;
    vector<int> used_col;
    int cur_row;

    /* graph info */
    int A_size = 0;
    int B_size = 0;
    int A_R, A_C, M_R, M_C;
    bool found;

    vector<vector<int> > A;
    vector<vector<int> > B;
    vector<vector<int> > M;
    vector<vector<int> > A_dist;
    vector<vector<int> > B_dist;

    const char* pvalue = "4"; //percentage, size A / size B, 2 for 20%, 4 for 40% etc.
    const char* bvalue = "20"; // # nodes of B
    const char* rvalue = "001"; // eta, could be 001, 005, or 01
    const char* gvalue = "01"; // graph id, could be 00, 01, ... , 99

    // rread graph files, and set parameters for the file
    int c;
    while((c = getopt(argc, argv, "p:b:r:g:")) != -1)
    {
        switch(c)
        {
        case 'p':
            pvalue = optarg;
            break;
        case 'b':
            bvalue = optarg;
            break;
        case 'r':
            rvalue = optarg;
            break;
        case 'g':
            gvalue = optarg;
            break;
        default:
            exit(0);

        }
    }
    stringstream ss;
    ss << "/home/yunhong/Documents/courses/CSEG_626/graphsdb/si" << pvalue << "/rand/r" << rvalue <<"/si" << pvalue << "_r" << rvalue << "_m" << bvalue << ".A" << gvalue;

    cout << "reading graph: " << ss.str() << endl;
    masterReadGraph(ss.str(), A, B, M);


    cout <<"A"<<endl;
    printMatrix(A);
    cout <<"B"<<endl;
    printMatrix(B);
    cout <<"M"<<endl;
    printMatrix(M);
    vector<vector<int> > A1 = A;

    degree_sort(A,M,1);// sort nodes in small graph in descending order
    degree_sort(B,M,0);// sort nodes in big graph in descending order
    A_dist = dist_matrix(A); // get the distance matrix for A
    B_dist = dist_matrix(B);// get the distance matrix for B

    A_size = A.size();
    B_size = B.size();

    A_R = A.size();
    A_C = A[0].size();
    M_R = M.size();
    M_C = M[0].size();
    cout << "M_C"<<M_C << endl;
    used_columns.reserve(M_C);//all used columns are false

    //get the start time
    gettimeofday(&c_start, &tzp);

    cur_row = 0;
    for(int k = 0; k < M_C; k++)
    {
        used_columns[k] = false;// for each point, initially, it's false
    }

    found =  recurse(used_columns, used_col, cur_row, A, B, M, A_dist, B_dist);
    // get the edn time
    gettimeofday(&c_end, &tzp);

    // display the running time
    cout << "big graph: " <<B_size<<"    small graph:  "<<A_size<<endl;
    cout <<"found: " << found <<endl;
    print_elapsed("Ullmann", &c_start, &c_end, 1);
    cout << endl;

    return 0;
}
