/*
	igscript.cpp v2.3 2012/7/22
	by 7k
	Debug Edition
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <windows.h>

using namespace std;

typedef char      s8;
typedef short     s16;
typedef int       s32;
typedef long long s64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

struct INS{//instruction
	u16 opcode;
	u16 opnum;
} ins;

struct TEMP{
	u16 unknown1;
	u16 unknown2;
} temp;

void ParseScript(char *scrfn, char *txtfn);
char* Crypt(char *fn, bool encrypt);

int main(int argc, char *argv[]) {
	if(argc < 3){
		cerr<<"igscript debug by 7k\n\n"\
		"usage:  Parsing Mode: "<<argv[0]<<" -p input.s output.txt\n\n\t"\

		"  Note: When dealing with \"CureGirl\", add an opt tag '-x' like this:\n\t"\
		<<argv[0]<<" -x -p input.s output.txt\n\n\t"\
		
		"!Debug edition only support \"Parsing Mode\".";
		
		return -1;
	}

	switch(argv[1][1]){
	case 'p':
		ParseScript(argv[2],argv[3]); break;
	case 'x':{
			if(argc<5){
				cerr<<"This flag requires you to set the input files. "\
				"See usage for more detail."<<endl; return -1;
			}
			char *tmp = Crypt(argv[3],1);
			if(strcmp(argv[2],"-p")==0)
			ParseScript(tmp,argv[4]);
			remove(tmp);
			break;
		}
	default:
		cerr<<"Unknown flag: "<<argv[1]<<endl; break;
	}

	return 0;
}

void ParseScript(char *scrfn, char *txtfn){

	ifstream infile(scrfn, ios::binary);
	if(!infile.is_open()){
		cerr<<"Could not open "<<scrfn<<" (No such file or directory)."<<endl;
		exit(1);
	}

	ofstream outtxt(txtfn, ios::binary);
	u8 len,*buf;

	char err[20];

	while(1){
		infile.read((char *)&ins, sizeof(ins));
		sprintf(err, "%04X %04X ", ins.opcode, ins.opnum);
		outtxt<<err;
		if(infile.eof()) break;
		
		if((ins.opcode>>8) == 0x08){
			infile.read((char *)&temp, sizeof(temp));
			sprintf(err, "%04X %04X\n", temp.unknown1, temp.unknown2);
			outtxt<<err;
			switch(ins.opcode){
			case 0x080D://jump
				outtxt<<"!JUMP!"<<endl;
				break;
				
			case 0x0817:
			case 0x081D:{//option and jump
					outtxt<<"!OPTIONJUMP!"<<endl;
					len = ins.opnum;
					if(len != 0){
						buf = new u8[len];
						infile.read((char *)buf, len);
						string txt((const char*)buf);
						s32 padlen = len - txt.length();
						if(padlen < 0) txt = txt.substr(0,len);
						outtxt<<txt<<endl;
					}
					break;
				}
			case 0x081E://ogg -- 1E 0800 00
			case 0x081F://ogg -- 1F 0800 00
			case 0x0820://ogg -- 20 0800 01
			case 0x0822://ogg -- 22 0800 01
			case 0x0827://ogg -- 27 0800 00
			case 0x0828://ogg -- 28 0800 00
			case 0x082E://ogg -- 2E 0800 00
			case 0x0830:{//ogg -- 30 0800 00
					len = temp.unknown2>>8;
					if(len != 0){
						buf = new u8[len];
						infile.read((char *)buf, len);
						outtxt.write((const char *)buf, len); outtxt<<endl;
					}
					break;
				}
			default:
				break;
			}
		}
		
		else if((ins.opcode>>8) == 0x04){
			switch(ins.opcode){
			case 0x0400://subtitle 00 0400 0C
			case 0x043F:{//subtitle 3F 0400 0C
					len = ins.opnum>>8;
					if(len != 0){
						buf = new u8[len];
						infile.read((char *)buf, len);
						string txt((const char*)buf);
						s32 padlen = len - txt.length();
						if(padlen < 0) txt = txt.substr(0,len);
						outtxt<<txt<<endl;
					}
					break;
				}
				// case 0x0401:
			case 0x0402:{//s
					len = ins.opnum>>8;
					if(len>=4){
						buf = new u8[len];
						infile.read((char *)buf, len);
						outtxt.write((const char *)buf, len); outtxt<<endl;
					}
					break;
				}
			case 0x040D://bmp -- 0D 0400 08
			case 0x040F://bmp -- 0F 0401 09
			case 0x0410://bmp -- 10 0401 09
			case 0x0412://bmp -- 12 0400 0Bs
			case 0x0418://bmp -- 18 0402 0A 
			case 0x0425://bmp -- 25 0401 08
			case 0x0430://bmp -- 30 0402 0B
			case 0x0435://bmp -- 35 0400 09
			case 0x043C://bmp -- 3C 0400 09
			case 0x047B://bmp -- 7B 0401 09
			case 0x0499://bmp -- 99 0400 0C/99 0401 0C
			case 0x049C://png -- 9C 0401 0D
			case 0x04AD://bmp -- AD 0400 09
			case 0x04B4:{//png -- B4 0400 0C
					len = ins.opnum>>8;
					if(len>=6){
						buf = new u8[len];
						infile.read((char *)buf, len);
						outtxt.write((const char *)buf, len); outtxt<<endl;
					}
					break;
				}
			default:
				break;
			}
		}
		
		else if(ins.opcode == 0x054A || ins.opcode == 0x0551){//special case
			u8 zero;
			infile.read((char *)&zero, 1);
			sprintf(err, "%02X\n", zero);
			outtxt<<err;
		}
		
		else if(ins.opcode == 0x1006 || ins.opcode == 0x1008){//jump
			outtxt<<"!JUMP!"<<endl;
			for (u8 i=1;i<=3;i++){
				infile.read((char *)&temp, sizeof(temp));
				sprintf(err, "%04X %04X ", temp.unknown1, temp.unknown2);
				outtxt<<err;
			}
			outtxt<<endl;
		}
		
		else if(ins.opcode == 0x0C49 || ins.opcode == 0x0C50){ //|| ins.opcode == 0x0190){
			infile.read((char *)&temp, sizeof(temp));
			sprintf(err, "%04X %04X\n", temp.unknown1, temp.unknown2);
			outtxt<<err;
		}
		
		else if(ins.opcode == 0x0C2B || ins.opcode == 0x0C2D || ins.opcode == 0x0C25){//ogg
			infile.read((char *)&temp, sizeof(temp));
			sprintf(err, "%04X %04X ", temp.unknown1, temp.unknown2);
			outtxt<<err;
			infile.read((char *)&temp, sizeof(temp));
			sprintf(err, "%04X %04X\n", temp.unknown1, temp.unknown2);
			outtxt<<err;
			len = temp.unknown1;
			buf = new u8[len];
			infile.read((char *)buf, len);
			outtxt.write((const char *)buf, len); outtxt<<endl;
		}
		
		else if(ins.opcode == 0x203D || ins.opcode == 0x2042){
			infile.read((char *)&temp, sizeof(temp));
			sprintf(err, "%04X %04X ", temp.unknown1, temp.unknown2);
			outtxt<<err;
			infile.read((char *)&temp, sizeof(temp));
			sprintf(err, "%04X %04X\n", temp.unknown1, temp.unknown2);
			outtxt<<err;
		}
		
		else if(ins.opcode == 0x0A37 ||ins.opcode == 0x0A3E){
			infile.read((char *)&temp, sizeof(temp));
			sprintf(err, "%04X %04X ", temp.unknown1, temp.unknown2);
			outtxt<<err;
			u16 len1;
			infile.read((char *)&len1, sizeof(len1));
			sprintf(err, "%04X\n", len1);
			outtxt<<err;
			if(len1 != 0){
				buf = new u8[len1];
				infile.read((char *)buf, len1);
				string txt((const char*)buf);
				s32 padlen = len1 - txt.length();
				if(padlen < 0) txt = txt.substr(0,len1);
				outtxt<<txt<<endl;
			}
		}
		
		else if (ins.opcode == 0x1472 || ins.opcode == 0x1473){//unknown
			for (u8 i=1;i<=4;i++){
				infile.read((char *)&temp, sizeof(temp));
				sprintf(err, "%04X %04X ", temp.unknown1, temp.unknown2);
				outtxt<<err;
			}
			outtxt<<endl;
		}
	}
	infile.close();
	outtxt.close();
}

char* Crypt(char *fn, bool encrypt){//encrypt or decrypt

	ifstream infile(fn, ios::binary);
	if(!infile.is_open()){
		cerr<<"Could not open "<<fn<<" (No such file or directory)."<<endl;
		exit(1);
	}

	char *tfn = new char[20];
	strcpy(tfn, fn);
	if(encrypt){
		strcat(tfn, ".x");
	}

	struct _stat buf;
	_stat(fn, &buf);
	u8 *buff = new u8[buf.st_size];

	infile.read((char *)buff, buf.st_size);
	ofstream outfile(tfn, ios::binary);

	for (s32 i=0;i<buf.st_size;i++){
		buff[i] ^= 0xFF;
	}
	outfile.write((const char*)buff, buf.st_size);
	
	infile.close();
	outfile.close();
	return tfn;
}
