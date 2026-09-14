#include "dbg.h"

void Reads::array_init(Parameter*para)
{
    int length_count;
    successor.clear();
    predecessor.clear();
    if(length<para->rLengthNormal)
    {
        length_count=length;
    }
    else
    {
        length_count=para->rLengthNormal;
    }
    string temp;
    string perifix_start_str=sequence.substr(0,para->sequence_o);
    //cout<<perifix_start_str<<endl;
    string suffix_start_str=sequence.substr(length-para->sequence_o,para->sequence_o);
    //cout<<suffix_start_str<<endl;
    perifix_array.push_back(get_hash(perifix_start_str,para->sequence_o,para));
    //cout<<perifix_start_str<<"---"<<suffix_start_str<<endl;
    suffix_array.push_back(get_hash(suffix_start_str,para->sequence_o,para));
    //cout<<get_hash(perifix_start_str,para->sequence_o,para)<<"---"<<get_hash(suffix_start_str,para->sequence_o,para)<<endl;
    if(length>=para->rLengthNormal)
    {
        for (int i = para->sequence_o; i < length_count-1; i++)
        {
            perifix_array.push_back(perifix_array[i-para->sequence_o]+para->hash_P_list[i]*para->base_value[sequence[i]]);
            //cout<<(perifix_array.back()==get_hash(sequence.substr(0,i+1),i+1,para))<<endl;
            temp=sequence.substr(length-i-1);
            suffix_array.push_back(get_hash(temp,temp.length(),para));
            //cout<<perifix_array.back()<<endl;
        }
        //cout<<perifix_array.size()<<endl;
    }
    else
    {
        //cout<<length_count<<endl;   
        for (int i = para->sequence_o; i < length_count-1; i++)
        {
            perifix_array.push_back(perifix_array[i-para->sequence_o]+para->hash_P_list[i]*para->base_value[sequence[i]]);
            //cout<<(perifix_array.back()==get_hash(sequence.substr(0,i+1),i+1,para))<<endl;
            temp=sequence.substr(length-i-1);
            suffix_array.push_back(get_hash(temp,temp.length(),para));
            //cout<<i<<endl;
            //cout<<perifix_array.back()<<endl;
        }
        //cout<<perifix_array.size()<<endl;
        for(int i=length_count-1;i<para->rLengthNormal-1;i++)
        {
            //cout<<"into"<<endl;
            perifix_array.push_back(0);
            suffix_array.push_back(0);
        }
    }
    
    
}

bool Reads::self_cycle()
{
    for (ull i=0;i<predecessor.size();i++)
    {
        if(predecessor[i]==seq_ID)
            return true;
    }
    return false;
}

bool Reads::find(int type,ull target)
{
    //type=0 --> judge if predecssor exists target read
    //type=1 --> judge if successor exists target read
    if(type==0)
    {
        for(int i=0;i<predecessor.size();i++)
        {
            if(predecessor[i]==target)
                return true;
        }
    }
    else if(type==1)
    {
        for(ull i=0;i<successor.size();i++)
        {
            if(successor[i]==target)
                return true;
        }
    }
    return false;
}
