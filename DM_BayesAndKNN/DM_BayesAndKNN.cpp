// DM_BayesAndKNN.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "BayesAndKNN.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	Deal_Data();
	char i = 'Y';
	while (i != 'N'){
		cout << "现有分类方法：" << endl << "1  贝叶斯方法" 
			<< endl << "2  K近邻方法" << endl << "选择分类方法：";
		int choice;
		cin >> choice;
		//贝叶斯方法
		if (choice == 1){	
			CalculateP_Ci();
			char loop = 'Y';
			while (loop != 'N'){
				CalculatePxkCi();
				CalculatePxCi();
				Get_Max_Ci();
				cout << "继续使用贝叶斯方法吗？（Y/N）:";
				cin >> loop;
				system("cls");
			}
		}
		//K近邻方法
		else
		{
			char loop = 'Y';
			while (loop != 'N'){
				CalculatLike();
				GetKsamples();
				CalculateMax();
				cout << "继续使用KNN方法吗？（Y/N）:";
				cin >> loop;
				system("cls");
			}
		}
		cout << "是否退出程序？(Y/N)" << endl;
		cin >> i;
	}
	system("pause");
	return 0;
}

