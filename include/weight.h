#ifndef WEIGHT_H
#define WEIGHT_H
#include "dbg.h"
// void cyc_weight_init(vector <Cycle> &cyc,vector<sdbg> &dbg,vector<sdbg> &origin)
// {
//     for(int i=0;i<cyc.size();i++)
//     {
//         ull cyc_st=cyc[i].st;
//         int suc_size=cyc[i].cur_suc.size();
//         int pre_size=cyc[i].cur_pre.size(); 
//         vector<ull>suc_temp(suc_size);
//         vector<ull>pre_temp(pre_size);
//         cyc[i].suc_weight=suc_temp;
//         cyc[i].pre_weight=pre_temp;
//         cout<<i<<" init done"<<endl;
//         // suc_temp.~vector();
//         // pre_temp.~vector();
//     }
// }

void cyc_suc_pre_weight_calculate(vector <Cycle> &cyc,vector<sdbg> &dbg,vector<sdbg> &origin,int tag)
{
    ull st=cyc[tag].st;
    ull empty=dbg.size();
    cout<<"suc part"<<endl;
    for(int i=0;i<cyc[tag].cur_suc.size();i++)
    {
        if(cyc[tag].original_suc.count(cyc[tag].cur_suc[i]))
        {
            cyc[tag].suc_weight[i]=dbg[st].cov;
        }
        if(cyc[tag].cur_suc[i]==empty)
            continue;
        ull suc=cyc[tag].cur_suc[i];
        auto id=cyc[tag].suc_map[suc];
        ull dbg_id=id.second;
        int cyc_id=id.first;
        vector<ull>cyc_path=cyc[tag].cycle_list[cyc_id];
        vector<ull>add_path;
        bool visit=false;
        for(int j=0;j<cyc_path.size();j++)
        {
            add_path.push_back(cyc_path[j]);
            if(cyc_path[j]==dbg_id)
                break;
        }
        add_path.push_back(suc);
        int weight=dbg[st].cov;
        for(int j=1;j<add_path.size()-1;j++)
        {
            ull cur=add_path[j];
            ull front=add_path[j-1];
            ull back=add_path[j+1];
            if(dbg[cur].cycle_tag==-1)
            {
                weight+=dbg[cur].cov;
                continue;
            }
            ull cyc_tag=dbg[cur].cycle_tag;
            if(!cyc[cyc_tag].original_pre.count(front))
            {
                ull pre_index=find_vec_index(cyc[cyc_tag].cur_pre,front);
                if(pre_index==-1)
                {
                    cout<<"error pre"<<endl;
                }
                else
                {
                    weight+=cyc[cyc_tag].pre_weight[pre_index];
                }
            }
            if(!cyc[cyc_tag].original_suc.count(back))
            {
                ull suc_index=find_vec_index(cyc[cyc_tag].cur_suc,back);
                if(suc_index==-1)
                {
                    cout<<"error suc"<<endl;
                }
                else
                {
                    weight+=cyc[cyc_tag].suc_weight[suc_index];
                }
            }
        }
        cyc[tag].suc_weight[i]=weight;

    }
    cout<<"pre part"<<endl;
    for(int i=0;i<cyc[tag].cur_pre.size();i++)
    {
        if(cyc[tag].original_pre.count(cyc[tag].cur_pre[i]))
        {
            cyc[tag].pre_weight[i]=dbg[st].cov;
        }
        if(cyc[tag].cur_pre[i]==empty)
            continue;
        ull pre=cyc[tag].cur_pre[i];
        auto id=cyc[tag].pre_map[pre];
        ull dbg_id=id.second;
        int cyc_id=id.first;
        //Hex_revive(id,cyc_id,dbg_id);
        vector<ull>cyc_path=cyc[tag].cycle_list[cyc_id];
        vector<ull>add_path;
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
        int weight=0;
        for(int j=1;j<add_path.size()-1;j++)
        {
            ull cur=add_path[j];
            ull front=add_path[j-1];
            ull back=add_path[j+1];
            if(dbg[cur].cycle_tag==-1)
            {
                weight+=dbg[cur].cov;
                continue;
            }
            ull cyc_tag=dbg[cur].cycle_tag;
            if(!cyc[cyc_tag].original_pre.count(front))
            {
                ull pre_index=find_vec_index(cyc[cyc_tag].cur_pre,front);
                if(pre_index==-1)
                {
                    cout<<"error pre"<<endl;
                }
                else
                {
                    weight+=cyc[cyc_tag].pre_weight[pre_index];
                }
            }
            if(!cyc[cyc_tag].original_suc.count(back))
            {
                ull suc_index=find_vec_index(cyc[cyc_tag].cur_suc,back);
                if(suc_index==-1)
                {
                    cout<<"error suc"<<endl;
                }
                else
                {
                    weight+=cyc[cyc_tag].suc_weight[suc_index];
                }
            }
        }
        cyc[tag].suc_weight[i]=weight+dbg[st].cov;

    }
}
void cyc_weight_calculate(vector <Cycle> &cyc,vector<sdbg> &dbg,vector<sdbg> &origin)
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
                if(dbg[node].cycle_tag!=-1)
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
            cyc_suc_pre_weight_calculate(cyc,dbg,origin,stack[i]);
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
                    if(dbg[node].cycle_tag!=-1)
                    {
                        if(visit[dbg[node].cycle_tag]==0)
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
void get_root_leave_Sdbg_weight(vector <sdbg> &dbg,vector <ull> &root,vector <ull> &leave,int reserve_size,vector <Cycle> &cyc,vector<Result> &results)
{
    int sdbg_size=dbg.size();
    unordered_set <ull> current;
    unordered_set <ull> previous;
    vector<set<pair<ull,int>,comp>>topo(sdbg_size);
    vector<set<pair<ull,int>,comp>>matrix(sdbg_size);
    vector<int>visit(sdbg_size);
    ull empty_node=dbg.size()-1;
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
    for(int i=0;i<root.size();i++)
    {
        current.insert(root[i]);
        pair <ull,int>p(root[i],dbg[root[i]].cov);
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
            for(int i=0;i<dbg[node].successor.size();i++)
            {
                ull span=dbg[node].successor[i];
                if(span==empty_node)
                    continue;
                visit[span]=1;
                current.insert(span);
                int add=0;
                
                if(dbg[node].cycle_tag==-1 && dbg[span].cycle_tag==-1)
                {
                    add+=dbg[span].cov;
                }
                if(dbg[node].cycle_tag!=-1)
                {
                    ull suc_index=find_vec_index(dbg[node].successor,span);
                    add+=cyc[dbg[node].cycle_tag].suc_weight[suc_index];
                }
                if(dbg[span].cycle_tag!=-1)
                {
                    ull pre_index=find_vec_index(dbg[node].predecessor,node);
                    add+=cyc[dbg[span].cycle_tag].pre_weight[pre_index];
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

    for(int i=0;i<dbg.size();i++)
    {
        if(dbg[i].predecessor.size() >0 && dbg[i].successor.size()==0)
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

#endif