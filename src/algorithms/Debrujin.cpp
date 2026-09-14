#include "dbg.h"
void Debrujin_node::init(Parameter*para)
{
    k_mer_hash=get_hash(k_mer,k_mer.length(),para);
    suffix_hash=get_hash(k_mer.substr(0,k_mer.size()-1),k_mer.length()-1,para);
    prefix_hash=get_hash(k_mer.substr(1,k_mer.length()-1),k_mer.length()-1,para);
}
void Debrujin_node :: coverage_update(ull readA,ull readB,int overlap,int st,int length, int countA, int countB)
{
    int ed=st+length-1;
    int A_st=0;
    int A_ed=length-1;
    int B_st=length-overlap;
    int B_ed=2*length-1;

    if(ReadSet_st.count(readA)==0)
    {
        ull sted;
        
        int A_length=A_ed-st+1;
        //Hex_merge_st_ed(sted,0,A_length-1);
        ReadSet_st.insert(readA);
        for(int i=0;i<=A_length-1;i++)
        {
            coverage[i]+=countA;
        }


    }
    if(ReadSet_ed.count(readB)==0)
    {
        ull sted;
        int B_length=ed-B_st+1;
        //Hex_merge_st_ed(sted,length-B_length-1,length-1);
        ReadSet_ed .insert(readB);
        for(int i=length-B_length;i<=length-1;i++)
        {
            coverage[i]+=countB;
        }
    }
    cov=coverage.back();
    pair <ull,ull> temp(readA,readB);
    pair_vec.push_back(temp);

}
bool compare(const pair<ull,double> &p1,const pair<ull,double> &p2)
{
    return p1.second>p2.second;
}
void average_count_sort_dbg(vector <Debrujin_node> &dbg,Parameter*para)
{
    vector <pair<ull,double>> average_count;
    for (int i=0;i<dbg.size();i++)
    {
        average_count.clear();
        if(dbg[i].successor.size()<=1)
            continue;
        vector <vector <ull>> path;
        if(dbg[i].cycle_tag!=-1)
            continue;
        for(int j=0;j<dbg[i].successor.size();j++)
        {
            
            pair <ull,double> p;
            vector <ull> temp_path;
            ull next=dbg[i].successor[j];
            p.first=next;
            int total_count=dbg[next].cov;
            int length=1;
            temp_path.push_back(next);
            while (true)
            {
               
                if(dbg[next].successor.size()==1 && dbg[next].predecessor.size()==1)
                {
                    next=dbg[next].successor[0];
                }
                else if(dbg[next].successor.size()==0 || dbg[next].successor.size()>1 ||dbg[next].predecessor.size()>1 || dbg[next].cycle_tag!=-1)
                {
                    break;
                }

                total_count+=dbg[next].cov;
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
        dbg[i].successor.clear();
        for(int j=0;j<average_count.size();j++)
        {
            result_suc.push_back(average_count[j].first);
            dbg[i].successor.push_back(average_count[j].first);
            //cout<<average_count[j].first<<"---"<<average_count[j].second<<endl;
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
void debrujin_delete_node (vector <Debrujin_node>&dbg,int node_id)
{
    if(node_id <0 || node_id>=dbg.size())
        return;
    for(int i=0;i<dbg[node_id].successor.size();i++)
    {
        int tg=dbg[node_id].successor[i];
        auto it=find(dbg[tg].predecessor.begin(),dbg[tg].predecessor.end(),node_id);
        if(it!=dbg[tg].predecessor.end())
            dbg[tg].predecessor.erase(it);
    }
    for(int i=0;i<dbg[node_id].predecessor.size();i++)
    {
        int tg=dbg[node_id].predecessor[i];
        auto it=find(dbg[tg].successor.begin(),dbg[tg].successor.end(),node_id);
        if(it!=dbg[tg].successor.end())
            dbg[tg].successor.erase(it);
    }
    dbg[node_id].successor.clear();
    dbg[node_id].predecessor.clear();

}
void Sdebrujin_delete_node (vector<sdbg>&dbg,int node_id)
{
    if(node_id <0 || node_id>=dbg.size())
        return;
    for(int i=0;i<dbg[node_id].successor.size();i++)
    {
        int tg=dbg[node_id].successor[i];
        auto it=find(dbg[tg].predecessor.begin(),dbg[tg].predecessor.end(),node_id);
        if(it!=dbg[tg].predecessor.end())
            dbg[tg].predecessor.erase(it);
    }
    for(int i=0;i<dbg[node_id].predecessor.size();i++)
    {
        int tg=dbg[node_id].predecessor[i];
        auto it=find(dbg[tg].successor.begin(),dbg[tg].successor.end(),node_id);
        if(it!=dbg[tg].successor.end())
            dbg[tg].successor.erase(it);
    }
    dbg[node_id].successor.clear();
    dbg[node_id].predecessor.clear();

}
void init_debrujin(Subgraph sub,vector <Reads> reads,vector <Debrujin_node>&dbg,Parameter*para)
{
    unordered_map <ull,ull> k_mer_map;
    //brand first get all edges
    vector <string> tmp_k_mer;
    for(ull i=0;i<sub.node_list.size();i++)
    {
        ull node=sub.node_list[i];
        for(ull k=0;k<reads[node].successor.size();k++)
        {
            ull next_node=reads[node].successor[k];
            tmp_k_mer.clear();
            tmp_k_mer=get_merged_sequence(reads[node],reads[next_node],reads[node].successor_o[k],para);
            int overlap_length=reads[node].successor_o[k];
            for (int j=0;j<tmp_k_mer.size();j++)
            {
                Debrujin_node tmp;
                tmp.k_mer=tmp_k_mer[j];
                tmp.init(para);
                tmp.coverage_update(node,next_node,overlap_length,j,para->rLengthNormal,reads[node].read_count,reads[next_node].read_count);
                if(j==0 && k_mer_map.count(tmp.k_mer_hash)==0)
                {
                    tmp.read_st=true;                 
                    k_mer_map[tmp.k_mer_hash]=dbg.size();
                    dbg.push_back(tmp);
                    ull id=dbg.size()-1;
                }
                else if(k_mer_map.count(tmp.k_mer_hash)==0)
                {
                    
                    k_mer_map[tmp.k_mer_hash]=dbg.size();
                    //cout<<tmp.k_mer<<endl;
                    dbg.push_back(tmp);
                    ull id=dbg.size()-1;
                }
                else
                {
                    ull id=k_mer_map[tmp.k_mer_hash];
                    dbg[id].coverage_update(node,next_node,overlap_length,j,para->rLengthNormal,reads[node].read_count,reads[next_node].read_count);
                    
                   
                }
            }
            //cout<<dbg.size()<<endl;
        }
    }
    unordered_map <ull,ull> ().swap(k_mer_map);
    malloc_trim(0);
    cout<<dbg.size()<<endl;
    // for(int i=0;i<dbg.size();i++)
    // {
    //     dbg[i].ReadSet.clear();
    // }
    unordered_map <ull,Zipline> fix_map;
    Zipline tmp_zip;
    for(ull i=0;i<dbg.size();i++)
    {
        if(fix_map.count(dbg[i].suffix_hash)==0)
        {
            tmp_zip.zip_stack.push_back(i);
            fix_map[dbg[i].suffix_hash]=tmp_zip;
            tmp_zip.zip_stack.clear();
        }
        else
        {
            fix_map[dbg[i].suffix_hash].zip_stack.push_back(i);
        }
    }
    for(ull i=0;i<dbg.size();i++)
    {
        if(fix_map.count(dbg[i].prefix_hash)==1)
        {
            for(ull j=0;j<fix_map[dbg[i].prefix_hash].zip_stack.size();j++)
            {
                ull node_next=fix_map[dbg[i].prefix_hash].zip_stack[j];
                dbg[i].successor.push_back(node_next);
                dbg[node_next].predecessor.push_back(i);
                para->dbg_edge_size++;
            }
        }
    }
}
bool if_debrujin_connected(vector <Debrujin_node> dbg)
{
    vector <vector <ull>> next;
    bool visit[dbg.size()];
    queue <ull> Q;
    for(int i=0;i<dbg.size();i++)
    {
        vector <ull> temp;
        temp.insert(temp.end(),dbg[i].predecessor.begin(),dbg[i].predecessor.end());
        temp.insert(temp.end(),dbg[i].successor.begin(),dbg[i].successor.end());
        next.push_back(temp);
        visit[i]=false;
    }
    Q.push(0);
    visit[0]=true;
    while(Q.size()!=0)
    {
        ull front=Q.front();
        for(int i=0;i<next[front].size();i++)
        {
            if(visit[next[front][i]]==false)
            {
                visit[next[front][i]]=true;
                Q.push(next[front][i]);
            }
        }
        Q.pop();
    }
    for(int i=0;i<dbg.size();i++)
    {
        if(visit[i]==false)
        {
            if(dbg[i].successor.size()==0 && dbg[i].predecessor.size()==0)
            {
                continue;
            }
            else
            {
                return false;
            }
        }
    }
    return true;

}
//delete frequency<=threshold nodes,get the largest subgraph if the dbg isn't connected.
void remove_low_frequency_node(vector <Debrujin_node> &dbg,int threshold)
{
    int size=dbg.size();
    vector <int> visit(size);
    for(int i=0;i<dbg.size();i++)
    {
        if(dbg[i].coverage[0]<=threshold)
        {
            debrujin_delete_node(dbg,i);
            //cout<<"del"<<endl;
        }
    }
    if(if_debrujin_connected(dbg))
        return;
    vector <vector<ull>> subgraph;
    for(int i=0;i<dbg.size();i++)
    {
        vector<ull> reach;
        ull root_node=i;
        if(visit[root_node]==1 || (dbg[root_node].predecessor.size()==0 && dbg[root_node].successor.size()==0))
            continue;
        queue <ull> Q;
        visit[root_node]=1;
        Q.push(root_node);
        reach.push_back(root_node);
        while (!Q.empty())
        {
            ull cur=Q.front();
            bool jump_tag=false;
            for(int j=0;j<dbg[cur].predecessor.size();j++)
            {
                ull next=dbg[cur].predecessor[j];
                if(visit[next]==0)
                {
                    Q.push(next);
                    reach.push_back(next);
                    visit[next]=1;
                }
            }
            for(int j=0;j<dbg[cur].successor.size();j++)
            {
                ull next=dbg[cur].successor[j];
                if(visit[next]==0)
                {
                    Q.push(next);
                    reach.push_back(next);
                    visit[next]=1;
                }
            }
            Q.pop();
        }
        subgraph.push_back(reach);        
    }
    int max_sub_size=0;
    int max_sub_id=-1;
    for(int i=0;i<subgraph.size();i++)
    {
        //cout<<"ppp "<<subgraph[i].size()<<endl;
        if(subgraph[i].size()>max_sub_size)
            max_sub_id=i;
    }
    //cout<<"max id " <<max_sub_id<<endl;
    // for(int i=0;i<subgraph.size();i++)
    // {
    //     if(i!=max_sub_id)
    //     {
    //         for(int j=0;j<subgraph[i].size();j++)
    //             debrujin_delete_node(dbg,subgraph[i][j]);
            
    //     }
    // }

}
bool traversal_v1(vector <Debrujin_node> dbg,ull root,ull leave,vector <Result>& result,vector <int>&position)
{
    vector <ull> stack;
    stack.push_back(root);
    bool jumptag=false;
    while (true)
    {
        ull back=stack.back();
        
        if(back==leave)
        {
            //cout<<"strain done "<<strain_count<<endl;
            if(jumptag)
            {
                Result temp;
                temp.path=stack;
                result.push_back(temp);
                stack.pop_back();
                //cout<<"add 2"<<endl;
                
            }
            break;
        }
        if(stack.size()==0)
            break;
        if(position[back]==dbg[back].successor.size())
        {
            stack.push_back(dbg[back].successor[0]);
        }
        else
        {
            ull next=dbg[back].successor[position[back]];
            //cout<<"using another suc "<<back<<" "<<position[back]<<endl;
            stack.push_back(next);
            position[back]++;
            if(dbg[back].successor.size()>1)
                jumptag=true;
            continue;
        }

    }
    cout<<"the size of traversal is "<<result.size()<<endl;
    return jumptag;
    
}

// void strain_print(vector<Debrujin_node>dbg,vector<Cycle>cyc,vector <Result> result,string result_path,Parameter*para)
// {
//     int dbg_size=dbg.size();
//     for(int i=0;i<result.size();i++)
//     {   
//         vector <ull> origin=result[i].path;
//         string k_mer_path=result_path+"/k_mer_"+to_string(i);
//         ofstream kout(k_mer_path,ios::app);
        
//         int tag=0;
//         vector<int>node_usage(dbg_size);
//         cout<<"start length "<<origin.size()<<endl;
//         while (true)
//         {
//             //cout<<"into print function"<<endl;
//             if(tag==origin.size())
//                 break;
//             ull current_node=origin[tag];
//             //cout<<current_node<<"----"<<dbg[current_node].cycle_tag<<endl;
//             if(dbg[current_node].cycle_tag==-1)
//             {
//                 tag++;
//                 continue;
//             }
//             else
//             {
//                 //cout<<"into"<<endl;
//                 if(node_usage[current_node]==0)
//                 {
//                     //cout<<current_node<<"  "<<node_usage[current_node]<<endl;
//                     ull pre=origin[tag-1],suc=origin[tag+1];
//                     ull cyc_tag=dbg[current_node].cycle_tag;
//                     ull real_tag=tag;
//                     if(cyc[cyc_tag].pre_map.count(pre)==0 && cyc[cyc_tag].suc_map.count(suc)==0)
//                     {
//                         cout<<"whole cycle"<<endl;
//                         vector <ull> edge=cyc[cyc_tag].cycle_list[cyc[cyc_tag].max_tag];
//                         origin.insert(origin.begin()+real_tag,edge.begin(),edge.end()-1);
//                         tag++;
//                         continue;
//                     }
//                     if(cyc[cyc_tag].pre_map.count(pre)==0)
//                     {
//                         //vector <ull> edge=cyc[cyc_tag].cycle_list[0];
//                         //origin.insert(origin.begin()+real_tag,edge.begin(),edge.end()-1);
//                         //real_tag+=edge.size();
//                     }
//                     else
//                     {
//                         cout<<"pre recover"<<endl;
//                         ull Hex=cyc[cyc_tag].pre_map[pre];
//                         int cyc_id;
//                         ull point;
//                         Hex_revive(Hex,cyc_id,point);
//                         vector <ull> edge=cyc[cyc_tag].cycle_list[cyc_id];
//                         bool point_arrive=false;
//                         for(int j=0;j<edge.size()-1;j++)
//                         {
//                             if(edge[j]==point)
//                                 point_arrive=true;
//                             if(point_arrive)
//                             {
//                                 origin.insert(origin.begin()+real_tag,edge[j]);
//                                 //cout<<"insert pre "<<edge[j]<<endl;
//                                 real_tag++;
//                             }
//                         }
                        
//                     }
//                     real_tag++;
//                     if(cyc[cyc_tag].suc_map.count(suc)==0)
//                     {
//                         //vector <ull> edge=cyc[cyc_tag].cycle_list[0];
//                         //origin.insert(origin.begin()+real_tag,edge.begin()+1,edge.end());
//                     }
//                     else
//                     {
//                         cout<<"suc recover"<<endl;
//                         ull Hex=cyc[cyc_tag].suc_map[suc];
//                         int cyc_id;
//                         ull point;
//                         Hex_revive(Hex,cyc_id,point);
//                         vector<ull>edge=cyc[cyc_tag].cycle_list[cyc_id];
//                         for(int j=1;j<edge.size();j++)
//                         {
//                             origin.insert(origin.begin()+real_tag,edge[j]);
//                             //cout<<point<<endl;
//                             //cout<<"insert suc "<<edge[j]<<endl;
//                             if(edge[j]==point)
//                                 break;
//                             real_tag++;
//                         }
//                     }
//                     node_usage[current_node]=1;
//                     tag++;
                        
//                 }
//                 else
//                 {
//                     tag++;
//                     continue;
//                 }
//             }
            
//         }
//         cout<<"end length "<<origin.size()<<endl;
        
//         for(int k=0;k<origin.size();k++)
//         {
//             kout<<dbg[origin[k]].k_mer<<" "<<origin[k]<<" "<<dbg[origin[k]].prefix_hash<<" "<<dbg[origin[k]].suffix_hash<<endl;
//             //cout<<origin[k]<<endl;
//         }
//         string temp="";
//         temp=temp+dbg[origin[0]].k_mer;
//         for(int p=1;p<origin.size();p++)
//         {
//             temp=temp+dbg[origin[p]].k_mer.back();
//         }
//         string strain_path=result_path+"/strain_"+to_string(i);
//         ofstream strainout(strain_path,ios::app);
//         int count=0;
//         strainout<<"strain "<<i<<endl;
//         strainout<<"length: "<<temp.size()<<endl;
//         strainout<<endl;
//         for(int j=0;j<temp.size();j++)
//         {
//             strainout<<temp[j];
//             count++;
//             if(count%140==0)
//                 strainout<<endl;
//         }
//         strainout<<endl;
//         cout<<endl;
//         cout<<endl;
//         cout<<endl;
//         cout<<endl;
//         cout<<endl;
//         cout<<endl;
//         string subgraph_result_path=result_path+"/another_coverage_"+to_string(i);
//         string cmd="mkdir -p "+result_path+"/another_coverage_"+to_string(i);
//         system(cmd.c_str());
//         get_coverage_info(subgraph_result_path,temp,dbg,para);
//         string coveragee_info_path=result_path+"/coverage_info_"+to_string(i);
//         ofstream coverout(coveragee_info_path,ios::app);
//         for(int p=0;p<origin.size();p++)
//         {
//             coverout<<p<<":\t";
//             coverout<<dbg[origin[p]].coverage[0]<<"\t"<<origin[p]<<"\t";
//             if(dbg[origin[p]].successor.size()>=2)
//                 coverout<<"outdegree>=2 tag\t";
//             if(dbg[origin[p]].predecessor.size()>=2)
//                 coverout<<"indegree>=2 tag\t";
//             if(dbg[origin[p]].cycle_tag!=-1)
//                 coverout<<"cycle beginning node tag";
//             coverout<<endl;
//             // if(p==27786)
//             // {
//             //     cout<<origin[p]<<" error "<<dbg[origin[p]].successor.size()<<endl;
//             //     for(int q=0;q<dbg[origin[p]].successor.size();q++)
//             //     {
//             //         cout<<"suc "<<dbg[origin[p]].successor[q]<<endl;
//             //     }
//             // }
//         }
//         for(int p=1;p<para->rLengthNormal;p++)
//         {
//             coverout<<origin.size()+p<<":\t";
//             coverout<<dbg[origin.back()].coverage[p]<<endl;
//         }   
//     }
//     string all_k_mer_path=result_path+"/all_k_mer";
//     ofstream allout(all_k_mer_path,ios::app);
//     for(int i=0;i<dbg.size();i++)
//     {
//         allout<<dbg[i].k_mer<<endl;
//     }

// }