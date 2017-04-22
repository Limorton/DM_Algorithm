/*
���ر�Ҷ˹���෽����KNN��ʵ��
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
using namespace std;

//ģ�庯������string���ͱ���ת��Ϊ���õ���ֵ���ͣ��˷��������ձ������ԣ�
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}

ifstream fin;
//ȫ�ֱ��������������Ϊn+1ά
int n = 0;	//���Ը���  
int s = 0;		//���ݸ���
int c = 0;		//Ŀ���ǩ���� 
vector<int> k;		//nά��������ά�ȵ�һ��nά���� 00
vector<vector<string>> label;	//n*(k[0~n])ά����һ����ά����,�洢��ά�ı�ǩ���ַ�����
vector<string> c_detail;	//cά��Ŀ���ǩ��������
vector<vector<string>> data;	//s*(n+1)ά���������ݣ���ά����
vector<string> new_data;	//nά������������
//��Ҷ˹�����Ĳ���
vector<double> PcI;//cά���м������
vector<int> si;	//cά���洢PcI��Ӧ�����ĸ�����PcI[0~c]=si[0~c]/s
vector<vector<double>> PxkCi;//n*cά
vector<vector<int>> counts;	//n*cά���洢pxkCi��Ӧ�����ĸ�����PxkCi[0~n][0~c]=count[0~n][0~c]/PcI[0~c]
vector<double> PxCi;//cά:PxCi[0~c]=(����)PxkCi[0***n][0~c]
vector<double>PCiX;//cά:PCiX[0~c]=PxCi[0~c]*PcI[0~c]

//KNN�����Ĳ���
unsigned int Kn=0;	//����k
vector<int> like;	//sά,������������s�����������Ƴ̶�
//���ȶ������ȼ����ã�ֵС������
struct cmp
{
	bool operator () (int &a, int &b)
	{
		return like[a] > like[b];              // ��С��������ֵ С�� ���ȼ����
	}
};
priority_queue<int, vector<int>, cmp> ksample;//kά�������ҵ�k�������
vector<int> knum;//kά���洢k������������ı��
vector<int> cnum;//cά������c����ǩ�ĸ���ͳ��
/*******************����Ϊ���ݻ�ȡ��ʵ�ֺ���******************************/
//���㲽�����ĵ���ȡ���룬��ȡ���������ݵ�������Ϣ�����Եĸ������������Եı�ǩ����
//��ʼ�������������飬���ַ���������ʹ�ô˺���
void Inital_PropertyInfo(){
	string temp;//��ʱ�ַ���
	fin.open("Source_data.csv");
	getline(fin, temp, '\n');//�����һ��
	n = count(temp.begin(), temp.end(), ',');//��ȡ���Ը���
	fin.close();
	//��ʼ������n,���Ը����ı���
	k.resize(n);
	new_data.resize(n);
	label.resize(n);

	//������λ�ö�ȡ�ļ�
	/*string filename;
	cout<<"ע�⣺·����\Ӧ��Ϊ\\��֧��txt��ʽ"<<endl;
	cout << "��������˵���ļ�·����" << endl;
	getline(cin, filename, '\n');
	fin.open(filename);*/
	fin.open("data_form.txt");//��ȡ����˵���ĵ�	
	getline(fin, temp, ',');
	c = atoi(temp.c_str());//��ʼ��c
	//cout << "C:" << c << endl;
	//��ʼ������c�ı���
	c_detail.resize(c);
	//��ʼ����Ŀ���ǩ��������
	for (int i = 0; i < c; i++){
		getline(fin, c_detail[i], ',');
		//cout << c_detail[i] << endl;
	}
	//��ʼ�����������Եı�ǩ��������
	for (int i = 1; i < n; i++){
		getline(fin, temp, ',');
		k[i] = atoi(temp.c_str());
		label[i].resize(k[i]);
		for (int j = 0; j < k[i]; j++){
			getline(fin, label[i][j], ',');
		}
	}	
	fin.close();

	cout << "���������ݸ�����" << endl;
	cin >> s;	//������������
	data.resize(s);
	for (int i = 0; i < s; i++){ data[i].resize(n + 1); }
}
//��һ�������ļ���ȡ����
void Deal_Data(){
	//������λ�ö�ȡ�ļ�
	/*string filename;
	cout<<"ע�⣺·����\Ӧ��Ϊ\\��ֻ֧��.csv��ʽ"<<endl;
	cout << "�����ļ�·����" << endl;
	getline(cin, filename, '\n');
	fin.open(filename);*/
	string temp;
	Inital_PropertyInfo();
	fin.open("Source_data.csv");
	getline(fin, temp, '\n');//�����һ��,������
	//��������
	for (int i = 0; i < s; i++){
		for (int j = 0; j < n; j++){
			getline(fin, temp, ',');
			data[i][j] = temp;
		}
		getline(fin, temp, '\n');
		data[i][n] = temp;
	}
	fin.close();
	cout << "��һ�����������ݣ�����ɹ�" << endl;
	cout << "����ʾ����";
	for (int i = 0; i <= n; i++){ cout << data[0][i] << " "; }
	cout << endl << endl;
}
/*******************����Ϊ��Ҷ˹������ʵ�ֺ���***************************/
//�ڶ���������P(Ci)
void  CalculateP_Ci(){
	//��Ҷ˹������ʼ��
	PcI.resize(c);
	si.resize(c);
	PxkCi.resize(n);
	counts.resize(n);
	PxCi.resize(c);
	PCiX.resize(c);
	for (int i = 0; i < n; i++)
	{
		PxkCi[i].resize(c);
		counts[i].resize(c);
		for (int j = 0; j < c; j++){
			counts[i][j] = 0;
		}
	}	
	//�������
	string temp;
	cout << "���еڶ���������P(Ci)" << endl;
	for (int i = 0; i < c; i++){ si[i] = 0; }
	for (int i = 0; i < s; i++){
		for (int j = 0; j < c; j++){
			if (data[i][n] == c_detail[j])
			{	si[j]++; break;		}
		}
	}
	cout << "���������£�" << endl;
	for (int i = 0; i < c; i++){
		PcI[i] = double(si[i]) / s;	//����PcI
		cout << c_detail[i] << ":" << si[i] << "  P(" << c_detail[i]
			<< ")=" << PcI[i] << endl;
	}
}
//�������������ؼ��裬�������໥��������������P(Xk|Ci)
void CalculatePxkCi(){
	cout << endl << "������������P(Xk|Ci):"<<endl;
	//��ȡ����������
	cout << "���������������,�Կո�ֿ���" << endl;
	for (int i = 0; i < n; i++){ cin >> new_data[i]; }
	cout << "�����Ե�P(Xk|Ci)�������£�" << endl;
	//����PxkCi
	for (int i = 1; i < n; i++){	//������ѭ��
		for (int j = 0; j < c; j++){	//��Ŀ���ǩѭ��
			for (int m = 0; m < s; m++){	//������ѭ��
				if (data[m][n] == c_detail[j] && data[m][i] == new_data[i])
				{	counts[i][j]++;		}
				else continue;
			}
			PxkCi[i][j] = double(counts[i][j]) / si[j];
			cout <<c_detail[j] << ":" <<PxkCi[i][j] << endl;
		}
	}
}
//���Ĳ�������P(X|Ci)��P(Ci|X)
void CalculatePxCi(){
	cout << endl << "���Ĳ�������P(X|Ci)��P(Ci|X)��" << endl;
	cout << "�����" << "P(X|Ci)  *   P(Ci)  =   P(Ci|X)" << endl;
	for (int i = 0; i < c; i++){
		PxCi[i] = 1;
		for (int j = 1; j < n; j++){
			PxCi[i] *= PxkCi[j][i];
		}
		PCiX[i] = PxCi[i] * PcI[i];
		cout <<c_detail[i] << " : " << PxCi[i] << "  * " << PcI[i] << "  =  " << PCiX[i] << endl;
	}
}
//���岽���ҵ�����P(Ci|X)������P(Ci)*P(X|Ci)������Ci,��Ϊ������
void Get_Max_Ci(){
	cout << endl << "���岽���ҵ����ֵ��" << endl;
	int max_num = 0;
	double max = PCiX[0];
	//cout << "����ǩ�������£�" << endl;
	for (int i = 0; i < c; i++){
		//cout << c_detail[i] << " �ĸ���:" << PCiX[i] << endl;
		if (PCiX[i]>max)
		{
			max_num = i;
			max = PCiX[i];
		}
	}
	cout << "������Ϊ��" << c_detail[max_num] << " " << max << endl;
	cout << "��������" << c_detail[max_num] << endl;
}
/********************����ΪKNN������ʵ�ֺ���*****************************/
//�������ݵ����ƶ�
void CalculatLike(){
	//��ȡ����������
	cout << "���������������,�Կո�ֿ���" << endl;
	for (int i = 0; i < n; i++){ cin >> new_data[i]; }

	like.resize(s);
	for (int i = 0; i < s; i++){
		like[i]= 0;
		for (int j = 1; j < n; j++){
			if (data[i][j] == new_data[j])
				like[i]++;
		}
		//cout << like[i] << endl;
	}
}
//�ҵ�k�����������
void GetKsamples(){
	cout << "������Kֵ��" << endl;
	cin >> Kn;
	//��ȡs��������likeֵ����kn������
	for (int j = 0; j < s;j++){
		if(ksample.size() < Kn)
			ksample.push(j);
		else{
			ksample.pop();
			ksample.push(j);
		}
	}
	int num = 0;
	knum.resize(Kn);
	while(!ksample.empty()){
		 knum[num++]=ksample.top();
		ksample.pop();
		//if(num<=Kn) cout << knum[num-1] << endl;
	}
}
//����k��������е��������������
void CalculateMax(){
	cnum.resize(c);
	for (int i = 0; i < c;i++){
		cnum[i] = 0;
		for (int j = 0; j < Kn; j++){
			if (data[knum[j]][n] == c_detail[i])
			{
				cnum[i]++;
			}
		}
		cout << c_detail[i] << "�ĸ���:" << cnum[i] << endl;
	}
	int maxNum=0;
	for (int i = 0; i < c; i++){
		if (cnum[i]>maxNum)
			maxNum = i;
	}
	cout << "��������" << c_detail[maxNum] << endl;
}