
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
#include "ulmann.h"
using namespace std;
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
    int s_vec = vec.size();
    for (int i = 0; i<s_vec; i++)
        cout << vec[i] << "  ";
    cout<<"\n";
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


bool recurse(vector<bool> &used_columns, vector<int> &used_col, int cur_row, vector<vector<int> > &A,
             vector<vector<int> > &B, vector<vector<int> > &M)//small graph A
{
    int num_rows_M = M.size();
    int M_C, M_R;
    bool found;
    int col_S;

    // recurse
    M_C = M[0].size(); //column size
    M_R = M.size();
    //all collumns
    for (int j = 0; j < M_C; j++)
    {

          // set this column to 1, find the 1st unused 1 in cur row, only one 1 in each row
        if((!used_columns[j]) & M[cur_row][j] == 1)
        {

            used_columns[j] = true; //mark as used
            used_col.push_back(j);

            // if valid, return true
            if (cur_row == num_rows_M-1)
            {
                if  (validPermMatrix(used_col,A,B))
                {
                     return true;
                }

            }
            if (cur_row < M_R-1)
                found = recurse(used_columns,used_col, cur_row+1, A, B, M);
            if (found)
            {
                return true;

            }
            //recurse
            //mark c as unused
            used_columns[j] = false;
            used_col.pop_back();
        }

    }
        return false;

}


vector<bool> prune(int cur_row, vector<vector<int> > &A,
             vector<vector<int> > &B, vector<vector<int> > &M)
{
    /*do
    for all (i,j) where M is 1
        for all neighbors x of vi in P
            if there is no neighbor y of vj s.t. M(x,y)=1
                M(i,j)=0
    while M was changed*/

    vector<int> neib_A;
    vector<int> neib_B;
    vector<bool> tem_used_columns;
    vector<bool> cur_row_elt;
    int A_R, A_C, B_R, B_C, M_R, M_C;
    int neib_A_s, neib_B_s;
    int found = 0;

    A_R = A.size();
    A_C = A[0].size();
    B_R = B.size();
    B_C = B[0].size();
    M_R = M.size();
	M_C = M[0].size();

    for(int j = 0; j < M_C; j++) // every point in this row
    {
        cur_row_elt.push_back(M[cur_row][j]);
    }
    for(int j = 0; j < M_C; j++) // every point in this row
    {
        cur_row_elt.push_back(M[cur_row][j]);
        neib_A.clear();
        neib_B.clear();
        if (M[cur_row][j] == 1)
        {
            for (int i = 0; i<A_C; i++) //small graph neighbor
            {
                if (A[cur_row][i] == 1)
                {
                    neib_A.push_back(i);
                }
            }
            for (int i = 0; i<B_C; i++) //big graph neighbor
            {
                if (B[j][i] == 1)
                {
                    neib_B.push_back(i);
                }
            }
            neib_A_s = neib_A.size();
            neib_B_s = neib_B.size();
            tem_used_columns.clear();
            tem_used_columns.resize(M_C,false);
            tem_used_columns[j] = true; // this point has occupied the column, so its neighbors cannot
            if (neib_A_s != 0 & neib_B_s != 0)
            found = neigbor(tem_used_columns,0,neib_A,neib_B,M);
            if (neib_A_s == 0) return cur_row_elt; // no neibors
                if(!found) //no neighbors can be projected to a neighbor in B
                {
                    cur_row_elt[j] = 0;
                }
        } // if M == 1
    } // for j

    return cur_row_elt;
}
bool neigbor(vector<bool> tem_used_columns,int neib_A_i,vector<int> neib_A,
             vector<int> neib_B,vector<vector<int> > &M)
{
    vector<bool> flag;
    bool found;
    int neib_A_s, neib_B_s;
    int f_s;

    neib_A_s = neib_A.size();
    neib_B_s = neib_B.size();
    //neib_A_i neighbor
    flag.clear();
    for(int k = 0; k<neib_B_s; k++)// all unused columns
    {

        if(!tem_used_columns[neib_B[k]] & M[neib_A[neib_A_i]][neib_B[k]] == 1)// can be projected to a neighbor
        {

            tem_used_columns[neib_B[k]] = true; //mark as used
            flag.push_back(true);
            if (neib_A_i < neib_A_s-1)
            found = neigbor(tem_used_columns,neib_A_i+1,neib_A,neib_B,M);
            if (!found) return false;
            tem_used_columns[neib_B[k]] = false;
        }

    }
    f_s = flag.size();//no projection
    if (f_s == 0) return false;

    return true;
}
