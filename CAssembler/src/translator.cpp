#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;

// 각 명령어를 저장할 map
map<string, int> mri;
map<string, int> nonMri;
map<string, int> pseudo;

// Address Symbol Table
map<string, int> addSymTable;

// 프로그램 경로
string programDir = "res/program3.txt";

// 어셈블러 초기화
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

// 테이블 전체 출력
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
	// inst에 mri 맵에 있는 내용이 있는지 확인
	// mri 에서 명령어 하나씩 잡고 inst에 포함됐는지 확인
	size_t foundMRI = string::npos;

	map<string, int>::iterator iter; // iterator
	for (iter = mri.begin(); iter != mri.end(); iter++) {
		if (foundMRI != string::npos) { // MRI 명령어를 찾았을 때
			break;
		}

		foundMRI = inst.find(iter->first); // inst에 map의 key값 (MRI 명령어)가 있는지 확인
		getInst = iter->first; // 찾은 명령어를 getInst에 저장
	}

	return foundMRI != string::npos;
}

bool IsMri(const string& inst)
{
	size_t foundMRI = string::npos;

	map<string, int>::iterator iter; // iterator
	for (iter = mri.begin(); iter != mri.end(); iter++) {
		if (foundMRI != string::npos) { // MRI 명령어를 찾았을 때
			break;
		}

		foundMRI = inst.find(iter->first); // inst에 map의 key값 (MRI 명령어)가 있는지 확인
	}

	return foundMRI != string::npos;
}

bool IsNonMri(const string& inst, string& getInst)
{
	size_t foundNonMRI = string::npos;

	map<string, int>::iterator iter; // iterator
	for (iter = nonMri.begin(); iter != nonMri.end(); iter++) {
		if (foundNonMRI != string::npos) { // Non-MRI 명령어를 찾았을 때
			break;
		}

		getInst = iter->first; // 찾은 명령어를 getInst에 저장
		foundNonMRI = inst.find(iter->first); // inst에 map의 key값 (Non-MRI 명령어)가 있는지 확인
	}

	return foundNonMRI != string::npos;
}

bool IsPseudo(const string& inst)
{
	size_t foundPseudo = string::npos;

	map<string, int>::iterator iter; // iterator
	for (iter = pseudo.begin(); iter != pseudo.end(); iter++) {
		if (foundPseudo != string::npos) { // MRI 명령어를 찾았을 때
			break;
		}

		foundPseudo = inst.find(iter->first); // inst에 map의 key값 (MRI 명령어)가 있는지 확인
	}

	return foundPseudo != string::npos;
}

bool HasLabel(const string& inst)
{
	// ,가 있으면 라벨이 있는 것으로 판단
	// ,가 없는 경우 False, 있는 경우 True
	// find 메소드에서 찾는 문자가 없는 경우 string::npos를 리턴
	return inst.find(",") != string::npos;
}

// 16진수(int) -> 2진수(string) 변환
string HexToBinary(int hex)
{
	// 16진수 -> string 형태로 변경
	stringstream ss; 
	ss << uppercase << std::hex << hex;
	string hexStr = ss.str();

	string binary = "";
	for (int i = 0; i < hexStr.length(); i++)
	{
		switch (hexStr[i]) // 각 자리에 대응되는 2진수
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

// 명령어에 있는 모든 요소를 공백으로 분리해 벡터에 추가해 전달
vector<string> SplitInstruction(string inst)
{
	vector<string> words;

	stringstream ss(inst); // 명령어 한 줄의 내용들을 공백으로 구분
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
	while (getline(inputFile, line)) // 파일에서 한 줄씩 읽기
	{ 
		if (HasLabel(line)) { // 만약 Label 필드면	
			string labelTemp = line.substr(0, line.find(",")); // 맨 앞 ~ ','전까지 읽음
			addSymTable.insert({ labelTemp, LC }); // key를 Label, value를 주소로 해서 map에 저장
		}
		else 
		{
			if (line.find("ORG") != string::npos) // ORG N
			{
				string addTemp = line.substr(line.find(" ") + 1, line.length()); // 주소 값만 읽어오기
				LC = stoi(addTemp); // 정수로 변환 후 저장
				continue; // 다음 라인을 N번으로 시작하기 위해 아래 주소 증가 스킵
			}
		}

		LC++; // 주소 증가
	}
	cout << "---------Address Symbol Table---------" << endl;
	PrintTable(addSymTable); // Address Symbol Table 출력
	cout << endl;

	inputFile.close();
}

// second pass
// 명령어 2진수로 출력
void TranslateInstruction()
{
	cout << "---------Machine Code---------" << endl;

	ifstream inputFile(programDir);

	int LC = 0x0000;
	string line;
	while (getline(inputFile, line)) // 파일에서 한 줄씩 읽기
	{
		string instruction = ""; // 명령어 한 라인

		if (IsPseudo(line)) // Pseudo Instruction
		{
			if (line.find("ORG") != string::npos) // ORG N
			{
				string addTemp = line.substr(line.find(" ") + 1, line.length()); // 주소 값만 읽어오기
				LC = stoi(addTemp); // 정수로 변환 후 저장
				continue; // 다음 라인을 N번으로 시작하기 위해 아래 주소 증가 스킵
			}
		}
		else
		{
			string inst = ""; // IsMri 함수에서 찾은 명령어를 받아옴
			if (IsMri(line, inst)) // MRI
			{
				// Opcode 설정
				// 해당하는 OPCODE 16(int)->2(string)로 변경한 후 instruction에 append
				// 만약 Indirect면 명령어에 8을 더해서 전달
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

				// Address 설정
				string address = "000000000000"; // 12비트
				string tempAddress;
				// 명령어에서 Address 값 찾기 -> 공백으로 다 쪼갠다음에 Label 아니고, Opcode 아니고, I 아닌 문자열 가져오기
				vector<string> lineComponents = SplitInstruction(line); // 명령어의 각 내용을 저장할 벡터
				// 벡터 한 번 돌면서 주소 나타내는 문자열 찾음
				vector<string>::iterator iter;
				for (iter = lineComponents.begin(); iter != lineComponents.end(); iter++)
				{
					bool notAddressMode = (*iter != "I"); // I가 아님
					bool notOpcode = !IsMri(*iter); // Opcode가 아님
					bool notLabel = !HasLabel(*iter); // 주소로 라벨이 사용되는 경우는 제외 (, 여부)

					if (notAddressMode && notOpcode && notLabel)
					{
						tempAddress = *iter;
					}
				}
				// Label이면 Table에서 주소 가져와서 그걸로, 아니면 값 바로 2진수로 바꿔서 사용
				// map.find -> 못 찾으면 map::end iterator 리턴
				if (addSymTable.find(tempAddress) != addSymTable.end())
				{
					tempAddress = HexToBinary(addSymTable[tempAddress]);
				}
				else
				{
					tempAddress = HexToBinary(stoi(tempAddress, 0, 16));
				}

				// address의 마지막부터 거꾸로 명령어 복사
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
			else // MRI, Non-MRI 아니면 오류
			{
				cout << "Error in instruction code" << endl;
			}

			cout << instruction << endl; // 명령어 출력

			if (inst == "HLT")
			{
				break; // HLT면 break
			}
		}
		
		LC++; // 주소 증가
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