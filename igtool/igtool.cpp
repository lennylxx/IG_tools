/*
  igtool.cpp 2011/08/15
  by 7k
  email: blxode [at] gmail.com
  extract and create the *.dat files used by Innocent Grey
  Supporting Games:
  Innocent Grey
    Caucasus: Nanatsuki no Nie(クロウカシス - 七憑キノ贄)
    PP pianissimo(ＰＰ-ピアニッシモ- 操リ人形ノ輪舞)
    Cartagra ~Tsuki kurui no Yamai~(カルタグラ～ツキ狂イノ病～)
    Kara no Shoujo(殻ノ少女)
    Nagomibako Innocent Grey Fandisc(和み匣 Innocent Greyファンディスク)
  Noesis
    Furifure(フリフレ)
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>

using namespace std;

typedef unsigned char BYTE;// 1 byte
typedef unsigned int DWORD;// 4 bytes

struct DATHDR{
  char magic[8]; // "PACKDAT."
  DWORD entry_count0;
  DWORD entry_count;
} hdr_x,hdr_c;

struct DATENTRY{
  char filename[32];
  DWORD offset;
  DWORD mode; // bmp: 0x30010000 others: 0x20000000
  DWORD uncompr_len;
  DWORD compr_len;
} entry_x,*entry_c;

void ExtractDAT(char *infilename);

void init1();
void init2();
void CreateDAT(char *folder, char *outfilename);

void CryptBuffer(DATENTRY entry, BYTE *buff);
void unCompress(DATENTRY entry, BYTE *compr, BYTE *uncompr);
void Compress(DATENTRY entry, BYTE *uncompr, BYTE *compr);

string GetExtension(char *filename);
string MakePath(char *filename);


/*        >>d(' _ ')b<<        */
int main(int argc, char **argv) {
  if(argc<3){
    cerr<<"igtool by 7k\n\n"\
    "usage:  Extraction mode: "<<argv[0]<<" -x input.dat\n\t"\
            "Creation mode:   "<<argv[0]<<" -c [folder] output.dat\n";
    return -1;
  }
  
  if(strcmp(argv[1],"-x")==0)
  ExtractDAT(argv[2]);
  else if(strcmp(argv[1],"-c")==0){
    if(argc!=4) 
    cerr<<"This flag requires you to set the output file. "\
          "See usage for more detail."<<endl;
    else 
      CreateDAT(argv[2],argv[3]);
  }
  else cerr<<"Unknown flag: "<<argv[1]<<endl;
  return 0;
}
/*********** Extraction Mode ************/
void ExtractDAT(char *infilename){
  ifstream infile(infilename, ios::binary);
  if(!infile.is_open()){
    cerr<<"Could not open "<<infilename<<" (No such file or directory)."<<endl;
    exit(1);
  }
  
  DATHDR hdr_x;
  infile.read((char *)&hdr_x, sizeof(hdr_x));
  DWORD p = sizeof(hdr_x);
  
  for (DWORD i=0; i<hdr_x.entry_count; i++) {
    DATENTRY entry_x;
    infile.seekg(p, ios::beg);
    infile.read((char *)&entry_x, sizeof(entry_x));
    p += sizeof(entry_x);
    
    infile.seekg(entry_x.offset, ios::beg);
    
    BYTE *compr = new BYTE[entry_x.compr_len];
    BYTE *uncompr = new BYTE[entry_x.uncompr_len];
    
    infile.read((char *)compr, entry_x.compr_len);
    
    unCompress(entry_x, compr, uncompr);
    CryptBuffer(entry_x, uncompr);
    
    printf("[+] %-32s offset[%08x] size[%08x]\n",
    entry_x.filename,entry_x.offset,entry_x.compr_len);
    
    string folder = MakePath(infilename);
    string filepath = folder + "/" + entry_x.filename;
    ofstream outfile(filepath.c_str(), ios::binary);
    outfile.write((const char*)uncompr, entry_x.uncompr_len);
    
    outfile.close();
    delete [] compr;
    delete [] uncompr;
  }
  cout<<"\nTotal: "<<hdr_x.entry_count<<endl;
  infile.close();
}
/*********** Creation Mode ************/
void init1(){
  ifstream flist("flist.tmp");
  string filename;
  char m[8] = {'P','A','C','K','D','A','T','.'};
  strcpy(hdr_c.magic, m);
  hdr_c.entry_count = 0;
  while(flist>>filename){
    hdr_c.entry_count++;
  }
  hdr_c.entry_count0 = hdr_c.entry_count;
  flist.close();
}

void init2(string path){
  ifstream flist("flist.tmp");
  string filename;
  DWORD p = sizeof(hdr_c) + hdr_c.entry_count * sizeof(*entry_c);
  for(DWORD i = 0; i < hdr_c.entry_count; i++){
    flist>>filename;
    memset(entry_c[i].filename, 0, 32);
    strcpy(entry_c[i].filename, filename.c_str());
    
    filename = path + '/' + filename;
    struct _stat buf;
    _stat(filename.c_str(), &buf);
    
    entry_c[i].compr_len = entry_c[i].uncompr_len = buf.st_size;//unreliable
    if(GetExtension(entry_c[i].filename) == ".bmp")
    entry_c[i].mode = 0x30010000;
    else entry_c[i].mode = 0x20000000;
    
    entry_c[i].offset = p;
    p += entry_c[i].compr_len;
  }
  flist.close();
}

void CreateDAT(char *folder, char *outfilename){
  string path(folder);
  string cmd = "dir /B " + path +" >flist.tmp";
  system(cmd.c_str());

  if(_access(outfilename,0) != -1) { 
    cout<<"File "<<outfilename<<" exists, program exits."<<endl;
    remove("flist.tmp");
    exit(1);
  }
  ofstream outfile(outfilename, ios::binary | ios::app);
  
  /* Initialize */
  init1();
  entry_c = (DATENTRY *)new BYTE[hdr_c.entry_count * sizeof(*entry_c)];
  init2(path);
  
  /* Write the header */
  outfile.write((const char*)&hdr_c,sizeof(hdr_c));

  /* Write the file table */
  for(DWORD i = 0; i < hdr_c.entry_count; i++)
  outfile.write((const char*)&entry_c[i], sizeof(entry_c[i]));
  
  /* Write the file data  */
  ifstream flist("flist.tmp");
  string filename;
  for(DWORD i = 0; i < hdr_c.entry_count; i++)
  {
    flist>>filename;
    BYTE *compr = new BYTE[entry_c[i].compr_len];
    BYTE *uncompr = new BYTE[entry_c[i].uncompr_len];
    
    filename = path + '/' + filename;
    const char *fn = filename.c_str();
    ifstream file(fn, ios::binary);
    if(!file.is_open()){
      cerr<<"Could not open "<<fn<<" (No such file or directory)"<<endl;
      exit(1);
    }
    
    file.read((char *)uncompr, entry_c[i].uncompr_len);
    file.close();
    CryptBuffer(entry_c[i], uncompr);
    Compress(entry_c[i], uncompr, compr);

    outfile.write((const char*)compr, entry_c[i].compr_len);
    
    printf("[-] %-32s offset[%08x] size[%08x]\n",
    entry_c[i].filename,entry_c[i].offset,entry_c[i].compr_len);
    
    delete [] compr;
    delete [] uncompr;
  }
  cout<<"\nTotal: "<<hdr_c.entry_count<<endl;
  
  outfile.close();
  flist.close();
  remove("flist.tmp");
}

void CryptBuffer(DATENTRY entry, BYTE *buff){
  if(GetExtension(entry.filename) == ".s"){
    for (DWORD i = 0; i < entry.compr_len; i++)
    buff[i] ^= 0xFF;
  }
}

void unCompress(DATENTRY entry, BYTE *compr, BYTE *uncompr){
  //this funtion comes from the source code of the Crass.
  if (entry.mode & 0xffffff) {
    if (entry.mode & 0x10000) {// mode 0x10000
      DWORD t = entry.compr_len / 4;
      t = (t << ((t & 7) + 8)) ^ t;
      DWORD *enc_buf = (DWORD *)compr;
      DWORD *dec_buf = (DWORD *)uncompr;
      for (DWORD i = 0; i < entry.compr_len / 4; i++) {
        BYTE shifts;
        dec_buf[i] = enc_buf[i] ^ t;
        shifts = (BYTE)(dec_buf[i] % 24);
        t = (t << shifts) | (t >> (32 - shifts));
      }
    }
  }
  else memcpy(uncompr, compr, entry.compr_len);
}

void Compress(DATENTRY entry, BYTE *uncompr, BYTE *compr){
  if (entry.mode & 0xffffff) {
    if (entry.mode & 0x10000) {// mode 0x10000
      DWORD t = entry.uncompr_len / 4;
      t = (t << ((t & 7) + 8)) ^ t;
      DWORD *dec_buf = (DWORD *)uncompr;
      DWORD *enc_buf = (DWORD *)compr;
      for (DWORD i = 0; i < entry.uncompr_len / 4; i++) {
        BYTE shifts;
        enc_buf[i] = dec_buf[i] ^ t;
        shifts = (BYTE)(dec_buf[i] % 24);
        t = (t << shifts) | (t >> (32 - shifts));
      }
    }
  }
  else memcpy(compr, uncompr, entry.uncompr_len);
}

string GetExtension(char *filename){
  string fn = filename;
  int end = fn.length();
  int pt = fn.rfind('.');
  string ext = fn.substr(pt,end);
  return ext;
}

string MakePath(char *filename){
  string fn = filename;
  int pt = fn.rfind('.');
  string folder = fn.substr(0,pt) + '~';
  _mkdir(folder.c_str());
  return folder;
}