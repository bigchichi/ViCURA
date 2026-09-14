#include "OLC_shrink.h"
void OLC_shrink(vector <Reads> &reads,vector <Reads> reads_origin,vector <ull> node_list)
{
    int node_size=node_list.size();
    unordered_map <ull,ull> map;
    for(int i=0;i<node_size;i++)
    {
        //cout<<node_list[i]<<endl;
        map[node_list[i]]=i;
    }
    for(int i=0;i<node_size;i++)
    {
        Reads temp;
        ull origin_ID=node_list[i];
        temp.sequence=reads_origin[origin_ID].sequence;
        //temp.successor_o=reads_origin[origin_ID].successor_o;
        temp.read_count=reads_origin[origin_ID].read_count;
        temp.cov.push_back(reads_origin[origin_ID].read_count);
       //temp.next=reads_origin[origin_ID].next;
        for(int j=0;j<reads_origin[origin_ID].successor.size();j++)
        {
            //cout<<map[reads_origin[origin_ID].successor[j]]<<endl;
            if(map.count(reads_origin[origin_ID].successor[j])==0)
                continue;
            temp.successor.push_back(map[reads_origin[origin_ID].successor[j]]);
            temp.next.push_back(map[reads_origin[origin_ID].successor[j]]);
            temp.successor_o.push_back(reads_origin[origin_ID].successor_o[j]);
        }
        for(int j=0;j<reads_origin[origin_ID].predecessor.size();j++)
        {
            if(map.count(reads_origin[origin_ID].predecessor[j])==0)
                continue;
            temp.predecessor.push_back(map[reads_origin[origin_ID].predecessor[j]]);
            temp.next.push_back(map[reads_origin[origin_ID].predecessor[j]]);
        }
        reads.push_back(temp);

    }
}
void OLC_remove_node(vector <Reads> &reads,ull target)
{
    if(target <0 || target>=reads.size())
        return;
    for(int i=0;i<reads[target].successor.size();i++)
    {
        int tg=reads[target].successor[i];
        auto it=find(reads[tg].predecessor.begin(),reads[tg].predecessor.end(),target);
        if(it!=reads[tg].predecessor.end())
            reads[tg].predecessor.erase(it);
    }
    for(int i=0;i<reads[target].predecessor.size();i++)
    {
        int tg=reads[target].predecessor[i];
        auto it=find(reads[tg].successor.begin(),reads[tg].successor.end(),target);
        if(it!=reads[tg].successor.end())
            reads[tg].successor.erase(it);
    }
    reads[target].successor.clear();
    reads[target].predecessor.clear();
}
bool OLC_cyc_shrink(vector <Reads> &reads,vector <Reads> origin)
{
    vector <int> count(reads.size());
    vector <int> position(reads.size());
    vector <int> visit(reads.size());
    vector <ull> stack;
    unordered_map <ull,int> id_node_map;
    bool cyc_tag=false;
    vector <unordered_set<ull>> node_suc;
    for(ull ii=0;ii<reads.size();ii++)
    {
        if(!(reads[ii].successor.size()>0 && reads[ii].predecessor.size()==0))
            continue;
        ull root=ii;
        stack.push_back(root);
        id_node_map.clear();
        node_suc.clear();
        count[root]++;
        visit[root]=1;
        bool jump_flag;
        bool cyc_flag;
        while(stack.size()!=0)
        {
            ull back=stack.back();
            jump_flag=false;
            cyc_flag=false;
            //cout<<"info1 "<<stack.size()<<" "<<back<<" "<<position[back]<<" "<<reads[back].successor.size()<<endl;
            int i=position[back];
            if(position[back]>=reads[back].successor.size())
            {
                count[back]--;
                stack.pop_back();
                continue;
            }
            for(;i<reads[back].successor.size();i++)
            {
                ull next=reads[back].successor[i];
                //cout<<next<<endl;
                if(visit[next]==0)
                {
                    visit[next]=1;
                    stack.push_back(next);
                    count[next]++;
                    jump_flag=true;
                    i++;
                    break;
                }
                else
                {
                    vector <ull> temp_stack;
                    int judge=count[next];
                    if(judge>=1)
                    {
                        cout<<"detect cyc "<<next<<" "<<position[next]<<" "<<reads[next].successor.size()<<" "<<origin[next].successor.size()<<endl;
                        cout<<back<<" "<<position[back]<<" "<<i<<" "<<reads[back].successor.size()<<endl;
                        reads[next].cyc=1;
                        temp_stack.push_back(next);
                        cyc_tag=true;
                        for(int j=stack.size()-1;j>=0;j--)
                        {
                            temp_stack.push_back(stack[j]);
                            if(stack[j]==next)
                                break;
                        }
                        reverse(temp_stack.begin(),temp_stack.end());
                        cout<<temp_stack.size()<<endl;
                        ull id;
                        if(id_node_map.count(next)==1)
                        {
                            id=id_node_map[next];
                        }
                        else
                        {
                            int map_size=id_node_map.size();
                            id_node_map[next]=map_size;
                            id=map_size;
                            unordered_set<ull>temp;
                            for(int p=0;p<reads[next].successor.size();p++)
                            {
                                temp.insert(reads[next].successor[p]);
                            }
                            node_suc.push_back(temp);
                        }
                        //cout<<"start "<<endl;
                        for(int j=1;j<temp_stack.size()-1;j++)
                        {
                            ull node=temp_stack[j];
                            ull pre=temp_stack[j-1];
                            ull suc=temp_stack[j+1];
                            for(int k=0;k<reads[node].successor.size();k++)
                            {
                                ull out_node=reads[node].successor[k];

                                if(reads[node].successor[k]!=suc)
                                {
                                    if(out_node==next)
                                    {
                                        continue;
                                    }
                                    if(node_suc[id].insert(out_node).second==true)
                                        reads[next].successor.push_back(out_node);
                                    //switch_element(reads[out_node].predecessor,node,next);
                                    //cout<<"add 1"<<endl;
                                }
                            }
                            //cout<<"suc done"<<endl;
                            for(int k=0;k<reads[node].predecessor.size();k++)
                            {
                                ull out_node=reads[node].predecessor[k];
                                if(reads[node].predecessor[k]!=pre)
                                {
                                    if(out_node==next)
                                    {
                                        continue;
                                    }
                                    //switch_element(reads[next].predecessor,out_node,-2);
                                    switch_element(reads[out_node].successor,node,next);
                                }
                            }
                            //cout<<"done"<<endl;
                            reads[node].successor.clear();
                            reads[node].predecessor.clear();
                        }
                        cout<<"detect cyc "<<next<<" "<<position[next]<<" "<<reads[next].successor.size()<<" "<<origin[next].successor.size()<<endl;
                        while(stack.back()!=next)
                        {
                            ull temp=stack.back();
                            count[temp]--;
                            stack.pop_back();
                        }
                        cyc_flag=true;
                        i++;
                        break;                        
                    }
                }
            }
            position[back]=i;
            
        }
    }
    return cyc_tag;
}
bool OLC_cyc_shrink_detail(vector <Reads> &reads,vector <Reads> origin,vector <Cycle> &cyc)
{
    ull size=reads.size();
    vector <ull> path_stack;
    vector <int> visit(size);
    vector <int> position(size);
    vector <int> stack_condition(size);
    unordered_map <ull,int> id_node_map;
    bool cyc_tag=false;
    vector <unordered_set<ull>> node_suc;
    vector <unordered_set<ull>> node_pre;
    vector<ull>root_v;
    for(int i=0;i<reads.size();i++)
    {
        if(reads[i].predecessor.size()==0 && reads[i].successor.size()>0)
            root_v.push_back(i);
    }
    int max_depth=0;
    cout<<"start Shrink "<<endl;
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
            if(position[back]>=reads[back].successor.size())
            {
                stack_condition[back]--;
                path_stack.pop_back();
                continue;
            }
            for(;i<reads[back].successor.size();i++)
            {
                ull next=reads[back].successor[i];
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
                        cout<<back<<" "<<position[back]<<" "<<position[next]<<" "<<reads[next].successor.size()<<endl;
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
                        if(reads[next].cyc_id==-1)
                        {
                            id_node_map[next]=node_suc.size();
                            unordered_set<ull>suc,pre;
                            for(int p=0;p<reads[next].successor.size();p++)
                            {
                                suc.insert(reads[next].successor[p]);
                            }
                            for(int p=0;p<reads[next].predecessor.size();p++)
                            {
                                pre.insert(reads[next].predecessor[p]);
                            }
                            node_suc.push_back(suc);
                            node_pre.push_back(pre);
                            cout<<"cyc detected,st:  "<<next<<" length: "<<stack.size()<<endl;
                            Cycle temp;
                            temp.init(reads[next].successor,reads[next].predecessor);
                            temp.st=next;
                            //temp.cycle_time=1;
                            //temp.init(Sdbg[next]);
                            int node_vec_back=node_suc.size()-1;
                            temp.cyc_shrink_reads(stack,reads,node_suc[node_vec_back],node_pre[node_vec_back]);
                            cyc.push_back(temp);
                            reads[next].cyc_id=cyc.size()-1;
                            cout<<reads[next].cyc_id<<endl;
                        }
                        else
                        {
                            cout<<"complex cyc detected,st:  "<<next<<" length: "<<stack.size()<<endl;
                            ull id=reads[next].cyc_id;
                            int node_vec_id=id_node_map[next];
                            cout<<"into shrink"<<endl;
                            cyc[id].cyc_shrink_reads(stack,reads,node_suc[node_vec_id],node_pre[node_vec_id]);
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
void OLC_test(vector <Reads> reads)
{
    cout<<"cycle test"<<endl;
    vector<int>visit(reads.size());
    vector<int>count(reads.size());
    vector<int>position(reads.size());
    vector<ull>root;
    for(int i=0;i<reads.size();i++)
    {
        if(reads[i].successor.size()>0 && reads[i].predecessor.size()==0)
            root.push_back(i);
    }
    for(int i=0;i<root.size();i++)
    {
        vector <ull> stack;
        stack.push_back(root[i]);
        visit[root[i]]=1;
        count[root[i]]++;
        while(stack.size()!=0)
        {
            ull back=stack.back();
            if(position[back]>=reads[back].successor.size())
            {
                stack.pop_back();
                count[back]--;
                continue;
            }      
            ull node=reads[back].successor[position[back]];
            visit[node]=1;
            if(count[node]>0)
            {
                cout<<"error,cycle start node_id "<<node<<endl;
            }
            count[node]++;
            stack.push_back(node);
            position[back]++;



        }

    }
}
void OLC_get_root_leave(vector <Reads> &reads,vector <ull> &root,vector <ull> &leave,int cyc_size,int path_size,Parameter*para)
{
    int reads_size=reads.size();
    vector<bool>visit(reads_size);
    int max_depth=-1;
    unordered_set <ull> current_layer_node;
    unordered_set <ull> last_layer_node;
    vector <int> min(reads.size());
    vector <set<pair<ull,int>,comp>> topo;

    for(int i=0;i<reads_size;i++)
    {
        set <pair<ull,int>,comp> temp;
        topo.push_back(temp);
    }
    for(int i=0;i<reads_size;i++)
    {
        if(reads[i].successor.size()>0 && reads[i].predecessor.size()==0)
        {
            //reads[i].layer=150;
            current_layer_node.insert(i);
            pair<ull,int> p(i,para->rLengthNormal);
           topo[i].insert(p);
           // visit[i]=true;
        }
    }
    int max_length=0;
    while(true)
    {
        //cout<<"laye :"<<layer<<endl;
        if(current_layer_node.size()==0)
            break;
        cout<<current_layer_node.size()<<endl;
        last_layer_node.clear();
        last_layer_node=current_layer_node;
        current_layer_node.clear();
        //cout<<"max "<<max_length<<endl;
        for(auto it=last_layer_node.begin();it!=last_layer_node.end();it++)
        {
            ull node=*it;
            // if(visit[node]==true)
            // {
            //     cout<<"? "<<node<<endl;
            //     //continue;
            // }
            visit[node]=true;
            for(int i=0;i<reads[node].successor.size();i++)
            {
                ull span=reads[node].successor[i];
                // if(visit[span]==true)
                //     cout<<"ll "<<span<<endl;
                current_layer_node.insert(span);
                int add;
                if(i<reads[node].successor_o.size())
                {
                    add=para->rLengthNormal-reads[node].successor_o[i];
                }
                else
                {
                    add=cyc_size;
                }
                //cout<<add<<endl;
                for(auto it2=topo[node].begin();it2!=topo[node].end();it2++)
                {
                    auto pair_span=*it2;

                    pair_span.second+=add;

                    //cout<<pair_span.second<<endl;
                    topo[span].insert(pair_span);
                    if(topo[span].size()>=path_size)
                    {
                        topo[span].erase(topo[span].begin());
                    }
                }
                
            }
        }
    }
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
                if(path.size()>=path_size)
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



}
void OLC_cut_branches(vector <Reads> &reads,vector <ull> root,vector<ull> leave,vector <ull>&node_list)
{
    unordered_set <ull> rs;
    unordered_set <ull> ls;
    for(int i=0;i<root.size();i++)
    {
        rs.insert(root[i]);
    }
    for(int i=0;i<leave.size();i++)
    {
        ls.insert(leave[i]);
    }
    for(int i=0;i<reads.size();i++)
    {
        // cout<<i<<endl;
        if(reads[i].successor.size()>0 && reads[i].predecessor.size()==0)
        {
            if(rs.count(i)==0)
                continue;
        }
        if(reads[i].successor.size()==0 && reads[i].predecessor.size()>0)
        {
            if(ls.count(i)==0)
                continue;
        }
        node_list.push_back(i);
    }

}
void traversal_OLC( vector <Reads> &reads,ull root,vector<ull> leave,vector <Result>& result)
{
    vector <ull> stack;
    vector <int> position(reads.size());
    stack.push_back(root);
    bool jumptag=false;
    //int empty=dbg.size()-1;
    cout<<"start travel"<<endl;
    int max_depth=0;
    vector<ull>max_stack;
    set<ull>leave_s;
    for(int i=0;i<leave.size();i++)
    {
        leave_s.insert(leave[i]);
    }

    while (true)
    {
        ull back=stack.back();
        if(stack.size()==0)
            break;
        if(stack.size()>max_depth)
        {
            max_depth=stack.size();
            max_stack=stack;
        }
        if(leave_s.count(back)==1)
        {
            //cout<<"strain done "<<strain_count<<endl;
            //cout<<"find leave"<<endl;
            Result temp;
            temp.path=stack;
            result.push_back(temp);
            //cout<<stack.size()<<endl;
            //break;
                //cout<<"add 2"<<endl;               
            
            //     add++;
            //     Result temp;
            //     temp.path=stack;
            //     result.push_back(temp);
            //     cout<<stack.size()<<endl;            
            // if(add>=500)
            //     break;

            
        }
        if(position[back]>=reads[back].successor.size())
        {
            stack.pop_back();
            continue;
        }
        ull next=reads[back].successor[position[back]];
        //cout<<"using another suc "<<back<<" "<<position[back]<<endl;
        stack.push_back(next);
        position[back]++;
    }
    Result temp;
    temp.path=max_stack;
    result.push_back(temp);
}
void OLC_mark_reads(vector<Reads> &reads,vector<ull>root,vector<ull>leave,int reserve_length)
{
    vector<int>visit(reads.size());
    vector<int>position(reads.size());
    for(int i=0;i<root.size();i++)
    {
        vector <ull> stack;
        stack.push_back(root[i]);
        while (true)
        {
            if(stack.size()>reserve_length || stack.size()==0)
                break;
            ull back=stack.back();
            visit[back]=true;
            reads[back].reserve_tag=1;
            if(position[back]>=reads[back].successor.size())
            {
                stack.pop_back();
                continue;
            }
            ull next=reads[back].successor[position[back]];
            stack.push_back(next);
            position[back]++;
        }
        
    }
    cout<<"root done"<<endl;
    vector<int>visit_l(reads.size());
    vector <int> position_l(reads.size());
    for(int i=0;i<leave.size();i++)
    {
        vector <ull> stack;
        stack.push_back(leave[i]);
        while (true)
        {
            if(stack.size()>reserve_length || stack.size()==0)
                break;
            ull back=stack.back();
            visit_l[back]=true;
            reads[back].leserve_tag=1;
            if(position_l[back]>=reads[back].predecessor.size())
            {
                stack.pop_back();
                continue;
            }
            ull next=reads[back].predecessor[position_l[back]];
            stack.push_back(next);
            position_l[back]++;
        }
        
    }
}
void OLC_cyc_sort(vector <Reads> &reads,vector <Reads> &origin,vector <Cycle> &cyc)
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
            cout<<i<<endl; 
            cycle_sort_OLC(reads,origin,cyc,stack[i]);
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
    //stack.~vector();
}
void get_root_leave_OLC(vector <Reads> &reads,vector <ull> &root,vector <ull> &leave,int reserve_size,vector <Cycle> &cyc,vector<Result> &results)
{
    int reads_size=reads.size();
    unordered_set <ull> current;
    unordered_set <ull> previous;
    vector<set<pair<ull,int>,comp>>topo(reads_size);
    vector<set<pair<ull,int>,comp>>matrix(reads_size);
    vector<int>visit(reads_size);
    ull empty_node=reads_size-1;
    int read_l=reads[0].sequence.size();
    // for(int i=0;i<sreads_size;i++)
    // {
    //     if(reads[i].predecessor.size()==0 && reads[i].successor.size()>0)
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
        pair <ull,int>p(root[i],read_l);
        topo[root[i]].insert(p);
        visit[root[i]]=1;
    }
    while (true)
    {
        //cout<<current.size()<<endl;
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
                    auto it = find(reads[node].successor.begin(),reads[node].successor.end(),span);
                    int index=0;
                    if (it != reads[node].successor.end())
                    { 
                        index=it - reads[node].successor.begin();
                    }
                    else
                        cout <<"no such suc"<<endl;
                    add+=read_l-reads[node].successor_o[index];
                }
                if(reads[node].cyc_id!=-1)
                {
                    add+=cyc[reads[node].cyc_id].max_length/2;
                }
                if(reads[span].cyc_id!=-1)
                {
                    add+=cyc[reads[span].cyc_id].max_length/2;
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
    //cout<<reads[213511].predecessor.size()<<" "<<dbg[213511].successor.size()<<endl;
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
        cout<<reads[*it].sequence<<endl;
    } 
    cout<<endl;
    cout<<"leave info"<<endl;
    for(auto it=leave_s.begin();it!=leave_s.end();it++)
    {
        leave.push_back(*it);
        cout<<*it<<" ";
        cout<<reads[*it].sequence<<endl;
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
void strain_print_reads( vector<Reads>&reads, vector <Reads> &origin,vector<Cycle>&cyc, vector <Result> &result,string result_path,Parameter*para)
{
    int reads_size=reads.size();
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
            if(reads[current_node].cyc_id==-1)
            {
                //true_path.push_back(current_node);
                tag++;
            }
            else
            {
                
                int cyc_tag=reads[current_node].cyc_id;
                ull pre=path[tag-1];
                ull suc=path[tag+1];
                int pre_judge=find_vec_index(origin[current_node].predecessor,pre);
                int suc_judge=find_vec_index(origin[current_node].successor,suc);
                if(pre_judge!=-1 && suc_judge!=-1)
                {
                    cout<<"good cycle"<<endl;
                    if(cyc_set.count(current_node)==0)
                    {
                        //path.erase(path.begin()+tag);
                        // vector<ull>max_path=cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag];
                        // ull pre_test=max_path[max_path.size()-2];
                        // ull suc_test=max_path[1];
                        // cout<<pre_test<<endl;
                        // cout<<suc_test<<endl;
                        // cout<<max_path[0]<<endl;
                        // cout<<max_path.back()<<endl;
                        // cout<<cyc[cyc_tag].original_pre.count(pre_test)<<endl;
                        // cout<<cyc[cyc_tag].original_suc.count(suc_test)<<endl;
                        // cout<<max_path.size()<<endl;
                        // //path.insert(path.begin()+tag,max_path.begin(),max_path.end());
                        // for(int p=1;p<max_path.size()-1;p++)
                        // {
                        //     if(max_path[p]==current_node)
                        //         cout<<"error "<<p<<endl;
                        // }
                        // cout<<"whole cycle"<<endl;
                        cyc_set.insert(current_node);
                        continue;
                    }
                    
                    tag++;
                    continue;
                }
                cyc_set.insert(current_node);
                cout<<"into cyc recover"<<endl;
                cout<<"pre "<<cyc[cyc_tag].original_pre.count(pre)<<endl;
                cout<<"pre in origin "<<find_vec_index(origin[current_node].predecessor,pre)<<endl;
                cout<<"suc "<<cyc[cyc_tag].original_suc.count(suc)<<endl;
                cout<<"suc in origin "<<find_vec_index(origin[current_node].successor,suc)<<endl;
                vector <ull> add_path;
                //speciality deal
                // if(cyc[cyc_tag].pre_map.count(pre)==1 && cyc[cyc_tag].suc_map.count(suc)==1)
                // {
                //     auto id=cyc[cyc_tag].pre_map[pre];
                //     ull dbg_id=id.second;
                //     int cyc_id=id.first;
                //     auto id1=cyc[cyc_tag].suc_map[suc];
                //     ull dbg_id1=id.second;
                //     int cyc_id1=id.first;
                //     if(cyc_id==cyc_id1 && dbg_id==dbg_id1)
                //     {
                //         cout<<"swtich node  "<<dbg_id<<endl;
                //         path[tag]=dbg_id;
                //         continue;
                //     }
                // }
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
                    //cout<<cyc[cyc_tag].original_suc.count(cyc_path[1])<<endl;
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
        string strain=reads[path[0]].sequence;
        for(int j=0;j<path.size()-1;j++)
        {
            ull cur=path[j];
            ull suc=path[j+1];
            auto it = find(origin[cur].successor.begin(),origin[cur].successor.end(),suc);
            int index=0;
            if (it != origin[cur].successor.end())
            { 
                index=it - origin[cur].successor.begin();
            }
            else
                cout <<"no such suc"<<endl;
            int overlap=origin[cur].successor_o[index];
            strain+=origin[suc].sequence.substr(overlap);
        }
        if(strain.size()<29800)
            continue;
        string output_path=result_path+"/strain_"+to_string(i);
        ofstream strain_out(output_path);
        strain_out<<"length "<<path.size()<<endl;
        strain_out<<strain<<endl;
        for(int j=0;j<path.size();j++)
        {
            strain_out<<reads[path[j]].sequence<<endl;
            for(int k=0;k<reads[path[j]].cov.size();k++)
            {
                strain_out<<reads[path[j]].cov[k]<<" ";
            }
            strain_out<<endl;
            if(reads[path[j]].cyc_id!=-1)
            {
                strain_out<<"cyc mark "<<reads[path[j]].cyc_id<<endl;
            }
        }
     }

}