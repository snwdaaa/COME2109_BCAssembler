#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;

// �� ��ɾ ������ map
map<string, int> mri;
map<string, int> nonMri;
map<string, int> pseudo;

// Address Symbol Table
map<string, int> addSymTable;

// ���α׷� ���
string programDir = "res/program3.txt";

// ����� �ʱ�ȭ
void InitTranslator()
{
	mri = {
		{"AND", 0x0},
		{"ADD", 0x1},
		{"LDA", 0x2},
		{"STA", 0x3},
		{"BUN", 0x4},
		{"BSA", 0x5},
		{"ISZ", 0x6}
	};

	nonMri = {
		{"CLA", 0x7800},
		{"CLE", 0x7400},
		{"CMA", 0x7200},
		{"CME", 0x7100},
		{"CIR", 0x7080},
		{"CIL", 0x7040},
		{"INC", 0x7020},
		{"SPA", 0x7010},
		{"SNA", 0x7008},
		{"SZA", 0x7004},
		{"SZE", 0x7002},
		{"HLT", 0x7001},
		{"INP", 0xF800},
		{"OUT", 0xF400},
		{"SKI", 0xF200},
		{"SKO", 0xF100},
		{"ION", 0xF080},
		{"IOF", 0xF040}
	};

	pseudo = {
		{"ORG", 0x0000},
		{"END", 0x0000},
		{"DEC", 0x0000},
		{"HEX", 0x0000}
	};
}

// ���̺� ��ü ���
void PrintTable(map<string, int> table)
{
	map<string, int>::iterator iter;
	for (iter = table.begin(); iter != table.end(); iter++)
	{
		cout << iter->first << " " << iter->second << endl;
	}
}

bool IsMri(const string& inst, string& getInst)
{
	// inst�� mri �ʿ� �ִ� ������ �ִ��� Ȯ��
	// mri ���� ��ɾ� �ϳ��� ��� inst�� ���Եƴ��� Ȯ��
	size_t foundMRI = string::npos;

	map<string, int>::iterator iter; // iterator
	for (iter = mri.begin(); iter != mri.end(); iter++) {
		if (foundMRI != string::npos) { // MRI ��ɾ ã���� ��
			break;
		}

		foundMRI = inst.find(iter->first); // inst�� map�� key�� (MRI ��ɾ�)�� �ִ��� Ȯ��
		getInst = iter->first; // ã�� ��ɾ getInst�� ����
	}

	return foundMRI != string::npos;
}

bool IsMri(const string& inst)
{
	size_t foundMRI = string::npos;

	map<string, int>::iterator iter; // iterator
	for (iter = mri.begin(); iter != mri.end(); iter++) {
		if (foundMRI != string::npos) { // MRI ��ɾ ã���� ��
			break;
		}

		foundMRI = inst.find(iter->first); // inst�� map�� key�� (MRI ��ɾ�)�� �ִ��� Ȯ��
	}

	return foundMRI != string::npos;
}

bool IsNonMri(const string& inst, string& getInst)
{
	size_t foundNonMRI = string::npos;

	map<string, int>::iterator iter; // iterator
	for (iter = nonMri.begin(); iter != nonMri.end(); iter++) {
		if (foundNonMRI != string::npos) { // Non-MRI ��ɾ ã���� ��
			break;
		}

		getInst = iter->first; // ã�� ��ɾ getInst�� ����
		foundNonMRI = inst.find(iter->first); // inst�� map�� key�� (Non-MRI ��ɾ�)�� �ִ��� Ȯ��
	}

	return foundNonMRI != string::npos;
}

bool IsPseudo(const string& inst)
{
	size_t foundPseudo = string::npos;

	map<string, int>::iterator iter; // iterator
	for (iter = pseudo.begin(); iter != pseudo.end(); iter++) {
		if (foundPseudo != string::npos) { // MRI ��ɾ ã���� ��
			break;
		}

		foundPseudo = inst.find(iter->first); // inst�� map�� key�� (MRI ��ɾ�)�� �ִ��� Ȯ��
	}

	return foundPseudo != string::npos;
}

bool HasLabel(const string& inst)
{
	// ,�� ������ ���� �ִ� ������ �Ǵ�
	// ,�� ���� ��� False, �ִ� ��� True
	// find �޼ҵ忡�� ã�� ���ڰ� ���� ��� string::npos�� ����
	return inst.find(",") != string::npos;
}

// 16����(int) -> 2����(string) ��ȯ
string HexToBinary(int hex)
{
	// 16���� -> string ���·� ����
	stringstream ss; 
	ss << uppercase << std::hex << hex;
	string hexStr = ss.str();

	string binary = "";
	for (int i = 0; i < hexStr.length(); i++)
	{
		switch (hexStr[i]) // �� �ڸ��� �����Ǵ� 2����
		{
			case '0': binary.append("0000"); break;
			case '1': binary.append("0001"); break;
			case '2': binary.append("0010"); break;
			case '3': binary.append("0011"); break;
			case '4': binary.append("0100"); break;
			case '5': binary.append("0101"); break;
			case '6': binary.append("0110"); break;
			case '7': binary.append("0111"); break;
			case '8': binary.append("1000"); break;
			case '9': binary.append("1001"); break;
			case 'A': binary.append("1010"); break;
			case 'B': binary.append("1011"); break;
			case 'C': binary.append("1100"); break;
			case 'D': binary.append("1101"); break;
			case 'E': binary.append("1110"); break;
			case 'F': binary.append("1111"); break;
			default: break;
		}
	}

	return binary;
}

// ��ɾ �ִ� ��� ��Ҹ� �������� �и��� ���Ϳ� �߰��� ����
vector<string> SplitInstruction(string inst)
{
	vector<string> words;

	stringstream ss(inst); // ��ɾ� �� ���� ������� �������� ����
	string word;
	while (getline(ss, word, ' '))
	{
		words.push_back(word);
	}

	return words;
}

// first pass
void MakeSymbolAddressTable()
{
	ifstream inputFile(programDir);

	int LC = 0x0000;

	string line;
	while (getline(inputFile, line)) // ���Ͽ��� �� �پ� �б�
	{ 
		if (HasLabel(line)) { // ���� Label �ʵ��	
			string labelTemp = line.substr(0, line.find(",")); // �� �� ~ ','������ ����
			addSymTable.insert({ labelTemp, LC }); // key�� Label, value�� �ּҷ� �ؼ� map�� ����
		}
		else 
		{
			if (line.find("ORG") != string::npos) // ORG N
			{
				string addTemp = line.substr(line.find(" ") + 1, line.length()); // �ּ� ���� �о����
				LC = stoi(addTemp); // ������ ��ȯ �� ����
				continue; // ���� ������ N������ �����ϱ� ���� �Ʒ� �ּ� ���� ��ŵ
			}
		}

		LC++; // �ּ� ����
	}
	cout << "---------Address Symbol Table---------" << endl;
	PrintTable(addSymTable); // Address Symbol Table ���
	cout << endl;

	inputFile.close();
}

// second pass
// ��ɾ� 2������ ���
void TranslateInstruction()
{
	cout << "---------Machine Code---------" << endl;

	ifstream inputFile(programDir);

	int LC = 0x0000;
	string line;
	while (getline(inputFile, line)) // ���Ͽ��� �� �پ� �б�
	{
		string instruction = ""; // ��ɾ� �� ����

		if (IsPseudo(line)) // Pseudo Instruction
		{
			if (line.find("ORG") != string::npos) // ORG N
			{
				string addTemp = line.substr(line.find(" ") + 1, line.length()); // �ּ� ���� �о����
				LC = stoi(addTemp); // ������ ��ȯ �� ����
				continue; // ���� ������ N������ �����ϱ� ���� �Ʒ� �ּ� ���� ��ŵ
			}
		}
		else
		{
			string inst = ""; // IsMri �Լ����� ã�� ��ɾ �޾ƿ�
			if (IsMri(line, inst)) // MRI
			{
				// Opcode ����
				// �ش��ϴ� OPCODE 16(int)->2(string)�� ������ �� instruction�� append
				// ���� Indirect�� ��ɾ 8�� ���ؼ� ����
				string binaryOpcode;
				if (line[line.length() - 1] == 'I')
				{
					binaryOpcode = HexToBinary(mri[inst] + 8);
				}
				else 
				{
					binaryOpcode = HexToBinary(mri[inst]);
				}		
				instruction.append(binaryOpcode);

				// Address ����
				string address = "000000000000"; // 12��Ʈ
				string tempAddress;
				// ��ɾ�� Address �� ã�� -> �������� �� �ɰ������� Label �ƴϰ�, Opcode �ƴϰ�, I �ƴ� ���ڿ� ��������
				vector<string> lineComponents = SplitInstruction(line); // ��ɾ��� �� ������ ������ ����
				// ���� �� �� ���鼭 �ּ� ��Ÿ���� ���ڿ� ã��
				vector<string>::iterator iter;
				for (iter = lineComponents.begin(); iter != lineComponents.end(); iter++)
				{
					bool notAddressMode = (*iter != "I"); // I�� �ƴ�
					bool notOpcode = !IsMri(*iter); // Opcode�� �ƴ�
					bool notLabel = !HasLabel(*iter); // �ּҷ� ���� ���Ǵ� ���� ���� (, ����)

					if (notAddressMode && notOpcode && notLabel)
					{
						tempAddress = *iter;
					}
				}
				// Label�̸� Table���� �ּ� �����ͼ� �װɷ�, �ƴϸ� �� �ٷ� 2������ �ٲ㼭 ���
				// map.find -> �� ã���� map::end iterator ����
				if (addSymTable.find(tempAddress) != addSymTable.end())
				{
					tempAddress = HexToBinary(addSymTable[tempAddress]);
				}
				else
				{
					tempAddress = HexToBinary(stoi(tempAddress, 0, 16));
				}

				// address�� ���������� �Ųٷ� ��ɾ� ����
				for (int i = 0; i < tempAddress.length(); i++)
				{
					address[address.length() - 1 - i] = tempAddress[tempAddress.length() - 1 - i];
				}

				instruction.append(address);
			}
			else if (IsNonMri(line, inst)) // Non-MRI
			{
				instruction.append(HexToBinary(nonMri[inst]));
			}
			else // MRI, Non-MRI �ƴϸ� ����
			{
				cout << "Error in instruction code" << endl;
			}

			cout << instruction << endl; // ��ɾ� ���

			if (inst == "HLT")
			{
				break; // HLT�� break
			}
		}
		
		LC++; // �ּ� ����
	}

	inputFile.close();
}

void FirstPass()
{
	MakeSymbolAddressTable();
}

void SecondPass()
{
	TranslateInstruction();
}