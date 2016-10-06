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

bool recurse(vector <bool> &used_columns, vector <int> &used_col,int cur_row, vector<vector<int> > &A,
             vector<vector<int> > &B, vector<vector<int> > &M, vector<vector<int> > &A_dist,
             vector<vector<int> > &B_dist);
vector<vector<int> > dist_matrix(vector<vector<int> > &A);
void degree_sort(vector<vector<int> > &A,vector<vector<int> > &M, bool row);
vector<int> dijikstra(vector<vector<int> > &G,int n, int startnode);
vector<int> neighbor_num(vector<vector<int> > &A);
void prune_dist(vector<vector<int> > &A, vector<vector<int> > &B, vector<vector<int> > &M,
                               vector<vector<int> > &A_dist, vector<vector<int> > &B_dist, int A_index, int B_index);
bool oneRow(vector<vector<int> > &M);
#endif
