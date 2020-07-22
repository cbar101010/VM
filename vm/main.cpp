//
//  main.cpp
//  vm
//
//  Created by Colby Barton on 4/5/20.
//  Copyright © 2020 Colby Barton. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <iterator>
#include <vector>
#include <algorithm>
#include <map>
#include <ctype.h>


std::map<std::string, int> dictionary;
char memory[499998];
int reg[12];
int start = 0;
bool inVector(std::vector<std::string> const& v, std::string str) {
    return (std::count(v.begin(), v.end(), str));
}
void loadInt(int i, int offset) {
    char* cp = (memory + reg[7] + offset);
    *(int*)cp = i;
}

void loadChar(int c) {
    memory[reg[7]] = c;
}

int readInt(int addy) {
    int i, * ip;
    ip = (int*)(memory + addy);
    i = *ip;
    return i;
}

void loadIntToAddress(int i, int address) {
    char* cp = (memory + address);
    *(int*)cp = i;
}

void loadCharToAddress(int c, int address) {
    memory[address] = c;
}

void pass(bool secondPass, std::string fileName) {
    std::ifstream myfile;
    myfile.open(fileName);
    std::string line;    std::regex splitSpace("[^\\s]+");
    std::map<std::string, int> directives{ {".INT", 0}, {".BYT", 0 } };
    std::map<std::string, int> operands{ {"MOV", 7}, {"DIV", 17}, {"MUL", 16}, {"CMP", 20}, {"TRP", 21}, {"ADD", 13}, {"ADI", 14}, {"SUB", 15}, {"LDB",12}, {"LDR",10}, {"MOV",7}, {"LDA", 8}, {"STR", 9}, {"STB", 11}, {"CMP",  20}, {"TEST", 998}, {"TEST2", 999} };
    std::map<std::string, int> irOperands{ {"STR", 22}, {"LDR", 23}, {"STB", 24}, {"LDB", 25} };
    std::map<std::string, int> jmpInstructions{ {"JMP", 1}, {"JMR", 2 }, {"BNZ", 3}, {"BGT", 4}, {"BLT", 5}, {"BRZ", 6} };
    std::map<std::string, int> registers{ {"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3}, {"R4", 4}, {"R5", 5}, {"R6", 6}, {"R0,", 0}, {"R1,", 1}, {"R2,", 2}, {"R3,", 3}, {"R4,", 4}, {"R5,", 5}, {"R6,", 6}, {"PC", 7}, {"SL", 8}, {"SP", 9}, {"FP", 10}, {"SB", 11}, {"PC,", 7}, {"SL,", 8}, {"SP,", 9}, {"FP,", 10}, {"SB,", 11} };
    int lineCount = 0;
    reg[7] = 0;
    while (std::getline(myfile, line)) {
        auto start = std::sregex_iterator(line.begin(), line.end(), splitSpace);
        auto end = std::sregex_iterator();
        std::vector<std::string> currentVector;
        for (std::sregex_iterator i = start; i != end; ++i) {
            std::smatch curMatch = *i;
            std::string curString = curMatch.str();
            if(curString == "false"){
                int x = 1;
            }
            if (!(curString[0] == ';')) {
                currentVector.push_back(curString);
            }
            else
                break;
        }
        //std::cout << lineCount;
        //std::cout << "\n";
        lineCount++;
        if (currentVector.empty() || currentVector[0][0] == '#')
            continue;
        int size = currentVector.size();
        if(lineCount == 21){
            int a = 1;
        }
        if (size > 4 || size < 2) {
            std::cout << "line is either too small or too big " << lineCount << std::endl;
        }

        int index = 0;
        if ((directives.count(currentVector[index]) + operands.count(currentVector[index]) + irOperands.count(currentVector[index]) + jmpInstructions.count(currentVector[index])) == 0) {
            if (!secondPass) {
                dictionary[currentVector[0]] = reg[7];
            }
            index++;
        }
        if (directives.count(currentVector[index])) {
            if (currentVector[index] == ".BYT") {
                if (secondPass) {
                    index++;
                    loadChar(std::stoi(currentVector[index]));
                }
                reg[7] += 1;
            }
            else if (currentVector[index] == ".INT") {
                if (secondPass) {
                    index++;
                    loadInt(std::stoi(currentVector[index]), 0);
                }
                reg[7] += 4;
            }
        }
        else if (irOperands.count(currentVector[index]) && registers.count(currentVector[index + 1]) && registers.count(currentVector[index + 2]) && secondPass) {
            loadInt(irOperands[currentVector[index]], 0);
            index++;
            loadInt(registers[currentVector[index]], 4);
            index++;
            loadInt(registers[currentVector[index]], 8);
        }
        else if (operands.count(currentVector[index]) && secondPass) {
            loadInt(operands[currentVector[index]], 0);
            index++;
            if (currentVector[index - 1] == "TRP") {
                loadInt(std::stoi(currentVector[index]), 4);
            }
            else if (currentVector[index - 1] == "LDA" || currentVector[index - 1] == "STR") {
                if (dictionary.count(currentVector[index + 1])) {
                    loadInt(registers[currentVector[index]], 4);
                    index++;
                    loadInt(dictionary[currentVector[index]], 8);
                }
                else {
                    std::cout << "LDA WITH A REGISTER NOT ALLOWED" << lineCount;
                }
            }
            else if (currentVector[index - 1] == "ADI") {
                loadInt(registers[currentVector[index]], 4);
                index++;
                std::string str = currentVector[index];
                str.replace(0,1,"");
                loadInt(std::stoi(str), 8);
            }
            else {
                loadInt(registers[currentVector[index]], 4);
                index++;
                if (dictionary.count(currentVector[index])) {
                    loadInt(dictionary[currentVector[index]], 8);
                }
                else {
                    loadInt(registers[currentVector[index]], 8);
                }
            }
        }
        else if (jmpInstructions.count(currentVector[index]) && secondPass) {
            loadInt(jmpInstructions[currentVector[index]], 0);
            index++;
            if (currentVector[index - 1] == "JMP") {
                int f = dictionary[currentVector[index]];
                loadInt(f, 4);
            }
            else if (currentVector[index - 1] == "JMR") {
                int f = registers[currentVector[index]];
                loadInt(f, 4);
            }
            else {
                loadInt(registers[currentVector[index]], 4);
                index++;
                int f = dictionary[currentVector[index]];
                loadInt(f, 8);
            }
        }
        if (directives.count(currentVector[0]) + directives.count(currentVector[1]) == 0) {
            reg[7] += 12;
        }
    }
    myfile.close();
    if (!secondPass)
        start = dictionary["START"];
}

void vm() {
    int* ip;
    int opCode, opd1, opd2;
    reg[8] = reg[7] + 1;
    reg[7] = start;
    reg[11] = sizeof(memory); //set sb
    reg[10] = -15; //set original fp to null
    reg[9] = reg[11] - 4; //set SP to the first free location
    bool running = true;
    while (running) {
        ip = (int*)(memory + reg[7]);
        opCode = *ip;
        ip = (int*)(memory + (reg[7] + 4));
        opd1 = *ip;
        ip = (int*)(memory + (reg[7] + 8));
        opd2 = *ip;
        reg[7] += 12;
        switch (opCode)
        {
        //TEST
        case 998:
            reg[opd1] = reg[opd1];
            break;
        //TEST2
        case 999:
            reg[opd1] = reg[opd1];
            break;
        //JMP this seems to work yippeee
        case 1:
            reg[7] = opd1;
            break;
            //JMR
        case 2:
            reg[7] = reg[opd1];
            break;
            //BNZ
        case 3:
            if (reg[opd1] != 0) {
                reg[7] = opd2;
            }
            break;
        //BGT
        case 4:
            if (reg[opd1] > 0) {
                reg[7] = opd2;
            }
            break;
        //BLT
        case 5:
            if (reg[opd1] < 0) {
                reg[7] = opd2;
            }
            break;
        //BRZ
        case 6:
            if (reg[opd1] == 0) {
                reg[7] = opd2;
            }
            break;
        //MOV
        case 7:
            reg[opd1] = reg[opd2];
            break;
        //LDA
        case 8:
            reg[opd1] = opd2;
            break;
        //STR
        case 9:
            //NOT TESTED
            loadIntToAddress(reg[opd1], opd2);
            break;
        //LDR
        case 10:
            reg[opd1] = readInt(opd2);
            break;
        //STB
        case 11:
            loadCharToAddress(reg[opd1], opd2);
            break;
        //LDB
        case 12:
            reg[opd1] = (char)readInt(opd2);
            break;
        //ADD
        case 13:
            reg[opd1] = reg[opd1] + reg[opd2];
            break;
        //ADI
        case 14:
            reg[opd1] = reg[opd1] + opd2;
            break;
        //SUB
        case 15:
            reg[opd1] = reg[opd1] - reg[opd2];
            break;
        //MUL
        case 16:
            reg[opd1] = reg[opd1] * reg[opd2];
            break;
        //DIV
        case 17:
            reg[opd1] = reg[opd1] / reg[opd2];
            break;
        //CMP
        case 20:
            //NOT SURE IF THIS IS CORRECT
            if (reg[opd1] == reg[opd2]) {
                reg[opd1] = 0;
            }
            else if (reg[opd1] > reg[opd2]) {
                reg[opd1] = 1;
            }
            else {
                reg[opd1] = -1;
            }
            break;
        //TRP
        case 21:
            if (opd1 == 0) {
                return;
            }
            else if (opd1 == 1) {
                std::cout << reg[3];
            }
            else if (opd1 == 3) {
                std::cout << (char)reg[3];
            }
            else if(opd1 == 2){
                 reg[3] = std::getchar();

            }
            else if (opd1 == 4) {
                std:: cin >> reg[3];
            }
            break;
        //STR (Register Indirect Addressing)
        case 22:
            //NOT TESTED
            loadIntToAddress(reg[opd1], reg[opd2]);
            break;
        //LDR (Register Indirect Addressing)
        case 23:
            reg[opd1] = readInt(reg[opd2]);
            break;
        //STB (Register Indirect Addressing)
        case 24:
            //NOT TESTED
            loadCharToAddress(reg[opd1], reg[opd2]);
            break;
        //LDB (Register Indirect Addressing)
        case 25:
            reg[opd1] = (char)readInt(reg[opd2]);
            break;
        default:
            break;
        }
    }
}
int main(int argc, const char * argv[]) {
    // insert code here...
    std::string fileName = "test1.asm";
     if (argv[1]) {
         fileName = argv[1];
     }

     pass(false, fileName);
     pass(true, fileName);
     vm();
     std::cout << std::endl;
     return 0;
}
