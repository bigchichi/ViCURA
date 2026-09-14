#include "dbg.h"
ull get_hash(string sequence,int sequence_length,Parameter*para)
{
    ull hash_value=0;
    for (int i = 0; i < sequence_length; i++)
    {
        hash_value+=para->base_value[sequence[i]]*para->hash_P_list[i];
    }
    return hash_value;
    
}
void remove_node(vector <Reads> &reads,ull node_id)
{
    vector <ull> suc=reads[node_id].successor;
    vector <ull> pre=reads[node_id].predecessor;
    reads[node_id].predecessor.clear();
    reads[node_id].successor.clear();
    reads[node_id].indegree=0;
    reads[node_id].outdegree=0;
    for(ull i=0;i<suc.size();i++)
    {
        reads[suc[i]].predecessor.erase(find(reads[suc[i]].predecessor.begin(),reads[suc[i]].predecessor.end(),node_id));
    }
    for(ull i=0;i<pre.size();i++)
    {
        reads[pre[i]].successor.erase(find(reads[suc[i]].successor.begin(),reads[suc[i]].successor.end(),node_id));
    }
}
vector<string> get_merged_sequence(Reads r1,Reads r2,int overlap_length,Parameter*para)
{
    string r1_s=r1.sequence.substr(0,para->rLengthNormal-overlap_length);
    string target_string=r1_s+r2.sequence;
    vector <string> ss;
    int k=para->rLengthNormal;
    for(int i=0;i<=target_string.size()-k;i++)
    {
        string k_mer=target_string.substr(i,k);
        ss.push_back(k_mer);
    }
    return ss;
}
void clear(queue<ull>& q) {
    queue<ull> empty;
    swap(empty, q);
}
bool if_undefined_character(string sequence)
{
    for(int i=0;i<sequence.size();i++)
    {
        if(sequence[i] !='A' && sequence[i]!='C' && sequence[i]!='G' && sequence[i]!='T')
        {
            return true;
        }
    }
    return false;
}
void Parameter :: print_current_info()
{
    cout<<"reads info"<<endl;
    cout<<"normal length is "<<rLengthNormal<<endl;
    cout<<"max length is "<<rLengthMax<<endl;
    cout<<"min length is "<<rLengthMin<<endl;
    cout<<"reads graph info"<<endl;
    cout<<"pair count is "<<pair_size<<endl;
    cout<<"de brujin graph info"<<endl;
    cout<<"edge count is "<<dbg_edge_size<<endl;
}
bool find_target(vector <ull> stack,int st,int ed,ull target)
{
    for(int i=st;i<=ed;i++)
    {
        if(stack[i]==target)
            return true;
    }
    return false;
}
string get_strain(vector <Debrujin_node> dbg,vector <ull> stack)
{
    string strain="";
    strain+=dbg[stack[0]].k_mer;
    for(int i=1;i<stack.size();i++)
    {
        strain+=dbg[stack[i]].k_mer.back();
    }
    return strain;
}
bool switch_element(vector <ull>&vec,ull target,ull switch_option)
{
    if(switch_option==-1) // delete element
    {
        for(auto it=vec.begin();it!=vec.end();it++)
        {
            if(*it==target)
            {
                vec.erase(it);
                return true;

            }
        }
        return false;

    }
    else if(switch_option==-2)//add element
    {
        bool find=false;
        for(int i=0;i<vec.size();i++)
        {
            if(vec[i]==target)
            {
                find=true;
                return false;
            }
        }
        if(find==false)
        {
            vec.push_back(target);
            return true;
        }
    }
    else //exchange element
    {
        for(int i=0;i<vec.size();i++)
        {
            if(vec[i]==target)
            {
                vec[i]=switch_option;
                return true;
            }
        }
        return false;
    }
}

void Hex_cycle_id(int cyc_time,ull &id)
{
    ull prime=0x0100000000000000;
    ull cyc=cyc_time*prime;
    id  =  id | cyc;
}
void Hex_revive(ull id,int &cyc_time,ull &dbg_id)
{
    cyc_time=id>>56;
    dbg_id=id & 0x00ffffffffffffff;
}
void Hex_get_st_ed(ull target,int &st,int &ed)
{
    st=(target & 0xffffffff00000000) >> 32;
    ed=target & 0x00000000ffffffff;
}
void Hex_merge_st_ed(ull &target,int st,int ed)
{
    target = ull(st)<<32 | ed;
}
void get_root_leaf_node(vector <Debrujin_node> dbg,vector <ull> &root,vector <ull> &leaf)
{
    for(int i=0;i<dbg.size();i++)
    {
        if(dbg[i].successor.size()>0 && dbg[i].predecessor.size()==0)
        {
            root.push_back(i);
            //cout<<i<<endl;
            //cout<<dbg[i].predecessor.size()<<endl;
        }
        if(dbg[i].predecessor.size()>0 && dbg[i].successor.size()==0)
        {
            leaf.push_back(i);
        }
    }
}
void OLC_get_root_leaf_node(vector <Reads> reads,vector <ull> &root,vector <ull> &leaf)
{
    for(int i=0;i<reads.size();i++)
    {
        if(reads[i].successor.size()>0 && reads[i].predecessor.size()==0)
        {
            root.push_back(i);
            //cout<<i<<endl;
            //cout<<reads[i].predecessor.size()<<endl;
        }
        if(reads[i].predecessor.size()>0 && reads[i].successor.size()==0)
        {
            leaf.push_back(i);
        }
    }
}  
void get_coverage_info(string path,string sequence,vector <Debrujin_node> dbg,Parameter*para)
{
    string info_path=path+"/sequence_coverage_info";
    ofstream infout(info_path,ios::app);
    //cout<<"done1"<<endl;
    unordered_map <ull,int> k_mer_hash_map;
    for(int i=0;i<dbg.size();i++)
    {
        k_mer_hash_map[dbg[i].k_mer_hash]=i;
    }
    //cout<<"done2"<<endl;
    for(int i=0;i<sequence.size();i++)
    {
        string k_mer=sequence.substr(i,150);
        //cout<<"done3"<<endl;
        if(k_mer.size()==150)
        {
            ull k_mer_hash=get_hash(k_mer,150,para);
            infout<<i<<":\t";
            if(k_mer_hash_map.count(k_mer_hash)==0)
            {
                infout<<"k_mer not found"<<endl;
            }
            else
            {
                int id=k_mer_hash_map[k_mer_hash];
                infout<<dbg[id].coverage[0]<<"\t"<<id<<endl;
            }
        }
    }
}
void coverage_info(vector <Reads> &reads,Parameter*para,string outputpath,string sequence,Dynamic_Tree*root,vector <Reads> &seq_reads)
{
    vector <int> cov(sequence.size()+100);
    //cout<<cov.capacity()<<endl;
    ofstream covout(outputpath);
    int origin_size=reads.size();
    //cout<<"origin size "<<origin_size<<endl;
    // for(int i=0;i<sequence.size()-para->rLengthNormal;i++)
    // {
    //     string kmer=sequence.substr(i,para->rLengthNormal);
    //     cout<<kmer<<endl;
    //     //cout<<sequence.substr(i+1,para->rLengthNormal)<<endl;
    //     ull exp_id=reads.size();
    //     ull ret_id=Dtree_accept(kmer,root,exp_id);
    //     if(exp_id==ret_id)
    //     {
    //         if(ret_id>=reads.size())
    //         {
    //             cout<<"sequence inside repeat "<<ret_id<<" "<<i<<endl;
    //         }
    //     }
    //     else
    //     {
    //         cout<<"align "<<ret_id<<" "<<i<<endl;
    //         seq_reads.push_back(reads[ret_id]);
    //         for(int j=0;j<para->rLengthNormal;j++)
    //         {
    //             cov[i+j]+=reads[ret_id].read_count;
    //         }
    //     }
    // }
    // for(int i=1;i<sequence.size();i++)
    // {
    //     covout<<sequence[i]<<":\t"<<cov[i]<<endl;
    // }
    sequence="$"+sequence;
    int suc_size=para->rLengthNormal-1;
    vector <pair<int,ull>> pos_r;
    pos_r.reserve(50000);
    //cout<<pos_r.capacity()<<endl;
    seq_reads.reserve(50000);
    for(int i=0;i<sequence.size()-para->rLengthNormal;i++)
    {
        string kmer=sequence.substr(i,para->rLengthNormal);
        string r_=sequence.substr(i+1,para->rLengthNormal);
        //cout<<kmer<<endl;
        //cout<<r_<<endl;
        //cout<<sequence.substr(i+1,para->rLengthNormal)<<endl;
        vector <ull> suc=Dtree_successor_accept(root,kmer,suc_size);
        //cout<<suc.size()<<endl;
        for(int j=0;j<suc.size();j++)
        {
            string r=reads[suc[j]].sequence;
            //cout<<r<<endl;
            // if(reads[suc[j]].read_count<2) 
            //     continue;
            if(r==r_)
            {
                pair<int,ull>temp(i+1,suc[j]);
                pos_r.push_back(temp);
                //cout<<"cap "<<pos_r.capacity()<<endl;
                seq_reads.push_back(reads[suc[j]]);
                for(int k=0;k<para->rLengthNormal;k++)
                {
                    cov[i+k+1]+=reads[suc[j]].read_count;
                }
            }
        }
        //cout<<endl;
    }
    int pos_count=0;
    pair<int,ull>temp(0,0);
    pos_r.push_back(temp);
    //cout<<pos_r.capacity()<<endl;
    for(int i=0;i<sequence.size();i++)
    {
        covout<<sequence[i]<<":\t"<<cov[i]<<" ";
        int pos=pos_r[pos_count].first;
        if(pos==i)
        {
            covout<<pos_r[pos_count].second<<" "<<reads[pos_r[pos_count].second].read_count<<" "<<reads[pos_r[pos_count].second].sequence;
            pos_count++;
        }
        covout<<endl;        
    }
    auto max_cov=max_element(cov.begin(),cov.end());
    auto min_cov=min_element(cov.begin()+2,cov.end());
    double avg_cov=accumulate(cov.begin(),cov.end(),0.0)/cov.size();
    covout<<"max cov "<<*max_cov<<endl;
    covout<<"min cov "<<*min_cov<<endl;
    covout<<"avg cov "<<avg_cov<<endl;

}
int find_vec_index(vector<ull> &vec,ull target)
{
    //cout<<"start cycle"<<endl;
    for(int i=0;i<vec.size();i++)
    {
        //cout<<"gothrough vec "<<i<<endl;
        if(vec[i]==target)
            return i;
    }
    return -1;
}

 
int EditDis(string reads1,string reads2)
{
    reads1="$"+reads1;
    reads2="$"+reads2;
    int len1 = reads1.size();
    int len2 = reads2.size();
    int INF=10000;
    vector<vector<int>> dp(len1+1,vector<int>(len2+1));
    for(int i=1;i<=len1;i++)
        for(int j=1;j<=len2;j++)
            dp[i][j] = INF;
    for(int i=1;i<=len1;i++)
        dp[i][0] = i;
    for(int j=1;j<=len2;j++)
        dp[0][j] = j;
    for(int i=1;i<=len1;i++)
    {
        for(int j=1;j<=len2;j++)
        {
            int flag;
            if(reads1[i]==reads2[j])
                flag=0;
            else
                flag=1;
            dp[i][j]=min(dp[i-1][j]+1,min(dp[i][j-1]+1,dp[i-1][j-1]+flag));
        }
    }
    dp.~vector();
    return dp[len1][len2];
}
void getFiles(string path, vector<string>& filenames)
{
	DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
            filenames.push_back(path + "/" + ptr->d_name);
    	}
    }
    closedir(pDir);
}