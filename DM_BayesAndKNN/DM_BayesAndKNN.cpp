// DM_BayesAndKNN.cpp : �������̨Ӧ�ó������ڵ㡣
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
		cout << "���з��෽����" << endl << "1  ��Ҷ˹����" 
			<< endl << "2  K���ڷ���" << endl << "ѡ����෽����";
		int choice;
		cin >> choice;
		//��Ҷ˹����
		if (choice == 1){	
			CalculateP_Ci();
			char loop = 'Y';
			while (loop != 'N'){
				CalculatePxkCi();
				CalculatePxCi();
				Get_Max_Ci();
				cout << "����ʹ�ñ�Ҷ˹�����𣿣�Y/N��:";
				cin >> loop;
				system("cls");
			}
		}
		//K���ڷ���
		else
		{
			char loop = 'Y';
			while (loop != 'N'){
				CalculatLike();
				GetKsamples();
				CalculateMax();
				cout << "����ʹ��KNN�����𣿣�Y/N��:";
				cin >> loop;
				system("cls");
			}
		}
		cout << "�Ƿ��˳�����(Y/N)" << endl;
		cin >> i;
	}
	system("pause");
	return 0;
}

