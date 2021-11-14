// dbms.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <map>
#include<vector>
#include<array>
#include<fstream>
#include <time.h>
#include"bpt.h"

using namespace std;






string dataDefine = "int_char(25)_varchar(140)_int_char(25)_float_varchar(101)";
string inputData = "小明_山西省太原市_2332_15234555454_2133.23_哈哈哈哈啊哈";
string inputData2 = "王大勇_北京_2332_15443245434_2667.767_嘿嘿嘿嘿嘿";

vector<string> vs = { "123_小张_北京市海淀区_321_15443245434_2667.767_嘿嘿嘿嘿嘿",
"大志_北京_111_15443245434_2667.767_白日依山尽",
"小轮_北京_001_15443245434_2667.767_黄河入海流",
"胡闹_北京_243_15443245434_2667.767_清明时节雨纷纷" };

class dataDictResolver
{
private:

	map<string, int> data_dict;



public:
	int column_num;
	vector<int> dataDef;//每个字段长度
	vector<string> data;//每条数据数组
	vector<unsigned char *> datas;//序列化的数据数组
	vector<string>dataDefs;//每个字段类型字符串


	vector<string> dataDefs_load;//只加载数据字典得到的每个字段类型
	vector<int> dataDef_load;//只加载数据字典的到的每个字段大小
	int column_num_load;//只加载数据字典得到的字段数
	dataDictResolver()
	{
		data_dict["int"] = sizeof(int);
		data_dict["float"] = sizeof(float);
		data_dict["double"] = sizeof(double);
		data_dict["date"] = 11;

	}
	void clear()
	{
		column_num = 0;
		dataDef.clear();
		data.clear();
		datas.clear();
		dataDefs.clear();
	}
	int loadDict(string dataDefine)
	{
		vector<string> dataD;
		this->SplitString(dataDefine, dataD, "_");
		this->column_num_load = dataD.size();

		for (int i = 0; i < dataD.size(); i++)
		{

			if (dataD[i].find("varchar") < 0 && dataD[i].find("char") < 0 && data_dict.find(dataD[i]) == data_dict.end())
			{
				cout << "error!" << dataD[i] << endl;
				return -1;
			}
			else
			{
				int indexvarchar = dataD[i].find("varchar");
				int indexchar = dataD[i].find("char");
				if (indexvarchar >= 0)
				{
					int indexup = indexvarchar + 8;
					string size = dataD[i].substr(indexup, dataD[i].length() - 1 - indexup);
					int num = std::stoi(size);//varchar定义大小
					dataDefs_load.push_back("varchar");
					dataDef_load.push_back(num);

				}
				else if (indexchar >= 0)
				{

					int indexup = indexchar + 5;
					string size = dataD[i].substr(indexup, dataD[i].length() - 1 - indexup);
					int num = std::stoi(size);
					dataDefs_load.push_back("char");
					dataDef_load.push_back(num);
				}
				else
				{
					int size = data_dict[dataD[i]];
					dataDefs_load.push_back(dataD[i]);
					dataDef_load.push_back(size);
				}
			}
		}
		return 0;
	}
	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
	{
		std::string::size_type pos1, pos2;
		pos2 = s.find(c);
		pos1 = 0;
		while (std::string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2 - pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != s.length())
			v.push_back(s.substr(pos1));
	}
	void deleteDatas()
	{
		for (int i = 0; i < datas.size(); i++) {
			if (datas[i] != NULL)
				free(datas[i]);
		}
	}


	int resolveByVector(string dataDefine, vector<string> inputData)
	{

		vector<string> dataD;
		vector<int> result;
		this->SplitString(dataDefine, dataD, "_");
		vector<string> inputs;
		inputs = inputData;

		this->column_num = dataD.size();


		if (dataD.size() != inputs.size())
		{
			cout << "输入和数据字段个数不匹配！" << endl;
			return -1;
		}

		for (int i = 0; i < dataD.size(); i++)
		{

			//cout << dataD[i] << "|"<< inputs[i]<<"|";
			if (dataD[i].find("varchar") < 0 && dataD[i].find("char") < 0 && data_dict.find(dataD[i]) == data_dict.end())
			{
				cout << "error!" << dataD[i] << endl;
				return -1;
			}
			else
			{

				int indexvarchar = dataD[i].find("varchar");
				int indexchar = dataD[i].find("char");
				if (indexvarchar >= 0)
				{

					int indexup = indexvarchar + 8;
					string size = dataD[i].substr(indexup, dataD[i].length() - 1 - indexup);
					int num = std::stoi(size);//varchar定义大小
					int size2 = inputs[i].size();//实际字符串大小
					size2++;

					int actualSize = size2;
					if (size2 > num)
					{
						cout << "varchar超过最长字段！" << endl;
						return -1;
					}


					unsigned char *d = (unsigned char *)malloc(actualSize + sizeof(int));

					memcpy(d, &actualSize, sizeof(int));

					memcpy(d + sizeof(int), inputs[i].c_str(), actualSize);

					datas.push_back(d);
					dataDefs.push_back("varchar");
					result.push_back(actualSize + sizeof(int));


				}
				else if (indexchar >= 0)
				{

					int indexup = indexchar + 5;
					string size = dataD[i].substr(indexup, dataD[i].length() - 1 - indexup);
					int num = std::stoi(size);
					int size2 = inputs[i].size();
					size2++;

					result.push_back(num);
					if (size2 > num) {
						cout << "数据太长超过char!" << endl;
						return -1;
					}
					unsigned char *d = (unsigned char *)malloc(num);


					memcpy(d, inputs[i].c_str(), size2);
					datas.push_back(d);
					dataDefs.push_back("char");

				}
				else
				{

					int size = data_dict[dataD[i]];
					if (dataD[i].compare("int") == 0)
					{



						int data = stoi(inputs[i].c_str());

						unsigned char *d = (unsigned char *)malloc(size);
						memcpy(d, &data, size);

						datas.push_back(d);
						dataDefs.push_back(dataD[i]);


					}
					else if (dataD[i].compare("float") == 0)
					{
						float data = stof(inputs[i].c_str());
						unsigned char *d = (unsigned char *)malloc(size);
						memcpy(d, &data, size);
						datas.push_back(d);
						dataDefs.push_back(dataD[i]);

					}
					else if (dataD[i].compare("date") == 0)
					{

						unsigned char *d = (unsigned char *)malloc(size);
						memcpy(d, inputs[i].c_str(), size);
						datas.push_back(d);
						dataDefs.push_back(dataD[i]);



					}
					else if (dataD[i].compare("double") == 0)
					{

						double data = stod(inputs[i].c_str());
						unsigned char *d = (unsigned char *)malloc(size);
						memcpy(d, &data, size);
						datas.push_back(d);
						dataDefs.push_back(dataD[i]);
					}

					result.push_back(size);
				}
			}
		}

		dataDef = result;
		data = inputs;
		return 0;
	}
	int resolve(string dataDefine, string inputData)
	{

		vector<string> dataD;
		vector<int> result;
		this->SplitString(dataDefine, dataD, "_");
		vector<string> inputs;
		this->SplitString(inputData, inputs, "_");

		this->column_num = dataD.size();


		if (dataD.size() != inputs.size())
		{
			cout << "输入和数据字段个数不匹配！" << endl;
			return -1;
		}

		for (int i = 0; i < dataD.size(); i++)
		{

			//cout << dataD[i] << "|"<< inputs[i]<<"|";
			if (dataD[i].find("varchar") < 0 && dataD[i].find("char") < 0 && data_dict.find(dataD[i]) == data_dict.end())
			{
				cout << "error!" << dataD[i] << endl;
				return -1;
			}
			else
			{

				int indexvarchar = dataD[i].find("varchar");
				int indexchar = dataD[i].find("char");
				if (indexvarchar >= 0)
				{

					int indexup = indexvarchar + 8;
					string size = dataD[i].substr(indexup, dataD[i].length() - 1 - indexup);
					int num = std::stoi(size);//varchar定义大小
					int size2 = inputs[i].size();//实际字符串大小
					size2++;

					int actualSize = size2;
					if (size2 > num)
					{
						cout << "varchar超过最长字段！" << endl;
						return -1;
					}


					unsigned char *d = (unsigned char *)malloc(actualSize + sizeof(int));

					memcpy(d, &actualSize, sizeof(int));

					memcpy(d + sizeof(int), inputs[i].c_str(), actualSize);

					datas.push_back(d);
					dataDefs.push_back("varchar");
					result.push_back(actualSize + sizeof(int));


				}
				else if (indexchar >= 0)
				{

					int indexup = indexchar + 5;
					string size = dataD[i].substr(indexup, dataD[i].length() - 1 - indexup);
					int num = std::stoi(size);
					int size2 = inputs[i].size();
					size2++;

					result.push_back(num);
					if (size2 > num) {
						cout << "数据太长超过char!" << endl;
						return -1;
					}
					unsigned char *d = (unsigned char *)malloc(num);


					memcpy(d, inputs[i].c_str(), size2);
					datas.push_back(d);
					dataDefs.push_back("char");

				}
				else
				{

					int size = data_dict[dataD[i]];
					if (dataD[i].compare("int") == 0)
					{



						int data = stoi(inputs[i].c_str());

						unsigned char *d = (unsigned char *)malloc(size);
						memcpy(d, &data, size);

						datas.push_back(d);
						dataDefs.push_back(dataD[i]);


					}
					else if (dataD[i].compare("float") == 0)
					{
						float data = stof(inputs[i].c_str());
						unsigned char *d = (unsigned char *)malloc(size);
						memcpy(d, &data, size);
						datas.push_back(d);
						dataDefs.push_back(dataD[i]);

					}
					else if (dataD[i].compare("date") == 0)
					{

						unsigned char *d = (unsigned char *)malloc(size);
						memcpy(d, inputs[i].c_str(), size);
						datas.push_back(d);
						dataDefs.push_back(dataD[i]);



					}
					else if (dataD[i].compare("double") == 0)
					{

						double data = stod(inputs[i].c_str());
						unsigned char *d = (unsigned char *)malloc(size);
						memcpy(d, &data, size);
						datas.push_back(d);
						dataDefs.push_back(dataD[i]);
					}

					result.push_back(size);
				}
			}
		}

		dataDef = result;
		data = inputs;
		return 0;
	}
	void tupleToStrings(vector<string>&tupleStrings, unsigned char *tuple)
	{
		int index = 0;



		for (int i = 0; i < this->column_num_load; i++)
		{

			if (dataDefs_load[i].compare("int") == 0)
			{
				int size = dataDef_load[i];
				int data;
				memcpy(&data, tuple + index, size);
				string dataStr = to_string(data);
				tupleStrings.push_back(dataStr);
				index += size;
				//cout << dataStr << endl;

			}
			else if (dataDefs_load[i].compare("float") == 0)
			{
				int size = dataDef_load[i];
				float data;
				memcpy(&data, tuple + index, size);
				string dataStr = to_string(data);
				tupleStrings.push_back(dataStr);
				index += size;
				// cout << dataStr << endl;
			}
			else if (dataDefs_load[i].compare("double") == 0)
			{
				int size = dataDef_load[i];
				double data;
				memcpy(&data, tuple + index, size);
				string dataStr = to_string(data);
				tupleStrings.push_back(dataStr);
				index += size;

				// cout << dataStr << endl;
			}
			else if (dataDefs_load[i].compare("char") == 0)
			{
				int size = dataDef_load[i];
				char *data;
				data = (char *)malloc(size);
				memcpy(data, tuple + index, size);
				string dataStr(data);
				tupleStrings.push_back(dataStr);
				index += size;
				free(data);
				//cout << dataStr << endl;

			}
			else if (dataDefs_load[i].compare("varchar") == 0)
			{
				//int size = dataDef_load[i];

				int head;
				memcpy(&head, tuple + index, sizeof(int));
				index += sizeof(int);
				char *data;
				data = (char *)malloc(head);
				memcpy(data, tuple + index, head);
				string dataStr(data);
				tupleStrings.push_back(dataStr);
				index += head;
				free(data);
				//cout << dataStr << endl;
			}
			else if (dataDefs_load[i].compare("date") == 0)
			{
				int size = dataDef_load[i];
				char *data;
				data = (char *)malloc(size);
				memcpy(data, tuple + index, size);
				string dataStr(data);
				tupleStrings.push_back(dataStr);
				index += size;
				free(data);
				//cout << dataStr << endl;

			}
		}

	}







};

class tuple_producer
{
private:
	unsigned char *tuple;
	int tupleLength;

public:
	tuple_producer()
	{
		tupleLength = 0;
	}
	int getTupleLength()
	{
		return this->tupleLength;
	}

	unsigned char* produceTupleTemp(vector<int> dataDefine, vector<unsigned char*> datas)
	{
		int currentIndex = 0;
		for (int i = 0; i < dataDefine.size(); i++)
		{
			tupleLength += dataDefine[i];
		}
		tuple = (unsigned char*)malloc(tupleLength * sizeof(unsigned char));

		for (int i = 0; i < dataDefine.size(); i++)
		{
			memset(tuple + currentIndex, 0, dataDefine[i]);
			memcpy(tuple + currentIndex, datas[i], dataDefine[i]);
			currentIndex += dataDefine[i];
		}
		return tuple;
	}
	void clear()
	{
		if (tuple != NULL)
			free(tuple);
		tupleLength = 0;
	}



};

#define OFFSETERNUM 64
#define BLOCKSIZE 4*1024
int curBlockNum = 0;


struct offset
{
	int pos;
	int length;
	int isActive;
	int isDelete;

};
class offsetTable
{
	int noActiveNum;
	int deleteNum;
	int index;


	array<offset, OFFSETERNUM> offsets;

public:
	offsetTable() {

	}
	void init()
	{
		noActiveNum = OFFSETERNUM;
		index = 0;
		deleteNum = 0;

		for (int i = 0; i < OFFSETERNUM; i++)
		{
			offsets[i].isActive = 0;
			offsets[i].isDelete = 0;
			offsets[i].length = 0;
			offsets[i].pos = 0;
		}
	}
	int addOffset(int pos, int length, int &offsetOut)
	{
		if (index < OFFSETERNUM) {
			offsetOut = index;
			offsets[index].isActive = 1;
			offsets[index].isDelete = 0;
			offsets[index].length = length;
			offsets[index].pos = pos;
			index++;
			noActiveNum--;
			return 0;
		}
		else {

			return -1;
		}
	}
	int getNoActiveNum()
	{
		return noActiveNum;
	}
	int getActiveNum()
	{
		return OFFSETERNUM-noActiveNum;
	}
	offset getOffset(int i)
	{
		if (i < OFFSETERNUM)
			return this->offsets[i];
	}
	int changeDeleteOffset(int index)
	{
		this->offsets[index].isDelete = 0;
		return this->offsets[index].pos;
	}

	int useDeleteOffset(int length, int &index)
	{
		for (int i = 0; i < offsets.size(); i++)
		{
			if (offsets[i].isDelete == 1)
			{
				if (length <= offsets[i].length)
				{
					offsets[i].isDelete = 0;
					deleteNum--;
					index = i;
					return changeDeleteOffset(i);
				}
			}

		}
		return -1;
	}
	int getCurrentIndex()
	{
		return this->index;
	}
	int getDeleteNum()
	{
		return deleteNum;
	}
	void deleteOffset(int index)
	{
		offsets[index].isDelete = 1;
		deleteNum++;

	}





};

int maxblocknumer = 0;
struct blockHeader
{
	int magic;
	int tupleBeginOffset;
	int LongestOffset;
	int blockNum;
	int isfull;
	int tupleNum;


};
class block
{
private:

	unsigned char *memBlock;
	blockHeader *bh;
	offsetTable *ost;



public:
	block()
	{
		memBlock = NULL;
		bh = NULL;
		ost = NULL;
	}
	int newBlock()
	{
		memBlock = (unsigned char *)malloc(BLOCKSIZE);
		if (memBlock == NULL) {
			cout << "空间分配失败!" << endl;
			return -1;
		}
		else
			return 0;
	}
	int format()
	{
		if (memBlock != NULL) {

			int index = 0;
			
			bh = (blockHeader*)malloc(sizeof(blockHeader));
			bh->blockNum = curBlockNum;
			bh->LongestOffset = sizeof(blockHeader) + sizeof(offsetTable);
			bh->tupleBeginOffset = sizeof(blockHeader) + sizeof(offsetTable);
			bh->isfull = 0;
			bh->tupleNum = 0;
			bh->magic = 199435;
			curBlockNum++;

			ost = new offsetTable();
			ost->init();

			memcpy(memBlock, bh, sizeof(blockHeader));
			index += sizeof(blockHeader);
			memcpy(memBlock + index, ost, sizeof(offsetTable));

			return 0;
		}
		else
		{
			cout << "memblock指针为空！" << endl;
		}
		return -1;
	}
	int formatWithBlockNum(int blockNum)
	{
		if (memBlock != NULL) {

			int index = 0;
			bh = (blockHeader*)malloc(sizeof(blockHeader));
			bh->blockNum = blockNum;
			bh->LongestOffset = sizeof(blockHeader) + sizeof(offsetTable);
			bh->tupleBeginOffset = sizeof(blockHeader) + sizeof(offsetTable);
			bh->isfull = 0;
			bh->magic = 199435;
			curBlockNum++;
			bh->tupleNum = 0;
			ost = new offsetTable();
			ost->init();

			memcpy(memBlock, bh, sizeof(blockHeader));
			index += sizeof(blockHeader);
			memcpy(memBlock + index, ost, sizeof(offsetTable));

			return 0;
		}
		else
		{
			cout << "memblock指针为空！" << endl;
		}
		return -1;
	}
	void clean()
	{
		if (bh != NULL)
			free(bh);
		if (memBlock != NULL)
			free(memBlock);
		if (ost != NULL)
			free(ost);

	}
	int tupleIsDeleted(int offset)
	{
		if (offset > this->ost->getActiveNum())
			return -1;
		return ost->getOffset(offset).isDelete;
	}
	int tupleIsActivated(int offset)
	{
		if (offset > this->ost->getActiveNum())
			return -1;
		return ost->getOffset(offset).isActive;
	}
	void loadABlock(unsigned char *block)
	{
		clean();
		memBlock = (unsigned char *)malloc(BLOCKSIZE);
		memcpy(memBlock, block, BLOCKSIZE);
		bh = (blockHeader*)malloc(sizeof(blockHeader));
		memcpy(bh, memBlock, sizeof(blockHeader));
		ost = new offsetTable();
		memcpy(ost, memBlock + sizeof(blockHeader), sizeof(offsetTable));
	}

	unsigned char * blockIntegrate()
	{
		memcpy(memBlock, bh, sizeof(blockHeader));
		memcpy(memBlock + sizeof(blockHeader), ost, sizeof(offsetTable));
		return memBlock;
	}

	void blockInfo()
	{
		cout << "未激活节点数" << ost->getNoActiveNum() << endl;
		cout << "block号" << bh->blockNum << endl;
		cout << "最长偏移" << bh->LongestOffset << endl;
		cout << "元组开始偏移" << bh->tupleBeginOffset << endl;
		cout << "元组数目" << bh->tupleNum << endl;

	}
	int getBlockNum()
	{
		return bh->blockNum;
	}


	int insertATuple(unsigned char *tuple, int length)
	{
		

		int insertOffset = 0;
		//cout << "插入！" << this->getBlockNum();
		if (bh->LongestOffset + length > BLOCKSIZE)
		{
			//cout << "块空间不足！" << endl;

			if (ost->getDeleteNum() > 0)
			{
				int pos = ost->useDeleteOffset(length, insertOffset);

				if (pos >= 0) {

					memcpy(memBlock + pos, tuple, length);
					bh->tupleNum++;
					cout << "利用了一个删除空间！" << endl;


					return 0;
				}
			}

		}
		else if (ost->getNoActiveNum() > 0) {

			ost->addOffset(bh->LongestOffset, length, insertOffset);
			memcpy(memBlock + bh->LongestOffset, tuple, length);
			bh->LongestOffset += length;
			bh->tupleNum++;
			return 0;
		}
		else {
			//cout << "无空闲偏移点以及无删除偏移点可以利用！" << endl;
			bh->isfull = 1;
			return -1;
		}
		return -1;
	}
	int insertATupleReturnOffset(unsigned char *tuple, int length, int &insertOffset)
	{
		//cout << "插入！" << this->getBlockNum();
		if (bh->LongestOffset + length > BLOCKSIZE)
		{
			//cout << "块空间不足！" << endl;

			if (ost->getDeleteNum() > 0)
			{
				int pos = ost->useDeleteOffset(length, insertOffset);

				if (pos >= 0) {

					memcpy(memBlock + pos, tuple, length);
					bh->tupleNum++;
					cout << "利用了一个删除空间！" << endl;


					return 0;
				}
			}

		}
		else if (ost->getNoActiveNum() > 0) {

			ost->addOffset(bh->LongestOffset, length, insertOffset);
			memcpy(memBlock + bh->LongestOffset, tuple, length);
			bh->LongestOffset += length;
			bh->tupleNum++;
			return 0;
		}
		else {
			//cout << "无空闲偏移点以及无删除偏移点可以利用！" << endl;
			bh->isfull = 1;
			return -1;
		}
		return -1;
	}
	int readAllTuple(vector<unsigned char *>&tuples)
	{

		//unsigned char *p = getTuple(0);
		//int d;
		//memcpy(&d, p, 4);
		//cout << d << "###";
		int num = 0;
		for (int i = 0; i < OFFSETERNUM; i++)
		{

			if (ost->getOffset(i).isActive == 1 && ost->getOffset(i).isDelete == 0)
			{
				num++;
				tuples.push_back(getTuple(i));
			}
		}
		return num;
	}
	void deleteATuple(int index)
	{
		if (ost->getOffset(index).isActive == 1)
		{
			bh->isfull = 0;
			ost->deleteOffset(index);
			bh->tupleNum--;
		}

	}
	int deleteATupleByoffset(int index)
	{
		if (index >= 0 && index <= OFFSETERNUM) {
			if (ost->getOffset(index).isActive == 1)
			{
				if (ost->getOffset(index).isDelete == 1)
					return -1;
				bh->isfull = 0;
				ost->deleteOffset(index);
				bh->tupleNum--;
				return 0;
			}
			
			else
				return -2;
		}
		else
			return -3;

	}
	unsigned char * getTuple(int index)
	{
		offset ofst = ost->getOffset(index);
		int pos = ofst.pos;
		int length = ofst.length;

		unsigned char *tuple = (unsigned char *)malloc(length);
		memcpy(tuple, memBlock + pos, length);
		return tuple;
	}
	unsigned char * getMemBlock()
	{
		return this->memBlock;
	}
	int isFull()
	{
		return bh->isfull;
	}
	int getMagic()
	{
		return this->bh->magic;
	}

};

void test()
{
	block f;
	f.newBlock();
	f.format();
	f.blockInfo();

	for (int i = 0; ; i++)
	{
		dataDictResolver dR;
		dR.resolve(dataDefine, vs[0]);
		tuple_producer tp;
		unsigned char * p = tp.produceTupleTemp(dR.dataDef, dR.datas);
		int status = f.insertATuple(p, tp.getTupleLength());
		if (status < 0)
			break;
		unsigned char * tuple = f.getTuple(i);
		//f.deleteATuple(0);

		vector<string> result;

		dR.loadDict(dataDefine);
		dR.tupleToStrings(result, tuple);
		for (int i = 0; i < result.size(); i++)
			cout << result[i] << " ";
		cout << endl;
		tp.clear();
		free(tuple);
		dR.clear();
		cout << "\n--------------------\n";
		f.blockInfo();

	}
	unsigned char * b = (unsigned char *)malloc(BLOCKSIZE);
	memcpy(b, f.blockIntegrate(), BLOCKSIZE);


	f.loadABlock(b);
	f.blockInfo();

	dataDictResolver dR;
	unsigned char * tuple = f.getTuple(0);
	//f.deleteATuple(0);

	vector<string> result;

	dR.loadDict(dataDefine);
	dR.tupleToStrings(result, tuple);
	for (int i = 0; i < result.size(); i++)
		cout << result[i] << " ";

}


#define BLOCKNUM 25600


class blockFile
{
private:
	int BlockNum;
public:
	blockFile()
	{
		BlockNum = BLOCKNUM;//2560*Blocksize
	}

	void createFile(string filepath)
	{
		std::basic_ofstream< unsigned char> out;
		//ofstream out;
		out.open(filepath, ios::out | ios::binary);
		/*for (int i = 0; i < BlockNum; i++) {
		block *bl = new block();
		bl->newBlock();
		bl->formatWithBlockNum(i);



		if (out.is_open())
		{
		out.write(bl->blockIntegrate(), BLOCKSIZE);
		}
		else
		cout << "文件未打开！" << endl;
		bl->clean();
		free(bl);
		}*/

		out.close();
	}
	void noFileCreateFile(string filepath)
	{
		ifstream f(filepath.c_str());
		if (!f.good()) {
			std::basic_ofstream< unsigned char> out;
			//ofstream out;
			out.open(filepath, ios::out | ios::binary);
			out.close();
		}
	}

	

	void createFile(string tablepath,string metapath)
	{
		std::basic_ofstream< unsigned char> out;
		//ofstream out;
		out.open(tablepath, ios::out | ios::binary);
		/*for (int i = 0; i < BlockNum; i++) {
			block *bl = new block();
			bl->newBlock();
			bl->formatWithBlockNum(i);



			if (out.is_open())
			{
				out.write(bl->blockIntegrate(), BLOCKSIZE);
			}
			else
				cout << "文件未打开！" << endl;
			bl->clean();
			free(bl);
		}*/
		saveMetaData(0, metapath);
		out.close();
	}

	void saveMetaData(int maxBlockNum,string metapath)
	{
		int s = maxBlockNum;
		std::basic_ofstream< int> out;
		out.open(metapath, ios::out | ios::binary);
		out.write(&s, 1);
		out.close();
	}
	int readMetaData(string metapath)
	{
		int maxBlockNum;
		std::basic_ifstream<int> in;
		in.open(metapath, ios::in | ios::binary);
		in.read(&maxBlockNum, 1);
		in.close();

		return maxBlockNum;

	}
	int readABlock(int index, unsigned char **bl,string datapath)
	{

		//cout << "读入 " << index;
		if (index < 0 || index > BlockNum) {
			cout << "块号不对！" << endl;
			return -1;
		}
		std::basic_ifstream< unsigned char> in;
		in.open(datapath, ios::in | ios::binary);
		in.seekg(index*BLOCKSIZE, ios::beg);
		unsigned char *blocka = (unsigned char *)malloc(BLOCKSIZE);
		in.read(blocka, BLOCKSIZE);

		block bb;
		bb.loadABlock(blocka);
		if (bb.getMagic() != 199435)
		{
			bb.formatWithBlockNum(index);
		}


		(*bl) = bb.getMemBlock();//bb创建了动态申请块，然后把块给了bl，bl就可以管理这个块了。然后bb就没用了
		in.close();

		return 0;
	}
	int writeABlock(int index, unsigned char *bl,string datapath)
	{
		//cout << "写入 " << index;
		if (index < 0 || index > BlockNum) {
			cout << "块号不对！" << endl;
			return -1;
		}
		std::basic_ofstream< unsigned char> out;
		out.open(datapath, ios::in | ios::binary);
		out.seekp(index*BLOCKSIZE, ios::beg);
		out.write(bl, BLOCKSIZE);


		out.close();

		return 0;
	}

};






#define INIT_NULLBLOCK_RECORD_NUM 10
#define NULLBLOCK_RECORD_STEP 50
class blockUsingStatusManager
{
private:
	bpt::bplus_tree *btree;
	string metaAddr;
	bpt::value_t currentMaxBlockNumber;
	bpt::value_t currentNum;

public:
	blockUsingStatusManager(string metaAddr)
	{

		this->metaAddr = metaAddr;
		//init();
		btree = new bpt::bplus_tree(metaAddr.c_str());
		bpt::value_t re;
		if (btree->search("-", &re) == 0)
			;
		else
			btree->insert("-",-1);
		if (btree->search("currentNum", &re) == 0)
			;
		else {
			btree->insert("currentNum", 0);
			this->currentNum = 0;
			
		}
			
		if (btree->search("currentMaxBlock", &re) != 0) {
			btree->insert("currentMaxBlock", INIT_NULLBLOCK_RECORD_NUM);
			
			
			for (int i = 0; i < INIT_NULLBLOCK_RECORD_NUM; i++) {
				//cout << this->currentNum;
				recordNullBlock(i);
			}
		}
		this->currentNum = getCurrentNum();
		this->currentMaxBlockNumber = getCurrentMaxBlock();
		cout << this->currentMaxBlockNumber;
	
	}

	int addNullBlockWaterLevel()
	{

		bpt::value_t old = this->currentMaxBlockNumber;
		if (btree != NULL) {
			//btree->search("currentMaxBlock", &re);
			if (NULLBLOCK_RECORD_STEP + this->currentMaxBlockNumber > BLOCKNUM) {
				btree->update("currentMaxBlock", BLOCKNUM);
				this->currentMaxBlockNumber = BLOCKNUM;
			}
			else {
				this->currentMaxBlockNumber = this->currentMaxBlockNumber + NULLBLOCK_RECORD_STEP;
				btree->update("currentMaxBlock", this->currentMaxBlockNumber + NULLBLOCK_RECORD_STEP);
			}
		}
		for (int i = 0; i < this->currentMaxBlockNumber - old; i++) {
			recordNullBlock(i+ old);
		}
		return this->currentMaxBlockNumber - old;
	

	}
	bpt::value_t getCurrentMaxBlock()
	{
		bpt::value_t re;
		btree->search("currentMaxBlock", &re);
		return re;
	}
	bpt::value_t getCurrentNum()
	{
		bpt::value_t re;
		btree->search("currentNum", &re);
		return re;
	}
	void init()
	{
		const char* fname = metaAddr.c_str();
		fstream fs;
		fs.open(fname, ios::in);
		if (!fs)
		{
			ofstream fout(fname);
			if (fout)
				fout.close();
		}
		else
			;
	
	}
	bpt::value_t getANullBlockNumber()
	{
		//cout << currentNum << endl;

		if (currentNum == 0) {
			int re;
			re = addNullBlockWaterLevel();
			if (re == 0)
			{
				cout << "没有任何空闲块了！" << endl;
				exit(0);
			}
		}
		bpt::key_t aa;
		bpt::value_t ree;
		int cur;
	
		cur = currentNum + 1;
		
	
		bool s = true;
		
		
		bpt::value_t *num = (bpt::value_t*)malloc((currentNum + 1) * sizeof(bpt::key_t));
		//long long num[10];
		bpt::value_t *badnum = (bpt::value_t*)malloc((currentNum + 1) * sizeof(bpt::key_t));
		
		int badnumber = 0;

		
		this->btree->search_range_number_key(&aa, to_string(currentMaxBlockNumber).c_str(), num, currentNum + 1, badnum, &badnumber, &s);
		
		if (badnumber > 0)
		{
			for (int i = 0; i < badnumber; i++)
			{
				cout << badnum[i] << "@@@"<<endl;
				this->btree->update(to_string(badnum[i]).c_str(), badnum[i]);
			}
		}


		//cout << aa.k << endl;
		
		//cout << endl;
		///for (int i = 1; i < currentNum+1; i++)
		//	cout << num[i] << " ";
		//cout << endl;
		bpt::value_t rer;
		rer = num[1];
	
		//this->btree->remove(to_string(num[1]).c_str());
		if (currentNum > 0)
		{
			//this->currentNum--;
			btree->update("currentNum", this->currentNum);
		}
	
		free(num);
		return rer;
	}
	bpt::value_t getNullBlockNumbers(int n)
	{
		//cout << currentNum << endl;

		if (currentNum == 0) {
			int re;
			re = addNullBlockWaterLevel();
			if (re == 0)
			{
				cout << "没有任何空闲块了！" << endl;
				exit(0);
			}
		}
		bpt::key_t aa;
		aa = "-";
		bool s = true;


		bpt::value_t *num = (bpt::value_t*)malloc((currentNum + 1) * sizeof(bpt::key_t));
		//long long num[10];
		bpt::value_t *badnum = (bpt::value_t*)malloc((currentNum + 1) * sizeof(bpt::key_t));
		int badnumber;
		this->btree->search_range_number_key(&aa, to_string(currentMaxBlockNumber).c_str(), num, currentNum + 1, badnum,&badnumber,&s);

		if (badnumber > 0)
		{
			for (int i = 0; i < badnumber; i++)
			{
				this->btree->update(to_string(badnum[i]).c_str(), badnum[i]);
			}
		}

		//cout << endl;
		///for (int i = 1; i < currentNum+1; i++)
		//	cout << num[i] << " ";
		//cout << endl;
		bpt::value_t rer = num[1];
		//this->btree->remove(to_string(num[1]).c_str());
		if (currentNum > 0)
		{
			//this->currentNum--;
			btree->update("currentNum", this->currentNum);
		}

		free(num);
		return rer;
	}
	void recordNullBlock(int blockNum)
	{
		if (btree != NULL) {
			if (blockNum < BLOCKNUM) {
				btree->insert(to_string(blockNum).c_str(),blockNum);
				this->currentNum++;
				btree->update("currentNum", this->currentNum);
			}
			else
			{
				cout << "已经达到最大块号！";
			}
		}
	}

	int reportAFullBlockNum(int blockNum)
	{
		bpt::value_t re;
		int ret = btree->search(to_string(blockNum).c_str(),&re);
		if (ret == 0) {
			this->currentNum--;
			btree->update("currentNum", this->currentNum);
			btree->remove(to_string(blockNum).c_str());
			return 0;
		}
		else
			return -1;
	}

	~blockUsingStatusManager()
	{
		free(btree);
	}

	bpt::bplus_tree * getBtree()
	{
		return btree;
	}

};








struct blockBuffer
{
	block *bl;
	int timestamp;
};

#define BLOCK_BUFFER_SIZE 2560
#define INIT_BUFFER_SIZE 2
class BlocksManager
{
private:
	blockBuffer buffer[BLOCK_BUFFER_SIZE];
	blockFile BF;
	blockUsingStatusManager *BUSM;
	map<int, bool> blockUseRecord;
	//map<int, bool> blockInMemoryRecord;
	int bufferWaterLevel;
	string dataPath;
	string metaPath;
public:
	BlocksManager(string datapath,string metapath)
	{
		this->dataPath = datapath;
		this->metaPath = metapath;
		bufferWaterLevel = INIT_BUFFER_SIZE;
		BUSM = new blockUsingStatusManager(this->metaPath);
	
	}
	BlocksManager()
	{
		
	}
	void setconfig(string datapath,string metapath)
	{
		bufferWaterLevel = INIT_BUFFER_SIZE;
		this->dataPath = datapath;
		this->metaPath = metapath;

		BUSM = new blockUsingStatusManager(this->metaPath);

	}
	void initBuffer()
	{
		//maxblocknumer =  BF.readMetaData(this->metaPath);
		
		for (int i = 0; i < INIT_BUFFER_SIZE; i++)
		{
			
			unsigned char *blo;
			BF.readABlock(i, &blo,this->dataPath);
			buffer[i].bl = new block();
			buffer[i].bl->loadABlock(blo);

			time_t t;
			time(&t);
			buffer[i].timestamp = t;
			free(blo);

		}
	}
	void flushAll()
	{
		//BF.saveMetaData(maxblocknumer,this->metaPath);
		for (int i = 0; i < bufferWaterLevel; i++)
		{
			BF.writeABlock(buffer[i].bl->getBlockNum(), buffer[i].bl->blockIntegrate(),this->dataPath);
		}
	}
	void blockFailRecord(int blockNum)
	{
		BUSM->reportAFullBlockNum(blockNum);
		//blockUseRecord[blockNum] = false;
	}
	void blockOkRecord(int blockNum)
	{
		BUSM->recordNullBlock(blockNum);
		//blockUseRecord[blockNum] = true;
	}
	bool getBlockStatus(int blockNum)
	{
		if (blockUseRecord.count(blockNum) > 0)
			return blockUseRecord[blockNum];
		else
			return true;
	}
	block* getABlock(int blockNum)
	{

		if (blockNum > maxblocknumer)
			maxblocknumer = blockNum;
		int minTimeStamp = (std::numeric_limits<int>::min)();
		int minIndex = 0;
		if (blockNum >= 0 && blockNum < BLOCKNUM) {
			for (int i = 0; i < bufferWaterLevel; i++)
			{
				if (buffer[i].timestamp < minTimeStamp) {
					minTimeStamp = buffer[i].timestamp;
					minIndex = i;
				}

				if (buffer[i].bl->getBlockNum() == blockNum) {
					time_t t;
					time(&t);
					buffer[i].timestamp = t;
					return buffer[i].bl;
				}
			}
			
			if (bufferWaterLevel < BLOCK_BUFFER_SIZE)
			{
				unsigned char *blo;
				BF.readABlock(blockNum, &blo,this->dataPath);
				buffer[bufferWaterLevel].bl = new block();
				buffer[bufferWaterLevel].bl->loadABlock(blo);

				time_t t;
				time(&t);
				buffer[bufferWaterLevel].timestamp = t;
				free(blo);
				bufferWaterLevel++;
				
				
				return buffer[bufferWaterLevel-1].bl;
			}

			BF.writeABlock(buffer[minIndex].bl->getBlockNum(), buffer[minIndex].bl->blockIntegrate(),this->dataPath);
			unsigned char *b;
			BF.readABlock(blockNum, &b,this->dataPath);
			buffer[minIndex].bl->loadABlock(b);
			time_t t;
			time(&t);
			buffer[minIndex].timestamp = t;
			free(b);
			return buffer[minIndex].bl;
		}
		return NULL;

	}
	block* getABufferBlockWithNoFull()
	{
		int blockNumber = BUSM->getANullBlockNumber();

		block *b = getABlock(blockNumber);

		//cout << blockNumber<<"!@##!@#" << b << "@!#!@#!@#";
		if (b == NULL)
			return NULL;
		else
			return b;
	}
	int getMaxBlockNum()
	{
		return BUSM->getCurrentMaxBlock();
	}
	~BlocksManager()
	{
		free(BUSM);
	}
};

class dataOperator
{
private:
	BlocksManager BM;
	

	


public:
	dataOperator(string dataPath,string metaPath)
	{
		BM.setconfig(dataPath, metaPath);
		cout << "创建缓存，加载文件..." << endl;
		BM.initBuffer();
		cout << "缓存加载完毕..." << endl;
	}


	int insertOverBlocks(string dataDefine, string data)
	{
		dataDictResolver dR;
		dR.resolve(dataDefine, data);
		tuple_producer tp;
		unsigned char * p = tp.produceTupleTemp(dR.dataDef, dR.datas);


		

		block *blin = BM.getABufferBlockWithNoFull();
		if (blin != NULL)
		{
			int status = blin->insertATuple(p, tp.getTupleLength());
			if (status >= 0) {
				tp.clear();
				dR.clear();
				return 0;
			}
			else
				BM.blockFailRecord(blin->getBlockNum());
		}


		for (int i = 0; i < BLOCKNUM; i++) {
			if (BM.getBlockStatus(i) == false) {
			//cout << "块" << i << "不行了！" << endl;
				continue;
			}
			block *b = BM.getABlock(i);
			if (b->isFull() == 0) {
				int status = b->insertATuple(p, tp.getTupleLength());
				if (status >= 0) {
					tp.clear();
					dR.clear();
					return 0;
				}
				else
					BM.blockFailRecord(i);
			}

		}
		
		tp.clear();
		dR.clear();
		return -1;
	}
	int insertOverBlocksByDR(dataDictResolver &dR)
	{
		
		tuple_producer tp;
		unsigned char * p = tp.produceTupleTemp(dR.dataDef, dR.datas);

	
		block *blin = NULL;
		int yes = 0;
		do {
			
			blin = BM.getABufferBlockWithNoFull();
			if (blin != NULL)
			{
				int status = blin->insertATuple(p, tp.getTupleLength());
				
				if (status >= 0) {
					yes = 1;
				}
				else
					BM.blockFailRecord(blin->getBlockNum());
			}
		} while (blin != NULL && yes == 0);

	
		tp.clear();
		dR.clear();
		if (yes == 1)
			return 0;
		else
			return -1;

	}
	int insertOverBlocksByDRReturnBO(dataDictResolver &dR,int &offset,int &blockNum)
	{

		tuple_producer tp;
		unsigned char * p = tp.produceTupleTemp(dR.dataDef, dR.datas);
	


		block *blin = NULL;
		int yes = 0;
		do {

			blin = BM.getABufferBlockWithNoFull();
			if (blin != NULL)
			{
				int status = blin->insertATupleReturnOffset(p, tp.getTupleLength(),offset);

				if (status >= 0) {
					blockNum = blin->getBlockNum();
					yes = 1;
				}
				else
					BM.blockFailRecord(blin->getBlockNum());
			}
		} while (blin != NULL && yes == 0);


		tp.clear();
		dR.clear();
		if (yes == 1)
			return 0;
		else
			return -1;

	}
	int insertOverBlocksByDRReturnBOAddIndex(dataDictResolver &dR,string key,int &offset, int &blockNum)
	{

		tuple_producer tp;
		unsigned char * p = tp.produceTupleTemp(dR.dataDef, dR.datas);



		block *blin = NULL;
		int yes = 0;
		do {

			blin = BM.getABufferBlockWithNoFull();
			if (blin != NULL)
			{
				int status = blin->insertATupleReturnOffset(p, tp.getTupleLength(), offset);

				if (status >= 0) {
					blockNum = blin->getBlockNum();
					yes = 1;
				}
				else
					BM.blockFailRecord(blin->getBlockNum());
			}
		} while (blin != NULL && yes == 0);


		tp.clear();
		dR.clear();
		if (yes == 1)
			return 0;
		else
			return -1;

	}
	int readOverBlocks(vector<unsigned char *>&result)
	{

		int num = 0;
		for (int i = 0; i < BLOCKNUM; i++)
		{
			block *b = BM.getABlock(i);

			num += b->readAllTuple(result);
			//cout << num << " ";
		}
		return num;
	}
	int insertOverBlocks(unsigned char* tuple, int length, int &blockNumber, int &offseter)
	{
		block *blin = BM.getABufferBlockWithNoFull();
		if (blin != NULL)
		{
			int status = blin->insertATupleReturnOffset(tuple, length, offseter);
			if (status >= 0) {
				blockNumber = blin->getBlockNum();
				return 0;
			}
		}


		for (int i = 0; i < BLOCKNUM; i++) {
			
			block *b = BM.getABlock(i);

		

			if (b->isFull() == 0) {
				int status = b->insertATupleReturnOffset(tuple, length, offseter);
				if (status >= 0) {
					blockNumber = b->getBlockNum();
					return 0;
				}
			}
		}

		return -1;
	}
	unsigned char * readOverBlocksByBlockOffset(int blockNum, int offset)
	{
		if (blockNum < 0 || blockNum > BLOCKNUM || offset < 0 || offset > OFFSETERNUM)
			return NULL;
		block *b = BM.getABlock(blockNum);
		if (b->tupleIsDeleted(offset) == 0 && b->tupleIsActivated(offset) == 1) {
			unsigned char *tuple = b->getTuple(offset);
			
			return tuple;
		}
		else
			return NULL;
	}
	void commit()
	{
		BM.flushAll();
	}
	void deletone()
	{
		block *b = BM.getABlock(255);
		b->deleteATuple(0);

	}

	void deleteOverBlocksByBlockOffset(int blockNum, int offset)
	{
		block *b = BM.getABlock(blockNum);

		
	
		int re = b->deleteATupleByoffset(offset);
		if (re == 0) {
			cout << "删除成功！" << endl;
			BM.blockOkRecord(blockNum);
		}
		else if (re == -1)
			cout << "重复删除？" << endl;
		else if (re == -2)
			cout << "未使用节点删除？" << endl;
		else
			cout << "offset使用不正确！" << endl;
		
	
	}
	void scanAllTable(string dataDefine)
	{
		int count = 0;
		time_t t1;
		time(&t1);
		
		for (int j = 0; j < BM.getMaxBlockNum(); j++)
			for (int i = 0; i < OFFSETERNUM; i++) {

				unsigned char *tuple;
				tuple = readOverBlocksByBlockOffset(j, i);

				if (tuple == NULL)
				{
					//cout << "tuple不存在!" << endl;

				}
				else {
					dataDictResolver dR;
					dR.loadDict(dataDefine);
					vector<string> vs;
					dR.tupleToStrings(vs, tuple);
					for (int i = 0; i < vs.size(); i++)
						cout << vs[i] << " ";
					cout << endl << endl;
					dR.clear();
					count++;
				}
			}
		time_t t2;
		time(&t2);
		cout << "运行了" << count << "条！" << endl;
		cout << t2 - t1 << "s" << endl;
	}
	int iterTableTupleRow(string dataDefine,int &iteri,int &iterj,int rowPos,string &rowStr)
	{
		int count = 0;
		time_t t1;
		time(&t1);

		for (; iteri < BM.getMaxBlockNum(); ) {
			for (; iterj < OFFSETERNUM; ) {

				unsigned char *tuple;
				tuple = readOverBlocksByBlockOffset(iteri, iterj);

				if (tuple == NULL)
				{
					//cout << "tuple不存在!" << endl;
					iterj++;

				}
				else {

					dataDictResolver dR;
					dR.loadDict(dataDefine);
					vector<string> vs;
					dR.tupleToStrings(vs, tuple);
					rowStr = vs[rowPos];
	
					dR.clear();
					
					iterj++;
					return 1;

				}
			}
			iterj = 0;
			iteri++;
		}

		return 0;
	}
	int iterTableTupleRows(string dataDefine, int &iteri, int &iterj, vector<int>rowPos, vector<string> &rowStr)
	{
		int count = 0;
		time_t t1;
		time(&t1);

		for (; iteri < BM.getMaxBlockNum(); ) {
			for (; iterj < OFFSETERNUM; ) {

				unsigned char *tuple;
				tuple = readOverBlocksByBlockOffset(iteri, iterj);

				if (tuple == NULL)
				{
					//cout << "tuple不存在!" << endl;
					iterj++;

				}
				else {

					dataDictResolver dR;
					dR.loadDict(dataDefine);
					vector<string> vs;
					dR.tupleToStrings(vs, tuple);
					for (int i = 0; i < rowPos.size(); i++) {
						//cout <<vs.size() << "#$$" << endl;
						rowStr.push_back(vs[rowPos[i]]);
					}

					dR.clear();

					iterj++;
					return 1;

				}
			}
			iterj = 0;
			iteri++;
		}

		return 0;
	}
	int iterTableTuple(string dataDefine, int &iteri, int &iterj,vector<string> &vs)
	{
		int count = 0;
		time_t t1;
		time(&t1);

		for (; iteri < BM.getMaxBlockNum(); ) {
			for (; iterj < OFFSETERNUM; ) {

				unsigned char *tuple;
				tuple = readOverBlocksByBlockOffset(iteri, iterj);

				if (tuple == NULL)
				{
					//cout << "tuple不存在!" << endl;
					iterj++;

				}
				else {

					dataDictResolver dR;
					dR.loadDict(dataDefine);
					
					dR.tupleToStrings(vs, tuple);
					dR.clear();
					
					iterj++;
					return 1;

				}
			}
			iterj = 0;
			iteri++;
		}

		return 0;
	}



};
class tableOperator
{
	dataOperator *dO;
	bpt::bplus_tree *index;
	string dataDefine;
	dataDictResolver dR;
	string tableName;
	int rowNum;
	vector<string> rows;
	



public:

	tableOperator(string dataPath, string metapath, string indexPath)
	{
		dO = new dataOperator(dataPath, metapath);
		index = new bpt::bplus_tree(indexPath.c_str());
		
	}

	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
	{
		std::string::size_type pos1, pos2;
		pos2 = s.find(c);
		pos1 = 0;
		while (std::string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2 - pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != s.length())
			v.push_back(s.substr(pos1));
	}

	void setDataDefine(string DD)
	{
		this->dataDefine = DD;
		vector<string> vs;
		SplitString(this->dataDefine, vs, "_");
		rowNum = vs.size();

	}
	void setRows(string DD)
	{
	
		vector<string> vs;
		SplitString(DD, vs, ",");
		for (int i = 0; i < vs.size(); i++)
		{
			this->rows.push_back(vs[i]);
		}

	}

	

	int insertATupleAddIndex(vector<string> datas,int keyPos)
	{
		if (keyPos > rowNum - 1 || keyPos < 0) {
			cout << "列选择错误！" << endl;
			return -1;
		}
			
		int offset, blocknum;
		if(dR.resolveByVector(dataDefine,datas) == -1 )
			return -1;
		
		bpt::value_t re;
		string in = datas[keyPos];
		if (index->search(in.c_str(), &re) == 0)
		{
			dR.clear();
			cout << "主键冲突！插入数据失败！" << endl;
			return -1;
		}

		dO->insertOverBlocksByDRReturnBO(dR, offset, blocknum);
		dR.clear();
	
		int bsize = to_string(blocknum).size();
		int osize = to_string(offset).size();
		if (bsize > 9 || osize > 9)
		{
			cout << "ID 达到上限！" << endl;
			return -1;
		}
		string ID =to_string(to_string(blocknum).size())+ to_string(blocknum)+ to_string(to_string(offset).size())+ to_string(offset);
		
	//	cout << in <<" "<< atoll(ID.c_str())<< endl;
		
		
		
		index->insert(in.c_str(),atoll(ID.c_str()));
		dR.clear();
		return 0;
	}
	int createBPTIndex(int rowPos)
	{
		if (rowPos > rowNum - 1 || rowPos < 0) {
			cout << "列选择错误！" << endl;
			return -1;
		}
		int iterb = 0,itero = 0;
		string rowStr;
		int ret;
		do {
			ret = dO->iterTableTupleRow(dataDefine, iterb, itero, rowPos, rowStr);
			cout << iterb << " " << itero << " "<<rowStr << endl;
		} while (ret == 1);

		return 1;

	}
	int iterTableRow(int rowPos)
	{
		if (rowPos > rowNum - 1 || rowPos < 0) {
			cout << "列选择错误！" << endl;
			return -1;
		}


		int iterb = 0, itero = 0;
		string rowStr;
		int ret;
		do {
			ret = dO->iterTableTupleRow(dataDefine, iterb, itero, rowPos, rowStr);
			if (ret == 0)
				return 0;

			cout << rowStr << endl;
			
		} while (ret == 1);

		return 0;

	}
	int iterTableRowByRowName(string rowName)
	{
		int rowPos = -1;
		for (int i = 0; i < rows.size() ; i++)
		{
			if (rows[i].compare(rowName) == 0)
			{
				rowPos = i;
			}
		}
		if (rowPos == -1) {
			cout << "没有此列！["+rowName+"]" << endl;
			return -1;
		}


		int iterb = 0, itero = 0;
		string rowStr;
		int ret;
		do {
			ret = dO->iterTableTupleRow(dataDefine, iterb, itero, rowPos, rowStr);
			if (ret == 0)
				return 0;

			cout << rowStr << endl;

		} while (ret == 1);

		return 0;

	}

	int iterTableRowByRowNames(vector<string> rowNames)
	{
		vector<int> rowPos;
		for (int i = 0; i < rows.size(); i++)
		{
			for (int j = 0; j < rowNames.size(); j++)
			{
				if (rowNames[j].compare(rows[i]) == 0)
				{
					
					rowPos.push_back(i);
				}
			}
		}
		if (rowPos.size() != rowNames.size()) {
			cout << "有未发现的列！" << endl;
			return -1;
		}

		 
		
		int iterb = 0, itero = 0;
		vector<string> rowStr;
		int ret;
		do {
			ret = dO->iterTableTupleRows(dataDefine, iterb, itero, rowPos, rowStr);
			if (ret == 0)
				return 0;

			for (int i = 0; i < rowStr.size(); i++)
			{
				cout << rowStr[i] << " ";
			}
			rowStr.clear();
			cout << endl;

		} while (ret == 1);

		return 0;

	}

	int iterTable()
	{
		int iterb = 0, itero = 0;
		vector<string> rowStr;
		int ret;
		do {
			ret = dO->iterTableTuple(dataDefine, iterb, itero,rowStr);
			if (ret == 0)
				return 0;

			for (int i = 0; i < rowStr.size(); i++)
				cout << rowStr[i] << " ";
			cout << endl;

			rowStr.clear();
		} while (ret == 1);

		return 0;

	}

	int iterTableGetData(vector<vector<string>> &vss)
	{
		int iterb = 0, itero = 0;
		vector<string> rowStr;
		int ret;
		do {
			ret = dO->iterTableTuple(dataDefine, iterb, itero, rowStr);
			if (ret == 0)
				return 0;

			vss.push_back(rowStr);

			rowStr.clear();
		} while (ret == 1);

		return 0;

	}

	int getTupleByIndex(string key,vector<string> &vs)
	{
		int ret;
		long long bo;
		ret = index->search(key.c_str(), &bo);
		if (ret != 0) {
			return -1;
		}
		
		string ID = to_string(bo);

		
		int bsize = atoi(ID.substr(0,1).c_str());
		int blockNum = atoi(ID.substr(1, bsize).c_str());
		int osize = atoi(ID.substr(bsize + 1, 1).c_str());
		int offset = atoi(ID.substr(bsize + 2, osize).c_str());

		//cout << blockNum <<"|"<< offset << endl;

		unsigned char * tuple = dO->readOverBlocksByBlockOffset(blockNum, offset);
		if (tuple == NULL)
			return -1;
		dR.loadDict(dataDefine);
		dR.tupleToStrings(vs,tuple); 
		free(tuple);
		return 0;

	}
	int deleteTupleByIndex(string key)
	{
		int ret;
		long long bo;
		ret = index->search(key.c_str(), &bo);
		if (ret != 0) {
			return -1;
		}
		string ID = to_string(bo);
		int bsize = atoi(ID.substr(0, 1).c_str());
		int blockNum = atoi(ID.substr(1, bsize).c_str());
		int osize = atoi(ID.substr(bsize + 1, 1).c_str());
		int offset = atoi(ID.substr(bsize + 2, osize).c_str());
		dO->deleteOverBlocksByBlockOffset(blockNum, offset);
		index->remove(key.c_str());

		return 0;
	}

	int updateTupleByIndex(string key,vector<string> newTuple)
	{
		int ret;
		long long bo;
		ret = index->search(key.c_str(), &bo);
		if (ret != 0) {
			return -1;
		}
		string ID = to_string(bo);
		int bsize = atoi(ID.substr(0, 1).c_str());
		int blockNum = atoi(ID.substr(1, bsize).c_str());
		int osize = atoi(ID.substr(bsize + 1, 1).c_str());
		int offset = atoi(ID.substr(bsize + 2, osize).c_str());
		dO->deleteOverBlocksByBlockOffset(blockNum, offset);
		index->remove(key.c_str());

		if (insertATupleAddIndex(newTuple, 0) == -1)
			return -1;
		

		return 0;
	}


	~tableOperator()
	{
		
		free(dO);
		free(index);
	}
	void commit()
	{
		dO->commit();
	}


};
class tablesMetaDataManager
{
	
	string tableName;
	vector<string> tableRows;
	blockFile bF;

public:
	tableOperator *tO;
	tablesMetaDataManager()
	{
		tableName = "tableManager";
		tableRows = { "tablename","tableRows","tableDefine","datapath","metapath","indexpath" };
		bF.noFileCreateFile("tablesMetaData");
		bF.noFileCreateFile("tablesMetaDataMD");
		bF.noFileCreateFile("tablesMetaDataIndex");
		tO = new tableOperator("tablesMetaData", "tablesMetaDataMD", "tablesMetaDataIndex");
		tO->setDataDefine("char(150)_char(150)_char(150)_char(150)_char(150)_char(150)");

	}

	int addTableInfo(string tableName,string tableRows,string tableDefine)
	{
		vector<string> vs;
		vs.push_back(tableName);
		vs.push_back(tableRows);	
		vs.push_back(tableDefine);
		vs.push_back(tableName+"_data");
		vs.push_back(tableName+"_meta");
		vs.push_back(tableName+"_index");
		int ret = tO->insertATupleAddIndex(vs, 0);
	
		if (ret == 0)
		{
			
			tO->commit();
			tO->iterTable();
			return ret;
		}
		else
			return -1;
	}

	int createTable(string tableName, string tableRows, string tableDefine)
	{
		if(addTableInfo(tableName, tableRows, tableDefine) == -1)
			return -1;

		bF.noFileCreateFile(tableName + "_data");
		bF.noFileCreateFile(tableName + "_meta");
		bF.noFileCreateFile(tableName + "_index");
		return 0;
	}

	void dropTable(string tableName)
	{
		tO->deleteTupleByIndex(tableName);
		tO->commit();
		remove((tableName + "_data").c_str());
		remove((tableName + "_meta").c_str());
		remove((tableName + "_index").c_str());
	}
	int getTableDefine(string tableName,string &result)
	{
		vector<string> vs;
		if (tO->getTupleByIndex(tableName, vs) == 0) {
			
			result = vs[2];
			return 0;
		}
		cout << "表不存在！" << endl;
		return -1;
	}
	int getTableRows(string tableName,string &result)
	{
		vector<string> vs;
		if (tO->getTupleByIndex(tableName, vs) == 0) {
			result = vs[1];
			return 0;
		}
		cout << "表不存在！" << endl;
		return -1;
	}
	int getTableDataPath(string tableName, string &result)
	{
		vector<string> vs;
		if (tO->getTupleByIndex(tableName, vs) == 0) {
			result = vs[3];
			return 0;
		}
		cout << "表不存在！" << endl;
		return -1;
	}
	int getTableMetaPath(string tableName, string &result)
	{
		vector<string> vs;
		if (tO->getTupleByIndex(tableName, vs) == 0) {
			result = vs[4];
			return 0;
		}
		cout << "表不存在！" << endl;
		return -1;
	}
	int getTableIndexPath(string tableName, string &result)
	{
		vector<string> vs;
		if (tO->getTupleByIndex(tableName, vs) == 0) {
			result = vs[5];
			return 0;
		}
		cout << "表不存在！" << endl;
		return -1;
	}

	int isTableExist(string tableName)
	{
		vector<string> vs;
		if (tO->getTupleByIndex(tableName, vs) == 0) {
			return 0;
		}
		return -1;
	}

	~tablesMetaDataManager()
	{
		free(tO); 
	}
};

class tableManager
{
	tablesMetaDataManager tMM;

public:
	tableOperator *tO;
	tableManager(string tableName)
	{
		if (tMM.isTableExist(tableName) == -1)
		{
			cout << "表不存在！" << endl;
			return;
		}
		string datapath;
		string metapath;
		string indexpath;
		tMM.getTableDataPath(tableName, datapath);
		tMM.getTableMetaPath(tableName, metapath);
		tMM.getTableIndexPath(tableName, indexpath);
		tO = new tableOperator(datapath, metapath, indexpath);
		string dataDefine;
		string datarows;
		tMM.getTableDefine(tableName, dataDefine);
		tMM.getTableRows(tableName, datarows);
		tO->setDataDefine(dataDefine);
		tO->setRows(datarows);
	}
	~tableManager()
	{
		free(tO);
	}
};


void test2()
{
	blockFile db;
	//createDataBase();


	BlocksManager BM;
	cout << "创建缓存，加载数据库..." << endl;
	BM.initBuffer();
	cout << "缓存加载完毕..." << endl;
	block *b = BM.getABlock(0);
	b->blockInfo();

	dataDictResolver dR;
	dR.resolve(dataDefine, vs[0]);
	tuple_producer tp;
	unsigned char * p = tp.produceTupleTemp(dR.dataDef, dR.datas);
	int status = b->insertATuple(p, tp.getTupleLength());

	BM.flushAll();
	BM.getABlock(1);
	BM.getABlock(2);
	b = BM.getABlock(0);



	unsigned char * tuple = b->getTuple(0);
	//f.deleteATuple(0);

	vector<string> result;

	dR.loadDict(dataDefine);
	dR.tupleToStrings(result, tuple);
	cout << result[2];
	tp.clear();
	free(tuple);
	dR.clear();

}

void test3()
{
	blockFile bf;
	cout << "创建文件..." << endl;
	bf.createFile("d://mydata","d://mymeta");
	cout << "创建文件成功！" << endl;
	dataOperator dO("d://mydata", "d://mymeta");
	

	//dO.deletone();
	time_t t;
	time(&t);
	int minTimeStamp1 = t;
	for (;;)
	{


		int a = dO.insertOverBlocks(dataDefine, vs[3]);
		int b = dO.insertOverBlocks(dataDefine, vs[2]);
		int c = dO.insertOverBlocks(dataDefine, vs[1]);


		if (a == -1 && b == -1 & c == -1) {
			cout << "文件写满了！" << endl;
			break;
		}


	}

	time(&t);
	int minTimeStamp2 = t;
	cout << minTimeStamp2 - minTimeStamp1 << endl;
	dO.commit();

	vector<unsigned char *> tuples;
	int nums;
	tuples.clear();
	nums = dO.readOverBlocks(tuples);
	cout << "tuple数量" << nums << endl;




	dataDictResolver dR;
	dR.loadDict(dataDefine);
	for (int i = 0; i < tuples.size(); i++) {
		vector<string> vs;
		dR.tupleToStrings(vs, tuples[i]);
		free(tuples[i]);
		for (int j = 0; j < vs.size(); j++)
			cout << vs[j] << " ";
		cout << endl;
	}

}
void test5()
{
	blockFile bf;
	cout << "创建文件..." << endl;
	bf.createFile("d://mydata", "d://mymeta");
	cout << "创建文件成功！" << endl;

	bpt::bplus_tree tree("test.db");



	dataOperator d0("d://mydata", "d://mymeta");
	int blockNum;
	int offset;
	dataDictResolver dr;
	dr.resolve(dataDefine, "张康康_东北乌鲁木齐石家庄_321_1234567890_1234312_嘿嘿嘿嘿嘿");
	tuple_producer tp;
	unsigned char * t = tp.produceTupleTemp(dr.dataDef, dr.datas);

	d0.insertOverBlocks(t, tp.getTupleLength(), blockNum, offset);
	string index = to_string(to_string(blockNum).size()) + "$" + to_string(blockNum) + "$" + to_string(to_string(offset).size()) + "$" + to_string(offset) + "$";
	cout << index << "?";
	tree.insert("张康康", stol(index));
	long long  re;
	tree.search("张康康", &re);
	cout << re;

	d0.readOverBlocksByBlockOffset(0, 0);

	exit(0);

}
void test6()
{
	blockFile bf;
	cout << "创建文件..." << endl;
	bf.createFile("d://mydata", "d://mymeta");
	cout << "创建文件成功！" << endl;
	dataOperator dO("d://mydata", "d://mymeta");
	dataDictResolver dR;
	tuple_producer tp;
	ifstream in("C:/Users/ZXK/Desktop/DBMS2017/benchmark_data/benchmark_data/supplier.tbl");
	string line;

	if (in) // 有该文件
	{
		int i = 0;
		cout << "加载文件..."<<endl;
		vector<vector<string>> vss;
		while (getline(in, line)) // line中不包括每行的换行符
		{
		
		//	cout << i;
			vector<string> vs;
			dR.SplitString(line, vs, "|");
			dR.clear();
			vss.push_back(vs);
			
		//	cout << "\b\b\b\b\b\b\b\b";
			i++;
			
		}
		cout << "文件加载成功！" << endl;
		i = 0;
		;

		time_t t1;
		time(&t1);
		cout << "插入数据..."<<endl;
		for (int ii = 0; ii < vss.size(); ii++)
		{
			dR.resolveByVector(dataDefine, vss[ii]);

			int b, n;
			int re = dO.insertOverBlocksByDRReturnBO(dR,b,n);
			//cout << b << " " << n << endl;
			
			if (re == -1) {
				cout << "卧槽数据库插满了我去！" << endl;
				break;

				
			}
			i++;
			//cout << "\b\b\b\b\b\b\b\b\b\b";
			dR.clear();
		}
		time_t t2;
		time(&t2);
		cout << t2 - t1 <<"s"<< endl;
		cout << "数据插入完毕,个数：" << i << endl;
		dO.commit();



	}
	else // 没有该文件
	{
		cout << "no such file" << endl;
	}
}
void test7()
{
	
	dataOperator dO("d://mydata", "d://mymeta");
	int count = 0;
	time_t t1;
	time(&t1);
	for(int j = 0 ; j < 3560 ; j++)
	for (int i = 0; i < 64; i++) {
		
		unsigned char *tuple;
		tuple = dO.readOverBlocksByBlockOffset(j,i);
	
		if (tuple == NULL)
		{
			//cout << "tuple不存在!" << endl;

		}
		else {
			dataDictResolver dR;
			dR.loadDict(dataDefine);
			vector<string> vs;
			dR.tupleToStrings(vs, tuple);
			for (int i = 0; i < vs.size(); i++)
				cout << vs[i] << " " ;
			cout << endl<<endl;
			dR.clear();
			count++;
		}
	}
	time_t t2;
	time(&t2);
	cout << "运行了" << count << "条！" << endl;
	cout << t2 - t1 << "s" << endl;
}
void test8()
{

	dataOperator dO("d://mydata", "d://mymeta");
	//int count = 0;
	time_t t1;
	time(&t1);
	dO.deleteOverBlocksByBlockOffset(8,10);
	//for(int i = 0 ; i < 30 ; i++)
	//int re  = dO.insertOverBlocks(dataDefine, vs[0]);
	//cout << re << endl;
	time_t t2;
	time(&t2);
	//cout << "运行了" << count << "条！" << endl;
	cout << t2 - t1 << "s" << endl;
	dO.commit();
}

void test9()
{
	time_t t1;
	time(&t1);
	//test6();
	//	system("echo > 1");
	blockUsingStatusManager busm("1");

	//	for(int i = 0 ;i < 2560 ; i++)	{
	//		busm.recordFullBlock(i);
	//	}
	for (;;) {
		int a = busm.getANullBlockNumber();

		cout << "$$$" << a << "$$$$" << endl;
		if (a == 25590)
			break;
		busm.reportAFullBlockNum(a);
		//cout << busm.getCurrentNum() << endl;
		getchar();

	}
	//cout << b<< endl;
	time_t t2;
	time(&t2);

	cout << t2 - t1 << "s" << endl;

}

void test11()
{
	//test99();

	tableOperator tO("d://mydata","d://mymeta","d://myindex");
tO.setDataDefine(dataDefine);

ifstream in("C:/Users/ZXK/Desktop/DBMS2017/benchmark_data/benchmark_data/supplier.tbl");
string line;

dataDictResolver dR;

if (in) // 有该文件
{
	int i = 0;
	cout << "加载文件..." << endl;
	vector<vector<string>> vss;
	while (getline(in, line)) // line中不包括每行的换行符
	{

		//	cout << i;
		vector<string> vs;
		dR.SplitString(line, vs, "|");
		dR.clear();
		vss.push_back(vs);

		//	cout << "\b\b\b\b\b\b\b\b";
		i++;

	}
	cout << "文件加载成功！" << endl;
	i = 0;
	;

	cout << "插入数据..." << endl;
	for (int ii = 0; ii < vss.size(); ii++)
	{

		int re = tO.insertATupleAddIndex(vss[ii],0);
		//cout << b << " " << n << endl;

		i++;
		//cout << "\b\b\b\b\b\b\b\b\b\b";

	}
}

tO.commit();

}


void test888()
{
	tableOperator tO("d://mydata", "d://mymeta", "d://myindex");

	tO.setDataDefine(dataDefine);

	//cout << tO.insertATupleAddIndex({ "10002","2","3","4","5","6","7" }, 0) <<"!!!!" << endl;
	///tO.commit();

	tO.createBPTIndex(5);

	exit(0);
	for (int j = 1023; j < 1201; j++) {
		vector<string> vs;
		tO.getTupleByIndex(to_string(j), vs);
		for (int i = 0; i < vs.size(); i++)
		{
			cout << vs[i] << " ";
		}
		cout << endl;
	}

}

void test99()
{
	{
		tablesMetaDataManager tMM;
		tMM.dropTable("supplier");
		tMM.createTable("supplier", "S_SUPPKEY,S_NAME,S_ADDRESS,S_NATIONKEY,S_PHONE,S_ACCTBAL,S_COMMENT", "int_char(25)_varchar(140)_int_char(25)_float_varchar(101)");


		tableManager tM("supplier");

		//vector<vector<string>> vss;



		//tM.tO->iterTableRowByRowName("S_ADDRESS");


		ifstream in("C:/Users/ZXK/Desktop/DBMS2017/benchmark_data/benchmark_data/supplier.tbl");
		string line;

		dataDictResolver dR;

		if (in) // 有该文件
		{
			int i = 0;
			cout << "加载文件..." << endl;
			vector<vector<string>> vss;
			while (getline(in, line)) // line中不包括每行的换行符
			{

				//	cout << i;
				vector<string> vs;
				dR.SplitString(line, vs, "|");
				dR.clear();
				vss.push_back(vs);

				//	cout << "\b\b\b\b\b\b\b\b";
				i++;

			}
			cout << "文件加载成功！" << endl;
			i = 0;
			;

			cout << "插入数据..." << endl;
			for (int ii = 0; ii < vss.size(); ii++)
			{

				tM.tO->insertATupleAddIndex(vss[ii], 0);
				//cout << b << " " << n << endl;

				i++;
				//cout << "\b\b\b\b\b\b\b\b\b\b";

			}
		}
		tM.tO->commit();

		
	}


}

int main()
{

	//tablesMetaDataManager tMM;



	//tMM.createTable("nation", "N_NATIONKEY,N_NAME,N_REGIONKEY,N_COMMENT", "int_char(25)_int_varchar(152)");

	//exit(0);


	tableManager tM("supplier");
	tableManager tM2("nation");

	
	tM2.tO->iterTableRowByRowNames({ "N_NATIONKEY" });
	
	tM.tO->iterTableRowByRowNames({ "S_NATIONKEY" });

	
	//vector<vector<string>> vss;


	//tMM.createTable("supplier", "S_SUPPKEY,S_NAME,S_ADDRESS,S_NATIONKEY,S_PHONE,S_ACCTBAL,S_COMMENT", "int_char(25)_varchar(140)_int_char(25)_float_varchar(101)");

	//tM.tO->iterTable();

	return 0;
}

