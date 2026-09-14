#include "dbg.h"
// 0000 0000 suc4 0000 0000 suc3 0000 0000 suc2 0000 0000 suc1
// 0000 0000 unreachable 0000 00001 reachable        
void Hex_bub_revise(Debrujin_node &node,int id)
{
    int reach=1<<(8*(id-1));
    node.bubble_tag=node.bubble_tag | reach;
}
vector <int> Hex_bub_revive(Debrujin_node node)
{
    int tag=node.bubble_tag;
    int id1=tag & 0x000000ff;
    int id2=tag & 0x0000ff00 >> 8;
    int id3=tag & 0x00ff0000 >> 16;
    int id4=tag & 0xff000000 >> 24;
    vector <int> result;
    if(id1==1)
        result.push_back(id1);
    if(id2==1)
        result.push_back(id2);
    if(id3==1)
        result.push_back(id3);
    if(id4==1)
        result.push_back(id4);
    return result;
}
vector <ull> DFS_target(vector<Debrujin_node> dbg,ull st, ull ed)
{
    int size=dbg.size();
    vector<int> position(size);
    vector <ull> result;
    result.push_back(st);
    position[st]++;
    while (true)
    {
        ull cur=result.back();
        if(cur==ed)
            return result;
        if(position[cur]==dbg[cur].successor.size())
        {
            position[cur]=0;
            result.pop_back();
        }
        else
        {
            result.push_back(dbg[cur].successor[position[cur]]);
            position[cur]++;
        }
    }
       
}
void bubble_detect(vector<Debrujin_node>&dbg,string result_path)
{
    string bubout_path=result_path+"/bubble_info";
    ofstream bubout(bubout_path,ios::app);
    int size=dbg.size();
    for(int i=0;i<dbg.size();i++)
    {
        int suc_count = dbg[i].successor.size();
        
        if(dbg[i].successor.size()<=1)
            continue;
        cout<<i<<"\t"<<suc_count<<endl;
        for(int j=0;j<suc_count;j++)
        {
            int id=j+1;
            vector<int> position(size);
            vector<int> visit(size);
            vector<int> stack_position(size);
            vector <ull> stack;
            stack.push_back(dbg[i].successor[j]);
            position[dbg[i].successor[j]]++;
            stack_position[dbg[i].successor[j]]++;
            while (true)
            {
                if(stack.size()==0)
                    break;
                ull cur=stack.back();
                
                // if(dbg[cur].predecessor.size()>=2)
                // {
                //     if (i==79102)
                //         cout<<"into1 "<<j<<endl;
                //     vector <int> temp=Hex_bub_revive(dbg[cur]);
                //     if (i==79102)
                //         cout<<"into2 "<<temp.size()<<endl;
                //     for(int p=0;p<temp.size();p++)
                //     {
                //         if(temp[p]!=id)
                //         {
                //             cout<<"into3"<<endl;
                //             vector <ull> edge=DFS_target(dbg,dbg[i].successor[temp[p]],cur);
                //             cout<<"out"<<endl;
                //             bubout<<"bubble start id: "<<i<<endl;
                //             if(dbg[cur].cycle_tag!=-1)
                //                 bubout<<"combined bubble"<<endl;
                //             if(stack.size()<edge.size())
                //             {
                //                 stack.swap(edge);
                //             }
                //             int longlength=stack.size();
                //             int shortlength=edge.size();
                //             int stack_total=dbg[i].coverage[0];
                //             int edge_total=dbg[i].coverage[0];
                //             bubout<<i<<": "<<dbg[i].coverage[0]<<"\t"<<i<<": "<<dbg[i].coverage[0]<<endl;
                //             for(int q=0;q<shortlength;q++)
                //             {
                //                 bubout<<stack[q]<<": "<<dbg[stack[q]].coverage[0]<<"\t"<<edge[q]<<": "<<dbg[edge[q]].coverage[0]<<endl;
                //                 stack_total+=dbg[stack[q]].coverage[0];
                //                 edge_total+=dbg[edge[q]].coverage[0];
                //             }
                //             for(int q=shortlength;q<longlength;q++)
                //             {
                //                 bubout<<stack[q]<<": "<<dbg[stack[q]].coverage[0]<<endl;
                //                 stack_total+=dbg[stack[q]].coverage[0];
                //             }
                //             bubout<<"length: "<<longlength+1<<"\t"<<"length: "<<shortlength+1<<endl;
                //             bubout<<"average coverage: "<<(double)stack_total/(double)(longlength+1)<<"\t"<<"average coverage: "<<(double)edge_total/(double)(shortlength+1)<<endl;        
                //             bubout<<endl;
                //             bubout<<endl;
                //             bubout<<endl;
                //         }
                //     }
                //     Hex_bub_revise(dbg[cur],id);
                // }
                if(position[cur]==dbg[cur].successor.size())
                {
                    position[cur]=0;
                    stack.pop_back();
                    stack_position[cur]--;
                }
                else
                {
                    stack.push_back(dbg[cur].successor[position[cur]]);
                    position[cur]++;
                    stack_position[cur]++;
                }
                if(stack_position[cur]>1)
                    cout<<"error cyc! "<<cur<<endl;
                 
            }
        }
        for(int j=0;j<dbg.size();j++)
        {
            dbg[j].bubble_tag=0;
        }
        cout<<"done1"<<endl;
    }
}