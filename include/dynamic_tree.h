#ifndef DTREE_H
#define DTREE_H
#include "dbg.h"
class Dtree_leaf
{
    public:
        ull id;
    Dtree_leaf(){}
};
class Dynamic_Tree
{
    public:
        void*A_next=NULL;
        void*C_next=NULL;
        void*G_next=NULL;
        void*T_next=NULL;
    Dynamic_Tree(){}
        //ull ID;
    void add_child(int tag)
    {
        // cout<<"---"<<endl;
        // cout<<(this->A_next==NULL)<<endl;
        // cout<<(this->C_next==NULL)<<endl;
        // cout<<(this->G_next==NULL)<<endl;
        // cout<<(this->T_next==NULL)<<endl;
        // cout<<"---"<<endl;
        if(tag==0)
        {
            A_next=new Dynamic_Tree();
        }
        else if(tag==1)
        {
            C_next=new Dynamic_Tree();
        }
        else if(tag==2)
        {
            G_next=new Dynamic_Tree();
        }
        else if(tag==3)
        {
            T_next=new Dynamic_Tree();
        }
        // cout<<"///"<<endl;
        // cout<<(this->A_next==NULL)<<endl;
        // cout<<(this->C_next==NULL)<<endl;
        // cout<<(this->G_next==NULL)<<endl;
        // cout<<(this->T_next==NULL)<<endl;
        // cout<<"///"<<endl;
    }
    void add_leaf(int tag,ull ex_id)
    {
        if(tag==0)
        {
            A_next=new Dtree_leaf();
            ((Dtree_leaf*)A_next)->id=ex_id;
            // cout<<((Dynamic_Tree*)A_next)->A_next<<endl;
            // cout<<((Dynamic_Tree*)A_next)->C_next<<endl;
            // cout<<((Dynamic_Tree*)A_next)->G_next<<endl;
            // cout<<((Dynamic_Tree*)A_next)->T_next<<endl;
        }
        else if(tag==1)
        {
            //cout<<"add leaf"<<endl;
            C_next=new Dtree_leaf();
            ((Dtree_leaf*)C_next)->id=ex_id;
            //cout<<"add done"<<endl;
        }
        else if(tag==2)
        {
            G_next=new Dtree_leaf();
            ((Dtree_leaf*)G_next)->id=ex_id;
        }
        else if(tag==3)
        {
            T_next=new Dtree_leaf();
            ((Dtree_leaf*)T_next)->id=ex_id;
        }
    }
};
ull Dtree_accept(string kmer,Dynamic_Tree*root,ull exptected_id)
{
    Dynamic_Tree*temp=root;
    bool find_tag=true;
    int max_depth=kmer.size()+1;
    int depth=0;
    typeid(int);
    //cout<<kmer.size()<<endl;
    for(int i=0;i<kmer.size()-1;i++)
    {
        int base_tag;
        bool acc=true;
        switch (kmer[i])
        {
            case 'A' : 
            {
                base_tag=0;
               // cout<<(temp->A_next==NULL)<<endl;
                if(temp->A_next==NULL)
                {
                    acc=false;
                }
                break;
            }
            case 'C' :
            {
                base_tag=1;

                
                if(temp->C_next==NULL)
                {
                    //cout<<"into"<<endl;
                    acc=false;
                }
                break;
            }
            case 'G':
            {
                base_tag=2;
                       // cout<<(temp->G_next==NULL)<<endl;
                if(temp->G_next==NULL)
                {
                    acc=false;
                }
                break;
            }
            case 'T':
            {
                base_tag=3;
                        //cout<<(temp->T_next==NULL)<<endl;
                if(temp->T_next==NULL)
                {
                    acc=false;
                }
                break;
            }
        }

        if(!acc)
        {

            temp->add_child(base_tag);


            find_tag=false;
        }

        switch(base_tag)
        {
            case 0:
            {
                temp=(Dynamic_Tree*)(temp->A_next);
            break;
            }
            case 1:{temp=(Dynamic_Tree*)(temp->C_next);
            break;}
            case 2:{temp=(Dynamic_Tree*)(temp->G_next);
            break;}
            case 3:{temp=(Dynamic_Tree*)(temp->T_next);
            break;}
        }
        

    }
   //cout<<"finish 149 base"<<endl;
    int base_tag;
    Dtree_leaf *node;
    //cout<<kmer.back()<<endl;
    switch (kmer.back())
    {
        case 'A' : 
        {
            base_tag=0;
            if(temp->A_next==NULL)
            {
                find_tag=false;
            }
            else
            {
                node=(Dtree_leaf*)temp->A_next;
            }
            break;

        }
        case 'C' :
        {
            base_tag=1;
            //cout<<(temp->C_next==NULL)<<endl;
            if(temp->C_next==NULL)
            {
                find_tag=false;
            }
            else
            {
                node=(Dtree_leaf*)temp->C_next;
            }
            break;
        }
        case 'G':
        {
            base_tag=2;
            if(temp->G_next==NULL)
            {
                find_tag=false;
            }
            else
            {
                node=(Dtree_leaf*)temp->G_next;
            }
            break;
        }
        case 'T':
        {
            base_tag=3;
            if(temp->T_next==NULL)
            {
                find_tag=false;
            }
            else
            {
                node=(Dtree_leaf*)temp->T_next;
            }
            break;
        }
    }
    //cout<<find_tag<<endl;
    if(find_tag)
    {
        return node->id;
    }
    else
    {
        temp->add_leaf(base_tag,exptected_id);
        return exptected_id;
    }

}
vector <ull> Dtree_successor_accept(Dynamic_Tree*root,string kmer)
{
    vector <ull> successor;
    Dynamic_Tree*temp=root;
    bool find=true;
    for(int i=1;i<kmer.size();i++)
    {
        switch (kmer[i])
        {
            case 'A':
            {
                if(temp->A_next!=NULL)
                    temp=(Dynamic_Tree*)temp->A_next;
                else
                    find=false;
                break;
            }
            case 'C':
            {
                if(temp->C_next!=NULL)
                    temp=(Dynamic_Tree*)temp->C_next;
                else
                    find=false;
                break;
            }
            case 'G':
            {
                if(temp->G_next!=NULL)
                    temp=(Dynamic_Tree*)temp->G_next;
                else
                    find=false;
                break;
            }
            case 'T':
            {
                if(temp->T_next!=NULL)
                    temp=(Dynamic_Tree*)temp->T_next;
                else
                    find=false;
                break;
            }
            if(!find)
                break;        
        }

    }
    if(!find)
        return successor;
    else
    {
        if(temp->A_next!=NULL)
        {
            successor.push_back(((Dtree_leaf*)(temp->A_next))->id);
        }
        if(temp->C_next!=NULL)
        {
            successor.push_back(((Dtree_leaf*)(temp->C_next))->id);
        }
        if(temp->G_next!=NULL)
        {
            successor.push_back(((Dtree_leaf*)(temp->G_next))->id);
        }
        if(temp->T_next!=NULL)
        {
            successor.push_back(((Dtree_leaf*)(temp->T_next))->id);
        }
        return successor;
    }
}
void init_debrujin_Dtree(Subgraph sub,vector <Reads> reads,vector <Debrujin_node>&dbg,Parameter*para)
{
    Dynamic_Tree pp;
    Dynamic_Tree *root_node=&pp;
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
                bool repeat_tag=true;
                ull expected_dbg_node_id=dbg.size();
                ull return_id=Dtree_accept(tmp.k_mer,root_node,expected_dbg_node_id);
                //cout<<"accept done"<<endl;
                //cout<<return_id<<endl;
                if(return_id==expected_dbg_node_id)
                    repeat_tag=false;
                //cout<<repeat_tag<<endl;
                if(j==0 && !repeat_tag)
                {
                    tmp.read_st=true;                 
                    dbg.push_back(tmp);
                }
                else if(!repeat_tag)
                {
                    
                    //cout<<tmp.k_mer<<endl;
                    dbg.push_back(tmp);
                }
                else
                {
                    ull id=return_id;
                    dbg[id].coverage_update(node,next_node,overlap_length,j,para->rLengthNormal,reads[node].read_count,reads[next_node].read_count);
                    
                   
                }
                //cout<<"j="<<j<< "done"<<endl;
            }
            

        }
        cout<<dbg.size()<<endl;
    }
   cout<<dbg.size()<<endl;
    // for(int i=0;i<dbg.size();i++)
    // {
    //     dbg[i].ReadSet.clear();
    // }
    for(ull i=0;i<dbg.size();i++)
    {
        vector <ull> successor=Dtree_successor_accept(root_node,dbg[i].k_mer);
        dbg[i].successor=successor;
        for(int j=0;j<successor.size();j++)
        {
            dbg[successor[j]].predecessor.push_back(i);
        }
    }
}
#endif


