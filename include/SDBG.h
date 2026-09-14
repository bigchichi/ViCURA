#ifndef SDBG_H
#define SDBG_H
#include "dbg.h"


void read_sdbg_info(string dbg_file,string pre_file,int minimizer_length,vector<sdbg> &dbg,Parameter*para,vector<string> r_root,vector<string> l_root)
{
    int bucket_num=(int)pow(4.0,(double)minimizer_length);
    vector <string> kmerv;
    cout<<"into "<<bucket_num<<endl;
    int edge_num=0;
    for(int i=0;i<bucket_num;i++)
    {
        string dbg_filepath=dbg_file+"/kmer150_unique_"+to_string(i);
        string bucket_file=dbg_file+"/bucket_num_"+to_string(i);
        string pre_filepath=pre_file+"/preinfo_"+to_string(i);
        cout<<"doing "<<i<<endl;
        //cout<<pre_filepath<<" "<<dbg_filepath<<endl;
        string numstring,line1,line2,reg;
        ifstream dbgin(dbg_filepath);
        ifstream prein(pre_filepath);
        ifstream bin(bucket_file);
        getline(bin,numstring);
        stringstream ss(numstring);
        int num;
        ss>>num;
        int dbg_count=0;
        while (true)
        {
            bool tg1= static_cast<bool>(getline(dbgin,line1));
            bool tg2= static_cast<bool>(getline(prein,line2));
            //cout<<line1<<" "<<line2<<endl;
            //cout<<tg1<<" "<<tg2<<endl;
            if(!tg1 && !tg2)
                break;
            stringstream s1(line1);
            stringstream s2(line2);
            vector <ull> pre;
            sdbg temp;
            string kmer;
            int id,cov;
            int count=0;
            while(getline(s1,reg,' '))
            {
                if(count==0)
                {
                    kmer=reg;
                }
                else if(count==1)
                {
                    stringstream tmp(reg);
                    tmp>>id;
                }
                else if(count==2)
                {
                    stringstream tmp(reg);
                    tmp>>cov;
                }
                count++;   
            }
            while ((getline(s2,reg,' ')))
            {
                stringstream tmp(reg);
                int pretemp;
                tmp>>pretemp;
                pre.push_back(pretemp);
                //cout<<pretemp<<" ";
            }
            //cout<<endl;
            temp.base=para->base_value[kmer.back()];
            temp.cov=cov;
            temp.predecessor=pre;
            temp.kmer=kmer;
            edge_num+=pre.size();
            kmerv.push_back(kmer);
            dbg.push_back(temp);
            dbg_count++;
        }
        cout<<num<<" "<<dbg_count<<endl;
        if(num!=dbg_count)
            cout<<"error "<<endl;
        cout<<"done "<<dbg.size()<<endl;
        cout<<"edge num: "<<edge_num<<endl;
        
    }
    cout<<"dbg size "<<dbg.size()<<endl;
    cout<<edge_num<<endl;
    for(int i=0;i<dbg.size();i++)
    {
        //cout<<i<<endl;
        for(int j=0;j<dbg[i].predecessor.size();j++)
        {
            ull pre=dbg[i].predecessor[j];
            //cout<<pre<<endl;
            dbg[pre].successor.push_back(i);
        }
    }
    int edg=0;
    for(int i=0;i<dbg.size();i++)
    {
        edg+=dbg[i].successor.size();
        //cout<<i<<endl;
        
    }
    cout<<"edg size "<<edg<<endl;
    int root_num=0,leave_num=0;
    string rootpath=dbg_file+"/rootinfo";
    string leavepath=dbg_file+"/leaveinfo";
    ofstream rootout(rootpath);
    ofstream leaveout(leavepath);
    for(int i=0;i<dbg.size();i++)
    {
        if(dbg[i].predecessor.size()==0  && dbg[i].successor.size()>0)
        {
            rootout<<kmerv[i]<<" "<<i<<endl;
            r_root.push_back(kmerv[i]);
            root_num++;
        }
    } for(int i=0;i<dbg.size();i++)
    {
        if(dbg[i].predecessor.size()>0  && dbg[i].successor.size()==0)
        {
            leaveout<<kmerv[i]<<" "<<i<<endl;
            l_root.push_back(kmerv[i]);
            leave_num++;
        }
    }
    cout<<root_num<<" "<<leave_num<<endl;
}
void average_count_sort_Sdbg(vector <sdbg> &Sdbg,Parameter*para)
{
    vector <pair<ull,double>> average_count;
    for (int i=0;i<Sdbg.size();i++)
    {
        average_count.clear();
        if(Sdbg[i].successor.size()<=1)
            continue;
        vector <vector <ull>> path;
        for(int j=0;j<Sdbg[i].successor.size();j++)
        {
            
            pair <ull,double> p;
            vector <ull> temp_path;
            ull next=Sdbg[i].successor[j];
            p.first=next;
            int total_count=Sdbg[next].cov;
            int length=1;
            temp_path.push_back(next);
            while (true)
            {
               
                if(Sdbg[next].successor.size()==1 && Sdbg[next].predecessor.size()==1)
                {
                    next=Sdbg[next].successor[0];
                }
                else if(Sdbg[next].successor.size()==0 || Sdbg[next].successor.size()>1 ||Sdbg[next].predecessor.size()>1 || Sdbg[next].cycle_tag!=-1)
                {
                    break;
                }

                total_count+=Sdbg[next].cov;
                length++;
                temp_path.push_back(next);
            }
            double average=double(total_count)/double(length);
            p.second=average;
            average_count.push_back(p);
            path.push_back(temp_path);

        }
        stable_sort(average_count.begin(),average_count.end(),compare);
        vector <ull> result_suc;
        //cout<<i<<endl;
        Sdbg[i].successor.clear();
        for(int j=0;j<average_count.size();j++)
        {
            result_suc.push_back(average_count[j].first);
            Sdbg[i].successor.push_back(average_count[j].first);
            // cout<<average_count[j].first<<"---"<<average_count[j].second<<endl;
            // for(int k=0;k<path[j].size();k++)
            // {
            //     cout<<path[j][k]<<" ";
            // }
            // cout<<endl;
        }

          //cout<<endl;
          //cout<<endl;

    }

}
bool detect_cyc_sdbg(vector <sdbg>&Sdbg,vector <Cycle>&cyc,vector <ull> root_v)
{
    ull size=Sdbg.size();
    vector <ull> path_stack;
    vector <int> visit(size);
    vector <int> position(size);
    vector <int> stack_condition(size);
    unordered_map <ull,int> id_node_map;
    bool cyc_tag=false;
    vector <unordered_set<ull>> node_suc;
    vector <unordered_set<ull>> node_pre;
    int max_depth=0;
    for(int i=0;i<root_v.size();i++)
    {
        ull root=root_v[i];
        //cout<<root<<endl;
        path_stack.push_back(root);
        visit[root]=1;
        stack_condition[root]++;
        bool jump_flag=false;
        bool cyc_detect=false;
        while (path_stack.size()!=0)
        {
            if(path_stack.size()>max_depth)
                max_depth=path_stack.size();
            ull back=path_stack.back();
            //cout<<path_stack.size()<<endl;
            jump_flag=false;
            cyc_detect=false;
            int i=position[back];
            if(position[back]>=Sdbg[back].successor.size())
            {
                stack_condition[back]--;
                path_stack.pop_back();
                continue;
            }
            for(;i<Sdbg[back].successor.size();i++)
            {
                ull next=Sdbg[back].successor[i];
                if(visit[next]==0)
                {
                    visit[next]=1;
                    path_stack.push_back(next);
                    stack_condition[next]++;
                    jump_flag=true;
                    i++;
                    break;
                }
                else
                {
                    //vector <ull> temp_stack;
                    //bool judge=is_cycle(path_stack,temp_stack,Sdbg,next);
                    int judge=stack_condition[next];
                    //if we find a cycle
                    if(judge>=1)
                    {
                        cout<<back<<" "<<position[back]<<" "<<position[next]<<" "<<Sdbg[next].successor.size()<<endl;
                        vector <ull> stack;
                        stack.push_back(next);
                        cyc_tag=true;
                        for (int p=path_stack.size()-1;p>=0;p--)
                        {
                            stack.push_back(path_stack[p]);
                            if(path_stack[p]==next)
                                break;
                        }
                        reverse(stack.begin(),stack.end());
                        for(int p=1;p<stack.size()-1;p++)
                        {
                            if(stack[p]==next)
                                cout<<"what the fuck stack "<<p<<" "<<next<<endl;
                        }
                        cyc_detect=true;
                        if(Sdbg[next].cycle_tag==-1)
                        {
                            id_node_map[next]=node_suc.size();
                            unordered_set<ull>suc,pre;
                            for(int p=0;p<Sdbg[next].successor.size();p++)
                            {
                                suc.insert(Sdbg[next].successor[p]);
                            }
                            for(int p=0;p<Sdbg[next].predecessor.size();p++)
                            {
                                pre.insert(Sdbg[next].predecessor[p]);
                            }
                            node_suc.push_back(suc);
                            node_pre.push_back(pre);
                            cout<<"cyc detected,st:  "<<next<<" length: "<<stack.size()<<endl;
                            Cycle temp;
                            temp.init(Sdbg[next].successor,Sdbg[next].predecessor);
                            temp.st=next;
                            //temp.cycle_time=1;
                            //temp.init(Sdbg[next]);
                            int node_vec_back=node_suc.size()-1;
                            temp.cyc_shrink_sdbg(stack,Sdbg,node_suc[node_vec_back],node_pre[node_vec_back]);
                            cyc.push_back(temp);
                            Sdbg[next].cycle_tag=cyc.size()-1;
                            cout<<Sdbg[next].cycle_tag<<endl;
                        }
                        else
                        {
                            cout<<"complex cyc detected,st:  "<<next<<" length: "<<stack.size()<<endl;
                            ull id=Sdbg[next].cycle_tag;
                            int node_vec_id=id_node_map[next];
                            cout<<"into shrink"<<endl;
                            cyc[id].cyc_shrink_sdbg(stack,Sdbg,node_suc[node_vec_id],node_pre[node_vec_id]);
                            cout<<"shrink done"<<endl;
                        }
                        while (true)
                        {
                            ull path_back=path_stack.back();
                            if(path_stack.back()!=next)
                            {
                                stack_condition[path_back]--;
                                path_stack.pop_back();
                            }
                            else
                                break;
                        }
                        i++;
                        break;
                        
                    }

                }
            }
            position[back]=i;
        }
    }
    cout<<"max depth "<<max_depth<<endl;
    cout<<count(visit.begin(),visit.end(),1)<<endl;
    return cyc_tag ;
}
void traversal_Sdbg(const vector <sdbg> &dbg,ull root,vector<ull> leave,vector <Result>& result,vector <int>&position)
{
    vector <ull> stack;
    unordered_set<ull>leaf_set;
    for(int i=0;i<leave.size();i++)
    {
        leaf_set.insert(leave[i]);
    }
    stack.push_back(root);
    bool jumptag=false;
    int empty=dbg.size()-1;
    cout<<"start travel"<<endl;
    int add=0;
    while (true)
    {
        ull back=stack.back();
        if(stack.size()==0)
            break;
        if(dbg[back].successor.size()==0 && dbg[back].predecessor.size()>0)
        {
            //cout<<"strain done "<<strain_count<<endl;
            cout<<"find leave"<<endl;
            if(leaf_set.count(back)==1)
            {
                Result temp;
                temp.path=stack;
                result.push_back(temp);
                cout<<stack.size()<<endl;
                break;
                //cout<<"add 2"<<endl;               
            }
            //     add++;
            //     Result temp;
            //     temp.path=stack;
            //     result.push_back(temp);
            //     cout<<stack.size()<<endl;            
            // if(add>=500)
            //     break;
            stack.pop_back();
            continue;
            
        }
        if(back==empty || position[back]>=dbg[back].successor.size())
        {
            stack.pop_back();
            continue;
        }
        ull next=dbg[back].successor[position[back]];
        //cout<<"using another suc "<<back<<" "<<position[back]<<endl;
        stack.push_back(next);
        position[back]++;
    }
}
void strain_print_Sdbg( vector<sdbg>&dbg, vector<Cycle>&cyc, vector <Result> &result,string result_path,Parameter*para)
{
    int dbg_size=dbg.size();
    for(int i=0;i<result.size();i++)
    {
        vector <ull> path=result[i].path;
        unordered_set<ull> cyc_set;
        //vector <ull> true_path;
        int tag=0;
        while (true)
        {
            if(tag>=path.size())
                break;
            ull current_node=path[tag];
            cout<<tag<<" "<<current_node<<endl;
            if(dbg[current_node].cycle_tag==-1)
            {
                //true_path.push_back(current_node);
                tag++;
            }
            else
            {
                
                int cyc_tag=dbg[current_node].cycle_tag;
                ull pre=path[tag-1];
                ull suc=path[tag+1];
                if(cyc[cyc_tag].original_pre.count(pre)==1 && cyc[cyc_tag].original_suc.count(suc)==1)
                {
                    cout<<"good cycle"<<endl;
                    if(cyc_set.count(current_node)==0)
                    {
                        path.erase(path.begin()+tag);
                        vector<ull>max_path=cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag];
                        ull pre_test=max_path[max_path.size()-2];
                        ull suc_test=max_path[1];
                        cout<<pre_test<<endl;
                        cout<<suc_test<<endl;
                        cout<<max_path[0]<<endl;
                        cout<<max_path.back()<<endl;
                        cout<<cyc[cyc_tag].original_pre.count(pre_test)<<endl;
                        cout<<cyc[cyc_tag].original_suc.count(suc_test)<<endl;
                        cout<<max_path.size()<<endl;
                        path.insert(path.begin()+tag,max_path.begin(),max_path.end());
                        for(int p=1;p<max_path.size()-1;p++)
                        {
                            if(max_path[p]==current_node)
                                cout<<"error "<<p<<endl;
                        }
                        cout<<"whole cycle"<<endl;
                        cyc_set.insert(current_node);
                        continue;
                    }
                    
                    tag++;
                    continue;
                }
                cyc_set.insert(current_node);
                cout<<"into cyc recover"<<endl;
                cout<<"pre "<<cyc[cyc_tag].original_pre.count(pre)<<endl;
                cout<<"suc "<<cyc[cyc_tag].original_suc.count(suc)<<endl;
                vector <ull> add_path;
                if(cyc[cyc_tag].pre_map.count(pre)==1)
                {
                    auto id=cyc[cyc_tag].pre_map[pre];
                    ull dbg_id=id.second;
                    int cyc_id=id.first;
                    //Hex_revive(id,cyc_id,dbg_id);
                    vector<ull>cyc_path=cyc[cyc_tag].cycle_list[cyc_id];
                    bool visit=false;
                    cout<<"dbg_id "<<dbg_id<<endl;
                    for(int j=0;j<cyc_path.size();j++)
                    {
                        if(cyc_path[j]==dbg_id)
                            visit=true;
                        if(visit)
                        {
                            add_path.push_back(cyc_path[j]);
                            continue;
                        }

                        
                    }
                }
                else
                {
                    add_path.push_back(current_node);
                }
                // add_path.pop_back();
                // cout<<cyc[cyc_tag].original_pre.count(add_path.back())<<endl;
                // ull pre_test=cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag][cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag].size()-2];
                // ull suc_test=cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag][1];
                 cout<<add_path.size()<<endl;
                // cout<<cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag].size()<<endl;
                // cout<<cyc[cyc_tag].original_pre.count(pre_test)<<endl;
                // cout<<cyc[cyc_tag].original_suc.count(suc_test)<<endl;
                //add_path.insert(add_path.end(),cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag].begin(),cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag].end());
                // cout<<add_path.size()<<endl;
                if(cyc[cyc_tag].suc_map.count(suc)==1)
                {
                    auto id=cyc[cyc_tag].suc_map[suc];
                    ull dbg_id=id.second;
                    int cyc_id=id.first;
                   // Hex_revive(id,cyc_id,dbg_id);
                    vector<ull>cyc_path=cyc[cyc_tag].cycle_list[cyc_id];
                    cout<<cyc[cyc_tag].original_suc.count(cyc_path[1])<<endl;
                    bool visit=false;
                    cout<<"dbg_id "<<dbg_id<<endl;
                    for(int j=1;j<cyc_path.size();j++)
                    {
                        add_path.push_back(cyc_path[j]);
                        if(cyc_path[j]==dbg_id)
                            break;
                        
                    }
                }
                cout<<add_path.size()<<endl;
                
                path.erase(path.begin()+tag);
                path.insert(path.begin()+tag,add_path.begin(),add_path.end());
                cout<<"cyc recover done"<<endl;
            }
        }
        cout<<"strain done"<<endl;
        string output_path=result_path+"/strain_"+to_string(i);
        ofstream strain_out(output_path);
        strain_out<<"length "<<path.size()<<endl;
        string strain="";
        for(int j=0;j<path.size();j++)
        {
            strain+=para->base_list[dbg[path[j]].base-1];
        }
        strain_out<<strain<<endl;
        for(int j=0;j<path.size();j++)
        {
            strain_out<<dbg[path[j]].kmer<<endl;
        }
    }

}
void coverage_deal(string coverageinfo_path,Dynamic_Tree*root,vector<Reads> &reads,Dynamic_Tree*root2,vector<Reads> &reads2)
{
    string strain="$";
    
    ifstream covin(coverageinfo_path);
    string alignoutpath="align2";
    int rlengthnormal=reads[0].sequence.size();
    ofstream alignout(alignoutpath); 
    string line;
    getline(covin,line);
    getline(covin,line);
    strain+=line;
    vector<int>coverage(strain.size());
    for(int i=0;i<strain.size()-rlengthnormal;i++)
    {
        string kmer=strain.substr(i,rlengthnormal);
        //cout<<kmer<<endl;
        string seq=strain.substr(i+1,rlengthnormal);
        //cout<<seq<<endl;
        
        vector<ull> protentail_reads;
        protentail_reads=Dtree_successor_accept(root,kmer,rlengthnormal-1);
        for(int j=0;j<protentail_reads.size();j++)
        {
            ull node=protentail_reads[j];
            if(seq==reads[node].sequence)
            {
                for(int k=0;k<rlengthnormal;k++)
                {
                    coverage[i+k+1]+=reads[node].read_count;
                }
                //cout<<i+1<<endl;
            }
        }
        cout<<i<<endl;
        protentail_reads=Dtree_successor_accept(root2,kmer,rlengthnormal-1);
        for(int j=0;j<protentail_reads.size();j++)
        {
            ull node=protentail_reads[j];
            if(seq==reads2[node].sequence)
            {
                for(int k=0;k<rlengthnormal;k++)
                {
                    coverage[i+k+1]+=reads2[node].read_count;
                }
                //cout<<i+1<<endl;
            }
        }
        
    }
    for(int i=1;i<strain.size();i++)
    {
        alignout<<strain[i]<<"\t"<<coverage[i]<<endl;
    }


}
void sdbg_mark(vector <sdbg>&SDBG,vector<unordered_set<ull>>front,vector<unordered_set<ull>>back,vector<Reads>reads)
{
    for(int i=0;i<SDBG.size();i++)
    {
        bool find=false;
        for(auto it=front[i].begin();it!=front[i].end();it++)
        {
            if(SDBG[i].kmer==reads[*it].sequence)
            {
                find=true;
                SDBG[i].read_tag=1;
                break;
            }
        }
        if(find)
            break;
        for(auto it=back[i].begin();it!=back[i].end();it++)
        {
            if(SDBG[i].kmer==reads[*it].sequence)
            {
                find=true;
                SDBG[i].read_tag=1;
                break;
            }
        }
    }
}
#endif