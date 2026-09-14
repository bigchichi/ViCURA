#ifndef BUCKET_H
#define BUCKET_H
#include "dbg.h"

//#include<fast_io_legacy.h>
int get_bucket_id(string minimizer,Parameter*para)
{
    int bucket_id=0;
    for(int i=0;i<minimizer.size();i++)
    {
       bucket_id+=(para->base_value[minimizer[i]]-1)*((int)pow(4.0,(double)i));
    }
    return bucket_id;
}
void count_update(vector <int> &count,int length,int st,int read_count)
{
    for(int i=0;i<length;i++)
    {
        count[st+i]+=read_count;
    }
}
// int path_kmer(vector <ull> path,vector <int> path_o,vector <Reads> reads,ull path_st,vector <fast_io:: obuf_file> kmerout,int minimizer_length,Parameter* para)
// {

//     string sequence;
//     int length=para->rLengthNormal;
//     //cout<<"into "<<path_st<<" "<<path[0]<<" "<<path_ed<<" "<<path.back()<<endl;
//     vector <int> count(length*(path.size()+1));
//    // cout<<count.size()<<endl;
//     sequence=reads[path_st].sequence;
//     count_update(count,length,0,reads[path_st].read_count);
//     int cur_length=length;      
//     cout<<"into cycle "<<path.size()<<" "<<path_o.size()<<endl;   
//     for(int i=0;i<path.size();i++)
//     {
//         //cout<<i<<endl;
//         ull node=path[i];
//         int overlap=path_o[i];
//         //cout<<overlap<<endl;
//         sequence+=reads[node].sequence.substr(overlap);
//         //cout<<reads[node].read_count<<endl;
//         //cout<<"go0"<<endl;
//         count_update(count,length-overlap,cur_length,reads[node].read_count);
//        // cout<<"go"<<endl;
//         cur_length+=length-overlap;
//     }
//    // cout<<"init "<<sequence.length()<<" "<<length<<endl;
//     //cout<<sequence<<endl;
//    // sequence+=reads[path.back()].sequence.substr(path_o.back());
//     for(int i=0;i<sequence.length()-length;i++)
//     {
//         //cout<<i<<endl;
//         string kmer=sequence.substr(i,length);
//         //cout<<"sub get"<<endl;
//         string minimizer=kmer.substr(0,minimizer_length);
//         //cout<<"sub get"<<endl;
//         int bucket_id=get_bucket_id(minimizer,para);
//        // cout<<"waiting for output"<<endl;
//         //*kmerout[bucket_id]<<kmer<<endl;
//         print(kmerout[bucket_id],kmer+"\n");
//         for (int j=0;j<length;j++)
//         {
//             //*kmerout[bucket_id]<<count[i+j]<<" ";
//             print(kmerout[bucket_id],count[i+j]+" ");
//         }
//         //*kmerout[bucket_id]<<endl;
//         print(kmerout[bucket_id],"\n");

//     }
//     return sequence.length()-length;
// }
int pair_kmer(string s1,string s2,ull st,ull ed,int overlap,int ct1,int ct2,vector<ofstream*>kmerout,Parameter*para,int minimizer_length)
{
    string sequence;
    int my_rank;
    int length=para->kmerlength;
    vector<int>count(2*length-overlap+100);
    sequence=s1;
    sequence+=s2.substr(overlap);
    MPI_Comm_rank( MPI_COMM_WORLD , &my_rank);
   // cout<<"substr get done"<<endl;
    // count_update(count,length,0,ct1);
    // count_update(count,length,length-overlap,ct2);
   // cout<<"update done"<<endl;
    for(int i=0;i<sequence.length()-length+1;i++)
    {
        //cout<<i<<endl;
        string kmer=sequence.substr(i,length);
        //cout<<"sub get"<<endl;
        string minimizer=kmer.substr(0,minimizer_length);
        //cout<<"sub get"<<endl;
        int bucket_id=get_bucket_id(minimizer,para);
       //cout<<"waiting for output "<<bucket_id<<" "<<kmer<<endl;
       //cout<<kmerout.size()<<endl;
       /*file order: kmer,start node,end node,kmer position,overlap length*/
        *kmerout[bucket_id]<<kmer<<" "<<st<<" "<<ed<<" "<<i<<" "<<overlap<<" ";
        if(i==0)
            *kmerout[bucket_id]<<0<<endl;
        else if(i==sequence.length()-length)
            *kmerout[bucket_id]<<1<<endl;
        else
            *kmerout[bucket_id]<<2<<endl;
    }
    return sequence.length()-length;
}
void get_all_kmer(vector <Reads> reads,int minimizer_length,string temp_path,Parameter*para)
{

    int my_rank,numprocs;    
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank( MPI_COMM_WORLD , &my_rank);
    string path=temp_path+"/temp_"+to_string(my_rank);
    vector <Debrujin_node>dbg;
    Dynamic_Tree *root=new Dynamic_Tree;
    string rm_dir="rm -rf "+path;
    string mk_dir="mkdir "+path;
    system(rm_dir.c_str());
    system(mk_dir.c_str());
   // cout<<"start"<<endl;
    std :: vector <ofstream*>kmerout;
    int total_num=0;
    for(int i=0;i<minimizer_length;i++)
    {
        total_num+=pow(4.0,i)*3.0;
    }
    for(int i=0;i<=total_num;i++)
    {
       // cout<<i<<endl;
        string path=temp_path+"/temp_"+to_string(my_rank)+"/kmer150_"+to_string(i);
        ofstream*tmp=new ofstream(path,ios::app);
        kmerout.push_back(tmp);
    }
   // cout<<total_num<<endl;
    // vector <ull> root;
    // for(int i=0;i<reads.size();i++)
    // {
    //     if(reads[i].successor.size()>0 && reads[i].predecessor.size()==0)
    //     {
    //         root.push_back(i);
    //         //cout<<i<<endl;   
    //     }  

    // }
    // cout<<root.size()<<endl;
    // vector <int> position(reads.size());
    // vector <int> reads_reach_mark(reads.size());
    ull kmer_num=0;
    int single_num=0;
    // for(int i=0;i<root.size();i++)cd
    // {
    //     ull start=root[i];
    //     vector <ull> stack;
    //     vector <int> stack_o;
    //     vector <int> reads_usage(reads.size());

    //     stack.push_back(start);
    //     //cout<<"start root "<<i<<" "<<root[i]<<endl;
    //     reads_reach_mark[start]=1;
    //     while (stack.size()!=0)
    //     {
    //         vector <int> in_stack(reads.size());
    //         vector <ull> stack_copy;
    //         for(int j=0;j<stack.size();j++)
    //         {
    //             ull cur=stack[j];
    //             reads_reach_mark[cur]=1;
    //             stack_copy.clear();
    //             int cur_position=position[cur];
    //             //cout<<"span "<<cur<<endl;
    //             for(int k=0;k<reads[cur].successor.size();k++)
    //             {
    //                 ull next_node=reads[cur].successor[k];
    //                 if(reads_reach_mark[next_node] || in_stack[next_node])
    //                 {
    //                     single_num+=reads[cur].successor_o[k];
    //                     continue;
    //                 }
                    
    //                 if(k>=position[cur])
    //                 {
    //                     //cout<<position[cur]<<" "<<reads[cur].successor.size()<<endl;
    //                     position[cur]++;
    //                     vector <ull> path;
    //                     vector <int> path_o;
    //                     path.push_back(next_node);
    //                     path_o.push_back(reads[cur].successor_o[k]);
    //                     while (true)
    //                     {
    //                         ull back=path.back();
    //                         if(position[back]<reads[back].successor.size())
    //                         {
    //                             ull next=reads[back].successor[position[back]];                                
    //                             path_o.push_back(reads[back].successor_o[position[back]]);
    //                             path.push_back(next);
    //                             position[back]++;

    //                         }
    //                         else
    //                             break;
    //                     }
    //                     kmer_num+=path_kmer(path,path_o,reads,cur,kmerout,minimizer_length,para);
    //                     //cout<<reads[path.back()].successor.size()<<" "<<position[path.back()]<<endl;                        
    //                 }

    //                 stack_copy.push_back(next_node);
    //                 in_stack[next_node]=1;
    //             }
    //             position[cur]=reads[cur].successor.size();

    //         }
    //         stack=stack_copy;
    //         cout<<"done  "<<endl;
    //     }
    // }
    // cout<<single_num<<" single"<<endl;
    // cout<<"kmer num "<<kmer_num<<endl;
    // for(int i=0;i<reads.size();i++)
    // {
    //     if(reads[i].successor.size() != position[i])
    //         cout<<"error1 "<<i<<" "<<reads[i].successor.size()<<" "<<position[i]<<endl;
    //     if(reads_reach_mark[i]==0)
    //         cout<<"error2 "<<i<<endl;
    // }
    int read_count=0;
    string process_log=temp_path+"/log_n_"+to_string(my_rank);
    ofstream logout(process_log,ios::app);
     for(int i=0;i<reads.size();i++)
    {
      // cout<<reads[i].successor.size()<<" "<<reads[i].successor_o.size()<<endl;
        if(i%numprocs==my_rank)
        {
            read_count++;
            //cout<<i<<" node start"<<reads[i].successor.size()<<" "<<reads[i].successor_o.size()<<endl;
            for(int j=0;j<reads[i].successor.size();j++)
            {
                ull next_node=reads[i].successor[j];
                //cout<<next_node<<endl;
                //vector <string> tmp_k_mer;
                //cout<<"into"<<endl;
            // cout<<reads[i].successor.size()<<" "<<next_node<<" "<<j<<endl;
                //tmp_k_mer=get_merged_sequence(reads[i],reads[next_node],reads[i].successor_o[j],para);
                //cout<<tmp_k_mer.size()<<" "<<reads[i].successor_o[j]<<endl;
                //cout<<i<<" "<<j<<endl;
                //cout<<reads[i].successor_o.size()<<endl;
                int overlap_length=reads[i].successor_o[j];
                //cout<<overlap_length<<endl;
                //cout<<overlap_length<<endl;
                //cout<<reads[i].sequence<<endl;
                //cout<<reads[next_node].sequence<<endl;
                kmer_num+=pair_kmer(reads[i].sequence,reads[next_node].sequence,i,next_node,overlap_length,reads[i].read_count,reads[next_node].read_count,kmerout,para,minimizer_length);
            // cout<<j<<endl;
            }
        }
       // cout<<i<<endl;
    }
    cout<<"doing "<<read_count<<endl;
    cout<<kmer_num<<endl;
    
}
void bucket_remove_repeated_kmer(int bucket_id,string filepath,Parameter*para,int minimizer_length,vector <Reads> reads)
{

    int procsnum;
    int my_rank;
    MPI_Comm_rank( MPI_COMM_WORLD , &my_rank);
    MPI_Comm_size( MPI_COMM_WORLD , &procsnum);
    string judge_path=filepath+"/unqtemp/bucket_num_"+to_string(bucket_id);
    string k150_file=filepath+"/unqtemp/kmer150_unique_"+to_string(bucket_id);
    if(access(judge_path.c_str(),0)==0 && access(k150_file.c_str(),0)==0)
    {
        cout<<"find "<<my_rank<<" "<<bucket_id<<endl;
        return;
    }
    vector <Debrujin_node> dbg_tmp;
    Dynamic_Tree *root=new Dynamic_Tree;

    remove(k150_file.c_str());
    for(int n=0;n<procsnum;n++)
    {
        ifstream kmerin; 
        kmerin.open(filepath+"/temp/temp_"+to_string(n)+"/kmer150_"+to_string(bucket_id));  
        string line,reg;
        int tag=0;


        while(true)
        {  
            if(!std::getline(kmerin,line))
                break;
            stringstream ss(line);
            string kmer;
            ull front,back;
            int overlap_length,st,front_count,back_count;
            vector <int> coverage;
            int count=0;
            ull first_id_copy=dbg_tmp.size();
            while (std::getline(ss,reg,' '))
            {
                if(count==0)
                {
                    kmer=reg;
                }
                else if(count==1)
                {
                    stringstream tmp(reg);
                    tmp>>front;

                }
                else if(count==2)
                {
                    stringstream tmp(reg);
                    tmp>>back;
                }
                else if(count==3)
                {
                    stringstream tmp(reg);
                    tmp>>st;
                }
                else if(count==4)
                {
                    stringstream tmp(reg);
                    tmp>>overlap_length;
                }
                count++;
            }
            ull expected_id=first_id_copy;
            ull return_id=Dtree_accept(kmer,root,expected_id);
            Debrujin_node tmp;
            tmp.k_mer=kmer;
            if(return_id==expected_id)
            {
                tmp.coverage_update(front,back,overlap_length,st,para->kmerlength,reads[front].read_count,reads[back].read_count);
                if(reads[front].reserve_tag || reads[back].reserve_tag)
                    tmp.reserve_tag=1;
                dbg_tmp.push_back(tmp);
            }
            else
            {
                dbg_tmp[return_id].coverage_update(front,back,overlap_length,st,para->kmerlength,reads[front].read_count,reads[back].read_count);
                if(reads[front].reserve_tag || reads[back].reserve_tag)
                     dbg_tmp[return_id].reserve_tag=1;
            }       
        }
        // string del_cmd="rm "+filepath+"/temp_"+to_string(n)+"/kmer150_"+to_string(bucket_id);
        // system(del_cmd.c_str());
    }
        string unique_filepath=filepath+"/unqtemp";

        string unique_kemr_file=unique_filepath+"/kmer150_unique_"+to_string(bucket_id);
        string unique_kmer_readsinfo_file=filepath+"/kmer150_unique_read_"+to_string(bucket_id);
        ofstream kmerout(unique_kemr_file,ios::app);
        ofstream readout(unique_kmer_readsinfo_file);

        int count=0;
        for(int i=0;i<dbg_tmp.size();i++)
        {
            // int single=std :: count(dbg_tmp[i].coverage.begin(),dbg_tmp[i].coverage.end(),1);
            // if(single==dbg_tmp[i].coverage.size())
            //     continue;
            // if(dbg_tmp[i].coverage.back()==1)
            //     continue;
            bool ct=false;
            for(int j=0;j<dbg_tmp[i].coverage.size();j++)
            {
                if(dbg_tmp[i].coverage[j]<=1)
                    ct=true;
            }
            if(ct && dbg_tmp[i].reserve_tag==0)
                continue;
            string kmer=dbg_tmp[i].k_mer;
            kmerout<<kmer<<" ";
            kmerout<<count<<" ";
            kmerout<<dbg_tmp[i].cov<<endl;
            for(auto it=dbg_tmp[i].ReadSet_st.begin();it!=dbg_tmp[i].ReadSet_st.end();it++)
            {
                readout<<*it<<" ";
            }
            readout<<endl;
            for(auto it=dbg_tmp[i].ReadSet_ed.begin();it!=dbg_tmp[i].ReadSet_ed.end();it++)
            {
                readout<<*it<<" ";
            }
            readout<<endl;
            count++;
            
        }
        string bucket_num_path=unique_filepath+"/bucket_num_"+to_string(bucket_id);
        remove(bucket_num_path.c_str());
        ofstream bucket_num_out(bucket_num_path);
        bucket_num_out<<count<<endl;
        //dtree_break(root,0);
      
     
}
void bucket_remove_repeated_kmer_all(int bucket_id,string filepath,Parameter*para,int minimizer_length,vector <Reads> reads)
{

    int procsnum;
    int my_rank;
    MPI_Comm_rank( MPI_COMM_WORLD , &my_rank);
    MPI_Comm_size( MPI_COMM_WORLD , &procsnum);
    string judge_path=filepath+"/unqtemp/bucket_num_"+to_string(bucket_id);
    string k150_file=filepath+"/unqtemp/kmer150_unique_"+to_string(bucket_id);
    string process_log=filepath+"/log_n_"+to_string(my_rank);
    ofstream logout(process_log,ios::app);
    if(access(judge_path.c_str(),0)==0 && access(k150_file.c_str(),0)==0)
    {
        cout<<"find "<<my_rank<<" "<<bucket_id<<endl;
        return;
    }
    vector <Debrujin_node> dbg_tmp;
    Dynamic_Tree *root=new Dynamic_Tree;

    remove(k150_file.c_str());
    logout<<"start reading "<<bucket_id<<endl;
    for(int n=0;n<procsnum;n++)
    {
        ifstream kmerin; 
        kmerin.open(filepath+"/temp/temp_"+to_string(n)+"/kmer150_"+to_string(bucket_id));  
        string line,reg;
        int tag=0;
        logout<<"deal rank "<<n<<endl;

        while(true)
        {  
            if(!std::getline(kmerin,line))
                break;
            stringstream ss(line);
            string kmer;
            ull front,back;
            int overlap_length,st,front_count,back_count,read_id;
            vector <int> coverage;
            int count=0;
            ull first_id_copy=dbg_tmp.size();
            //logout<<"read 1 line"<<endl;
            while (std::getline(ss,reg,' '))
            {
                if(count==0)
                {
                    kmer=reg;
                }
                else if(count==1)
                {
                    stringstream tmp(reg);
                    tmp>>front;

                }
                else if(count==2)
                {
                    stringstream tmp(reg);
                    tmp>>back;
                }
                else if(count==3)
                {
                    stringstream tmp(reg);
                    tmp>>st;
                }
                else if(count==4)
                {
                    stringstream tmp(reg);
                    tmp>>overlap_length;
                }
                else if(count==5)
                {
                    stringstream tmp(reg);
                    tmp>>read_id;
                    if(read_id==0)
                        read_id=front;
                    else if(read_id==1)
                        read_id=back;
                    else if(read_id==2)
                        read_id=-1;
                }
                count++;
            }
            ull expected_id=first_id_copy;
            ull return_id=Dtree_accept(kmer,root,expected_id);
            Debrujin_node tmp;
            tmp.k_mer=kmer;
            tmp.read_id=read_id;
            //logout<<"deal done"<<endl;
            if(return_id==expected_id)
            {
                tmp.coverage_update(front,back,overlap_length,st,para->kmerlength,reads[front].read_count,reads[back].read_count);
                // if(reads[front].reserve_tag || reads[back].reserve_tag)
                //     tmp.reserve_tag=1;
                dbg_tmp.push_back(tmp);
            }
            else
            {
                dbg_tmp[return_id].coverage_update(front,back,overlap_length,st,para->kmerlength,reads[front].read_count,reads[back].read_count);
                if(dbg_tmp[return_id].read_id==-1)
                    dbg_tmp[return_id].read_id=read_id;
                // if(reads[front].reserve_tag || reads[back].reserve_tag)
                //      dbg_tmp[return_id].reserve_tag=1;
            }       
        }
        // string del_cmd="rm "+filepath+"/temp_"+to_string(n)+"/kmer150_"+to_string(bucket_id);
        // system(del_cmd.c_str());
    }
       // logout<<"read temp info done"<<endl;
        string unique_filepath=filepath+"/unqtemp";

        string unique_kemr_file=unique_filepath+"/kmer150_unique_"+to_string(bucket_id);
        string unique_kmer_readsinfo_file=filepath+"/info/kmer150_unique_read_"+to_string(bucket_id);
        string testfile=filepath+"/test/test_"+to_string(bucket_id);
        ofstream kmerout(unique_kemr_file,ios::app);
        ofstream readout(unique_kmer_readsinfo_file);
        ofstream testout(testfile);
       int count=0;
       int read_num=0;
        for(int i=0;i<dbg_tmp.size();i++)
        {
            // int single=std :: count(dbg_tmp[i].coverage.begin(),dbg_tmp[i].coverage.end(),1);
            // if(single==dbg_tmp[i].coverage.size())
            //     continue;
            // if(dbg_tmp[i].coverage.back()==1)
            //     continue;
            // bool ct=false;
            // for(int j=0;j<dbg_tmp[i].coverage.size();j++)
            // {
            //     if(dbg_tmp[i].coverage[j]<=1)
            //         ct=true;
            // }
            // if(ct && dbg_tmp[i].reserve_tag==0)
            //     continue;
            string kmer=dbg_tmp[i].k_mer;
            kmerout<<kmer<<" ";
            kmerout<<count<<" ";
            kmerout<<dbg_tmp[i].read_id<<endl;
            for(int j=0;j<para->kmerlength;j++)
            {
                kmerout<<dbg_tmp[i].coverage[j]<<" ";
            }
            kmerout<<endl;
            if(dbg_tmp[i].read_id!=-1)
                read_num++;
           // testout<<kmer<<endl;
            // if(dbg_tmp[i].read_id!=-1)
            // {
            //     logout<<dbg_tmp[i].k_mer<<" "<<dbg_tmp[i].k_mer<<endl;
            // }
            // for(int j=0;j<dbg_tmp[i].pair_vec.size();j++)
            // {
            //     readout<<dbg_tmp[i].pair_vec[j].first<<" "<<dbg_tmp[i].pair_vec[j].second<<" ";
            // }
            // readout<<endl;
            count++;
            
        }
        string bucket_num_path=unique_filepath+"/bucket_num_"+to_string(bucket_id);
        remove(bucket_num_path.c_str());
        ofstream bucket_num_out(bucket_num_path);
        bucket_num_out<<count<<endl;
        logout<<"read num "<<read_num<<endl;
        //logout<<dbg_tmp.size()<<endl;
        //dtree_break(root,0);
      
     
}

void bucket_dbg_init(int bucket_id,int minimizer_length,string filepath,int* bucket_kmer_num,Parameter*para)
{
    vector <Debrujin_node> dbg;
    Dynamic_Tree *root=new Dynamic_Tree;
    int comm_sz,my_rank;
    MPI_Comm_size( MPI_COMM_WORLD , &comm_sz);
    MPI_Comm_rank( MPI_COMM_WORLD , &my_rank);
    vector <string> value{"A","C","G","T"};
    vector <int> bucket_kmer;
    bucket_kmer.push_back(0);
    vector <int> bucket_id_v;
    //vector <ofstream*>preout;
    string line;
    int exist_count=0;
    //cout<<"deal "<<bucket_id<<endl;
    for(int i=0;i<4;i++)
    {
        int b=bucket_id+(para->base_value[value[i][0]]-1)*((int)pow(4.0,(double)(minimizer_length-1)));
        string prefile=filepath+"/dbgtmp/preinfo_"+to_string(b);
        string successfile=filepath+"/dbgtmp/success_"+to_string(b);
        if(access(prefile.c_str(),0)==0 && access(successfile.c_str(),0)==0)
            exist_count++;
    }
    if(exist_count==4)
        return;
    for(int i=0;i<4;i++)
    {
        int this_bucket=bucket_id+(para->base_value[value[i][0]]-1)*((int)pow(4.0,(double)(minimizer_length-1)));
        //cout<<bucket_id<<" "<<this_bucket<<endl;
        ifstream kmerin;
        string kmer_path=filepath+"/unqtemp/kmer150_unique_"+to_string(this_bucket);
        kmerin.open(kmer_path);
        //getline(kmerin,line);
        string bucket_num_file=filepath+"/unqtemp/bucket_num_"+to_string(this_bucket);
        string readinfo_file=filepath+"/kmer150_unique_read_"+to_string(this_bucket);
        ifstream bnumout(bucket_num_file);
        ifstream readin(readinfo_file);
        getline(bnumout,line);
        int num;
        stringstream s(line);
        s>>num;
        int t=bucket_kmer.back();
        bucket_kmer.push_back(t+num);
        bucket_id_v.push_back(this_bucket);
        while (true)
        {
            if(! std:: getline(kmerin,line))
                break;
            stringstream ss(line);
            string reg,kmer;
            Debrujin_node temp;
            int id,cov;
            int count=0;
            while (std::getline(ss,reg,' '))
            {
                if(count==0)
                {
                    kmer=reg;
                }
                else if(count==1)
                {
                    stringstream tmp(reg);
                    tmp>>id;
                }
                count++;
            }
            temp.k_mer=kmer;
            temp.cov=cov;
            ull exp_id=dbg.size();
            ull r_id=Dtree_accept(kmer,root,exp_id);
            string read_line;
            // getline(readin,read_line);
            // stringstream readss(read_line);
            // while (getline(readss,reg,' '))
            // {
            //     stringstream tmp(reg);
            //     ull read_id;
            //     tmp>>read_id;
            //     temp.ReadSet_st.insert(read_id);
            // }
            // getline(readin,read_line);
            // stringstream readsss(read_line);
            // while (getline(readsss,reg,' '))
            // {
            //     stringstream tmp(reg);
            //     ull read_id;
            //     tmp>>read_id;
            //     temp.ReadSet_ed.insert(read_id);
            // }
            
            

            //tmp.id=id+bucket_kmer_num[this_bucket];
            dbg.push_back(temp);
            getline(kmerin,line);
        }
        kmerin.close();
        //cout<<dbg.size()<<" rank "<<my_rank<<endl;
    }
    

    for(int i=0;i<4;i++)
    {
        string suc_minimizer=value[i];
        suc_minimizer+=dbg[0].k_mer.substr(0,minimizer_length-1);
        //cout<<suc_minimizer<<" "<<dbg[0].k_mer.substr(0,minimizer_length)<<endl;
        int suc_bucketnum=get_bucket_id(suc_minimizer,para);
        //cout<<suc_bucketnum<<endl;
        ifstream sucin(filepath+"/unqtemp/kmer150_unique_"+to_string(suc_bucketnum));
        string readinfo_file=filepath+"/kmer150_unique_read_"+to_string(suc_bucketnum);
        ifstream readin(readinfo_file);
        //cout<<"start read"<<endl;
        //getline(sucin,line);
        int ct=0;
        while (true)
        {
            if(! std:: getline(sucin,line))
                break;
            stringstream ss(line);
            string reg,kmer;
            Debrujin_node tmp;
            unordered_set <ull> set_st,set_ed;
            ct++;
            int id,cov;
            int count=0;
            while (std::getline(ss,reg,' '))
            {
                if(count==0)
                {
                    kmer=reg;
                }
                else if(count==1)
                {
                    stringstream tmp(reg);
                    tmp>>id;
                }
                count++;
            }
           id+=bucket_kmer_num[suc_bucketnum];
           vector <ull> suc= Dtree_successor_accept(root,kmer,kmer.length()-1);
             string read_line;
            // getline(readin,read_line);
            // stringstream readss(read_line);
            // while (getline(readss,reg,' '))
            // {
            //     stringstream tmp(reg);
            //     ull read_id;
            //     tmp>>read_id;
            //     set_st.insert(read_id);
            // }
            // getline(readin,read_line);
            // stringstream readsss(read_line);
            // while (getline(readsss,reg,' '))
            // {
            //     stringstream tmp(reg);
            //     ull read_id;
            //     tmp>>read_id;
            //     set_ed.insert(read_id);
            // }
           for(int j=0;j<suc.size();j++)
           {
                 ull front=id;
                 ull back=suc[j];
                // bool st_judge=0,ed_judge=0;
                // for(auto it=set_st.begin();it!=set_st.end();it++)
                // {
                //     if(dbg[back].ReadSet_st.count(*it)==1)
                //     {
                //         st_judge=1;
                //         break;
                //     }
                // }
                // for(auto it=set_ed.begin();it!=set_ed.end();it++)
                // {
                //     if(dbg[back].ReadSet_ed.count(*it)==1)
                //     {
                //         ed_judge=1;
                //         break;
                //     }
                // }
                // if(st_judge && ed_judge)
                    dbg[back].predecessor.push_back(front);
           }
           getline(sucin,line);
        }
        //cout<<ct<<endl;

    }

   
    for(int i=0;i<bucket_id_v.size();i++)
    {
        string pre_path=filepath+"/dbgtmp/preinfo_"+to_string(bucket_id_v[i]);
        remove(pre_path.c_str());
        ofstream preout(pre_path);
        
        for(int j=bucket_kmer[i];j<bucket_kmer[i+1];j++)
        {
            //*preout[i]<<dbg[j].predecessor.size()<<" ";
            for(int k=0;k<dbg[j].predecessor.size();k++)
            {
                preout<<dbg[j].predecessor[k]<<" ";
            }
            preout<<endl;
        }
        string success_tag=filepath+"/dbgtmp/success_"+to_string(bucket_id_v[i]);
        remove(success_tag.c_str());
        ofstream success(success_tag);
        success<<"success"<<endl;
    }
    //dtree_break(root,0);
}
// void get_unqkmer_onepass(vector <Reads> reads,Parameter* para,int bucket_id,string path,int minimizer_length)
// {
//     Dynamic_Tree* root=new Dynamic_Tree;
//     ofstream kmerout(path+"/unqtemp/unqkmer150_"+to_string(bucket_id),ios::app);
//     vector <Debrujin_node> dbg;
//     for(int i=0;i<reads.size();i++)
//     {
//         cout<<"start "<<i<<endl;
//         for(int j=0;j<reads[i].successor.size();j++)
//         {
//             ull next=reads[i].successor[j];
//             string s1=reads[i].sequence;
//             int overlap=reads[i].successor_o[j];
//             string s2=reads[next].sequence;
//             string s=s1;
//             s+=s2.substr(overlap);
//             // vector <int> count(2*para->rLengthNormal-overlap);
//             // count_update(count,para->rLengthNormal,0,reads[i].read_count);
//             // count_update(count,para->rLengthNormal,para->rLengthNormal-overlap,reads[next].read_count);
//             //cout<<"init done "<<endl;
//             for(int k=0;k<s.length()-para->rLengthNormal;k++)
//             {
//                 ull id=dbg.size();
//                 string kmer=s.substr(k,para->rLengthNormal);
//                 string minimizer=kmer.substr(0,minimizer_length);
//                 int minimizer_id=get_bucket_id(minimizer,para);
//                 if(minimizer_id!=bucket_id)
//                     continue;
//                 ull exp_id=Dtree_accept(kmer,root,id);
//                 //cout<<exp_id<<endl;

//                 if(exp_id==id)
//                 {
//                     Debrujin_node tmp;
//                     tmp.k_mer=kmer;
//                     tmp.coverage_update(i,next,overlap,k,para->rLengthNormal,reads[i].read_count,reads[next].read_count);
//                     dbg.push_back(tmp);
//                 }
//                 else
//                 {
//                     dbg[exp_id].coverage_update(i,next,overlap,k,para->rLengthNormal,reads[i].read_count,reads[next].read_count);
//                 }
//             }

//         }
//     }
//     kmerout<<dbg.size()<<endl;
//     for(int i=0;i<dbg.size();i++)
//     {
//         string kmer=dbg[i].k_mer;
//         kmerout<<kmer<<" ";
//         kmerout<<i<<endl;
//         for(int j=0;j<dbg[i].coverage.size();j++)
//         {
//             kmerout<<dbg[i].coverage[j]<<" ";
//         }
//         kmerout<<endl;
//     }

// }
void seq_align_SDBG(int tag,int minimizer_size,int bucket_num,vector <int> bucket_kmer_num,string work_space,string seq,Parameter*para)
{
    int kmer_l=para->kmerlength;
    vector <string> seq_kmer;
    vector <int> pos;
    //sequence parse
    for(int i=0;i<seq.size()-kmer_l;i++)
    {
        string kmer=seq.substr(i,kmer_l);
        string minimizer=kmer.substr(0,minimizer_size);
        int bucket_id=get_bucket_id(minimizer,para);
        if(bucket_id!=tag)
            continue;
        seq_kmer.push_back(kmer);
        pos.push_back(i);
    }
    cout<<"seq parse done"<<endl;
    string dbg_filepath="/public/home/qinchenhui/data/OLC_ref/unqtemp/kmer150_unique_"+to_string(tag);
    ifstream dbgin(dbg_filepath);
    string line,reg;
    Dynamic_Tree *root=new Dynamic_Tree;
    vector <sdbg> dbg;
    cout<<"start kmer reading"<<endl;
    while(true)
    {
        bool tg1= static_cast<bool>(getline(dbgin,line));
        if(!tg1)
            break;
        stringstream s1(line);
        vector <ull> pre;
        sdbg temp;
        string kmer;
        int id,cov;
        int count=0;
        while(getline(s1,reg,' '))
        {
            if(count==0)
            {
                kmer=reg;
            }
            else if(count==1)
            {
                stringstream tmp(reg);
                tmp>>id;
            }
            else if(count==2)
            {
                stringstream tmp(reg);
                tmp>>cov;
            }
            count++;   
        }
      //  cout<<kmer<<endl;
        ull rt_id=Dtree_accept(kmer,root,dbg.size());
       // cout<<"accept done"<<endl;
        temp.kmer=kmer;
        dbg.push_back(temp);
    }
    int dbg_size=dbg.size();
    string kmeroutpath=work_space+"/kmerinfo_"+to_string(tag);
    ofstream kmerout(kmeroutpath);
    for(int i=0;i<seq_kmer.size();i++)
    {
        ull rt_id=Dtree_accept(seq_kmer[i],root,dbg_size+1);
        if(rt_id>dbg_size)
        {
            kmerout<<"error kmer"<<endl;
        }
        else
        {
            kmerout<<pos[i]<<" "<<dbg[rt_id].kmer<<" "<<rt_id<<endl;
        }
    }


}
void kmer_align(string kmerinfo_path,vector<string> kmer_v,int minimizer_size,int bucket_num,vector <int> bucket_kmer_num,vector<ull> &seq_id,Parameter*para)
{
    int kmer_l=para->kmerlength;
    vector <string> seq_kmer;
    unordered_map <int,vector<string>> minimizerMap;
    //sequence parse
    for(int i=0;i<kmer_v.size();i++)
    {
        string minimizer=kmer_v[i].substr(0,minimizer_size);
        int bucket_id=get_bucket_id(minimizer,para);
        if(minimizerMap.count(bucket_id)==1)
        {
            minimizerMap[bucket_id].push_back(kmer_v[i]);
        }
        else
        {
            vector <string> temp;
            temp.push_back(kmer_v[i]);
            minimizerMap[bucket_id]=temp;
        }
    }
    cout<<"seq parse done"<<endl;
    for(auto& it:minimizerMap)
    {
        int tag=it.first;
        vector <string> v=it.second;
        string dbg_filepath=kmerinfo_path+"/kmer150_unique_"+to_string(tag);
        ifstream dbgin(dbg_filepath);
        string line,reg;
        Dynamic_Tree *root=new Dynamic_Tree;
        vector <sdbg> dbg;
        cout<<"start kmer reading"<<endl;
        while(true)
        {
            bool tg1= static_cast<bool>(getline(dbgin,line));
            if(!tg1)
                break;
            stringstream s1(line);
            vector <ull> pre;
            sdbg temp;
            string kmer;
            int id,cov;
            int count=0;
            while(getline(s1,reg,' '))
            {
                if(count==0)
                {
                    kmer=reg;
                }
                else if(count==1)
                {
                    stringstream tmp(reg);
                    tmp>>id;
                }
                else if(count==2)
                {
                    stringstream tmp(reg);
                    tmp>>cov;
                }
                count++;   
            }
        //  cout<<kmer<<endl;
            ull rt_id=Dtree_accept(kmer,root,dbg.size());
        // cout<<"accept done"<<endl;
            temp.kmer=kmer;
            dbg.push_back(temp);
        }
        int dbg_size=dbg.size();
        for(int i=0;i<seq_kmer.size();i++)
        {
            ull rt_id=Dtree_accept(seq_kmer[i],root,dbg_size+1);
            cout<<rt_id<<endl;
            if(rt_id>dbg_size)
            {
                cout<<"error kmer"<<endl;
            }
            else
            {
                seq_id.push_back(rt_id+bucket_kmer_num[tag]);
            }
        }

    }


}
void ref_seq_info_read(int tag,string seq,string path,vector <ull> &seq_id,vector<int> bucket_kmer_num)
{
    ifstream infoin(path+"/kmerinfo_"+to_string(tag));
    string line,reg;
    while (getline(infoin,line))
    {
        stringstream s1(line);
        int pos ;
        string kmer;
        ull id;
        int count=0;
        while(getline(s1,reg,' '))
        {
            if(count==0)
            {
                stringstream tmp(reg);
                tmp>>pos;
            }
            else if(count==1)
            {
                kmer=reg;
            }
            else if(count==2)
            {
                stringstream tmp(reg);
                tmp>>id;
            }
            count++;   
        }
        id=id+bucket_kmer_num[tag];
        seq_id[pos]=id;
    }
    
}
#endif