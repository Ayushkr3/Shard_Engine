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
std::vector<ObjectBlocks> Serialization::ReadFromFile() {
	std::ifstream in("D:/program/Eng/Op.bin");
	std::string line;
	std::vector<ObjectBlocks> ObjectBlock;
	while (std::getline(in, line)) {
		if (in.eof())break;
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