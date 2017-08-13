#include<iostream>
#include<fstream>
#include<string>
#include <string.h>
#include<vector>
#include<algorithm>
#define MAX 200

using namespace std;

char inputtext[MAX];
char tok[5] = { ' ','.' ,'!',',','?' };
vector<int> query_idx;

//term_freq = Doc1, term_freq = Doc2, term_freq = Doc3, term_freq = Query
typedef struct posting {
	int term_freq[5];
}posting;

typedef struct term {
	string term_name;
	posting *posting_list;
}term;

int document_id = 1;
vector<term> store_terms;
double matrix[50][5] = { { 0, }, };

//create new posting
void newInsert(char* token) {
	term temp_term;
	temp_term.term_name = strlwr(token);

	posting *temp_pos = new posting();
	for (int j = 0; j < 5; j++)
		temp_pos->term_freq[j] = 0;
	temp_pos->term_freq[document_id]++;

	temp_term.posting_list = temp_pos;

	store_terms.push_back(temp_term);
}
//create posting list about term of each document
void Create(string filename) {
	//save file to inputtext
	int positon_count = 0;
	ifstream inputFile(filename);
	inputFile.getline(inputtext, MAX);

	char* token = NULL;
	token = strtok(inputtext, tok);

	while (token != NULL) {
		bool exists = false;
		for (int i = 0; i < store_terms.size(); i++) {
			//if exist
			if (store_terms[i].term_name == strlwr(token)) {
				store_terms[i].posting_list->term_freq[document_id]++;
				exists = true;
				break;
			}
		}
		if (exists != true)
			newInsert(token);
		token = strtok(NULL, tok);
	}
	document_id++;
	inputFile.close();
}
void query_p(char* q) {

	char* token = NULL;
	token = strtok(q, tok);

	while (token != NULL) {
		char c = '\n';
		if (strchr(token, c) != 0)
			token = strtok(token, "\n");
		for (int i = 0; i < store_terms.size(); i++) {
			if (store_terms[i].term_name == strlwr(token)) {
				store_terms[i].posting_list->term_freq[4]++;
				query_idx.push_back(i);
				break;
			}
		}
		token = strtok(NULL, tok);
	}
}
int calculate_document_num(int num) {
	int cnt = 0;
	for (int i = 1; i < 4; i++)
		if (store_terms[num].posting_list->term_freq[i] != 0)
			cnt++;
	return cnt;
}
//w(t,d) = 1 + log10(tf(t,d))
double calculate_tf(int a) {
	if (a == 0)
		return 0;
	else
		return 1 + log10(a);
}
//idf(t) = log10(N/df(t))
double calculate_df(int a) {
	return log10(100 / a);
}
//tf-idf = w(t,d) * idf(t)
double calculate_weight(int a, int b)
{
	if (a == 0 || b == 0)
		return 0;
	else
		return calculate_tf(a)*calculate_df(b);
}
//fix size as same
void normalization() {
	for (int k = 1; k < 5; k++) {
		double sum = 0;
		for (int i = 0; i < store_terms.size(); i++) {
			sum += matrix[i][k];
		}
		double nor = sqrt(sum);
		for (int j = 0; j < store_terms.size(); j++) {
			if (matrix[j][k] != 0)
				matrix[j][k] /= nor;
		}
	}
}
//create matrix for calculating similarity
void make_weight_matrix() {
	for (int i = 0; i < store_terms.size(); i++) {
		for (int j = 1; j < 5; j++) {
			if (j == 4)
				matrix[i][j] = calculate_weight(store_terms[i].posting_list->term_freq[j], 1);
			else
				matrix[i][j] = calculate_weight(store_terms[i].posting_list->term_freq[j], calculate_document_num(i));
		}
	}
	normalization();
}
//calculate similarity
double similarity(int term_num, int doc_num) {
	return matrix[term_num][doc_num] * matrix[term_num][4];
}

int main() {
	Create("C:\\Users\\Soeun\\Desktop\\Doc1.txt");
	Create("C:\\Users\\Soeun\\Desktop\\Doc2.txt");
	Create("C:\\Users\\Soeun\\Desktop\\Doc3.txt");

	char query[100];

	cout << "Documents to Inverted index" << endl << endl;
	for (int i = 0; i < store_terms.size(); i++) {
		cout << i<<". "<<store_terms[i].term_name << " : "<<endl;
		for (int j = 1; j < 5; j++)
			cout << j << " : " <<store_terms[i].posting_list->term_freq[j] << endl;
	}

	cout << endl;
	cout << endl;
	cout << "Input Keyword : ";
	fgets(query, sizeof(query), stdin);
	query_p(query);
	make_weight_matrix();
	cout << endl << endl;
	cout << "tf-idf 계산된 행렬" << endl;
	for (int i = 0; i < store_terms.size(); i++) {
		for (int j = 1; j < 5; j++)
			printf("%0.2f\t", matrix[i][j]);
		cout << endl;
	}
	double sim[4] = { 0, };
	int first = 0;
	cout << endl << endl;
	cout << "Similariry of each Doc ( .2f )" << endl;
	for (int j = 1; j <= 3; j++) {
		for (int i = 0; i < query_idx.size(); i++)
		{
			sim[j] += similarity(query_idx[i], j);
		}
		cout <<"Doc"<<j<<"'s Similarity : "<< sim[j] << endl;
		if (sim[first] < sim[j]) first = j;
	}
	cout<<"The most similar Doc to query : " << "Doc" << first << " -> " << sim[first]<<endl;

}