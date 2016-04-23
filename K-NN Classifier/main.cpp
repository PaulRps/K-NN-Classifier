#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <list>
#include <map>
#include <vector>
#include <math.h>

using namespace std;

struct data{
	float TF_IDF;
	int numberOfAppears;
	data(float f, int i){ TF_IDF = f; numberOfAppears = i; }
};


map<string, data> split_count_TF(string line);
vector<map<string, data>> readFile(vector<string> pathFiles);
map<string, data> normalize_TF(map<string, data> mapa);
vector<map<string, data>> IDF(vector<map<string, data>> mapa, int sizeCorpus);
float cossine_similarity(vector<map<string, data>> d1, vector<map<string, data>> d2, int posD1, int posD2);

//https://janav.wordpress.com/2013/10/27/tf-idf-and-cosine-similarity/
//http://blog.christianperone.com/2013/09/machine-learning-cosine-similarity-for-vector-space-models-part-iii/

int main(){

	vector<string> pathFiles;
	pathFiles.push_back("d1.txt");
	pathFiles.push_back("d2.txt");
	
	vector<map<string, data>> train(readFile(pathFiles));
	
	train = IDF(train, 2);	

	pathFiles.clear();
	
	pathFiles.push_back("d3.txt");
	pathFiles.push_back("d4.txt");

	vector<map<string, data>> test(readFile(pathFiles));

	test = IDF(test, 2);

	pathFiles.clear();

	cout << cossine_similarity(test, test, 0, 1) << endl;	
		
	return 0;
}

vector<map<string, data>> readFile(vector<string> pathFiles){
			
	vector<map<string, data>> mapWords;

	int n = pathFiles.size();
	
	for (int it = 0; it < n; it++){

		fstream file(pathFiles[it], fstream::in);

		if (!file){	cout << "FAIL IN OPEN OF THE FILE" << endl;	}

		string line;
		getline(file, line);		
		
		map<string, data> aux(split_count_TF(line));
		
		aux = normalize_TF(aux);
				
		mapWords.push_back(aux);			
		
		file.close();
	}

	return mapWords;
}

map<string, data> split_count_TF(string line){
		
	string aux2;
	map<string, data> result;
	pair<map<string, data>::iterator, bool> ret;

	int sizeLine = line.size();

	for (int i = 0; i < sizeLine; i++){
		
		if (line.at(i) != ' '){

			aux2.push_back(line.at(i));
		}
		else{
									
			ret = result.insert(pair<string, data>(aux2, data(1.0,1)));
			
			if (ret.second == false){

				map<string, data>::iterator it = result.find(aux2);
				it->second.numberOfAppears++;
			}

			aux2.erase();
		}
		
	}

	ret = result.insert(pair<string, data>(aux2, data(1.0, 1)));
	
	if (ret.second == false){

		map<string, data>::iterator it = result.find(aux2);
		it->second.numberOfAppears++;
	}
			
	return result;
}

map<string, data> normalize_TF(map<string, data> mapa){

	int n = mapa.size();

	for (map<string, data>::iterator it = mapa.begin(); it != mapa.end(); it++){
		
		it->second.TF_IDF = (float) it->second.numberOfAppears / n; 
		//cout << fixed;		cout << it->first << "  " << setprecision(2) << it->second.TF_IDF << endl;
	}

	return mapa;
}

vector<map<string, data>> IDF(vector<map<string, data>> mapa, int sizeCorpus){
		
	int qtdAppears = 0, n = mapa.size(), m = 0; 

	for (int k = 0; k < n; k++){
	
		for (map<string, data>::iterator j = mapa[k].begin(); j != mapa[k].end(); j++){
			
			for (int i = 0; i < n; i++){

				if (mapa[i].find(j->first) != mapa[i].end()){

					qtdAppears++;
					//cout << j->first << "  " << qtdAppears << "  " << i << endl;
				}
			}

			//cout << j->first << "  " << j->second.TF_IDF << "  " << log10((float)sizeCorpus / (1 + qtdAppears)) << "  " << j->second.TF_IDF*log10((float)sizeCorpus / (1 + qtdAppears)) << endl;
			j->second.TF_IDF = j->second.TF_IDF * log10((float)sizeCorpus / (1 + qtdAppears));
			//cout << j->first << "  " << j->second.TF_IDF << "  " << qtdAppears << endl;
			qtdAppears = 0;
			
		}

		//cout << endl;
	}
	
	return mapa;
}

float cossine_similarity(vector<map<string, data>> d1, vector<map<string, data>> d2, int posD1, int posD2){
	
	float similarity = 0, dotProdruct = 0, magnitudeD1 = 0, magnitudeD2 = 0;
		
	map<string, data>::iterator smaller = d2[posD2].begin(),
								smallerEnd = d2[posD2].end();
	map<string, data> bigger = d1[posD1];

	if (d1[posD1].size() < d2[posD2].size()){
		smaller = d1[posD1].begin();
		smallerEnd = d1[posD1].end();
		bigger = d2[posD2];
	}

	//cout << "prod: ";
	//for (map<string, data>::iterator itD1 = d1[posD1].begin(); itD1 != d1[posD1].end(); itD1++){
	for (; smaller != smallerEnd; smaller++){

		if (bigger.find(smaller->first) != bigger.end()){

			//cout << bigger.find(smaller->first)->first << "  " << smaller->first << endl;

			dotProdruct += bigger.find(smaller->first)->second.TF_IDF * smaller->second.TF_IDF;

			//cout << bigger.find(smaller->first)->second.TF_IDF << "  " << smaller->second.TF_IDF << " " << smaller->first << endl;
		}
	}
	//}

	//cout << endl;

	map<string, data>::iterator itD1 = d1[posD1].begin(), 
								itD2 = d2[posD2].begin();

	//cout << "mag1: ";
	while (itD1 != d1[posD1].end()){
	
		magnitudeD1 += (itD1->second.TF_IDF) * (itD1->second.TF_IDF);

		//cout << itD1->second.TF_IDF << "  " << itD1->first << " " << endl;

		itD1++;		
	}

	//cout << endl << "mag2: ";

	while (itD2 != d2[posD2].end()){
				
		magnitudeD2 += (itD2->second.TF_IDF) * (itD2->second.TF_IDF);

		//cout << itD2->second.TF_IDF << "  " << itD2->first << " " << endl;;

		itD2++;
	}

	/*cout << endl;

	cout << dotProdruct << "  " << magnitudeD1 << "  " << magnitudeD2 << endl;*/

	similarity = dotProdruct / (sqrt(magnitudeD1) * sqrt(magnitudeD2));

	return similarity;
}