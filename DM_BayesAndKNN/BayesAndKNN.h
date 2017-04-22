/*
朴素贝叶斯分类方法和KNN的实现
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

//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}

ifstream fin;
//全局变量，总输入个数为n+1维
int n = 0;	//属性个数  
int s = 0;		//数据个数
int c = 0;		//目标标签个数 
vector<int> k;		//n维：各属性维度的一个n维数组 00
vector<vector<string>> label;	//n*(k[0~n])维：的一个二维数组,存储各维的标签的字符描述
vector<string> c_detail;	//c维：目标标签描述数组
vector<vector<string>> data;	//s*(n+1)维：输入数据，二维数组
vector<string> new_data;	//n维：待分类样本
//贝叶斯方法的参数
vector<double> PcI;//c维：中间计算结果
vector<int> si;	//c维：存储PcI对应样本的个数：PcI[0~c]=si[0~c]/s
vector<vector<double>> PxkCi;//n*c维
vector<vector<int>> counts;	//n*c维，存储pxkCi对应样本的个数：PxkCi[0~n][0~c]=count[0~n][0~c]/PcI[0~c]
vector<double> PxCi;//c维:PxCi[0~c]=(连乘)PxkCi[0***n][0~c]
vector<double>PCiX;//c维:PCiX[0~c]=PxCi[0~c]*PcI[0~c]

//KNN方法的参数
unsigned int Kn=0;	//参数k
vector<int> like;	//s维,待分类样本和s个样本的相似程度
//优先队列优先级设置，值小的优先
struct cmp
{
	bool operator () (int &a, int &b)
	{
		return like[a] > like[b];              // 从小到大排序，值 小的 优先级别高
	}
};
priority_queue<int, vector<int>, cmp> ksample;//k维，用于找到k个最近邻
vector<int> knum;//k维，存储k个最近邻样本的编号
vector<int> cnum;//c维，储存c个标签的个数统计
/*******************以下为数据获取的实现函数******************************/
//第零步：从文档获取输入，获取待分类数据的特征信息：属性的个数，各个属性的标签个数
//初始化各个容器数组，两种方法都可以使用此函数
void Inital_PropertyInfo(){
	string temp;//临时字符串
	fin.open("Source_data.csv");
	getline(fin, temp, '\n');//读入第一行
	n = count(temp.begin(), temp.end(), ',');//获取属性个数
	fin.close();
	//初始化依赖n,属性个数的变量
	k.resize(n);
	new_data.resize(n);
	label.resize(n);

	//从任意位置读取文件
	/*string filename;
	cout<<"注意：路径符\应改为\\，支持txt格式"<<endl;
	cout << "输入数据说明文件路径：" << endl;
	getline(cin, filename, '\n');
	fin.open(filename);*/
	fin.open("data_form.txt");//获取数据说明文档	
	getline(fin, temp, ',');
	c = atoi(temp.c_str());//初始化c
	//cout << "C:" << c << endl;
	//初始化依赖c的变量
	c_detail.resize(c);
	//初始化：目标标签描述数组
	for (int i = 0; i < c; i++){
		getline(fin, c_detail[i], ',');
		//cout << c_detail[i] << endl;
	}
	//初始化：各个属性的标签描述数组
	for (int i = 1; i < n; i++){
		getline(fin, temp, ',');
		k[i] = atoi(temp.c_str());
		label[i].resize(k[i]);
		for (int j = 0; j < k[i]; j++){
			getline(fin, label[i][j], ',');
		}
	}	
	fin.close();

	cout << "请输入数据个数：" << endl;
	cin >> s;	//输入数据行数
	data.resize(s);
	for (int i = 0; i < s; i++){ data[i].resize(n + 1); }
}
//第一步：从文件获取数据
void Deal_Data(){
	//从任意位置读取文件
	/*string filename;
	cout<<"注意：路径符\应改为\\，只支持.csv格式"<<endl;
	cout << "输入文件路径：" << endl;
	getline(cin, filename, '\n');
	fin.open(filename);*/
	string temp;
	Inital_PropertyInfo();
	fin.open("Source_data.csv");
	getline(fin, temp, '\n');//读入第一行,非数据
	//读入数据
	for (int i = 0; i < s; i++){
		for (int j = 0; j < n; j++){
			getline(fin, temp, ',');
			data[i][j] = temp;
		}
		getline(fin, temp, '\n');
		data[i][n] = temp;
	}
	fin.close();
	cout << "第一步，导入数据，导入成功" << endl;
	cout << "数据示例：";
	for (int i = 0; i <= n; i++){ cout << data[0][i] << " "; }
	cout << endl << endl;
}
/*******************以下为贝叶斯方法的实现函数***************************/
//第二步：计算P(Ci)
void  CalculateP_Ci(){
	//贝叶斯变量初始化
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
	//计算过程
	string temp;
	cout << "进行第二步，计算P(Ci)" << endl;
	for (int i = 0; i < c; i++){ si[i] = 0; }
	for (int i = 0; i < s; i++){
		for (int j = 0; j < c; j++){
			if (data[i][n] == c_detail[j])
			{	si[j]++; break;		}
		}
	}
	cout << "计算结果如下：" << endl;
	for (int i = 0; i < c; i++){
		PcI[i] = double(si[i]) / s;	//计算PcI
		cout << c_detail[i] << ":" << si[i] << "  P(" << c_detail[i]
			<< ")=" << PcI[i] << endl;
	}
}
//第三步：做朴素假设，各属性相互条件独立，计算P(Xk|Ci)
void CalculatePxkCi(){
	cout << endl << "第三步，计算P(Xk|Ci):"<<endl;
	//获取待分类样本
	cout << "请输入待分类样本,以空格分开：" << endl;
	for (int i = 0; i < n; i++){ cin >> new_data[i]; }
	cout << "各属性的P(Xk|Ci)计算如下：" << endl;
	//计算PxkCi
	for (int i = 1; i < n; i++){	//对属性循环
		for (int j = 0; j < c; j++){	//对目标标签循环
			for (int m = 0; m < s; m++){	//对数据循环
				if (data[m][n] == c_detail[j] && data[m][i] == new_data[i])
				{	counts[i][j]++;		}
				else continue;
			}
			PxkCi[i][j] = double(counts[i][j]) / si[j];
			cout <<c_detail[j] << ":" <<PxkCi[i][j] << endl;
		}
	}
}
//第四步：计算P(X|Ci)和P(Ci|X)
void CalculatePxCi(){
	cout << endl << "第四步，计算P(X|Ci)和P(Ci|X)：" << endl;
	cout << "结果：" << "P(X|Ci)  *   P(Ci)  =   P(Ci|X)" << endl;
	for (int i = 0; i < c; i++){
		PxCi[i] = 1;
		for (int j = 1; j < n; j++){
			PxCi[i] *= PxkCi[j][i];
		}
		PCiX[i] = PxCi[i] * PcI[i];
		cout <<c_detail[i] << " : " << PxCi[i] << "  * " << PcI[i] << "  =  " << PCiX[i] << endl;
	}
}
//第五步：找到最大的P(Ci|X)正比于P(Ci)*P(X|Ci)所属的Ci,即为分类结果
void Get_Max_Ci(){
	cout << endl << "第五步，找到最大值：" << endl;
	int max_num = 0;
	double max = PCiX[0];
	//cout << "各标签概率如下：" << endl;
	for (int i = 0; i < c; i++){
		//cout << c_detail[i] << " 的概率:" << PCiX[i] << endl;
		if (PCiX[i]>max)
		{
			max_num = i;
			max = PCiX[i];
		}
	}
	cout << "最大概率为：" << c_detail[max_num] << " " << max << endl;
	cout << "分类结果：" << c_detail[max_num] << endl;
}
/********************以下为KNN方法的实现函数*****************************/
//计算数据的相似度
void CalculatLike(){
	//获取待分类样本
	cout << "请输入待分类样本,以空格分开：" << endl;
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
//找到k个最近的样本
void GetKsamples(){
	cout << "请输入K值：" << endl;
	cin >> Kn;
	//获取s个样本中like值最大的kn个样本
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
//计算k个最近邻中的样本的最多属性
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
		cout << c_detail[i] << "的个数:" << cnum[i] << endl;
	}
	int maxNum=0;
	for (int i = 0; i < c; i++){
		if (cnum[i]>maxNum)
			maxNum = i;
	}
	cout << "分类结果：" << c_detail[maxNum] << endl;
}