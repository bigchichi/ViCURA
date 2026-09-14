#include "dbg.h"
void Cycle ::  init(vector <ull> suc,vector<ull> pre)
{
    for(int i=0;i<suc.size();i++)
    {
        original_suc.insert(suc[i]);
    }
    for(int i=0;i<pre.size();i++)
    {
        original_pre.insert(pre[i]);
    }
}
// void Cycle :: cyc_shrink(vector <ull> stack,vector <Debrujin_node>&dbg)
// {
//     ull suc=stack[1];
//     ull pre=stack[stack.size()-2];
//     vector <ull> cyc_edge;
//     cycle_time++;
//     //delete the edge(the corresponding nodes' successor and predecessor)
//     switch_element(dbg[st].successor,suc,-1);
//     switch_element(dbg[suc].predecessor,st,-1);
//     switch_element(dbg[st].predecessor,pre,-1);
//     switch_element(dbg[pre].successor,st,-1);
//     if(suc_map.count(suc)==1)//  such suc exists,load.
//     {
//         ull Hex=suc_map[suc];
//         int cyc_id;
//         ull point;
//         Hex_revive(Hex,cyc_id,point);
//         vector <ull> temp=cycle_list[cyc_id];
//         for (int i=0;i<temp.size();i++)
//         {
//             cyc_edge.push_back(temp[i]);
//             if(temp[i]==point)
//                 break;
//         }
        
//     }
//     else//no such suc exists,new.
//     {
//         cyc_edge.push_back(st);
//     }
//     for(int i=1;i<stack.size()-1;i++)
//     {
//         cyc_edge.push_back(stack[i]);
//         ull node_pre=stack[i-1];
//         ull node=stack[i];
//         ull node_suc=stack[i+1];
//         for(int j=0;j<dbg[node].successor.size();j++)
//         {
//             ull suc_wait_add=dbg[node].successor[j];
//             if(suc_wait_add==node_suc)
//                 continue;
//             if(suc_wait_add==st)
//                 continue;
//             ull Hex;
//             ull into_map=node;
//             Hex_cycle_id(cycle_time,into_map);
//             if(suc_map.count(suc_wait_add)==0)
//                 suc_map[suc_wait_add]=into_map;
//             switch_element(dbg[st].successor,suc_wait_add,-2);
//             switch_element(dbg[suc_wait_add].predecessor,node,st);
//         }
//         for(int j=0;j<dbg[node].predecessor.size();j++)
//         {
//             ull pre_wait_add=dbg[node].predecessor[j];
//             if(pre_wait_add==node_pre)
//                 continue;
//             if(pre_wait_add==st)
//                 continue;
//             ull Hex;
//             ull into_map=node;
//             Hex_cycle_id(cycle_time,into_map);
//             if(pre_map.count(pre_wait_add)==0)
//                 pre_map[pre_wait_add]=into_map;
//             switch_element(dbg[st].predecessor,pre_wait_add,-2);
//             switch_element(dbg[pre_wait_add].successor,node,st);
//         }
//         debrujin_delete_node(dbg,node);
//     }

//     if(pre_map.count(pre)==1)
//     {
//         ull Hex=pre_map[pre];
//         int cyc_id;
//         ull point;
//         Hex_revive(Hex,cyc_id,point);
//         vector <ull> temp=cycle_list[cyc_id];
//         bool into_edge=false;
//         for(int i=0;i<temp.size();i++)
//         {
//             if(temp[i]==point)
//                 into_edge=true;
//             if(into_edge)
//             {
//                 cyc_edge.push_back(temp[i]);
//             }
                
//         }
//     }
//     else
//     {
//         cyc_edge.push_back(st);
//     }
//     cycle_list.push_back(cyc_edge);
// }
void Cycle :: cyc_shrink_sdbg(vector <ull> stack,vector <sdbg>&dbg,unordered_set<ull>&suc_set,unordered_set<ull>&pre_set)
{
    ull suc=stack[1];
    ull pre=stack[stack.size()-2];
    vector <ull> cyc_edge;
    cycle_time++;
    ull empty_node_id=dbg.size()-1;
    //delete the edge(the corresponding nodes' successor and predecessor)
    switch_element(dbg[st].successor,suc,empty_node_id);
    //switch_element(dbg[suc].predecessor,st,-1);
    switch_element(dbg[st].predecessor,pre,empty_node_id);
    //switch_element(dbg[pre].successor,st,-1);
    bool suc_judge=suc_map.count(suc);
    bool pre_judge=pre_map.count(pre);
    cout<<suc_judge<<" "<<pre_judge<<endl;
    if(suc_judge==1)//  such suc exists,load.
    {
        auto Hex=suc_map[suc];
        int cyc_id=Hex.first;
        ull point=Hex.second;
        //Hex_revive(Hex,cyc_id,point);
        cout<<cyc_id<<endl;
        cout<<point<<endl;
        vector <ull> temp=cycle_list[cyc_id];
        for (int i=0;i<temp.size();i++)
        {
            cyc_edge.push_back(temp[i]);
            if(temp[i]==point)
            {
                cout<<"find "<<endl;
                break;
            }
                
        }
        
    }
    else//no such suc exists,new.
    {
        cyc_edge.push_back(st);
    }
    cout<<cyc_edge.size()<<endl;
    for(int i=1;i<stack.size()-1;i++)
    {
        
        cyc_edge.push_back(stack[i]);
        if(stack[i]==st)
            cout<<"shit stack "<<i<<" "<<st<<endl;
        ull node_pre=stack[i-1];
        ull node=stack[i];
        ull node_suc=stack[i+1];
        if(dbg[node].cycle_tag!=-1)
            cycle_container.insert(node);
        for(int j=0;j<dbg[node].successor.size();j++)
        {
            ull suc_wait_add=dbg[node].successor[j];
            if(suc_wait_add==node_suc)
                continue;
            if(suc_wait_add==st)
                continue;
            pair<int,ull> Hex;
            Hex.first=cycle_time;
            Hex.second=node;
            //Hex_cycle_id(cycle_time,into_map);

            if(suc_map.count(suc_wait_add)==0)
            {
                suc_map[suc_wait_add]=Hex;
                if(suc_set.insert(suc_wait_add).second==true)
                    dbg[st].successor.push_back(suc_wait_add);
                switch_element(dbg[suc_wait_add].predecessor,node,st);
            }
            else
            {
                switch_element(dbg[suc_wait_add].predecessor,node,empty_node_id);
            }
        }
        for(int j=0;j<dbg[node].predecessor.size();j++)
        {
            ull pre_wait_add=dbg[node].predecessor[j];
            if(pre_wait_add==node_pre)
                continue;
            if(pre_wait_add==st)
                continue;
            pair<int,ull> Hex;
            Hex.first=cycle_time;
            Hex.second=node;
            //Hex_cycle_id(cycle_time,into_map);
            if(pre_map.count(pre_wait_add)==0)
            {
                pre_map[pre_wait_add]=Hex;
                if(pre_set.insert(pre_wait_add).second==true)
                    dbg[st].predecessor.push_back(pre_wait_add);
                switch_element(dbg[pre_wait_add].successor,node,st);
            }
            else
            {
                switch_element(dbg[pre_wait_add].successor,node,empty_node_id);
            }
        }
        //debrujin_delete_node(dbg,node);
        dbg[node].successor.clear();
        dbg[node].predecessor.clear();
    }
    cout<<cyc_edge.size()<<endl;
    if(pre_judge==1)
    {
        auto Hex=pre_map[pre];
        int cyc_id=Hex.first;
        ull point=Hex.second;
        //Hex_revive(Hex,cyc_id,point);
        cout<<cyc_id<<endl;
        vector <ull> temp=cycle_list[cyc_id];

        bool into_edge=false;
        for(int i=0;i<temp.size();i++)
        {
            if(temp[i]==point)
                into_edge=true;
            if(into_edge)
            {
                cyc_edge.push_back(temp[i]);
            }
                
        }
    }
    else
    {

        cyc_edge.push_back(st);
    }
    for(int p=1;p<cyc_edge.size()-1;p++)
    {
        if(cyc_edge[p]==st)
            cout<<"what the fuck "<<p<<" "<<st<<endl;
    }
    cout<<"actual length "<<cyc_edge.size()<<endl;
    cycle_list.push_back(cyc_edge);
    cur_suc=dbg[st].successor;
    cur_pre=dbg[st].predecessor;
}
void Cycle :: cyc_shrink_reads(vector <ull> stack,vector <Reads>&reads,unordered_set<ull>&suc_set,unordered_set<ull>&pre_set)
{
    ull suc=stack[1];
    ull pre=stack[stack.size()-2];
    vector <ull> cyc_edge;
    cycle_time++;
    ull empty_node_id=reads.size()-1;
    //delete the edge(the corresponding nodes' successor and predecessor)
    switch_element(reads[st].successor,suc,empty_node_id);
    //switch_element(reads[suc].predecessor,st,-1);
    switch_element(reads[st].predecessor,pre,empty_node_id);
    //switch_element(reads[pre].successor,st,-1);
    bool suc_judge=suc_map.count(suc);
    bool pre_judge=pre_map.count(pre);
    cout<<suc_judge<<" "<<pre_judge<<endl;
    if(suc_judge==1)//  such suc exists,load.
    {
        auto Hex=suc_map[suc];
        int cyc_id=Hex.first;
        ull point=Hex.second;
        //Hex_revive(Hex,cyc_id,point);
        cout<<cyc_id<<endl;
        cout<<point<<endl;
        vector <ull> temp=cycle_list[cyc_id];
        for (int i=0;i<temp.size();i++)
        {
            cyc_edge.push_back(temp[i]);
            if(temp[i]==point)
            {
                cout<<"find "<<endl;
                break;
            }
                
        }
        
    }
    else//no such suc exists,new.
    {
        cyc_edge.push_back(st);
    }
    cout<<cyc_edge.size()<<endl;
    for(int i=1;i<stack.size()-1;i++)
    {
        
        cyc_edge.push_back(stack[i]);
        if(stack[i]==st)
            cout<<"shit stack "<<i<<" "<<st<<endl;
        ull node_pre=stack[i-1];
        ull node=stack[i];
        ull node_suc=stack[i+1];
        if(reads[node].cyc_id!=-1)
            cycle_container.insert(node);
        for(int j=0;j<reads[node].successor.size();j++)
        {
            ull suc_wait_add=reads[node].successor[j];
            if(suc_wait_add==node_suc)
                continue;
            if(suc_wait_add==st)
                continue;
            pair<int,ull> Hex;
            Hex.first=cycle_time;
            Hex.second=node;
            //Hex_cycle_id(cycle_time,into_map);

            if(suc_map.count(suc_wait_add)==0)
            {
                suc_map[suc_wait_add]=Hex;
                if(suc_set.insert(suc_wait_add).second==true)
                    reads[st].successor.push_back(suc_wait_add);
                switch_element(reads[suc_wait_add].predecessor,node,st);
            }
            else
            {
                switch_element(reads[suc_wait_add].predecessor,node,empty_node_id);
            }
        }
        for(int j=0;j<reads[node].predecessor.size();j++)
        {
            ull pre_wait_add=reads[node].predecessor[j];
            if(pre_wait_add==node_pre)
                continue;
            if(pre_wait_add==st)
                continue;
            pair<int,ull> Hex;
            Hex.first=cycle_time;
            Hex.second=node;
            //Hex_cycle_id(cycle_time,into_map);
            if(pre_map.count(pre_wait_add)==0)
            {
                pre_map[pre_wait_add]=Hex;
                if(pre_set.insert(pre_wait_add).second==true)
                    reads[st].predecessor.push_back(pre_wait_add);
                switch_element(reads[pre_wait_add].successor,node,st);
            }
            else
            {
                switch_element(reads[pre_wait_add].successor,node,empty_node_id);
            }
        }
        //debrujin_delete_node(reads,node);
        reads[node].successor.clear();
        reads[node].predecessor.clear();
    }
    cout<<cyc_edge.size()<<endl;
    if(pre_judge==1)
    {
        auto Hex=pre_map[pre];
        int cyc_id=Hex.first;
        ull point=Hex.second;
        //Hex_revive(Hex,cyc_id,point);
        cout<<cyc_id<<endl;
        vector <ull> temp=cycle_list[cyc_id];

        bool into_edge=false;
        for(int i=0;i<temp.size();i++)
        {
            if(temp[i]==point)
                into_edge=true;
            if(into_edge)
            {
                cyc_edge.push_back(temp[i]);
            }
                
        }
    }
    else
    {

        cyc_edge.push_back(st);
    }
    for(int p=1;p<cyc_edge.size()-1;p++)
    {
        if(cyc_edge[p]==st)
            cout<<"what the fuck "<<p<<" "<<st<<endl;
    }
    cout<<"actual length "<<cyc_edge.size()<<endl;
    cycle_list.push_back(cyc_edge);
    cur_suc=reads[st].successor;
    cur_pre=reads[st].predecessor;
}
void Cycle :: cycle_sort()
{
    int max_length=0;
    for(int i=0;i<cycle_list.size();i++)
    {
        if(cycle_list[i].size()>max_length)
        {
            max_length=cycle_list[i].size();
            max_tag=i;
        }
    }

}
void cycle_sort_OLC(vector <Reads> &reads,vector <Reads> &origin,vector<Cycle> &cyc,int tag)
{
    int reads_l=reads[0].sequence.size();
    cout<<"sort size "<<cyc[tag].cycle_list.size()<<endl;
    for(int i=0;i<cyc[tag].cycle_list.size();i++)
    {
        vector <ull> path=cyc[tag].cycle_list[i];
        int length=reads_l;
        //cout<<i<<endl;
        for(int j=0;j<path.size()-1;j++)
        {
            ull cur=path[j];
            ull suc=path[j+1];
            if(j==0)
            {
                int cyc_id=reads[j+1].cyc_id;
                if(reads[suc].cyc_id==-1)
                {
                    int index=find_vec_index(origin[cur].successor,suc);
                    if(index==-1)
                    {
                        cout<<"error end1"<<endl;
                        index=0;
                    }
                    length+=reads_l-reads[cur].successor_o[index];
                }
                else
                {
                    length+=cyc[reads[suc].cyc_id].max_length;
                }
                continue;
            }
            else if(j==path.size()-2)
            {
                int cyc_id=reads[j].cyc_id;
                if(reads[cur].cyc_id==-1)
                {
                    int index=find_vec_index(origin[cur].successor,suc);
                    if(index==-1)
                    {
                        cout<<"error end2"<<endl;
                        index=0;
                    }
                    length+=reads_l-reads[cur].successor_o[index];
                }
                else
                {
                    //length+=cyc[reads[cur].cyc_id].max_length;
                }
                continue;
            }
            if(reads[suc].cyc_id==-1 && reads[cur].cyc_id==-1)
            {   
                int index=find_vec_index(origin[cur].successor,suc);
                if(index==-1)
                {
                    cout<<"error"<<endl;
                    index=0;
                }
                length+=reads_l-reads[cur].successor_o[index];
                
            }
            else if(reads[suc].cyc_id!=-1 && reads[cur].cyc_id==-1)
            {
                length+=cyc[reads[suc].cyc_id].max_length;
            }
            else if(reads[suc].cyc_id==-1 && reads[cur].cyc_id!=-1)
            {
                continue;
            }
            else if(reads[suc].cyc_id!=-1 && reads[cur].cyc_id!=-1)
            {
                length+=cyc[reads[suc].cyc_id].max_length;
            }

        }
        if(length>cyc[tag].max_length)
        {
            cyc[tag].max_length=length;
            cyc[tag].max_tag=i; 
            cout<<"max length update "<<cyc[tag].max_length<<endl;
        }
    }

}
void cycle_sort_SDBG(vector <sdbg> &dbg,vector <sdbg> &origin,vector<Cycle> &cyc,int tag)
{
    int reads_l=dbg[0].kmer.size();
    cout<<"sort size "<<cyc[tag].cycle_list.size()<<endl;
    for(int i=0;i<cyc[tag].cycle_list.size();i++)
    {
        vector <ull> path=cyc[tag].cycle_list[i];
        int length=reads_l;
        //cout<<i<<endl;
        for(int j=0;j<path.size()-1;j++)
        {
            ull cur=path[j];
            ull suc=path[j+1];
            if(j==0)
            {
                int cyc_id=dbg[j+1].cycle_tag;
                if(dbg[suc].cycle_tag==-1)
                {
                    // int index=find_vec_index(origin[cur].successor,suc);
                    // if(index==-1)
                    // {
                    //     cout<<"error end1"<<endl;
                    //     index=0;
                    // }
                    length+=1;
                }
                else
                {
                    length+=cyc[dbg[suc].cycle_tag].max_length;
                }
                continue;
            }
            else if(j==path.size()-2)
            {
                int cyc_id=dbg[j].cycle_tag;
                if(dbg[cur].cycle_tag==-1)
                {
                    // int index=find_vec_index(origin[cur].successor,suc);
                    // if(index==-1)
                    // {
                    //     cout<<"error end2"<<endl;
                    //     index=0;
                    // }
                    length+=1;
                }
                else
                {
                    //length+=cyc[reads[cur].cyc_id].max_length;
                }
                continue;
            }
            if(dbg[suc].cycle_tag==-1 && dbg[cur].cycle_tag==-1)
            {   
                // int index=find_vec_index(origin[cur].successor,suc);
                // if(index==-1)
                // {
                //     cout<<"error"<<endl;
                //     index=0;
                // }
                length+=1;
                
            }
            else if(dbg[suc].cycle_tag!=-1 && dbg[cur].cycle_tag==-1)
            {
                length+=cyc[dbg[suc].cycle_tag].max_length;
            }
            else if(dbg[suc].cycle_tag==-1 && dbg[cur].cycle_tag!=-1)
            {
                continue;
            }
            else if(dbg[suc].cycle_tag!=-1 && dbg[cur].cycle_tag!=-1)
            {
                length+=cyc[dbg[suc].cycle_tag].max_length;
            }

        }
        if(length>cyc[tag].max_length)
        {
            cyc[tag].max_length=length;
            cyc[tag].max_tag=i; 
            cout<<"max length update "<<cyc[tag].max_length<<endl;
        }
    }

}
bool is_cycle(vector <ull> stack,vector <ull> &cycle_list,vector<Debrujin_node>dbg,ull cyc_st)
{
    ull path_depth=stack.size();
    cycle_list.push_back(cyc_st);
    for(int i=path_depth-1;i>=0;i--)
    {
        ull temp=stack[i];
        cycle_list.push_back(temp);
        if(temp==cyc_st)
        {
            cycle_list=vector <ull>(cycle_list.rbegin(),cycle_list.rend());
            return true;
        }
    }
    
    return false;
}
// void detect_cyc(vector <Debrujin_node>&dbg,ull root,vector <Cycle>&cyc)
// {
//     ull size=dbg.size();
//     vector <ull> path_stack;
//     bool visit[size];
//     int position[size];
//     int stack_condition[size];
//     for (int i=0;i<size;i++)
//     {
//         visit[i]=false;
//         position[i]=0;
//         stack_condition[i]=0;
//     }
//     path_stack.push_back(root);
//     visit[root]=true;
//     stack_condition[root]++;
//     bool jump_flag=false;
//     bool cyc_detect=false;
//     int cyc_tag=0;
//     while (path_stack.size()!=0)
//     {
//         ull back=path_stack.back();
//         jump_flag=false;
//         cyc_detect=false;
//         for(ull i=position[back];i<dbg[back].successor.size();i++)
//         {
//             ull next=dbg[back].successor[i];
//             if(visit[next]==false)
//             {
//                 visit[next]=true;
//                 path_stack.push_back(next);
//                 stack_condition[next]++;
//                 jump_flag=true;
//                 position[back]=i+1;
//                 break;
//             }
//             else
//             {
//                 vector <ull> temp_stack;
//                 //bool judge=is_cycle(path_stack,temp_stack,dbg,next);
//                 int judge=stack_condition[next];
//                 //if we find a cycle
//                 if(judge>=1)
//                 {
//                     vector <ull> stack;
//                     is_cycle(path_stack,stack,dbg,next);
//                     cyc_detect=true;
//                     while(path_stack.back()!=next)
//                     {
//                         stack_condition[path_stack.back()]--;
//                         path_stack.pop_back();

//                     }
//                     position[next]=0;
//                     if(dbg[next].cycle_tag==-1)
//                     {
//                         cout<<"cyc detected,st:  "<<next<<" length: "<<stack.size()<<endl;
//                         Cycle temp;
//                         temp.st=next;
//                         //temp.cycle_time=1;
//                         //temp.init(dbg[next]);
//                         temp.cyc_shrink(stack,dbg);
//                         cyc.push_back(temp);
//                         dbg[next].cycle_tag=cyc.size()-1;
//                         cout<<dbg[next].cycle_tag<<endl;
//                         if(dbg[next].read_st=true)
//                             cout<<"double traversal cyc detected"<<endl;
//                     }
//                     else
//                     {
//                         cout<<"complex cyc detected,st:  "<<next<<" length: "<<stack.size()<<endl;
//                         ull id=dbg[next].cycle_tag;
//                         cyc[id].cyc_shrink(stack,dbg);
//                         if(dbg[next].read_st=true)
//                             cout<<"double traversal cyc detected"<<endl;
//                     }
//                 }

//             }
//         }
//         if(cyc_detect==true)
//             continue;
//         if(jump_flag==false)
//         {
//             stack_condition[back]--;
//             path_stack.pop_back();
//             continue;
//         }
        
//     }
// }
