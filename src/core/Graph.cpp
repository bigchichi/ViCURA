#include "dbg.h"
void get_pairs(vector <Reads> &reads,Parameter*para)
{
    unordered_map <ull,Zipline> fix_map;
    Pairs temp_pair;
    int fix_length=para->rLengthNormal-para->sequence_o;
    //cout<<fix_length<<"!!!!"<<reads[0].perifix_array.size()<<endl;
    int i;
    ull j,k;
    Zipline temp;
    ull ID,usage;
    string start,overlap,end;
    int overlap_length;
    for(i=fix_length-1;i>=0;i--)
    {
        overlap_length=para->sequence_o+i;
        //cout<<overlap_length<<endl;
        for(j=0;j<reads.size();j++)
        {
            if(reads[j].perifix_array[i]!=0)
            {
                if(fix_map.count(reads[j].perifix_array[i])==0)
                {
                    temp.zip_stack.clear();
                    temp.zip_stack.push_back(j);
                    fix_map[reads[j].perifix_array[i]]=temp;
                }
                else
                {
                    //vector<ull>::iterator s = find(fix_map[reads[j].perifix_array[i]].zip_stack.begin(), fix_map[reads[j].perifix_array[i]].zip_stack.end(), j);
                    //if(s==fix_map[reads[j].perifix_array[i]].zip_stack.end())
                    fix_map[reads[j].perifix_array[i]].zip_stack.push_back(j);
                    
                }    
            }       
        }
        // cout<<"---"<<overlap_length<<endl;
        // cout<<para->pair_size<<endl;
        for(j=0;j<reads.size();j++)
        {
            if(reads[j].sequence.length()>overlap_length)
            {
                if(fix_map.count(reads[j].suffix_array[i])!=0)
                {
                    for(int p=0;p<fix_map[reads[j].suffix_array[i]].zip_stack.size();p++)
                    {
                        ID=fix_map[reads[j].suffix_array[i]].zip_stack[p];
                        temp_pair.st_ID=j;
                        temp_pair.ed_ID=ID;
                        temp_pair.overlap_length=overlap_length;
                        temp_pair.uov_init(reads[j],reads[ID],para);
                        if(temp_pair.total_length<para->rLengthNormal+1)
                            continue;
                        //cout<<temp_pair.total_length<<endl;
                        if(!reads[j].find(1,ID))
                        {
                            reads[j].successor_o.push_back(overlap_length);
                            reads[ID].predecessor_o.push_back(overlap_length);
                            if(reads[j].length<para->rLengthNormal)
                            {                                                                
                                reads[j].successor_o[reads[j].successor_o.size()-1]=overlap_length+para->rLengthNormal-reads[j].length;
                                reads[j].sequence=temp_pair.uov_sequence.substr(0,para->rLengthNormal);
                                reads[j].length=para->rLengthNormal;
                                reads[j].array_init(para);

                            }
                            if(reads[ID].length<para->rLengthNormal)
                            {
                                reads[ID].predecessor_o[reads[ID].predecessor_o.size()-1]=overlap_length+para->rLengthNormal-reads[ID].length;
                                reads[ID].sequence=temp_pair.uov_sequence.substr(temp_pair.total_length-para->rLengthNormal,para->rLengthNormal);
                                reads[ID].length=para->rLengthNormal;
                                reads[ID].array_init(para);
                            }
                            reads[j].outdegree++;
                            reads[j].successor.push_back(ID);
                            reads[j].next.push_back(ID);
                            reads[ID].indegree++;
                            reads[ID].predecessor.push_back(j);
                            reads[ID].next.push_back(j);
                            //pairs.push_back(temp_pair); 
                            para->pair_size++;
                      
                        }
                    }
                }
            }
        }
        //cout<<"---"<<overlap_length<<"--- done"<<endl;
        //cout<<para->pair_size<<endl;
        unordered_map<ull,Zipline>().swap(fix_map);
        malloc_trim(0);

    }
}
// void delete_isolated_reads(vector <Reads> reads)
// {
//     for (ull i=0;i<reads.size();i++)
//     {
//         if(reads[i].indegree==0 && reads[i].outdegree==0)
//         {
//             reads[i].usage=0;
//         }
        
//     }

// }
void subgraph(vector <Subgraph> &sub,vector <Reads> reads)
{
    ull read_size=reads.size();
    vector <bool> reads_usage;
    vector <ull> root_list;
    vector <ull> stack;
    ull temp=0;
    for(ull i=0;i<read_size;i++)
    {
        reads_usage.push_back(true);
        if(reads[i].indegree==0 && reads[i].outdegree!=0)
        {
            //cout<<i<<" root"<<endl;
            root_list.push_back(i);
        }
    }
    //root_list=vector<ull>(root_list.rbegin(),root_list.rend());
    //root_list.pop_back();
    //cout<<root_list.size()<<endl;
    for(ull i=0;i<root_list.size();i++)
    {
        if(reads_usage[root_list[i]]==true)
        {
            reads_usage[root_list[i]]=false;
            //cout<<root_list[i]<<endl;
            Subgraph temp_sub;
            stack.push_back(root_list[i]);
            temp_sub.node_list.push_back(root_list[i]);
            //cout<<reads_usage[reads[stack.back()].next[0]]<<endl;
            while(stack.size()!=0)
            {
                ull top=stack.back();
                //cout<<temp_sub.node_list.size()<<endl;
                for(ull j=0;j<reads[top].next.size();j++)
                {
                    if(reads_usage[reads[top].next[j]]==true)
                    {
                        reads_usage[reads[top].next[j]]=false;
                        //cout<<reads[top].next[j]<<endl;
                        temp_sub.node_list.push_back(reads[top].next[j]);
                        stack.push_back(reads[top].next[j]);
                        temp=1;
                        break;
                    }
                }
                //cout<<temp;
                if(temp==0)
                {
                    stack.pop_back();
                }
                temp=0;
                // cout<<reads_usage[reads[stack.back()].next[0]]<<endl;
                // cout<<rea
            }
            temp_sub.size=temp_sub.node_list.size();
            //cout<<temp_sub.size<<">>>"<<root_list[i]<<endl;
            //cout<<temp_sub.node_list.size()<<"---"<<temp_sub.size<<endl;

            for(ull k=0;k<temp_sub.size;k++)
            {
                if(reads[temp_sub.node_list[k]].indegree==0 && reads[temp_sub.node_list[k]].outdegree!=0)
                {
                    //cout<<"root"<<endl;
                    temp_sub.root_list.push_back(temp_sub.node_list[k]);
                }
                if(reads[temp_sub.node_list[k]].indegree!=0 && reads[temp_sub.node_list[k]].outdegree==0)
                {
                    //cout<<"leave"<<endl;
                    temp_sub.leave_list.push_back(temp_sub.node_list[k]);
                }
            }
            //cout<<"done "<<temp_sub.size<<endl;
            sub.push_back(temp_sub);
            temp_sub.node_list.clear();
            temp_sub.root_list.clear();
            temp_sub.leave_list.clear();
            stack.clear();
        }
        
    }
}
vector <Subgraph> large_sub(vector <Subgraph> sub,int size)
{
    vector <Subgraph> sub_list;
    for(int i=0;i<=size;i++)
    {
        Subgraph s;
        s.size=0;
        sub_list.push_back(s);
    }

    for(int i=0;i<sub.size();i++)
    {
        sub_list[size]=sub[i];
            //cout<<"into"<<endl;
        for (int j=size-1;j>=0;j--)
        {
            if(sub_list[j].size<sub_list[j+1].size)
            {
                Subgraph tmp=sub_list[j];
                sub_list[j]=sub_list[j+1];
                sub_list[j+1]=tmp;
            }
        }
    }
    return sub_list;
}