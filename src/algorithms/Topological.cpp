#include "OLC_shrink.h"
void Topology :: inherit(unordered_set <ull> reach)
{
    for(auto i=reach.begin();i!=reach.end();i++)
    {
        root_reachable.insert(*i);
    }
}
void get_root_leave(vector <Debrujin_node> &dbg,vector <ull> &root,vector <ull> &leaf)
{
    int dbg_size=dbg.size();
    bool visit[dbg_size];
    for(int i=0;i<dbg_size;i++)
    {
        visit[i]=false;
    }
    int layer=0;
    int max_depth=-1;
    unordered_set <ull> current_layer_node;
    vector <Topology> topo;
    vector <ull> leave;

    for(int i=0;i<dbg_size;i++)
    {
        Topology temp;
        topo.push_back(temp);
    }
    for(int i=0;i<dbg_size;i++)
    {
        if(dbg[i].successor.size()!=0 && dbg[i].predecessor.size()==0)
        {
            dbg[i].layer=layer;
            current_layer_node.insert(i);
            topo[i].root_reachable.insert(i);
            visit[i]=true;
        }
    }

    while(true)
    {
        //cout<<"laye :"<<layer<<endl;
        if(current_layer_node.size()==0)
            break;
        layer++;
        unordered_set <ull> temp;
        for(auto it=current_layer_node.begin();it!=current_layer_node.end();it++)
        {
            ull current=*it;
            if(dbg[current].successor.size()==0)
            {
                if(layer>max_depth)
                {
                    max_depth=layer;
                    leave.clear();
                    leave.push_back(current);
                }
                else if(layer==max_depth)
                {
                    leave.push_back(current);
                }
                continue;
                
            }
            for(int j=0;j<dbg[current].successor.size();j++)
            {
                ull node=dbg[current].successor[j];
                ull new_root_tag=true;
                if(visit[node])
                    continue;
                for(int p=0;p<dbg[node].predecessor.size();p++)
                {
                    ull node_pre=dbg[node].predecessor[p];
                    if(visit[node_pre]==false)
                    {
                        new_root_tag=false;
                        break;
                    }
                }
                if(new_root_tag)
                {
                    dbg[node].layer=layer;
                    //cout<<node<<"---"<<dbg[node].layer<<endl;
                    temp.insert(node);
                    visit[node]=true;
                    topo[node].inherit(topo[current].root_reachable);
                }

            }
        }
        current_layer_node.clear();
        current_layer_node=temp;
    }

    for(int i=0;i<leave.size();i++)
    {
        cout<<"deepest leave "<<leave[i]<<endl;
        cout<<"depth: "<<dbg[leave[i]].layer<<endl;
        leaf.push_back(leave[i]);
        for(auto it=topo[leave[i]].root_reachable.begin();it!=topo[leave[i]].root_reachable.end();it++)
        {
            cout<<"root: "<<*it<<endl;
            root.push_back(*it);
        }
    }
   
    
    

    cout<<"topo sort done"<<endl;

}
void cut_branches(vector <Debrujin_node> &dbg,ull root,ull leave)
{
     ull target_root=root;
    ull target_leaf=leave;
    while (true)
    {
        vector <ull> temp_root,temp_leaf;
        temp_root.clear();
        temp_leaf.clear();
        //cout<<temp_root.size()<<" "<<temp_leaf.size()<<endl;
        get_root_leaf_node(dbg,temp_root,temp_leaf);
        //cout<<temp_root.size()<<" "<<temp_leaf.size()<<endl;
        if(temp_leaf.size()==1 && temp_root.size()==1)
            break;
        //cout<<temp_root.size()<<" "<<temp_leaf.size()<<endl;
        for(int i=0;i<temp_root.size();i++)
        {
            //cout<<temp_root[i]<<endl;
            if(temp_root[i]!=target_root)
            {
                queue <ull> Q;
                Q.push(temp_root[i]);
                //cout<<dbg[temp_root[i]].predecessor.size()<<endl;
                while (Q.size()!=0)
                {
                    ull top=Q.front();
                    if(dbg[top].predecessor.size()==0)
                    {
                        for(int j=0;j<dbg[top].successor.size();j++)
                        {
                            Q.push(dbg[top].successor[j]);
                        }
                        debrujin_delete_node(dbg,top);
                       // cout<<"del "<<top<<endl;
                    }

                    //cout<<dbg[top].predecessor.size()<<endl;
                    
                    Q.pop();
                }
                
            }
        }
        for(int i=0;i<temp_leaf.size();i++)
        {
             if(temp_leaf[i]!=target_leaf)
            {
                queue <ull> Q;
                Q.push(temp_leaf[i]);
                while (Q.size()!=0)
                {
                    ull top=Q.front();

                    if(dbg[top].successor.size()==0)
                    {                    
                        for(int j=0;j<dbg[top].predecessor.size();j++)
                        {
                            Q.push(dbg[top].predecessor[j]);
                        }
                        debrujin_delete_node(dbg,top);
                        //cout<<"del "<<top<<endl;
                    }
                    Q.pop();
                }
                
            }
        }
    }
}
void get_root_leave_Sdbg(vector <sdbg> &dbg,vector <ull> &root,vector <ull> &leave,int reserve_size,vector <Cycle> &cyc,vector<Result> &results)
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
        pair <ull,int>p(root[i],1);
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
                
                if(dbg[node].cycle_tag!=-1)
                {
                    unordered_set<ull>cur;
                    cur.insert(node);
                    while(true)
                    {
                        unordered_set<ull>tmp;
                        if(cur.size()==0)
                            break;
                        for(auto it2=cur.begin();it2!=cur.end();it2++)
                        {
                            ull cyc_st=dbg[*it2].cycle_tag;
                            if(cyc[cyc_st].suc_map.count(span)==1)
                            {
                                add+=cyc[cyc_st].cycle_list[cyc[cyc_st].max_tag].size()/2;
                                tmp=cyc[cyc_st].cycle_container;
                                break;
                            }
                        }
                        cur.swap(tmp);
                    }
                    //cout<<span<<" huge add "<<add<<endl;
                }
                if(dbg[span].cycle_tag==-1)
                    add+=1;
                else
                {
                    unordered_set<ull>cur;
                    cur.insert(span);
                    while(true)
                    {
                        unordered_set<ull>tmp;
                        if(cur.size()==0)
                            break;
                        for(auto it2=cur.begin();it2!=cur.end();it2++)
                        {
                            ull cyc_st=dbg[*it2].cycle_tag;
                            if(cyc[cyc_st].pre_map.count(node)==1)
                            {
                                add+=cyc[cyc_st].cycle_list[cyc[cyc_st].max_tag].size()/2;
                                tmp=cyc[cyc_st].cycle_container;
                                break;
                            }
                        }
                        cur.swap(tmp);
                    }
                    //add=cyc[dbg[span].cycle_tag].cycle_list[0].size();
                    //cout<<span<<" huge add "<<add<<endl;
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
    cout<<dbg[213511].predecessor.size()<<" "<<dbg[213511].successor.size()<<endl;
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
