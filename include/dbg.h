#ifndef DBG_H
#define DBG_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <map>
#include <vector>
#include <cmath>
#include <iomanip>
#include <malloc.h>
#include <unordered_map>
#include <queue>
#include <set>
#include <unordered_set>
#include <numeric>
#include <ctime>
#include<stdio.h>
#include <sys/io.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
// #include <fast_io.h>
// #include <fast_io_legacy.h>
// #include <fast_io_device.h>
#include "mpi_wrapper.h"
//#include "dynamic_tree.h"
using namespace std;
typedef unsigned long long ull;
#define CMD_RESULT_BUF_SIZE 1024
class Parameter
{
    public:
        ull reads_count=0;
        int rLengthNormal;
        int kmerlength;
        int rLengthMax;
        int rLengthMin;
        string work_space; 
        ull hash_N=18446744073709551615;
        int hash_P=131;
        char base_list[4]={'A','T','G','C'};
        ull hash_P_list[500];
        map <char,char> base_map;
        map <char,int>  base_value;
        int sequence_o=21;
        int pair_size;
        int dbg_edge_size;
    Parameter()
    {
        base_map['A']='T';
        base_map['T']='A';
        base_map['G']='C';
        base_map['C']='G';
        base_map['N']='N';
        base_value['A']=1;
        base_value['T']=2;
        base_value['G']=3;
        base_value['C']=4;
        base_value['N']=5;
        hash_P_list[0]=1;
        for (int i = 1; i < 500; i++)
        {
            hash_P_list[i]=hash_P_list[i-1]*hash_P;
        }
        pair_size=0;
        dbg_edge_size=0;
        rLengthMax=-1;
        rLengthMin=-1;
        rLengthNormal=-1;
    }
    void print_current_info();
};
class Dtree_leaf
{
    public:
        ull id;
    Dtree_leaf(){}
};
class Dynamic_Tree
{
    public:
        void*A_next=NULL;
        void*C_next=NULL;
        void*G_next=NULL;
        void*T_next=NULL;
    Dynamic_Tree(){}
        //ull ID;
    void add_child(int tag);
    void add_leaf(int tag,ull ex_id);
};
class Reads
{
    public:
        string sequence;
        int length;
        ull read_count=1;
        ull r1_count=0;
        ull r2_count=0;
        ull read_per_usage=0;
        ull read_suf_usage=0;
        ull sequence_hash;
        vector <ull> perifix_array;
        vector <ull> suffix_array;
        ull seq_ID;
        int layer;
        //bool usage=1;
        ull indegree=0;
        ull outdegree=0;
        vector <ull> successor;
        vector <int> successor_o;
        vector <ull> predecessor;
        vector <int> predecessor_o;
        vector <ull> next;
        vector <int> cov;
        vector <int> suc_cov;
        vector <int> pre_cov;
        bool reserve_tag=false;
        bool leserve_tag=false;
        ull cyc_id=-1;
        bool cyc=0;
        void array_init(Parameter*para);
        bool self_cycle();
        bool find(int type,ull target);
        void print(string path);
};
class Pairs
{
    public:
        ull st_ID;
        ull ed_ID;
        ull pairs_hash;
        int overlap_length;
        int total_length; 
        string uov_sequence;
        void uov_init(Reads st,Reads ed,Parameter*para);
};
class Files
{
    public:
        ifstream fastq_R1,fastq_R2,sam;
        ofstream out;
    Files()
    {
        fastq_R1.close();
        fastq_R2.close();
        out.close();
    }
    std::vector<Reads> file_read(string file_name_R1,string file_name_R2,Parameter*para,int tag);
    void file_read_Dtree(string file_name_R1,string file_name_R2,Parameter*para,int tag,Dynamic_Tree*root,vector<Reads> &reads);
    std::vector<Reads> file_read_sam(string sam_name,Parameter*para);
    std::vector<Reads> file_read_Dtree_shrink(string file_name_R1,string file_name_R2,Parameter*para,int tag,Dynamic_Tree*root,int threshold);
    std::vector<Reads> file_read_Dtree_frequency_filter(string file_name_R1,string file_name_R2,Parameter*para,int tag,Dynamic_Tree*root,int threshold);
    std::vector<Reads> file_read_Dtree_mem(string file_name_R1,string file_name_R2,vector<Reads> &treads,Parameter*para,int tag,Dynamic_Tree*root,Dynamic_Tree*root2,int threshold);
};
class Zipline
{
    public:
        vector <ull> zip_stack;
        //vector <string> zip_s_stack;
    //bool find_string_element(string target);
};
class Subgraph
{
    public:
        vector <ull> node_list;
        vector <ull> root_list;
        vector <ull> leave_list;
        ull size=0;
};

class Debrujin_node
{
    public:
        string k_mer;
        vector <ull> successor;
        vector <ull> predecessor;
        int cycle_tag=-1;
        int layer=-1;
        int bubble_tag=0;
        ull k_mer_hash;
        ull prefix_hash;
        ull suffix_hash;
        bool read_st=false;
        ull read_id=-1;
        ull id=-1;
        bool reserve_tag=false;
        //unordered_map <ull,ull> readID_sted_map;void coverage_init(ull readA,ull readB,int overlap,int st,int length);
        unordered_set <ull> ReadSet_st;
        unordered_set <ull> ReadSet_ed;
        vector <pair<ull,ull>> pair_vec;
        vector <int> coverage;
        int cov;
        void init(Parameter*para);
        void coverage_update(ull readA,ull readB,int overlap,int st,int length,int Acount,int Bcount);
        Debrujin_node()
        {
            for(int i=0;i<300;i++)
            {
                coverage.push_back(0);
            }
        }

};
class sdbg
{
    public:
        int base;
        string kmer="";
        vector <ull> successor;
        vector <ull> predecessor;
       // vector <pair<ull,ull>> pair_vec;
        int cov;
        vector<int>coverage;
        int cycle_tag=-1;
        ull read_tag=-1;
};
class Cycle
{
    public:
        ull st;
        vector < vector<ull> > cycle_list;
        ull cycle_time=-1;
        bool read_st=false;
        //vector <vector<int>> overlap_info;
        unordered_map <ull,pair<int,ull>> suc_map;
        unordered_map <ull,pair<int,ull>> pre_map;
        unordered_set <ull> original_pre;
        unordered_set <ull> original_suc;
        vector <int> cycle_order_length;
        vector <int> cycle_order_count;
        vector <int> cyc_length;
        unordered_set <ull> cycle_container;
        unordered_map <ull,int> cycle_map;
        vector <vector<int>> cycle_container_vector;
        vector <pair<int,int>> cycle_pair;
        vector <int> suc_weight;
        vector <int> pre_weight;
        vector <ull> cur_suc;
        vector <ull> cur_pre;
        int max_tag=-1;
        int max_length=0;
        void print(int cyc_id);
        void init(vector <ull> suc,vector<ull> pre);
        //void cyc_shrink(vector <ull> cyc_path,vector<Debrujin_node> &dbg);
        void cyc_shrink_sdbg(vector <ull> stack,vector<sdbg>&dbg,unordered_set<ull>&suc_set,unordered_set<ull>&pre_set);
        void cyc_shrink_reads(vector <ull> stack,vector <Reads>&reads  ,unordered_set<ull>&suc_set,unordered_set<ull>&pre_set);
        void init(Debrujin_node dbg_st);
        void cycle_sort();
        void cycle_sort_reads(vector <Reads> &reads,Parameter*para,vector <unordered_map<ull,int>> &posmap,vector <Cycle>&cyc);

};

class Result
{
    public:
        int strain_num=0;
        vector <ull> path;
        string strain;
    void strain_print(vector <Debrujin_node> dbg,vector <Cycle> cyc,string result_path);
};
class Topology
{
    public:
        unordered_set <ull> root_reachable;
    void inherit(unordered_set <ull> reachable);

};

ull Dtree_accept(string kmer,Dynamic_Tree*root,ull exptected_id);
vector <ull> Dtree_successor_accept(Dynamic_Tree*root,string kmer,int length);
void init_debrujin_Dtree(vector <Reads> reads,vector <Debrujin_node>&dbg,Parameter*para);
void get_pairs_Dtree(vector <Reads> &reads,Parameter*para,int st_o,int ed_o,Dynamic_Tree*root);
void Hex_cycle_id(int cyc_time,ull &id);
void Hex_revive(ull id,int &cyc_time,ull &dbg_id);
void Hex_get_st_ed(ull target,int &st,int &ed);
void Hex_merge_st_ed(ull &target,int st,int ed);
void Hex_bub_revise(Debrujin_node &node,int id);
vector <int> Hex_bub_revive(Debrujin_node node);
void get_root_leaf_node(vector <Debrujin_node> dbg,vector <ull> &root,vector <ull> &leaf);
void OLC_get_root_leaf_node(vector <Reads> reads,vector <ull> &root,vector <ull> &leaf);
string sequence_deal(string sequence,int sequence_length,Parameter*para);
bool if_undefined_character(string sequence);
bool find_target(vector <ull> stack,int st,int ed,ull target);
bool switch_element(vector <ull>&vec,ull target,ull switch_option);
ull get_hash(string sequence,int sequence_length,Parameter*para);
void get_pairs(vector <Reads>&reads,Parameter*para);
//void delete_isolated_reads(vector <Reads> reads);
void debrujin_delete_node (vector<Debrujin_node>&dbg,int node_id);
void Sdebrujin_delete_node (vector<sdbg>&dbg,int node_id);
bool if_debrujin_connected(vector <Debrujin_node> dbg);
vector <string> get_merged_sequence(Reads r1,Reads r2,int overlap_length,Parameter*para);
void init_debrujin(Subgraph sub,vector <Reads> reads,vector <Debrujin_node>&dbg,Parameter*para);
void remove_low_frequency_node(vector <Debrujin_node> &dbg,int threshold);
bool compare(const pair<ull,double> &p1,const pair<ull,double> &p2);
void average_count_sort_dbg(vector <Debrujin_node> &dbg,Parameter*para);
void delete_by_order(vector <Debrujin_node> &dbg,vector<ull>&root_list,vector <ull> pure_del,ull del);
//void get_root_leave(vector <Debrujin_node> dbg,vector <ull> &root,vector <ull> &leave,int root_size,int leave_size);
void get_root_leave(vector <Debrujin_node> &dbg,vector <ull> &root,vector <ull> &leaf);
void get_root_leave_Sdbg(vector <sdbg> &dbg,vector <ull> &root,vector <ull> &leave,int reserve_size,vector <Cycle> &cyc,vector <Result> &results);
void remove_node(vector <Reads> &reads,ull node_id);
void subgraph(vector <Subgraph> &sub,vector <Reads> reads);
void detect_cyc(vector <Debrujin_node>&dbg,ull root,vector <Cycle>&cyc);
vector <Subgraph> large_sub(vector <Subgraph> sub,int size);
void clear(queue<ull>& q);
bool is_cycle(vector <ull> stack,vector <ull> &cycle_list,vector<Debrujin_node>dbg,ull cyc_st);
bool traversal_v1(vector <Debrujin_node> dbg,ull root,ull leave,vector <Result>& result,vector <int>&position);
string get_strain(vector <Debrujin_node> dbg,vector <ull> stack);
void shortest_path(vector <Debrujin_node> dbg,ull root,ull leave,vector<ull>&path);
void strain_print(vector<Debrujin_node>dbg,vector<Cycle>cyc,vector <Result> result,string result_path,Parameter*para);
void get_coverage_info(string path,string sequence,vector <Debrujin_node> dbg,Parameter*para);
void cut_branches(vector <Debrujin_node> &dbg,ull root,ull leave);
vector <ull> DFS_target(vector<Debrujin_node> dbg,ull st, ull ed);
void bubble_detect(vector<Debrujin_node>&dbg,string result_path);
void read_reads_info(vector <Reads> &reads,string path,int file_num,int option);
int ExecuteCMD(const char *cmd);
void read_dbg_info(string dbg_file,string pre_file,int minimizer_length,vector<Debrujin_node> &dbg);
void dtree_break(Dynamic_Tree *root,int depth);
void file_kmer_info_read(string path,vector<unordered_set<ull>> front,vector<unordered_set<ull>> back,int file_num);
void cycle_sort_OLC(vector <Reads> &reads,vector <Reads> &origin,vector<Cycle> &cyc,int tag);
bool OLC_cyc_shrink_detail(vector <Reads> &reads,vector <Reads> origin,vector <Cycle> &cyc);
void OLC_cyc_sort(vector <Reads> &reads,vector <Reads> &origin,vector <Cycle> &cyc);
void get_root_leave_OLC(vector <Reads> &reads,vector <ull> &root,vector <ull> &leave,int reserve_size,vector <Cycle> &cyc,vector<Result> &results);
void strain_print_reads( vector<Reads>&reads, vector <Reads> &origin,vector<Cycle>&cyc, vector <Result> &result,string result_path,Parameter*para);
int find_vec_index(vector<ull> &vec,ull target);
void coverage_info(vector <Reads> &reads,Parameter*para,string outputpath,string sequence,Dynamic_Tree*root,vector <Reads> &seq_reads);
void reads_combine(vector <Reads> &reads,vector <Reads> SeqReads,Dynamic_Tree*root);
void get_pairs_Dcombine(vector <Reads> &full_reads,Dynamic_Tree*root1,vector<Reads> &reads,Dynamic_Tree*root2,int st_o,int ed_o,int mid_o,int threshold);
void cycle_sort_SDBG(vector <sdbg> &dbg,vector <sdbg> &origin,vector<Cycle> &cyc,int tag);
int EditDis(string reads1,string reads2);
#endif