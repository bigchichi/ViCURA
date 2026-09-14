#ifndef SHRINK_H
#define SHRINK_H
#include "dbg.h"
struct comp
{
    bool operator()(const pair <ull,int> &a,const pair <ull,int> &b)const
    {
        return a.second<b.second;
    }
};
struct comp_tuple
{
    bool operator()(const tuple <ull,ull,int> &a,const tuple <ull,ull,int> &b)const
    {
            return get<2>(a)<get<2>(b);
    }
};
void OLC_shrink(vector <Reads> &reads,vector <Reads> reads_origin,vector <ull> node_list);
bool OLC_cyc_shrink(vector <Reads> &reads,vector <Reads> origin);
void OLC_remove_node(vector <Reads> &reads,ull target);
void OLC_get_root_leave(vector <Reads> &reads,vector <ull> &root,vector <ull> &leave,int cyc_size,int path_size,Parameter*para);
void OLC_cut_branches(vector <Reads> &reads,vector <ull> root,vector<ull> leave,vector <ull>&node_list);
void OLC_test(vector <Reads> reads);
void OLC_mark_reads(vector<Reads> &reads,vector<ull>root,vector<ull>leave,int reserve_length);
#endif