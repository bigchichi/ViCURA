#ifndef COMBINE_H
#define COMBINE_H
#include "dbg.h"
class Combinedbg
{
    public:
        string sequence;
        vector<ull>successor;
        vector<ull>predecessor;
        vector<int>coverage;
        vector<int>overlap;
    Combinedbg(Parameter*para)
    {
        vector <int> temp(para->kmerlength);
        coverage=temp;
    }
};
void SDBG_simplified(vector <sdbg> dbg,vector<Combinedbg> &cdbg,int MinOverlap,Parameter*para)
{
    cout<<"into function"<<endl;
    //int ReadNodeSize=0;
    unordered_map<ull,ull>shrink;
    vector <vector <int> >overalp;
    int read_num=0;
    for(int i=0;i<dbg.size();i++)
    {
        if(dbg[i].read_tag==-1)
            continue;
        //ReadNodeSize++;
        read_num++;
        cout<<"searching read "<<dbg[i].read_tag<<" suc size "<<dbg[i].successor.size()<<endl;
        cout<<"read num "<<read_num<<endl;
        vector<ull>read_node;
        //vector<int>visit(dbg.size());
       // unordered_set<ull>visit;
       unordered_set<ull> suc_set;
        vector <int> temp_overlap;
        for(int j=0;j<dbg[i].successor.size();j++)
        {
            //fill(visit.begin(),visit.end(),0);
            //visit.clear();
            //suc_set.clear();
            queue<ull>Q;
            ull suc=dbg[i].successor[j];
            int depth;
            ull layer_mark=-1;
            Q.push(suc);
            //visit[suc]=1;
            //visit.insert(suc);
            Q.push(layer_mark);
            int layer_depth=0;
            while (true)
            {
                //cout<<Q.size()<<" "<<visit.size()<<endl;
                if(Q.size()==1)
                    break;
                ull front=Q.front();
                //cout<<front<<endl;
                if(layer_depth>para->kmerlength)
                    break;
                if(front==layer_mark)
                {
                    layer_depth++;
                    Q.pop();
                    Q.push(layer_mark);
                    continue;
                }
                if(dbg[front].read_tag!=-1)
                {
                    int overlap=para->kmerlength-layer_depth-1;
                    if(overlap>=MinOverlap)
                    {                    
                        if(suc_set.count(front)==0)
                        {
                            suc_set.insert(front);
                            read_node.push_back(front);
                            temp_overlap.push_back(para->kmerlength-layer_depth-1);
                        }
                    }
                    Q.pop();
                    //visit.insert(front);
                    continue;
                }
                else
                {
                    //visit.insert(front);
                    for(int k=0;k<dbg[front].successor.size();k++)
                    {
                        ull node=dbg[front].successor[k];
                        // if(visit.count(node)==1)
                        //     continue;
                        Q.push(node);
                    }
                    Q.pop();
                 }
            }
            
        }
        dbg[i].successor.clear();
        dbg[i].successor=read_node;
        //cout<<"add read suc "<<read_node.size()<<endl;
        overalp.push_back(temp_overlap);
    }
    int tag=0;
    for(int i=0;i<dbg.size();i++)
    {
        if(dbg[i].read_tag==-1)
            continue;
        shrink[i]=tag;
        Combinedbg temp(para);
        temp.coverage=dbg[i].coverage;
        temp.successor=dbg[i].successor;
        temp.overlap=overalp[tag];
        temp.sequence=dbg[i].kmer;
        cdbg.push_back(temp);
        tag++;
    }
    for(int i=0;i<cdbg.size();i++)
    {
        for(int j=0;j<cdbg[i].successor.size();j++)
        {
            ull suc=cdbg[i].successor[j];
            cdbg[i].successor[j]=shrink[suc];
        }
    }
    for(int i=0;i<cdbg.size();i++)
    {
        for(int j=0;j<cdbg[i].successor.size();j++)
        {
            ull suc=cdbg[i].successor[j];
            cdbg[suc].predecessor.push_back(i);
        }
    }
}
void convert_cdbg_OLC(vector <Reads>&reads,vector<Combinedbg>&cdbg)
{
    for(int i=0;i<cdbg.size();i++)
    {
       // cout<<"convert "<<i<<endl;
        Reads temp;
        temp.sequence=cdbg[i].sequence;
        temp.successor=cdbg[i].successor;
        temp.predecessor=cdbg[i].predecessor;
        temp.read_count=cdbg[i].coverage[0];
        temp.successor_o=cdbg[i].overlap;
        temp.cov=cdbg[i].coverage;
        int st_cov=temp.cov[0];
        temp.suc_cov.push_back(st_cov);
        for(int j=1;j<temp.cov.size();j++)
        {
            //cout<<temp.suc_cov.back()+temp.cov[j]<<" ";
            temp.suc_cov.push_back(temp.suc_cov.back()+temp.cov[j]);
        }
        //cout<<endl;
        st_cov=temp.cov.back();
        temp.pre_cov.push_back(st_cov);
        for(int j=temp.cov.size()-2;j>=0;j--)
        {
            //cout<<temp.pre_cov.back()+temp.cov[j]<<" ";
            temp.pre_cov.push_back(temp.pre_cov.back()+temp.cov[j]);
        }
        //cout<<endl;
        reverse(temp.pre_cov.begin(),temp.pre_cov.end());
        reads.push_back(temp);
    }
}

void OLC_cov_calculate(vector <Reads> &reads)
{
    for(int i=0;i<reads.size();i++)
    {
        int st_cov=reads[i].cov[0];
        reads[i].suc_cov.push_back(st_cov);
        for(int j=1;j<reads[i].cov.size();j++)
        {
            reads[i].suc_cov.push_back(reads[i].suc_cov.back()+reads[i].cov[j]);
        }
        st_cov=reads[i].cov.back();
        reads[i].pre_cov.push_back(st_cov);
        for(int j=reads[i].cov.size()-2;j>=0;j--)
        {
            reads[i].pre_cov.push_back(reads[i].pre_cov.back()+reads[i].cov[j]);
        }
        reverse(reads[i].pre_cov.begin(),reads[i].pre_cov.end());
    }
}
#endif