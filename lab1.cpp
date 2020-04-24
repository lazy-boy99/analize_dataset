#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
using namespace std;

vector<vector<float>> getArrCov(vector<vector<string>>& set,int index1,int index2){
    vector <vector<float>> ar;
    vector<float> first;
    vector<float> second; 
    for(int i=0;i<set.size();i++){
        stringstream str(set[i][index1]);
        stringstream str1(set[i][index2]);
        float num,num1;
        try{
            str>>num;
            str1>>num1;
            first.push_back(num);
            second.push_back(num1);
        }catch(...){
            continue;
        }
    }
    ar.push_back(first);
    ar.push_back(second);
    return ar;
}

float getCorrelation(const vector<vector<float>>&ar){
    float avrX=0,avrY=0;
    int size=ar[0].size();
    float first=0,second=0,third=0;
    for(float el:ar[0]){
        avrX+=el/size;
    }
    for(float el:ar[1]){
        avrY+=el/size;
    }
    for (int i=0;i<size;i++){
        first+=(ar[0][i]-avrX)*(ar[1][i]-avrY);
        second+=powf(ar[0][i]-avrX,2);
        third+=powf(ar[1][i]-avrY,2);
    }
    return first/sqrt(second*third);
}

vector <vector <float>>getCovMatrix(vector<vector<string>>&set,int shift_beg,int shift_end){
    vector <vector <float>> matrix;    
    for(int i=shift_beg;i<set[0].size()-shift_end;i++){
       vector <float> tmp;
        for(int j=shift_beg;j<set[0].size()-shift_end;j++){
              tmp.push_back(getCorrelation(getArrCov(set,i,j)));
        }
        matrix.push_back(tmp);
    }
    return matrix;
}

double getEntropy(vector <vector <string >>& set,int num_class){
    map<string,double> probe;
    for(int i=0;i<set.size();i++){
        string name;
        for(int j=set[0].size()-num_class;j<set[0].size();j++){
            name+=","+set[i][j];
        }
        probe[name]=probe[name]+1.0/set.size();
    }
    double entropy=0;
    for(auto it:probe ){
        entropy-=it.second*log2(it.second);
    }
    return entropy;
}

vector<vector<string>> getSubSet(int index,vector <vector <string >>& set,string name){
    vector <vector<string>> tmp;
    for(int i=0;i<set.size();i++){
        if(set[i][index]==name){
            tmp.push_back(set[i]);
        }
    }
    return tmp;
}
double getGain(vector <vector <string >>& set,int index,double entropy){
    map<string,int> ready_subset;
    vector<vector<string>>subset;
    double info=entropy;
    for(int i=0;i<set.size();i++){
        if(ready_subset[set[i][index]]==0){
            subset=getSubSet(index,set,set[i][index]);
            info-=getEntropy(subset,2)*((double)subset.size())/set.size();
        }
        ready_subset[set[i][index]]++;
    }
    return info;
}
vector<string> getAttr(vector <vector<string>>& set,int sh_v,int sh_h,int sh_h_end ){
    vector<string> attr;
    for(int i=sh_h;i<set[sh_v].size()-sh_h_end;i++){
        attr.push_back(set[sh_v][i]);
    }
    return attr;
}

map<string,double> getGainMap(vector <vector<string>>& set,vector<string>& attr){
    map<string,double> map_gain;
    double entropy=getEntropy(set,2);
    for(int i=0;i<attr.size();i++){
        map_gain[attr[i]]=getGain(set,i,entropy);
    }
    return map_gain;
}


vector<vector <string>> getRealSet(vector<vector<string>> &tmp_set,vector <string>& attr,int shift_vert,int shift_hor){
    vector <vector<string>> set;
    attr=getAttr(tmp_set,shift_vert-2,shift_hor,2);
    for(int i=shift_vert;i<tmp_set.size();i++){
        vector<string> tmp;
        if(tmp_set[i][tmp_set[i].size()-2]!="" || tmp_set[i][tmp_set[i].size()-1]!=""){
        for(int j=shift_hor;j<tmp_set[shift_vert].size();j++){
            tmp.push_back(tmp_set[i][j]);
        }
        set.push_back(tmp);
        }
    }  
    return set;
}
int main(){
    vector<vector<string>>set_tmp;    
    ifstream file;
    file.open("tab.csv");
    string line;
    while( getline(file,line)){
        stringstream str(line);
        string tmp;
        vector<string>tmp_vec;
        while(getline(str,tmp,','))
            tmp_vec.push_back(tmp);
        set_tmp.push_back(tmp_vec);
    }
    vector <string> attr;
    vector <vector<string>> set=getRealSet(set_tmp,attr,3,0);
    map <string,double> mapGain=getGainMap(set,attr);
    fstream file_out;
    file_out.open("gain.csv"); 
    for(auto el=mapGain.begin();el!=mapGain.end();el++){
        if(next(el)->second!=mapGain.end()->second)
            file_out<<el->first<<",";
        else
            file_out<<el->first<<endl;
    }
    for(auto el=mapGain.begin();el!=mapGain.end();el++){
        if(next(el)->second!=mapGain.end()->second)
            file_out<<el->second<<",";
        else
            file_out<<el->second;
    }
    vector<vector<float>>mat=getCovMatrix(set,2,2);
    fstream file_cor;
    file_cor.open("corrMatrix.csv");
    for(auto el:mat){
        for(auto el1:el){
            if(el1!=el[el.size()-1])
                file_cor<<el1<<",";
            else
                file_cor<<el1<<endl;
        }
    }
}
