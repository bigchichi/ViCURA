
#include "dbg.h"
void Dynamic_Tree :: add_child(int tag)
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
void Dynamic_Tree ::  add_leaf(int tag,ull ex_id)
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

ull Dtree_accept(string kmer,Dynamic_Tree*root,ull exptected_id)
{
    Dynamic_Tree*temp=root;
    bool find_tag=true;
    int max_depth=kmer.size()+1;
    int depth=0;
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
       // cout<<kmer[i]<<endl;
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
        //if find,return the exist node id
        return node->id;
    }
    else
    {
        //if not find, establish a new path and return expected id
        temp->add_leaf(base_tag,exptected_id);
        return exptected_id;
    }

}
vector <ull> Dtree_successor_accept(Dynamic_Tree*root,string kmer,int length)
{
    vector <ull> successor;
    vector <int> position(kmer.size());
    Dynamic_Tree*temp=root;
    bool find=true;
    for(int i=kmer.size()-length;i<kmer.size();i++)
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
        }
        //cout<<kmer[i]<<endl;

    }
    vector <Dynamic_Tree*> stack;
    stack.push_back(temp);
    if(!find)
    {
        return successor;
    }
    else
    {
        //cout<<"find"<<endl;
        while (stack.size()!=0)
        {
            Dynamic_Tree*back=stack.back();
            Dynamic_Tree*next;
            int tag=stack.size()-1;
            if(tag+1==kmer.size()-length)
            {
                if(back->A_next!=NULL)
                {
                    successor.push_back(((Dtree_leaf*)(back->A_next))->id);
                }
                if(back->C_next!=NULL)
                {
                    successor.push_back(((Dtree_leaf*)(back->C_next))->id);
                }
                if(back->G_next!=NULL)
                {
                    successor.push_back(((Dtree_leaf*)(back->G_next))->id);
                }
                if(back->T_next!=NULL)
                {
                    successor.push_back(((Dtree_leaf*)(back->T_next))->id);
                }
                stack.pop_back();
                continue;
            }
            bool exist_suc=false;
            for(int i=position[tag];i<4;i++)
            {
                if(i==0)
                {
                    if(back->A_next!=NULL)
                    {
                        next=(Dynamic_Tree*)back->A_next;
                        exist_suc=true;
                        position[tag]=i+1;
                        break;
                    }

                }
                else if(i==1)
                {
                    if(back->C_next!=NULL)
                    {
                        next=(Dynamic_Tree*)back->C_next;
                        exist_suc=true;
                        position[tag]=i+1;
                        break;
                    }
                }
                else if(i==2)
                {
                    if(back->G_next!=NULL)
                    {
                        next=(Dynamic_Tree*)back->G_next;
                         exist_suc=true;
                        position[tag]=i+1;
                        break;
                    }
                }
                else if(i==3)
                {
                    if(back->T_next!=NULL)
                    {
                        next=(Dynamic_Tree*)back->T_next;
                        exist_suc=true;
                        position[tag]=i+1;
                        break;
                    }
                }
            }
            if(exist_suc)
            {
                stack.push_back(next);
            }
            else
            {
                position[tag]=0;
                stack.pop_back();
            }
        }
        return successor;
    }
}
// void Dtree_borken(Dynamic_Tree*root,Parameter*para)
// {
//     Dynamic_Tree*temp=root;
//     vector <int> tree_register(para->rLengthNormal);
     
// }
void init_debrujin_Dtree(vector <Reads> reads,vector <Debrujin_node>&dbg,Parameter*para)
{
    Dynamic_Tree pp;
    Dynamic_Tree *root_node=&pp;
    //brand first get all edges
    vector <string> tmp_k_mer;
    int old_count=0;
    for(ull i=0;i<reads.size();i++)
    {
        cout<<"size "<<dbg.size()<<endl;
        cout<<"id "<<i<<endl;
        cout<<reads[i].successor.size()<<endl;
        ull node=i;
        for(ull k=0;k<reads[node].successor.size();k++)
        {
            ull next_node=reads[node].successor[k];
            tmp_k_mer.clear();
            cout<<node<<" "<<next_node<<endl;
            cout<<reads[node].sequence<<endl;
            cout<<reads[next_node].sequence<<endl;
            cout<<reads[node].successor_o[k]<<endl;
            cout<<k<<endl;
            tmp_k_mer=get_merged_sequence(reads[node],reads[next_node],reads[node].successor_o[k],para);
            int overlap_length=reads[node].successor_o[k];
            cout<<"tmp kmer "<<tmp_k_mer.size()<<endl;
            for (int j=0;j<tmp_k_mer.size();j++)
            {
                Debrujin_node tmp;
                tmp.k_mer=tmp_k_mer[j];
                tmp.init(para);
                tmp.coverage_update(node,next_node,overlap_length,j,para->rLengthNormal,reads[node].read_count,reads[next_node].read_count);
                bool repeat_tag=true;
                ull expected_dbg_node_id=dbg.size();
                ull return_id=Dtree_accept(tmp.k_mer,root_node,expected_dbg_node_id);
                cout<<"accept done"<<endl;
                cout<<return_id<<endl;
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
                cout<<"j="<<j<< "done"<<endl;
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
        vector <ull> successor=Dtree_successor_accept(root_node,dbg[i].k_mer,dbg[i].k_mer.size()-1);
        //dbg[i].successor=successor;
        for(int j=0;j<successor.size();j++)
        {
            if(i==successor[j])
                continue;
            dbg[i].successor.push_back(successor[j]);
            dbg[successor[j]].predecessor.push_back(i);
        }
    }
    //Dtree_borken(root_node,para);
}
void get_pairs_Dtree(vector <Reads> &reads,Parameter*para,int st_o,int ed_o,Dynamic_Tree*root)
{
    Pairs temp_pair;
    int fix_length=para->rLengthNormal-para->sequence_o;
    vector<unordered_set<ull>>suc_v(reads.size());
    //cout<<fix_length<<"!!!!"<<reads[0].perifix_array.size()<<endl;
    int i;
    ull j,k;
    Zipline temp;
    ull ID,usage;
    string start,overlap,end;
    int overlap_length;
    for(i=st_o;i<=ed_o;i++)
    {
        overlap_length=i;
        // cout<<"---"<<overlap_length<<endl;
        // cout<<para->pair_size<<endl;
        for(j=0;j<reads.size();j++)
        {
            vector <ull> suc=Dtree_successor_accept(root,reads[j].sequence,overlap_length);
            for(int p=0;p<suc.size();p++)
            {
                if(suc_v[j].insert(suc[p]).second==false)
                    continue;
                if(suc[p]==j)
                    continue;
                    ID=suc[p];
                    reads[j].successor_o.push_back(overlap_length);
                    reads[ID].predecessor_o.push_back(overlap_length);
                    // if(reads[j].length<para->rLengthNormal)
                    // {                                                                
                    //     reads[j].successor_o[reads[j].successor_o.size()-1]=overlap_length+para->rLengthNormal-reads[j].length;
                    //     reads[j].sequence=temp_pair.uov_sequence.substr(0,para->rLengthNormal);
                    //     reads[j].length=para->rLengthNormal;
                    //     reads[j].array_init(para);

                    // }
                    // if(reads[ID].length<para->rLengthNormal)
                    // {
                    //     reads[ID].predecessor_o[reads[ID].predecessor_o.size()-1]=overlap_length+para->rLengthNormal-reads[ID].length;
                    //     reads[ID].sequence=temp_pair.uov_sequence.substr(temp_pair.total_length-para->rLengthNormal,para->rLengthNormal);
                    //     reads[ID].length=para->rLengthNormal;
                    //     reads[ID].array_init(para);
                    // }
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
        //cout<<"---"<<overlap_length<<"--- done"<<endl;
        //cout<<para->pair_size<<endl;
        malloc_trim(0);

    }
}
void dtree_break(Dynamic_Tree *root,int depth)
{
    
    if(depth==150)
    {
         if(root->A_next!=NULL)
        {
            delete (Dtree_leaf*)root->A_next;
            root->A_next=NULL;
        }
        if(root->C_next!=NULL)
        {
 
            delete  (Dtree_leaf*)root->C_next;
            root->C_next=NULL;
       
        }
        if(root->G_next!=NULL)
        {
            
            delete  (Dtree_leaf*)root->G_next;
            root->G_next=NULL;
             
        }
        if(root->G_next!=NULL)
        {
            
            delete  (Dtree_leaf*)root->G_next;
             root->G_next=NULL;
            
        }
        return;
    }

    else
    {
        if(root->A_next!=NULL)
        {
            dtree_break((Dynamic_Tree*)root->A_next,depth+1);
            delete (Dynamic_Tree*)root->A_next;
            root->A_next=NULL;
        }
        if(root->C_next!=NULL)
        {
            dtree_break((Dynamic_Tree*)root->C_next,depth+1);
            delete (Dynamic_Tree*)root->C_next;
            root->C_next=NULL;
        }
        if(root->G_next!=NULL)
        {
            dtree_break((Dynamic_Tree*)root->G_next,depth+1);
            delete (Dynamic_Tree*)root->G_next;
            root->G_next=NULL;
        }
        if(root->G_next!=NULL)
        {
            dtree_break((Dynamic_Tree*)root->G_next,depth+1);
            delete (Dynamic_Tree*)root->G_next;
            root->G_next=NULL;
        }
    }
}



