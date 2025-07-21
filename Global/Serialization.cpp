#include "pch.h"
#include "Serialization.h"
#define MAXBYTESIZE 5
#define MAXINDEXSIZE 3
using namespace Serialization;
void Serialization::SaveToFile(std::string buffer) {
	std::ofstream out("D:/program/Eng/Op.bin");
	if (out.is_open()) {
		buffer.erase(0,1);
		out << buffer;
		out.close();
	}
}
MetaDataBlock Serialization::LoadMetaData() {
	std::ifstream in("D:/program/Eng/MOp.bin");
	std::string line;
	MetaDataBlock MB;
	while (std::getline(in, line)) {
		if (in.eof())break;
		size_t z = line.find(":");
		std::string v = line.substr(0, z);
		short index = std::stoi(v);
		switch (index)
		{
		case 0: {
			MB.LastGlobalID = std::stoi(line.substr(z + 1));
			break;
		}
		case 1: {
			std::string val = line.substr(z+1);
			int first = 0;
			int second = val.find("'");//here
			while (second !=-1) {
				auto valx = val.substr(first, second - first);
				short c = std::stoi(valx);
				MB.LeftOverIds.push_back(c);
				first = second+1;
				second = val.find("'",first);
			}
		}
		}
	}
	in.close();
	return MB;
}
void Serialization::SaveMetaData(MetaDataBlock MB) {
	std::string buffer;
	buffer += "0:"+std::to_string(MB.LastGlobalID)+"\n"; //1
	std::string nums;
	for (int i = 0; i < MB.LeftOverIds.size(); i++) {
		nums += std::to_string(MB.LeftOverIds[i]) + "'";
	}
	buffer += "1:"+(nums.substr(0, nums.size()))+"\n";//2
	std::ofstream out("D:/program/Eng/MOp.bin");
	if (out.is_open()) {
		out << buffer;
		out.close();
	}
}
std::vector<ObjectBlocks> Serialization::ReadFromFile() {
	std::ifstream in("D:/program/Eng/Op.bin");
	std::string line;
	std::vector<ObjectBlocks> ObjectBlock;
	while (std::getline(in, line)) {
		if (in.eof())break;
		if (line == "")continue;
		auto z = line.find(":");
		std::string className =line.substr(1,z-1);
		int prev = z;
		z = line.find(":",z+1);
		int index = std::stoi(line.substr(prev+1, MAXINDEXSIZE).c_str());
		std::string bytes = line.substr(z + 1,MAXBYTESIZE);
		int byte = std::atoi(bytes.c_str());
		std::string buffer(byte, ' ');
		in.read(&buffer[0],byte+1);
		buffer = line +"\n"+ buffer;

		ObjectBlock.push_back(ObjectBlocks(index,className,buffer));
	}
	for (auto& it : ObjectBlock) {
		std::istringstream  iss(it.blockBuffer);
		while (std::getline(iss, line)) {
			if (line._Starts_with("-1:")) {
				//fix inherited object
				size_t off = line.find(":");
				size_t off2 = line.find(":", off+1);
				std::string val = line.substr(off2+1);
				val.erase(0,1);
				val.erase(val.size()-1,1);
				if (val == "null")continue;
				short Id = std::stoi(val);
				it.InheritedId = Id;
			}
			if (line._Starts_with("{")) {
				size_t off = line.find(":");
				std::string bytes = line.substr(off + 1, MAXBYTESIZE);
				std::string name = line.substr(1, off-1);
				int byte = std::atoi(bytes.c_str());
				std::string buffer(byte,' ');
				iss.read(&buffer[0],byte);
				buffer = line + "\n" + buffer;
				it.propBlocks.push_back(PropertyBlock(name, buffer));
			}
		}
	}
	in.close();
	return ObjectBlock;
}