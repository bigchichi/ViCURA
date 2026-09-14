#include "dbg.h"
void Pairs::uov_init(Reads st,Reads ed,Parameter*para)
{
    string str1,str2,overlap_str;
    int str1_length=st.length-overlap_length;
    int str2_length=ed.length-overlap_length;
    total_length=str1_length+overlap_length+str2_length;
    str1=st.sequence.substr(0,str1_length);
    str2=ed.sequence.substr(overlap_length);
    overlap_str=ed.sequence.substr(0,overlap_length);
    uov_sequence=str1+overlap_str+str2;
    pairs_hash=get_hash(uov_sequence,total_length,para);
}
// bool Zipline ::find_string_element(string target)
// {
//     for(int i; i<zip_s_stack.size();i++)
//     {
//         if(zip_s_stack[i]==target)
//             return true; 
//     }
//     return false;
// }

// void Cycle ::detect_cyc(vector <ull> stack)
// {
//     cycle.push_back(st);
//     ull p=stack.size();
//     for(ull i=p-1;i>0;i--)
//     {
//         //cout<<i<<endl;
//         if(stack[i]!=st)
//             cycle.push_back(stack[i]);
//         else
//         {

//             cycle.push_back(stack[i]);
//             break;
//         }
//     }

//     cycle = vector<ull>(cycle.rbegin() , cycle.rend());
//     size=cycle.size();
// }