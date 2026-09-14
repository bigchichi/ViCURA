#ifndef WEIGHTOLC_H
#define WEIGHTOLC_H
#include "dbg.h"
void cyc_weight_init_OLC(vector <Cycle> &cyc,vector<Reads> &reads)
{
    for(int i=0;i<cyc.size();i++)
    {
        ull cyc_st=cyc[i].st;
        int suc_size=cyc[i].cur_suc.size();
        int pre_size=cyc[i].cur_pre.size();
        vector<int>suc_temp(suc_size);
        vector<int>pre_temp(pre_size);
        cyc[i].suc_weight=suc_temp;
        cyc[i].pre_weight=pre_temp;
        cout<<i<<" init done"<<endl;
        // suc_temp.~vector();
        // pre_temp.~vector();
    }
}

void cyc_suc_pre_weight_calculate_OLC(vector <Cycle> &cyc,vector<Reads> &reads,vector<Reads> &origin,int tag)
{
    ull st=cyc[tag].st;
    ull empty=reads.size();
    vector<ull>add_path;
    vector<ull> cyc_path;
    add_path.reserve(5000);
    cyc_path.reserve(5000);
    // cout<<"suc part "<<cyc[tag].cur_suc.size()<<endl;
    for(int i=0;i<cyc[tag].cur_suc.size();i++)
    {
        cout<<i<<endl;
        if(cyc[tag].cur_suc[i]==empty)
            continue;
        if(cyc[tag].suc_map.count(cyc[tag].cur_suc[i])==0)
        {
            cout<<"origin suc"<<endl;
            int index=find_vec_index(origin[st].successor,cyc[tag].cur_suc[i]);
            cout<<index<<endl;
            if(index!=-1)
            {
                //cout<<"suc_o "<<origin[st].successor_o.size()<<endl;
                int overlap=origin[st].successor_o[index];

                cout<<overlap<<endl;
                cout<<cyc[tag].suc_weight[i]<<endl;
                cyc[tag].suc_weight[i]+=origin[cyc[tag].cur_suc[i]].pre_cov[overlap];
                continue;
            }
            cyc[tag].suc_weight[i]=0;
            continue;
        }
        ull suc=cyc[tag].cur_suc[i];
        auto id=cyc[tag].suc_map[suc];
        ull dbg_id=id.second;
        int cyc_id=id.first;
        cout<<"dbg_id "<<dbg_id<<endl;
        cyc_path=cyc[tag].cycle_list[cyc_id];
        add_path.clear();
        bool visit=false;
        for(int j=0;j<cyc_path.size();j++)
        {
            add_path.push_back(cyc_path[j]);
            if(cyc_path[j]==dbg_id)
                break;
        }
        add_path.push_back(suc);
        cout<<"path length "<<add_path.size()<<endl;
        int weight=reads[st].suc_cov.back();
         for(int j=1;j<add_path.size()-1;j++)
        {
            ull cur=add_path[j];
            ull front=add_path[j-1];
            ull back=add_path[j+1];
            if(reads[cur].cyc_id==-1)
            {
                int id=find_vec_index(origin[front].successor,cur);
                if(id==-1)
                {
                    cout<<"error index"<<endl;
                }
                else
                {
                    int overlap=origin[front].successor_o[id];
                    weight+=origin[front].pre_cov[overlap];
                }
                continue;
            }
            int cyc_tag=reads[cur].cyc_id;

            int pre_index=find_vec_index(cyc[cyc_tag].cur_pre,front);
            if(pre_index==-1)
            {
                cout<<"error pre"<<endl;
            }
            else
            {
                weight+=cyc[cyc_tag].pre_weight[pre_index];
            }
            int suc_index=find_vec_index(cyc[cyc_tag].cur_suc,back);
            if(suc_index==-1)
            {
                cout<<"error suc"<<endl;
            }
            else
            {
                weight+=cyc[cyc_tag].suc_weight[suc_index];
            }
        }
        int index=find_vec_index(origin[add_path[add_path.size()-2]].successor,add_path.back());
        if(index!=-1)
        {
            int overlap=origin[add_path[add_path.size()-2]].successor_o[index];
            weight+=origin[add_path.back()].pre_cov[overlap];
        }
        cyc[tag].suc_weight[i]=weight;

    }
    cout<<"pre part "<<cyc[tag].cur_pre.size()<<endl;
    for(int i=0;i<cyc[tag].cur_pre.size();i++)
    {
        cout<<i<<" "<<cyc[tag].cur_pre[i]<<endl;
        if(cyc[tag].cur_pre[i]==empty)
            continue;
        cout<<cyc[tag].pre_map.size()<<endl;
        cout<<cyc[tag].pre_map.count(cyc[tag].cur_pre[i])<<endl;
        if(cyc[tag].pre_map.count(cyc[tag].cur_pre[i])==0)
        {
            cout<<"origin pre"<<endl;
            ull pre_node=cyc[tag].cur_pre[i];            
            int index=find_vec_index(origin[pre_node].successor,st);
            if(index!=-1)
            {
                int overlap=origin[pre_node].successor_o[index];
                cyc[tag].pre_weight[i]+=origin[st].pre_cov[overlap];
                continue;
            }
            cyc[tag].pre_weight[i]=0;
            continue;
        }
        cout<<"finish judge"<<endl;
        ull pre=cyc[tag].cur_pre[i];
        auto id=cyc[tag].pre_map[pre];
        ull dbg_id=id.second;
        int cyc_id=id.first;
       // Hex_revive(id,cyc_id,dbg_id);
        cyc_path=cyc[tag].cycle_list[cyc_id];
        add_path.clear();
        add_path.push_back(pre);
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
        cout<<"path length "<<add_path.size()<<endl;
        int weight=0;
        for(int j=1;j<add_path.size()-1;j++)
        {
            cout<<j<<endl;
            ull cur=add_path[j];
            ull front=add_path[j-1];
            ull back=add_path[j+1];
            cout<<reads[cur].cyc_id<<endl;
            if(reads[cur].cyc_id==-1)
            {
                int id=find_vec_index(origin[front].successor,cur);
                cout<<id<<endl;
                if(id==-1)
                {
                    cout<<"error index"<<endl;
                }
                else
                {
                    int overlap=origin[front].successor_o[id];
                    weight+=origin[front].pre_cov[overlap];
                }
                continue;
            }
            ull cyc_tag=reads[cur].cyc_id;
            int pre_index=find_vec_index(cyc[cyc_tag].cur_pre,front);
            if(pre_index==-1)
            {
                cout<<"error pre"<<endl;
            }
            else
            {
                weight+=cyc[cyc_tag].pre_weight[pre_index];
            }
            int suc_index=find_vec_index(cyc[cyc_tag].cur_suc,back);
            if(suc_index==-1)
            {
                cout<<"error suc"<<endl;
            }
            else
            {
                weight+=cyc[cyc_tag].suc_weight[suc_index];
            }
        }
        cout<<"weight deal done"<<endl;
        int index=find_vec_index(origin[add_path[add_path.size()-2]].successor,add_path.back());
        if(index!=-1)
        {
            int overlap=origin[add_path[add_path.size()-2]].successor_o[index];
            weight+=origin[add_path.back()].pre_cov[overlap];
        }
        cyc[tag].pre_weight[i]=weight;

    }
}
void cyc_weight_calculate_OLC(vector <Cycle> &cyc,vector<Reads> &reads,vector<Reads> &origin)
{
    vector <ull> stack;
    for(int i=0;i<cyc.size();i++)
    {
        bool tag=true;
        for(int j=0;j<cyc[i].cycle_list.size();j++)
        {
            for(int k=1;k<cyc[i].cycle_list[j].size()-1;k++)
            {
                ull node=cyc[i].cycle_list[j][k];
                if(reads[node].cyc_id!=-1)
                {
                    tag=false;
                    //cyc[i].cycle_container.insert(reads[node].cyc_id);
                    break;
                }
            }
        }
        if(tag) 
            stack.push_back(i);
    }
    vector <ull> visit(cyc.size());
    cout<<"start sort "<<stack.size()<<endl;
    while(stack.size()!=0)
    {
        cout<<"layer stack size: "<<stack.size()<<endl;
        for(int i=0;i<stack.size();i++)
        {
            cout<<i<<" "<<stack[i]<<endl;
            cout<<cyc[stack[i]].st<<endl; 
            cyc_suc_pre_weight_calculate_OLC(cyc,reads,origin,stack[i]);
            visit[stack[i]]=1;
        }
        stack.clear();
        cout<<"sort done"<<endl;
        for(int i=0;i<cyc.size();i++)
        {
            bool tag=true;
            if(visit[i]==1)
                continue;
            for(int j=0;j<cyc[i].cycle_list.size();j++)
            {
                for(int k=1;k<cyc[i].cycle_list[j].size()-1;k++)
                {
                    ull node=cyc[i].cycle_list[j][k];
                    if(reads[node].cyc_id!=-1)
                    {
                        if(visit[reads[node].cyc_id]==0)
                            tag=false;
                        //cyc[i].cycle_container.insert(reads[node].cyc_id);
                        break;
                    }
                }
                if(tag==false)
                    break;
            }
            if(tag) 
                stack.push_back(i);
        }
        cout<<"new stack size "<<stack.size()<<endl;
    }
}
void get_root_leave_OLC_weight(vector <Reads> &reads,vector <Reads> &origin,vector <ull> &root,vector <ull> &leave,int reserve_size,vector <Cycle> &cyc,vector<Result> &results)
{
    int reads_size=reads.size();
    unordered_set <ull> current;
    unordered_set <ull> previous;
    vector<set<pair<ull,int>,comp>>topo(reads_size);
    vector<set<pair<ull,int>,comp>>matrix(reads_size);
    vector<int>visit(reads_size);
    ull empty_node=reads.size()-1;
    // for(int i=0;i<sdbg_size;i++)
    // {
    //     if(dbg[i].predecessor.size()==0 && dbg[i].successor.size()>0)
    //     {
    //         current.insert(i);
    //         pair <ull,int>p(i,1);
    //         topo[i].insert(p);
    //         visit[i]=1;
    //     }
    // }
    for(int i=0;i<reads.size();i++)
    {
        if(reads[i].predecessor.size()==0 && reads[i].successor.size()>0)
        {
            root.push_back(i);
        }
    }
    for(int i=0;i<root.size();i++)
    {
        current.insert(root[i]);
        pair <ull,int>p(root[i],reads[root[i]].pre_cov[0]);
        topo[root[i]].insert(p);
        visit[root[i]]=1;
    }
    while (true)
    {
        cout<<current.size()<<endl;
        previous.swap(current);
        current.clear();
        for(auto it=previous.begin();it!=previous.end();it++)
        {
            ull node=*it;
            for(int i=0;i<reads[node].successor.size();i++)
            {
                ull span=reads[node].successor[i];
                if(span==empty_node)
                    continue;
                visit[span]=1;
                current.insert(span);
                int add=0;
                
                if(reads[node].cyc_id==-1 && reads[span].cyc_id==-1)
                {
                    int index=find_vec_index(origin[node].successor,span);
                    if(index!=-1)
                    {
                        int overlap=origin[node].successor_o[index];
                        add+=origin[span].pre_cov[overlap];
                    }
                }
                else if(reads[node].cyc_id!=-1)
                {
                    ull suc_index=find_vec_index(reads[node].successor,span);
                    add+=cyc[reads[node].cyc_id].suc_weight[suc_index];
                }
                else if(reads[span].cyc_id!=-1)
                {
                    ull pre_index=find_vec_index(reads[node].predecessor,node);
                    add+=cyc[reads[span].cyc_id].pre_weight[pre_index];
                }
                for(auto it=topo[node].begin();it!=topo[node].end();it++)
                {
                    auto pair_span=*it;
                    pair_span.second+=add;
                    topo[span].insert(pair_span);
                    if(topo[span].size()>reserve_size)
                        topo[span].erase(topo[span].begin());
                    pair<ull,int>p(node,pair_span.second);
                    matrix[span].insert(p);
                    if(matrix[span].size()>reserve_size)
                        matrix[span].erase(matrix[span].begin());
                }
            }
        }
        if(current.size()==0)
            break;
    }
    cout<<"visit  "<<count(visit.begin(),visit.end(),1)<<endl;
    //cout<<dbg[213511].predecessor.size()<<" "<<dbg[213511].successor.size()<<endl;
    set<tuple<ull,ull,int>,comp_tuple> path;

    for(int i=0;i<reads.size();i++)
    {
        if(reads[i].predecessor.size() >0 && reads[i].successor.size()==0)
        {
            for(auto it=topo[i].begin();it!=topo[i].end();it++)
            {
                auto pair_temp=*it;
                //cout<<pair_temp.first<<" "<<i<<" "<<pair_temp.second<<endl;
                tuple <ull,ull,int> p(pair_temp.first,i,pair_temp.second);
                path.insert(p);
                //cout<<path.size()<<endl;
                if(path.size()>=reserve_size)
                    path.erase(path.begin());
            }
        }
    }
    set<ull>root_s;
    set<ull>leave_s;
    cout<<"done"<<endl;
    for(auto it=path.begin();it!=path.end();it++)
    {
        auto tup=*it;
        cout<<get<0>(tup)<<" "<<get<1>(tup)<<" "<<get<2>(tup)<<endl;
        root_s.insert(get<0>(tup));
        leave_s.insert(get<1>(tup));
    }
    cout<<"root info"<<endl;
    for(auto it=root_s.begin();it!=root_s.end();it++)
    {
        root.push_back(*it);
        cout<<*it<<" ";
    } 
    cout<<endl;
    cout<<"leave info"<<endl;
    for(auto it=leave_s.begin();it!=leave_s.end();it++)
    {
        leave.push_back(*it);
        cout<<*it<<" ";
    } 
    cout<<endl;
    for(int i=0;i<leave.size();i++)
    {
        ull cur_leave=leave[i];
        vector<ull>stack;
        stack.push_back(cur_leave);
        while (true)
        {
            ull back=stack.back();
            if(matrix[back].size()==0)
                break;
            auto p=*matrix[back].rbegin();
            ull next=p.first;
            cout<<next<<" "<<p.second<<endl;
            stack.push_back(next);
        }
        reverse(stack.begin(),stack.end());
        Result temp_r;
        temp_r.path=stack;
        results.push_back(temp_r);
        
    }
}
void cyc_test(vector <Cycle> &cyc,vector<Reads> &reads,vector<Reads> &origin)
{
    for(int i=0;i<cyc.size();i++)
    {
        cout<<"doing "<<i<<endl;
        cout<<"pre size "<<cyc[i].cur_pre.size()<<endl;
        for(int j=0;j<cyc[i].cur_pre.size();j++)
        {
            ull node=cyc[i].cur_pre[j];
            cout<<cyc[i].pre_map.count(node)<<endl;
            int index=find_vec_index(origin[node].successor,cyc[i].cur_pre[j]);
            cout<<index<<endl;
        }
        cout<<"suc size "<<cyc[i].cur_suc.size()<<endl;
        for(int j=0;j<cyc[i].cur_suc.size();j++)
        {
            ull node=cyc[i].cur_suc[j];
            cout<<cyc[i].suc_map.count(node)<<endl;
            int index=find_vec_index(origin[node].successor,cyc[i].cur_suc[j]);
            cout<<index<<endl;
        }
    }
}
#endif