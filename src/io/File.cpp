#include "dbg.h"
//#include "dynamic_tree.h"
string sequence_deal(string sequence,int sequence_length,Parameter*para)
{
    reverse(sequence.begin(),sequence.end());
    for (int i = 0; i < sequence_length; i++)
    {
        sequence[i]=para->base_map[sequence[i]];
    }
    return sequence;
    
}
std::vector<Reads> Files :: file_read(string file_name_R1,string file_name_R2,Parameter*para,int tag)
//tag=1 fastq    tag=2 fasta
{
    vector <Reads> reads;
    Reads temp;
    fastq_R1.open(file_name_R1);
    fastq_R2.open(file_name_R2);
    string line;
    ull count=0;
    ull into_reads=0;
    vector <int> length_frequency(300);
    unordered_map <ull,int> sequence_frequency_map;
    int mod;
    if(tag==1)
        mod=4;
    else if(tag==2)
        mod=2;
    while (std::getline(fastq_R1,line))
    {   
        if(count%mod==1)
        {
        //cout<<line<<endl;
        if(if_undefined_character(line))
            continue;
        temp.sequence=sequence_deal(line,line.size(),para);
        
        temp.length=temp.sequence.size();
        temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
        // if(temp.length!=150)
        // {
        //     cout<<temp.length<<endl;
        //     cout<<temp.sequence<<endl;
        //     cout<<sequence_deal(line,line.size(),para)<<endl;
        // }
        //cout<<temp.sequence_hash<<endl;
        length_frequency[temp.length]++;
        if(sequence_frequency_map.count(temp.sequence_hash)==0)
        {
            temp.seq_ID=into_reads;
            sequence_frequency_map[temp.sequence_hash]=temp.seq_ID;
            reads.push_back(temp);
            into_reads++;
        }
        else if(sequence_frequency_map.count(temp.sequence_hash)==1)
        {
            reads[sequence_frequency_map[temp.sequence_hash]].read_count+=1;
            reads[sequence_frequency_map[temp.sequence_hash]].read_suf_usage+=1;
            reads[sequence_frequency_map[temp.sequence_hash]].read_per_usage+=1;
            // cout<<temp.sequence<<endl;
            // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence<<endl;
            // cout<<temp.sequence_hash<<endl;
            // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence_hash<<endl;
            // cout<<(temp.sequence==reads[sequence_frequency_map[temp.sequence_hash]].sequence)<<endl;
            // cout<<"-------------split--line---------------"<<endl;
        }

        }
         count++;

    }
    count=0;
    //cout<<reads.size()<<endl;
    while (std::getline(fastq_R2,line))
    {   
        if(count%mod==1)
        {
        //cout<<line<<endl;
        if(if_undefined_character(line))
            continue;
        temp.sequence=line;
        temp.length=temp.sequence.size();
        // if(temp.length!=150)
        // {
        //     cout<<line<<endl;
        // }
        //temp.sequence=sequence_deal(line,temp.length,para);
        //cout<<temp.sequence<<endl;
        temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
        // if(temp.length!=150)
        //     //cout<<"special reads"<<endl;
        //cout<<temp.sequence_hash<<endl;
        length_frequency[temp.length]++;
        if(sequence_frequency_map.count(temp.sequence_hash)==0)
        {
            temp.seq_ID=into_reads;
            sequence_frequency_map[temp.sequence_hash]=temp.seq_ID;
            reads.push_back(temp);
            into_reads++;
            //cout<<"new read"<<endl;
        }
        else if(sequence_frequency_map.count(temp.sequence_hash)==1)
        {
            reads[sequence_frequency_map[temp.sequence_hash]].read_count+=1;
            reads[sequence_frequency_map[temp.sequence_hash]].read_suf_usage+=1;
            reads[sequence_frequency_map[temp.sequence_hash]].read_per_usage+=1;
            // cout<<temp.sequence<<endl;
            // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence<<endl;
            // cout<<temp.sequence_hash<<endl;
            // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence_hash<<endl;
            // cout<<(temp.sequence==reads[sequence_frequency_map[temp.sequence_hash]].sequence)<<endl;
            // cout<<"-------------split--line---------------"<<endl;
        }

        }
         count++;

    }
    unordered_map<ull,int>().swap(sequence_frequency_map);
    malloc_trim(0);
    para->reads_count=reads.size();
    int max_frequency=0;
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]!=0)
        {
            para->rLengthMin=i;
            break;
        }
    }
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]!=0)
        {
            para->rLengthMax=i;
        }
    }
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]>=max_frequency)
        {
            max_frequency=length_frequency[i];
            para->rLengthNormal=i;
        }
    }
    return reads;

}

std::vector<Reads> Files :: file_read_sam(string sam_name,Parameter*para)
{
    vector <Reads> reads;
    Reads temp;
    sam.open(sam_name);
    string line,word;
    ull count=0;
    ull into_reads=0;
    map <int,ull> length_frequency;
    unordered_map <ull,int> sequence_frequency_map;


    int ct=0;
    while (std::getline(sam,line))
    {   
        istringstream record(line);
        ct=0;
        //cout<<"into"<<endl;
        while(record>>word)
        {
            if(ct==3)
            {
                //cout<<word<<endl;
                temp.length=word.size();
                temp.sequence=word;
                temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
                if(temp.length!=150)
                    cout<<"special reads"<<endl;
                //cout<<temp.sequence_hash<<endl;
                if(length_frequency.count(temp.length)==1)
                {
                    length_frequency[temp.length]++;
                }
                else if(length_frequency.count(temp.length)==0)
                {
                    length_frequency[temp.length]=1;
                }
                if(sequence_frequency_map.count(temp.sequence_hash)==0)
                {
                    temp.seq_ID=into_reads;
                    sequence_frequency_map[temp.sequence_hash]=temp.seq_ID;
                    reads.push_back(temp);
                    into_reads++;
                }
                else if(sequence_frequency_map.count(temp.sequence_hash)==1)
                {
                    reads[sequence_frequency_map[temp.sequence_hash]].read_count+=1;
                    reads[sequence_frequency_map[temp.sequence_hash]].read_suf_usage+=1;
                    reads[sequence_frequency_map[temp.sequence_hash]].read_per_usage+=1;
                    // cout<<temp.sequence<<endl;
                    // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence<<endl;
                    // cout<<temp.sequence_hash<<endl;
                    // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence_hash<<endl;
                    // cout<<(temp.sequence==reads[sequence_frequency_map[temp.sequence_hash]].sequence)<<endl;
                    // cout<<"-------------split--line---------------"<<endl;
                }

            }
            ct++;
        }
         count++;

    }
    //cout<<reads.size()<<endl;
    para->reads_count=reads.size();
    para->rLengthMax=(--length_frequency.end())->first;
    para->rLengthMin=length_frequency.begin()->first;
    //cout<<length_frequency[150]<<"--"<<length_frequency[151]<<endl;
    para->rLengthNormal = std::max_element(length_frequency.begin(), length_frequency.end(),
    [](const pair<int, int>& p1, const pair<int, int>& p2) {
        return p1.second < p2.second; })->first;
    return reads;

}
void read_reads_info(vector <Reads> &reads,string path,int file_num,int option)
//option=0 initialize the "reads"   option=1   add information to the "reads"
{
    string reads_file=path+"/temp_"+to_string(file_num);
    reads_file=path+"/shrink_O50_reads";
    reads_file=path+"/reads_shrink_O50_reads";
    reads_file=path+"/shrink_O31_2set_reads";
    reads_file=path+"/OLC_3";
    reads_file=path+"/OLC_20";
    //reads_file=path+"/OLC_2";
    reads_file=path+"/OLC_2_35";
    reads_file=path+"/OLC_combine";
    reads_file=path;
    //reads_file=path+"/shrink_O31_3set_reads";
    ifstream filein(reads_file);
    int tag=0;
    vector <int> int_vec;
    ull ull_word=0;
    vector <ull> ull_vec;
    int int_word=0;
    string word,line;
    while (getline(filein,line))
    {
        if(tag%5==0)
        {
            if(option==0)
            {
                Reads temp;
                temp.sequence=line;
                reads.push_back(temp);
            }
        }
        if(tag%5==1)
        {
            ull_vec.clear();
            istringstream sucin(line);
            while (sucin>>word)
            {
                istringstream ss(word);
                ss>>ull_word;
                ull_vec.push_back(ull_word);
            }
            if(option==0)
            {
                int back=reads.size()-1;
                reads[back].successor=ull_vec;
            }
            else
            {
                int id=tag/5;
                reads[id].successor.insert(reads[id].successor.end(),ull_vec.begin(),ull_vec.end()); 
            }
        }
        if(tag%5==2)
        {
            int_vec.clear();
            istringstream sucoin(line);
            while (sucoin>>word)
            {
                istringstream ss(word);
                ss>>int_word;
                int_vec.push_back(int_word);
            }
            if(option==0)
            {
                int back=reads.size()-1;
                reads[back].successor_o=int_vec;
            }
            else
            {
                int id=tag/5;
                reads[id].successor_o.insert(reads[id].successor_o.end(),int_vec.begin(),int_vec.end()); 
            }

        }
        if(tag%5==3)
        {
            ull_vec.clear();
            istringstream prein(line);
            while (prein>>word)
            {
                istringstream ss(word);
                ss>>ull_word;
                ull_vec.push_back(ull_word);
            }
            if(option==0)
            {
                int back=reads.size()-1;
                reads[back].predecessor=ull_vec;
            }
            else
            {
                int id=tag/5;
                reads[id].predecessor.insert(reads[id].predecessor.end(),ull_vec.begin(),ull_vec.end()); 
            }
        }
        if(tag%5==4)
        {
            int_vec.clear();
            istringstream covin(line);
            while (covin>>word)
            {
                istringstream ss(word);
                ss>>int_word;
                int_vec.push_back(int_word);
            }
            if(option==0)
            {
                int back=reads.size()-1;
                reads[back].cov=int_vec;
            }
        }
        tag++;
    }
    
}
void Reads::print(string path)
{
    ofstream readsout(path,ios::app);
    readsout<<sequence<<endl;
    for(int i=0;i<successor.size();i++)
    {
        readsout<<successor[i]<<" ";
    }
    readsout<<endl;
    for(int i=0;i<successor_o.size();i++)
    {
        readsout<<successor_o[i]<<" ";
    }
    readsout<<endl;
    for(int i=0;i<predecessor.size();i++)
    {
        readsout<<predecessor[i]<<" ";
    }
    readsout<<endl;
    for(int i=0;i<cov.size();i++)
    {
        readsout<<cov[i]<<" ";
    }
    readsout<<endl;
    //readsout<<read_count<<endl;
}
void Files :: file_read_Dtree(string file_name_R1,string file_name_R2,Parameter*para,int tag,Dynamic_Tree*root,vector<Reads> &reads)
//tag=1 fastq    tag=2 fasta
{
    //vector <Reads> reads;

    fastq_R1.open(file_name_R1);
    fastq_R2.open(file_name_R2);
    string line;
    ull count=0;
    ull into_reads=0;
    vector <int> length_frequency(300);
    //unordered_map <ull,int> sequence_frequency_map;
    int mod;
    if(tag==1)
        mod=4;
    else if(tag==2)
        mod=2;
    cout<<"start R1"<<endl;
    while (std::getline(fastq_R1,line))
    {   
        //cout<<line<<endl;
        if(count%mod==1)
        {
        //cout<<line<<endl;
        if(if_undefined_character(line))
            continue;
        ull expected_id=reads.size();
        Reads temp;
        temp.sequence=sequence_deal(line,line.size(),para);
        //cout<<temp.sequence<<endl;
        temp.length=temp.sequence.size();
        //temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
        // if(temp.length!=150)
        // {
        //     cout<<temp.length<<endl;
        //     cout<<temp.sequence<<endl;
        //     cout<<sequence_deal(line,line.size(),para)<<endl;
        // }
        //cout<<temp.sequence_hash<<endl;
        length_frequency[temp.length]++;
        //cout<<"into 1"<<endl;
        ull return_id=Dtree_accept(temp.sequence,root,expected_id);
        //ull return_id=expected_id;
        //cout<<return_id<<" "<<expected_id<<endl;
        //cout<<"into 2"<<endl;
        if(return_id==expected_id)
        {
            //cout<<"new"<<endl;
            temp.seq_ID=into_reads;
            //cout<<temp.seq_ID<<endl;
           //cout<<reads.size()<<endl;
            //temp.r1_count++;
            temp.cov.push_back(1);
            reads.push_back(temp);
            //cout<<"push"<<endl;
            into_reads++;
            
        }
        else
        {
            //cout<<"not new"<<endl;
            reads[return_id].read_count+=1;
            reads[return_id].cov[0]=reads[return_id].read_count;
            //reads[return_id].r1_count+=1;
            reads[return_id].read_suf_usage+=1;
            reads[return_id].read_per_usage+=1;
        }
        // if(sequence_frequency_map.count(temp.sequence_hash)==0)
        // {
        //     temp.seq_ID=into_reads;
        //     sequence_frequency_map[temp.sequence_hash]=temp.seq_ID;
        //     reads.push_back(temp);
        //     into_reads++;
        // }
        // else if(sequence_frequency_map.count(temp.sequence_hash)==1)
        // {
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_count+=1;
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_suf_usage+=1;
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_per_usage+=1;
        //     // cout<<temp.sequence<<endl;
        //     // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence<<endl;
        //     // cout<<temp.sequence_hash<<endl;
        //     // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence_hash<<endl;
        //     // cout<<(temp.sequence==reads[sequence_frequency_map[temp.sequence_hash]].sequence)<<endl;
        //     // cout<<"-------------split--line---------------"<<endl;
        // }

        }
         count++;

    }
    count=0;
    //cout<<reads.size()<<endl;
    while (std::getline(fastq_R2,line))
    {   
        if(count%mod==1)
        {
        //cout<<line<<endl;
        if(if_undefined_character(line))
            continue;
        ull expected_id=reads.size();
        Reads temp;
        temp.sequence=line;
        
        temp.length=temp.sequence.size();
        //temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
        // if(temp.length!=150)
        // {
        //     cout<<temp.length<<endl;
        //     cout<<temp.sequence<<endl;
        //     cout<<sequence_deal(line,line.size(),para)<<endl;
        // }
        //cout<<temp.sequence_hash<<endl;
        length_frequency[temp.length]++;
        ull return_id=Dtree_accept(temp.sequence,root,expected_id);
        if(return_id==expected_id)
        {
            temp.seq_ID=into_reads;
            //temp.r2_count++;
            temp.cov.push_back(1);
            reads.push_back(temp);
            into_reads++;
        }
        else
        {
            reads[return_id].read_count+=1;
            reads[return_id].cov[0]=reads[return_id].read_count;
            //reads[return_id].r2_count+=1;
            reads[return_id].read_suf_usage+=1;
            reads[return_id].read_per_usage+=1;
        }

        }
         count++;

    }
    para->reads_count=reads.size();
    int max_frequency=0;
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]!=0)
        {
            para->rLengthMin=i;
            break;
        }
    }
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]!=0)
        {
            para->rLengthMax=i;
        }
    }
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]>=max_frequency)
        {
            max_frequency=length_frequency[i];
            para->rLengthNormal=i;
        }
    }
    //return reads;
    int max_freq=0;
    ofstream feqout("frequency");
    for(int i=0;i<reads.size();i++)
    {
        if(reads[i].read_count>max_freq)
            max_freq=reads[i].read_count;
    }
    vector <int> feq(max_freq+1);
    for(int i=0;i<reads.size();i++)
    {
        int f=reads[i].read_count;
        feq[f]++;
    }
    vector <vector<string>> feq_s(max_freq+1);
    for(int i=0;i<reads.size();i++)
    {
        int f=reads[i].read_count;
        feq_s[f].push_back(reads[i].sequence);
    }
    for(int i=2;i<feq.size();i++)
    {
        if(feq[i]==0)
            continue;
        feqout<<i<<" "<<feq[i]<<endl;
        for(int j=0;j<feq_s[i].size();j++)
        {
            feqout<<feq_s[i][j]<<endl;
        }
    }
}
std::vector<Reads> Files :: file_read_Dtree_frequency_filter(string file_name_R1,string file_name_R2,Parameter*para,int tag,Dynamic_Tree*root,int threshold)
//tag=1 fastq    tag=2 fasta
{
    vector <Reads> reads,reads1,reads1_rc,reads2,reads2_rc;
    Reads temp,temp1;
    fastq_R1.open(file_name_R1);
    fastq_R2.open(file_name_R2);
    string line;
    ull count=0;
    ull into_reads=0;
    vector <int> length_frequency(300);
    Dynamic_Tree *Droot1=new Dynamic_Tree;
    Dynamic_Tree *Droot2=new Dynamic_Tree;
    Dynamic_Tree *Droot3=new Dynamic_Tree;
    Dynamic_Tree *Droot4=new Dynamic_Tree;
    //unordered_map <ull,int> sequence_frequency_map;
    int mod;
    if(tag==1)
        mod=4;
    else if(tag==2)
        mod=2;
    while (std::getline(fastq_R1,line))
    {   
        if(count%mod==1)
        {
        //cout<<line<<endl;
        if(if_undefined_character(line))
            continue;
        ull expected_id=reads.size();
        temp.sequence=sequence_deal(line,line.size(),para);
        temp1.sequence=line;
        temp.length=temp.sequence.size();
        temp1.length=temp1.sequence.size();
        //temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
        // if(temp.length!=150)
        // {
        //     cout<<temp.length<<endl;
        //     cout<<temp.sequence<<endl;
        //     cout<<sequence_deal(line,line.size(),para)<<endl;
        // }
        //cout<<temp.sequence_hash<<endl;
        length_frequency[temp.length]++;
        //cout<<"into 1"<<endl;
        ull return_id=Dtree_accept(temp.sequence,Droot1,expected_id);
        //cout<<"into 2"<<endl;
        if(return_id==expected_id)
        {
            temp.seq_ID=reads1.size();
            reads1.push_back(temp);
            //into_reads++;
        }
        else
        {
            reads1[return_id].read_count+=1;
        }
        ull return_id1=Dtree_accept(temp1.sequence,Droot2,expected_id);
        //cout<<"into 2"<<endl;
        if(return_id1==expected_id)
        {
            temp1.seq_ID=into_reads;
            reads1_rc.push_back(temp1);
        }
        else
        {
            reads1_rc[return_id].read_count+=1;
        }
        // if(sequence_frequency_map.count(temp.sequence_hash)==0)
        // {
        //     temp.seq_ID=into_reads;
        //     sequence_frequency_map[temp.sequence_hash]=temp.seq_ID;
        //     reads.push_back(temp);
        //     into_reads++;
        // }
        // else if(sequence_frequency_map.count(temp.sequence_hash)==1)
        // {
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_count+=1;
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_suf_usage+=1;
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_per_usage+=1;
        //     // cout<<temp.sequence<<endl;
        //     // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence<<endl;
        //     // cout<<temp.sequence_hash<<endl;
        //     // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence_hash<<endl;
        //     // cout<<(temp.sequence==reads[sequence_frequency_map[temp.sequence_hash]].sequence)<<endl;
        //     // cout<<"-------------split--line---------------"<<endl;
        // }

        }
         count++;

    }
    count=0;
    // for(int i=0;i<reads1.size();i++)
    // {
    //     if(reads1[i].read_count <= threshold)
    //     {
    //         continue;
    //     }
    //     Dtree_accept(reads1[i].sequence,root,reads.size());
    //     reads.push_back(reads1[i]);
    // }
    for(int i=0;i<reads1_rc.size();i++)
    {
        if(reads1_rc[i].read_count <= threshold)
        {
            continue;
        }
        Dtree_accept(reads1_rc[i].sequence,root,reads.size());
        reads.push_back(reads1_rc[i]);
    }
    //cout<<reads.size()<<endl;
    while (std::getline(fastq_R2,line))
    {   
        if(count%mod==1)
        {
        //cout<<line<<endl;
        if(if_undefined_character(line))
            continue;
        ull expected_id=reads.size();
        temp.sequence=line;
        
        temp.length=temp.sequence.size();
        temp1.sequence=sequence_deal(line,line.size(),para);
        temp1.sequence=temp1.sequence.size();
        //temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
        // if(temp.length!=150)
        // {
        //     cout<<temp.length<<endl;
        //     cout<<temp.sequence<<endl;
        //     cout<<sequence_deal(line,line.size(),para)<<endl;
        // }
        //cout<<temp.sequence_hash<<endl;
        length_frequency[temp.length]++;
        ull return_id=Dtree_accept(temp.sequence,Droot3,expected_id);
        if(return_id==expected_id)
        {
            temp.seq_ID=into_reads;
            reads2.push_back(temp);
            into_reads++;
        }
        else
        {
            reads2[return_id].read_count+=1;
        }
        ull return_id1=Dtree_accept(temp1.sequence,Droot4,expected_id);
        if(return_id1==expected_id)
        {
            temp1.seq_ID=into_reads;
            reads2_rc.push_back(temp1);
            into_reads++;
        }
        else
        {
            reads2_rc[return_id1].read_count+=1;
        }
        }
         count++;

    }
    // for(int i=0;i<reads2.size();i++)
    // {
    //     if(reads2[i].read_count <= threshold)
    //     {
    //         continue;
    //     }
    //     int return_id=Dtree_accept(reads2[i].sequence,root,reads.size());
    //     if(return_id==reads.size())
    //     {
    //         //temp.seq_ID=into_reads;
    //         reads2.push_back(reads2[i]);
    //         into_reads++;
    //     }
    //     else
    //     {
    //         reads2[return_id].read_count+=1;
    //     }
    //     reads.push_back(reads2[i]);
    // }
    for(int i=0;i<reads2_rc.size();i++)
    {
        if(reads2_rc[i].read_count <= threshold)
        {
            continue;
        }
        int return_id=Dtree_accept(reads2_rc[i].sequence,root,reads.size());
        if(return_id==reads.size())
        {
            //temp.seq_ID=into_reads;
            reads.push_back(reads2_rc[i]);
            //into_reads++;
        }
        else
        {
            reads[return_id].read_count+=reads2_rc[i].read_count;
        }
        
    }
    para->reads_count=reads.size();
    int max_frequency=0;
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]!=0)
        {
            para->rLengthMin=i;
            break;
        }
    }
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]!=0)
        {
            para->rLengthMax=i;
        }
    }
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]>=max_frequency)
        {
            max_frequency=length_frequency[i];
            para->rLengthNormal=i;
        }
    }
    return reads;

}

void read_dbg_info(string dbg_file,string pre_file,int minimizer_length,vector<Debrujin_node> &dbg)
{
    int bucket_num=(int)pow(4.0,(double)minimizer_length);
    cout<<"into "<<bucket_num<<endl;
    for(int i=0;i<bucket_num;i++)
    {
        string dbg_filepath=dbg_file+"/kmer150_unique_"+to_string(i);
        string bucket_file=dbg_file+"/bucket_num_"+to_string(i);
        string pre_filepath=pre_file+"/preinfo_"+to_string(i);
        cout<<"doing "<<i<<endl;
        //cout<<pre_filepath<<" "<<dbg_filepath<<endl;
        string numstring,line1,line2,reg;
        ifstream dbgin(dbg_filepath);
        ifstream prein(pre_filepath);
        ifstream bin(bucket_file);
        getline(bin,numstring);
        stringstream ss(numstring);
        int num;
        ss>>num;
        int dbg_count=0;
        while (true)
        {
            bool tg1= static_cast<bool>(std :: getline(dbgin,line1));
            bool tg2= static_cast<bool>(std :: getline(prein,line2));
            //cout<<line1<<" "<<line2<<endl;
            //cout<<tg1<<" "<<tg2<<endl;
            if(!tg1 && !tg2)
                break;
            stringstream s1(line1);
            stringstream s2(line2);
            vector <ull> pre;
            Debrujin_node temp;
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
            while ((getline(s2,reg,' ')))
            {
                stringstream tmp(reg);
                int pretemp;
                tmp>>pretemp;
                pre.push_back(pretemp);
            }
            temp.k_mer=kmer;
            temp.cov=cov;
            temp.predecessor=pre;
            dbg.push_back(temp);
            dbg_count++;
        }
        cout<<num<<" "<<dbg_count<<endl;
        if(num!=dbg_count)
            cout<<"error "<<endl;
        cout<<"done "<<dbg.size()<<endl;
        
    }
    cout<<"dbg size "<<dbg.size()<<endl;
    for(int i=0;i<dbg.size();i++)
    {
        for(int j=0;j<dbg[i].predecessor.size();j++)
        {
            ull pre=dbg[i].predecessor[j];
            dbg[pre].successor.push_back(i);
        }
    }
}
std::vector<Reads> Files :: file_read_Dtree_shrink(string file_name_R1,string file_name_R2,Parameter*para,int tag,Dynamic_Tree*root,int threshold)
{
    vector <Reads> reads;
    fastq_R1.open(file_name_R1);
    fastq_R2.open(file_name_R2);
    string line;
    ull count=0;
    ull into_reads=0;
    vector <int> length_frequency(300);
    //unordered_map <ull,int> sequence_frequency_map;
    // Dynamic_Tree* root_r1=new Dynamic_Tree;
    // Dynamic_Tree* root_r1_rc=new Dynamic_Tree;
    // Dynamic_Tree* root_r2=new Dynamic_Tree;
    // Dynamic_Tree* root_r2_rc=new Dynamic_Tree;
    Dynamic_Tree*root1=new Dynamic_Tree;
    Dynamic_Tree*root2=new Dynamic_Tree;
    vector <Reads> temp_reads;
    vector <Reads> temp_reads_rc;
    temp_reads.reserve(11000000);
    temp_reads_rc.reserve(11000000);
    int mod;
    if(tag==1)
        mod=4;
    else if(tag==2)
        mod=2;
    cout<<"start R1"<<endl;
    while (std::getline(fastq_R1,line))
    {   
        if(count%mod==1)
        {
            //cout<<line<<endl;
            Reads temp,temp_rc;
            if(if_undefined_character(line))
                continue;
            ull expected_id=temp_reads.size();
            ull expected_id_rc=temp_reads_rc.size();
            temp.sequence=sequence_deal(line,line.size(),para);
            temp_rc.sequence=line;
            
            temp.length=temp.sequence.size();
            temp_rc.length=temp.sequence.size();
            //temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
            // if(temp.length!=150)
            // {
            //     cout<<temp.length<<endl;
            //     cout<<temp.sequence<<endl;
            //     cout<<sequence_deal(line,line.size(),para)<<endl;
            // }
            //cout<<temp.sequence_hash<<endl;
            length_frequency[temp.length]++;
            //cout<<"into 1"<<endl;
            ull return_id=Dtree_accept(temp.sequence,root1,expected_id);
            ull return_id_rc=Dtree_accept(temp_rc.sequence,root2,expected_id_rc);
            //cout<<"into 2"<<endl;
            if(return_id==expected_id)
            {
                temp.seq_ID=temp_reads.size();
                temp.cov.push_back(1);
                temp_reads.push_back(temp);
            }
            else
            {
                temp_reads[return_id].read_count+=1;
                temp_reads[return_id].cov[0]=temp_reads[return_id].read_count;
                // temp_reads[return_id].read_suf_usage+=1;
                // temp_reads[return_id].read_per_usage+=1;
            }

            if(return_id_rc==expected_id_rc)
            {
                temp_rc.seq_ID=temp_reads_rc.size();
                temp_rc.cov.push_back(1);
                temp_reads_rc.push_back(temp_rc);
            }
            else
            {
                temp_reads_rc[return_id_rc].read_count+=1;
                temp_reads_rc[return_id_rc].cov[0]=temp_reads_rc[return_id_rc].read_count;
                // temp_reads_rc[return_id_rc].read_suf_usage+=1;
                // temp_reads_rc[return_id_rc].read_per_usage+=1;
            }
        }
         count++;
         //cout<<count<<endl;

    }
    // for(int i=0;i<temp_reads.size();i++)
    // {
    //     if(temp_reads[i].read_count<threshold)
    //         continue;
    //     //cout<<"repeat "<<endl;
    //     ull expid=reads.size();
    //     ull rt_id=Dtree_accept(temp_reads[i].sequence,root,expid);
    //     if(rt_id==expid)
    //     {
    //         Reads r=temp_reads[i];
    //         reads.push_back(r);
    //     }
    //     else
    //     {
    //         reads[rt_id].read_count+=temp_reads[i].read_count;
    //     }
    // }

    // for(int i=0;i<temp_reads_rc.size();i++)
    // {
    //     if(temp_reads_rc[i].read_count<=1)
    //         continue;
    //     //cout<<"repeat "<<endl;
    //     ull expid=reads.size();
    //     ull rt_id=Dtree_accept(temp_reads_rc[i].sequence,root,expid);
    //     if(rt_id==expid)
    //     {
    //         Reads r=temp_reads_rc[i];
    //         reads.push_back(r);
    //     }
    //     else
    //     {
    //         reads[rt_id].read_count+=temp_reads_rc[i].read_count;
    //     }
    // }
    cout<<temp_reads.size()<<" "<<temp_reads_rc.size()<<endl;
    count=0;
    // temp_reads.clear();
    // temp_reads_rc.clear();
    cout<<"R1 done"<<endl;
    cout<<reads.size()<<endl;
    //cout<<reads.size()<<endl;
    while (std::getline(fastq_R2,line))
    {   
        if(count%mod==1)
        {
            //cout<<line<<endl;
            Reads temp,temp_rc;
            if(if_undefined_character(line))
                continue;
            ull expected_id=temp_reads.size();
            ull expected_id_rc=temp_reads_rc.size();
            temp.sequence=line;
            temp_rc.sequence=sequence_deal(line,line.size(),para);
            
            temp.length=temp.sequence.size();
            temp_rc.length=temp_rc.sequence.size();
            //temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
            // if(temp.length!=150)
            // {
            //     cout<<temp.length<<endl;
            //     cout<<temp.sequence<<endl;
            //     cout<<sequence_deal(line,line.size(),para)<<endl;
            // }
            //cout<<temp.sequence_hash<<endl;
            length_frequency[temp.length]++;
            ull return_id=Dtree_accept(temp.sequence,root1,expected_id);
            ull return_id_rc=Dtree_accept(temp_rc.sequence,root2,expected_id_rc);
            if(return_id==expected_id)
            {
                temp.seq_ID=temp_reads.size();
                temp.cov.push_back(1);
                temp_reads.push_back(temp);
            }
            else
            {
                temp_reads[return_id].read_count+=1;
                temp_reads[return_id].cov[0]=temp_reads[return_id].read_count;
                temp_reads[return_id].read_suf_usage+=1;
                temp_reads[return_id].read_per_usage+=1;
            }
            if(return_id_rc==expected_id_rc)
            {
                temp_rc.seq_ID=temp_reads_rc.size();
                temp_rc.cov.push_back(1);
                temp_reads_rc.push_back(temp_rc);
            }
            else
            {
                temp_reads_rc[return_id_rc].read_count+=1;
                temp_reads_rc[return_id_rc].cov[0]=temp_reads_rc[return_id_rc].read_count;
                temp_reads_rc[return_id_rc].read_suf_usage+=1;
                temp_reads_rc[return_id_rc].read_per_usage+=1;
            }
        }
         count++;

    }
    cout<<"R2 done"<<endl;
    for(int i=0;i<temp_reads.size();i++)
    {
        if(temp_reads[i].read_count<threshold)
            continue;
        //cout<<"repeat "<<endl;
        ull expid=reads.size();
        ull rt_id=Dtree_accept(temp_reads[i].sequence,root,expid);
        if(rt_id==expid)
        {
            Reads r=temp_reads[i];
            reads.push_back(r);
        }
        else
        {
            reads[rt_id].read_count+=temp_reads[i].read_count;
        }
    }
    // for(int i=0;i<temp_reads_rc.size();i++)
    // {
    //     if(temp_reads_rc[i].read_count<=1)
    //         continue;
    //     //cout<<"repeat "<<endl;
    //     ull expid=reads.size();
    //     ull rt_id=Dtree_accept(temp_reads_rc[i].sequence,root,expid);
    //     if(rt_id==expid)
    //     {
    //         Reads r=temp_reads_rc[i];
    //         reads.push_back(r);
    //     }
    //     else
    //     {
    //         reads[rt_id].read_count+=temp_reads_rc[i].read_count;
    //     }
    // }
    cout<<temp_reads.size()<<" "<<temp_reads_rc.size()<<endl;
    fastq_R1.close();
    fastq_R2.close();
    return reads;
}
std::vector<Reads> Files :: file_read_Dtree_mem(string file_name_R1,string file_name_R2,vector<Reads> &treads,Parameter*para,int tag,Dynamic_Tree*root,Dynamic_Tree*root2,int threshold)
{
    vector <Reads> reads;
    Reads temp;
    fastq_R1.open(file_name_R1);
    fastq_R2.open(file_name_R2);
    string line;
    ull count=0;
    ull into_reads=0;
    vector <int> length_frequency(300);
    //unordered_map <ull,int> sequence_frequency_map;
    int mod;
    if(tag==1)
        mod=4;
    else if(tag==2)
        mod=2;
    while (std::getline(fastq_R1,line))
    {   
        if(count%mod==1)
        {
        //cout<<line<<endl;
        if(if_undefined_character(line))
            continue;
        ull expected_id=reads.size();
        temp.sequence=sequence_deal(line,line.size(),para);
        
        temp.length=temp.sequence.size();
        //temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
        // if(temp.length!=150)
        // {
        //     cout<<temp.length<<endl;
        //     cout<<temp.sequence<<endl;
        //     cout<<sequence_deal(line,line.size(),para)<<endl;
        // }
        //cout<<temp.sequence_hash<<endl;
        length_frequency[temp.length]++;
        //cout<<"into 1"<<endl;
        ull return_id=Dtree_accept(temp.sequence,root,expected_id);
        //cout<<"into 2"<<endl;
        if(return_id==expected_id)
        {
            temp.seq_ID=into_reads;
            reads.push_back(temp);
            into_reads++;
        }
        else
        {
            reads[return_id].read_count+=1;
            reads[return_id].read_suf_usage+=1;
            reads[return_id].read_per_usage+=1;
        }
        // if(sequence_frequency_map.count(temp.sequence_hash)==0)
        // {
        //     temp.seq_ID=into_reads;
        //     sequence_frequency_map[temp.sequence_hash]=temp.seq_ID;
        //     reads.push_back(temp);
        //     into_reads++;
        // }
        // else if(sequence_frequency_map.count(temp.sequence_hash)==1)
        // {
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_count+=1;
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_suf_usage+=1;
        //     reads[sequence_frequency_map[temp.sequence_hash]].read_per_usage+=1;
        //     // cout<<temp.sequence<<endl;
        //     // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence<<endl;
        //     // cout<<temp.sequence_hash<<endl;
        //     // cout<<reads[sequence_frequency_map[temp.sequence_hash]].sequence_hash<<endl;
        //     // cout<<(temp.sequence==reads[sequence_frequency_map[temp.sequence_hash]].sequence)<<endl;
        //     // cout<<"-------------split--line---------------"<<endl;
        // }

        }
         count++;

    }
    count=0;
    //cout<<reads.size()<<endl;
    while (std::getline(fastq_R2,line))
    {   
        if(count%mod==1)
        {
        //cout<<line<<endl;
        if(if_undefined_character(line))
            continue;
        ull expected_id=reads.size();
        temp.sequence=line;
        
        temp.length=temp.sequence.size();
        //temp.sequence_hash=get_hash(temp.sequence,temp.length,para);
        // if(temp.length!=150)
        // {
        //     cout<<temp.length<<endl;
        //     cout<<temp.sequence<<endl;
        //     cout<<sequence_deal(line,line.size(),para)<<endl;
        // }
        //cout<<temp.sequence_hash<<endl;
        length_frequency[temp.length]++;
        ull return_id=Dtree_accept(temp.sequence,root,expected_id);
        if(return_id==expected_id)
        {
            temp.seq_ID=into_reads;
            reads.push_back(temp);
            into_reads++;
        }
        else
        {
            reads[return_id].read_count+=1;
            reads[return_id].read_suf_usage+=1;
            reads[return_id].read_per_usage+=1;
        }

        }
         count++;

    }
    para->reads_count=reads.size();
    int max_frequency=0;
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]!=0)
        {
            para->rLengthMin=i;
            break;
        }
    }
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]!=0)
        {
            para->rLengthMax=i;
        }
    }
    for(int i=0;i<300;i++)
    {
        if(length_frequency[i]>=max_frequency)
        {
            max_frequency=length_frequency[i];
            para->rLengthNormal=i;
        }
    }
    for(int i=0;i<reads.size();i++)
    {
        if(reads[i].read_count<threshold)
            continue;
        //cout<<"repeat "<<endl;
        ull expid=treads.size();
        ull rt_id=Dtree_accept(reads[i].sequence,root2,expid);
        if(rt_id==expid)
        {
            Reads r=reads[i];
            treads.push_back(r);
        }
        else
        {
            //tr[rt_id].read_count+=temp_reads_rc[i].read_count;
            cout<<"error"<<endl;
        }
    }
    return reads;
}
void file_kmer_info_read(string path,vector<unordered_set<ull>> front,vector<unordered_set<ull>> back,int file_num)
{
    front.clear();
    back.clear();
    for(int i=0;i<file_num;i++)
    {
        string file_name=path+"/kmer150_unique_read_"+to_string(i);
        ifstream infoin(file_name);
        string line;
        while(true)
        {
            if(!getline(infoin,line))
                break;
            stringstream ss(line);
            string reg;
            ull temp_node;
            unordered_set <ull> temp_front,temp_back;
            while (std :: getline(ss,reg,' '))
            {
                stringstream tmp(reg);
                tmp>>temp_node;
                temp_front.insert(temp_node);
            }
            front.push_back(temp_front);
            getline(infoin,line);
            stringstream ss2(line);
            while (std :: getline(ss2,reg,' '))
            {
                stringstream tmp(reg);
                tmp>>temp_node;
                temp_back.insert(temp_node);
            }
            back.push_back(temp_back);
        }
    }
}