//
//  utility.cpp
//  ullmann_mpi
//
//  Created by ljh on 3/15/15.
//  Copyright (c) 2015 ljh. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include "ulmann.h"
using namespace std;
const int INFINITY = 100;
/*==============================================================
 * print_elapsed (prints timing statistics)
 *==============================================================*/
void print_elapsed(const char* desc, struct timeval* start, struct timeval* end, int niters)
{

    struct timeval elapsed;
    /* calculate elapsed time */
    if(start->tv_usec > end->tv_usec)
    {

        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec  = end->tv_sec  - start->tv_sec;

    printf("\n %s total elapsed time = %ld (usec)",
           desc, (elapsed.tv_sec*1000000 + elapsed.tv_usec) / niters);
}

unsigned short read_word(FILE *in)
{
    unsigned char b1, b2;

    b1=getc(in); /* Least-significant Byte */
    b2=getc(in); /* Most-significant Byte */

    return b1 | (b2 << 8);
}
vector<vector<int> > readGraph(FILE *in)
{
    int nodes;
    int edges;
    int target;

    nodes = read_word(in);
    vector<vector<int> > matrix(nodes, vector<int>(nodes, 0));
    for(int i = 0; i < nodes; ++i)
    {
        edges = read_word(in);
        for(int j = 0; j < edges; ++j)
        {
            target = read_word(in);
            matrix[i][target]  = 1;
            matrix[target][i] = 1;
        }
    }

    return matrix;

}

vector<vector<int> > initPermMatrix(vector<vector<int> > &A, vector<vector<int> > &B)
{
    int nA = (int) A.size();
    int nB = (int) B.size();
    vector<vector<int> > M(nA, vector<int>(nB, 0));

    vector<int> degA(nA, 0);
    for(int i = 0; i < nA; ++i)
    {
        degA[i] = accumulate(A[i].begin(), A[i].end(), 0);
    }

    vector<int> degB(nB, 0);
    for(int i = 0; i < nB; ++i)
    {
        degB[i] = accumulate(B[i].begin(), B[i].end(), 0);
    }

    for(int i = 0; i < nA; ++i)
    {
        for(int j = 0; j < nB; ++j)
        {
            if(degA[i] <= degB[j])
            {
                M[i][j] = 1;
            }
        }
    }

    return M;
}

void printMatrix(vector<vector<int> > &M)
{
    ostream_iterator<int> printLine(cout, " ");
    for(int i = 0; i < M.size(); ++i)
    {
        copy(M[i].begin(), M[i].end(), printLine);
        cout << endl;
    }

}

void printVector(vector<int> &vec)
{
    ostream_iterator<int> printLine(cout, " ");
    copy(vec.begin(), vec.end(), printLine);
    cout << endl;

}

void masterReadGraph(string graph_A, vector<vector<int> > &A,
                     vector<vector<int> > &B,
                     vector<vector<int> > &M)
{
    FILE* in;
    in = fopen(graph_A.c_str(), "rb");
    A = readGraph(in);
    fclose(in);
    string graph_B = graph_A;
    graph_B[graph_B.size() - 3] = 'B';
    in = fopen(graph_B.c_str(), "rb");
    B = readGraph(in);
    fclose(in);
    M = initPermMatrix(A, B);
}

vector<vector<int> > getPermMatrix(int A_size, int B_size, const std::vector<int> &id)
{
    vector<vector<int> > M(A_size, vector<int>(B_size, 0));
    int row;
    int col;
    for(int i = 0; i < id.size(); ++i)
    {
        row = (id[i] - 1) / B_size;
        col = (id[i] - 1) % B_size;
        M[row][col] = 1;
    }
    return M;

}

vector<vector<int> > matrixTranspose(const vector<vector<int> > &M)
{
    int row = M.size();
    int col = M[0].size();
    vector<vector<int> > T(col, vector<int>(row, 0));
    for(int i = 0; i < row; ++i)
    {
        for(int j = 0; j < col; ++j)
        {
            T[j][i] = M[i][j];
        }
    }
    return T;
}

vector<vector<int> > matrixMult(const vector<vector<int> > &A, const vector<vector<int> > &B)
{
    int A_R = A.size();
    int A_C = A[0].size();
    int B_R = B.size();
    int B_C = B[0].size();
    vector<vector<int> > T(A_R, vector<int>(B_C, 0));
    for(int i = 0; i < A_R; ++i)
    {
        for(int j = 0; j < B_C; ++j)
        {
            for(int k = 0; k < A_C; ++k)
            {
                T[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return T;
}

// neighbor number, output: all vertices neighbor number
vector<int> neighbor_num(vector<vector<int> > &A)
{

    int A_R, A_C;
    // rows and columns
    A_R = A.size();
    vector<int> nei_num(vector<int> (A_R, 0));
    A_C = A[0].size();

    //nei_num.reserve(A_R);
    for (int i = 0; i < A_R; i++)
    {
        nei_num[i] = 0;
        for(int j = 0; j < A_C; j++)
        {
            if (A[i][j] == 1) // have neighbor
            {
                nei_num[i]++;
            }
        }

    }
    return nei_num;

}


// degree sort, vertices sort according to degree
void degree_sort(vector<vector<int> > &A,vector<vector<int> > &M, bool row)
{
    vector<int> nei_num;
    vector<int> rctmp; // temp value
    vector<int> rctmp_M;
    int maxIndex;
    int nei_C, M_R, M_C;
    nei_num = neighbor_num(A);
    nei_C = nei_num.size();
    M_R = M.size();
    M_C = M[0].size();
    rctmp.reserve(nei_C);
    if (row == 1)
        rctmp_M.reserve(M_C);
    else
        rctmp_M.reserve(M_R);
    for (int i = 0; i < nei_C; i++)
    {
        nei_num = neighbor_num(A);
        // find the index for the biggest degree vertex
        maxIndex = i;
        for (int j = i+1; j < nei_C; j++)
        {
            if (nei_num[j] > nei_num[maxIndex])
                maxIndex = j;
        }
        // change it with the first vertex
        if (maxIndex != i)
        {
            // row change
            for (int j = 0; j < nei_C; j++)
            {
                rctmp[j] = A[i][j];
                A[i][j] = A[maxIndex][j];
                A[maxIndex][j] = rctmp[j];
            }

            //column change
            for (int j = 0; j < nei_C; j++)
            {
                rctmp[j] = A[j][i];
                A[j][i] = A[j][maxIndex];
                A[j][maxIndex] = rctmp[j];
            }

            if (row == 1)
            {
                //row change
                for (int j = 0; j < M_C; j++)
                {
                    rctmp_M[j] = M[i][j];
                    M[i][j] = M[maxIndex][j];
                    M[maxIndex][j] = rctmp_M[j];
                }
            }
            else
            {
                //column change
                for (int j = 0; j < M_R; j++)
                {
                    rctmp_M[j] = M[j][i];
                    M[j][i] = M[j][maxIndex];
                    M[j][maxIndex] = rctmp_M[j];
                }

            }
        }

    }
    // return A;

}

vector<int> dijikstra(vector<vector<int> > &G,int n, int startnode)
{
    vector<vector<int> > cost;
    vector<int> dik_dist(vector<int>(n, 0));
    vector<int> pred;
    vector<int> visited;
    int count,mindistance,nextnode,i,j;

    cost = vector<vector<int> > (n, vector<int>(n, 0));
    pred.reserve(n);
    visited.reserve(n);

    /*pred[] stores the predecessor of each node
    count gives the number of nodes seen so far*/
    //create the cost matrix
    for(i=0; i<n; i++)
        for(j=0; j<n; j++)
            if(G[i][j]==0)
                cost[i][j]=INFINITY;
            else
                cost[i][j]=G[i][j];
    //initialize
    for(i=0; i<n; i++)
    {
        dik_dist[i]=cost[startnode][i];
        pred[i]=startnode;
        visited[i]=0;
    }
    dik_dist[startnode]=0;
    visited[startnode]=1;
    count=1;
    while(count<n-1)
    {
        mindistance=INFINITY ;
// nextnode is the node at minimum distance
        for(i=0; i<n; i++)
            if(dik_dist[i] < mindistance && !visited[i])
            {
                mindistance=dik_dist[i];
                nextnode=i;
            }
//check if a better path exist through nextnode
        visited[nextnode]=1;
        for(i=0; i<n; i++)
            if(!visited[i])
                if(mindistance+cost[nextnode][i]<dik_dist[i])
                {
                    dik_dist[i]=mindistance+cost[nextnode][i];
                    pred[i]=nextnode;
                }
        count++;
    }

    return dik_dist;
}
// output the distance matrix
vector<vector<int> > dist_matrix(vector<vector<int> > &A)
{
    vector<vector<int> > A_dist;
    vector<int> A_C_dist;
    int A_R;
    A_R = A.size();
    A_dist = vector<vector<int> > (A_R, vector<int>(A_R, 0));
    A_C_dist.reserve(A_R);
    for(int i = 0; i < A_R; i++)
    {
        A_C_dist = dijikstra(A,A_R,i);
        for (int j = 0; j <A_R; j ++)
            A_dist[i][j] = A_C_dist[j];
    }
    return A_dist;
}
// check whether find a match
bool validPermMatrix(vector<int> &used_columns, vector<vector<int> > &A, vector<vector<int> > &B)
{

    int A_size = A.size();
    for(int i = 0; i < A_size; i++)
    {
        for(int j = 0; j < A_size; j++)
        {
            if(A[i][j] == 1 && B[used_columns[i]][used_columns[j]] == 0)
                return false;
        }
    }
    return true;
}

// check whether there is at least one 1 in each row
bool oneRow(vector<vector<int> > &M) // 1 has 1, 0: no 1
{

    int M_R = M.size();
    int M_C = M[0].size();
    bool oneRowflag = 0;
    for(int i = 0; i < M_R; i++)
    {
        oneRowflag = 0;
        for(int j = 0; j < M_C; j++)
        {
            if (M[i][j] == 1)
            {
                oneRowflag = 1;
                break;
            }

        }


        if (oneRowflag == 0)
            return oneRowflag;
    }

    return oneRowflag;
}

bool recurse(vector<bool> &used_columns, vector<int> &used_col, int cur_row, vector<vector<int> > &A,
             vector<vector<int> > &B, vector<vector<int> > &M, vector<vector<int> > &A_dist,
             vector<vector<int> > &B_dist)//small graph A
{
    int num_rows_M = M.size();
    int M_C, M_R;
    bool found;
    int col_S;
    bool oneRowflag;

    vector<vector<int> > M_p = M;

    // recurse
    M_C = M[0].size(); //column size
    M_R = M.size();
    //all collumns
    for (int j = 0; j < M_C; j++)
    {
        M_p = M;
        // set this column to 1, find the 1st unused 1 in cur row, only one 1 in each row
        if((!used_columns[j]) & M[cur_row][j] == 1)
        {

            //prune
            prune_dist(A,B,M_p,A_dist,B_dist,cur_row,j);

            oneRowflag = oneRow(M_p);
            if (!oneRowflag)
            {

                continue;
            }

            used_columns[j] = true; //mark as used
            used_col.push_back(j);

            // if valid, found = 1
            if (cur_row == num_rows_M-1)
            {
                if  (validPermMatrix(used_col,A,B))
                {
                    found = 1;
                }

            }
            if (cur_row < M_R-1)
                found = recurse(used_columns,used_col, cur_row+1, A, B, M_p, A_dist, B_dist);
            //recurse
            //mark c as unused
            used_columns[j] = false;
            used_col.pop_back();
        }

    }
    return false;

}


// prune all elements in M that the dikstra distance in A is smaller than B
void prune_dist(vector<vector<int> > &A, vector<vector<int> > &B, vector<vector<int> > &M,
                vector<vector<int> > &A_dist, vector<vector<int> > &B_dist, int A_index, int B_index)
{
    //small graph A
    int A_R, B_R;

    A_R = A.size();
    B_R = B.size();

    for (int i = 0; i < A_R; i++)
    {

        if (i != A_index)
        {
            M[i][B_index] = 0;
            for (int j = 0; j < B_R; j++)
            {
                if (A_dist[A_index][i] < B_dist[B_index][j]) //// prune all elements in M that the dijkstra distance in A is smaller than B
                    M[i][j] = 0;
            }
        }

    }
}

