//
//  utility.h
//  ullmann_mpi
//
//  Created by ljh on 3/15/15.
//  Copyright (c) 2015 ljh. All rights reserved.
//

#ifndef ullmann_mpi_utility_h
#define ullmann_mpi_utility_h

#include <sys/time.h>
#include <sys/resource.h>
#include <vector>
#include <map>
#include <iostream>
#include <string>

using namespace std;

void print_elapsed(const char* desc, struct timeval* start, struct timeval* end, int niters);

unsigned short read_word(FILE *in);

vector<vector<int> > readGraph(FILE *in);

vector<vector<int> > initPermMatrix(vector<vector<int> > &A, vector<vector<int> > &B);

void printMatrix(vector<vector<int> > &M);

void masterReadGraph(string graph_A, vector<std::vector<int> > &A,
                     vector<vector<int> > &B,
                     vector<vector<int> > &M);

vector<vector<int> > getPermMatrix(int A_size, int B_size, const vector<int> &id);
bool validPermMatrix(vector<vector<int> > &A, vector<vector<int> > &B, vector<vector<int> > &M);

bool recurse(vector <bool> &used_columns, vector <int> &used_col,int cur_row, vector<vector<int> > &A, vector<vector<int> > &B, vector<vector<int> > &M);
vector<bool>  prune(int cur_row, vector<vector<int> > &A,
             vector<vector<int> > &B, vector<vector<int> > &M);
bool neigbor(vector<bool> tem_used_columns,int neib_A_i,vector<int> neib_A,
             vector<int> neib_B,vector<vector<int> > &M);

#endif
