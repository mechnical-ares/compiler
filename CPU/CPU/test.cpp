#include <iostream>
#include <bitset>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <stdlib.h>
#include <string>
using namespace std;

ifstream code;					//���������
ofstream prog;					//���rom
ofstream ram;				//�����ROM��RAMģʽ�µ�ram
ofstream ram0;				//�����ROM4RAMģʽ�µ�ram0
ofstream ram1;				//�����ROM4RAMģʽ�µ�ram1
ofstream ram2;				//�����ROM4RAMģʽ�µ�ram2
ofstream ram3;				//�����ROM4RAMģʽ�µ�ram3

int p_line=0;					//�������ڴ���ε��к�
int d_line=0;					//�����������ݶε��к�
int total_line=0;				//ʵ�ʻ�������к�
int reach_code=0;				//�Ƿ񵽴�����
int reach_data=0;				//�Ƿ񵽴����ݶ�
int baseaddress = 0;			//��¼���ݶλ���ַ����Ϊ.data�������ֱ��ָ�����ݶο�ʼ�ĵ�ַ
string err;
int line_prog = 0;					//��¼prog.coe�������

struct data_info
{
	string info;				//��¼���ݶ�ÿ�е���Ϣ
	int line;					//��¼�����ڴ����еľ��Ե�ַ���ڴ������붨λ
};


struct parameter				//�洢һ��������������Ϣ
{
	int base;					//�ñ����������ݶεĶλ�ַ
	//int line;					//�����ڴ����еľ����к�
    int offset;					//�ñ����������ݶ��е���Ե�ַ
	string name;				//��������
	vector<string> p_data;		//�洢�����Ķ�������ʽ�����ñ����洢�˶������ʱ��һ�μӽ�ȥ
	int p_type;					//��������
	/*
	p_type=1	.byte		1���ֽ�			8λ
	p_type=2	.half		����			16λ
	p_type=3	.word		��				32λ
	p_type=4	.ascii		ASCII�ַ�������	һ���ַ�ռһ���ֽڣ�����ascii��ֵ�洢
	p_type=5	.asciiz		��'\0'��β��ASCII�ַ�������		
	*/
};



map <string,int> label;				//�����г��ֵı��
vector<data_info> dataseg;			//�����ݶε�ÿһ�м�¼�����Ա��������
vector<parameter> para_info;		//������б�������Ϣ
map<string,int>::iterator itofmap;
vector<parameter>::iterator itofpara;
vector<data_info>::iterator itofdatainfo;
vector<string>::iterator it;


void scan_first(char * str);	//��һ��ɨ��
void scan_second(char * str);	//�ڶ���ɨ��
int ident(string str);			//����ָ��str�ı��
int ident_reg(string name);		//ʶ������Ϊname�ļĴ�����Ӧ�ļĴ������
void store_dataseg();			//���ݶε�ʶ����洢
void write_data();				//����RAM��д��
void outputerror(string err);	//���error
void check_operand(vector<string> & operand,string check,int id);	//���������������ظ�ָ��������ĸ���
void no_bracket(string name,vector<string> & operand,int id);	//�������������ݵĴ���
void get_p_data(string base,int radix,parameter * p);	//��ȡparameter��p_data���ԣ�����radixΪ����
string to32(string & str);		//��str�Ƿ�����չ��32λ

string filepath[7];

int main()
{
	ifstream paths("d:\\paths.txt");

	char path[100];
	paths.getline(path, sizeof(path));
	filepath[0] = path;
	code.open(path, ios::in);
	paths.getline(path, sizeof(path));
	filepath[1] = path;
	prog.open(path, ios::out);
	paths.getline(path, sizeof(path));
	filepath[2] = path;
	ram.open(path, ios::out);
	paths.getline(path, sizeof(path));
	filepath[3] = path;
	ram0.open(path, ios::out);
	paths.getline(path, sizeof(path));
	filepath[4] = path;
	ram1.open(path, ios::out);
	paths.getline(path, sizeof(path));
	filepath[5] = path;
	ram2.open(path, ios::out);
	paths.getline(path, sizeof(path));
	filepath[6] = path;
	ram3.open(path, ios::out);
	paths.close();

	cout<<"��һ��ɨ��..."<<endl;
	itofdatainfo = dataseg.begin();
	while(!code.eof())
	{
		total_line++;
		/*һ�������ܳ��ֶ��ٸ��ַ���100���Ƿ����*/
		char str[100];
		code.getline(str,sizeof(str));

		stringstream s(str);
		string temps;
		s>>temps;
		if(temps[0]=='\0')			//ȥ�����У����в��������ε��������
			continue;
		/*���濪ʼ��һ��ɨ�裬�����б����ͱ�Ŷ�ȡ��������*/
		scan_first(str);			
		if(reach_code==1)
			p_line++;				//�����¼����ʵ����Ϊ�˶�λ������
		if(reach_data==1)
			d_line++;				
	}
	cout<<"��һ��ɨ����ɣ�"<<endl<<endl;

	
	//��һ��ɨ����ɺ�������ݴ洢
	store_dataseg();

	cout<<"������Ϣ��"<<endl;
	itofpara=para_info.begin();
	for(;itofpara!=para_info.end();itofpara++)
	{
		cout<<"������"<<itofpara->name<<"��ֵΪ��";
		for(it = (itofpara->p_data).begin();it!= (itofpara->p_data).end();it++)
		{
			cout<<*it;
		}
		cout<<"�������ݶεĶλ�ַΪ��"<<itofpara->base<<"�����ݶ��е���Ե�ַΪ��"<<itofpara->offset<<endl;
	}
	cout<<endl;

	cout<<"�����Ϣ��"<<endl;
	for(itofmap=label.begin();itofmap!=label.end();itofmap++)
	{
		bitset<32> label_addr(itofmap->second);
		cout<<"��ţ�"<<itofmap->first<<"��rom�еĵ�ַΪ"<<label_addr<<"("<<itofmap->second<<")"<<endl;
	}
	cout<<endl;

	//��һ��ɨ��󼴿ɽ���RAM��д��
	write_data();

	reach_code=0;
	reach_data=0;
	total_line=0;
	p_line=0;

	code.close();
	code.open(filepath[0]);
	//code.seekg(0,ios::beg);
	cout<<"�ڶ���ɨ��..."<<endl<<endl;
	cout<<"��ʼд��prog.coe�ļ���"<<endl;

	prog<<"memory_initialization_radix = 2;"<<endl;
	prog<<"memory_initialization_vector ="<<endl;

	while(!code.eof())
	{
		total_line++;
		/*һ�������ܳ��ֶ��ٸ��ַ���100���Ƿ����*/
		char str[100];
		code.getline(str,sizeof(str));

		stringstream s(str);
		string temps;
		s>>temps;
		if(temps[0]=='\0')			//ȥ�����У����в��������ε��������
			continue;
		/*���濪ʼ�ڶ���ɨ�裬�����б����ͱ�Ŷ�ȡ��������*/
		scan_second(str);			
		if(reach_code==1)
			p_line++;
		if(reach_data==1)
			d_line++;
	}
	for(int i = line_prog;i<pow(2,14)-1;i++)
	{
		prog<<"00000000000000000000000000000000,"<<endl;
	}
	prog << "00000000000000000000000000000000;";


	cout<<"�ڶ���ɨ����ɣ�"<<endl<<endl;

	code.close();
	prog.close();
	ram.close();
	ram0.close();
	ram1.close();
	ram2.close();
	ram3.close();

	return 0;
}


void scan_first(char * str)
{
	vector<string> split_str;		//���ڴ�Ű��տո�ָ���str
	string tempword;
	stringstream ss(str);
	while(ss>>tempword)
		split_str.push_back(tempword);
	ss.clear();
	it=split_str.begin();

	int idofstr=ident(*it);			//���ڼ�¼str�ı��


	if(reach_code==0)	//��δ��������
	{
		if(reach_data==0)//��δ�������ݶΣ�������ܻ���ģʽ��ѡ��
		{
			if(idofstr!=58)
			{
				err="��ʽ�������ݶ�֮ǰ���ܳ��ַǹؼ��֣�";
				outputerror(err);
			}
			else
			{
					//������������ж���ѡģʽ
			}
		}
		else//�������ݶλ�δ��������,������ȫ�����ݵĶ���
		{
			data_info temp;
			temp.info = str;		//��¼����������Ϣ������
			temp.line = total_line;	//��¼��������Ϣ�ڴ����еľ���λ�ã��Ա㶨λ������
			dataseg.push_back(temp);	
		}
	}


	else//��������
	{
		if(*it==".CODE"||*it==".code")
			p_line--;						//.code�ǵ�0��

		else if(idofstr==60)				//��һ��ɨ��������б��
		{
			if(++it!=split_str.end())
			{
				err="��ʽ���󣡱�Ŷ���ֻ�ܵ���������һ�У����к��治�ܳ��ִ��룡";
				outputerror(err);
			}
			--it;						//���¶�λ�����ֵ
			label.insert(pair<string ,int >((*it).substr(0,(*it).length()-1),4*p_line));
		}

	}//����ε�һ��������
}



void scan_second(char * str)
{
	vector<string> split_str;		//���ڴ�Ű��տո�ָ���str
	string tempword;
	stringstream ss(str);
	while(ss>>tempword)
	{
		if(tempword[0]=='#')		//#�ź������ע��
			break;
		split_str.push_back(tempword);
	}
	ss.clear();
	it=split_str.begin();
	
	int idofstr=ident(*it);				//���ڼ�¼str�ı��
	if(reach_code==1 && idofstr!=58)	//ֻ��ȡʵ�ʴ��룬���ٷ������
	{
		cout<<str<<"      "<<idofstr<<endl;	//���ڲ���id�Ƿ�ʶ����ȷ
		vector<string> operand;			//���ڼ�¼ָ���еĲ�����

		++it;							//��λ���ڶ����ַ�

		string merge_operand;			//���ڴ�źϲ��ո������в�����
		//���ڿ��ܳ��ֵ�����������֮�䲻���ж��ţ��û����ܻ����˿ո��������Ⱥϲ��������������ַ����ٴ���
		for(;it!=split_str.end();it++)
		{
			merge_operand+=(*it);	//merge_operand����Ȼ�ж��������ŵ�
		}
		
		check_operand(operand,merge_operand,idofstr);

		string outcode="";
		switch(idofstr)
		{
		case 1://add
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100000";
			break;
			
		case 2://addu
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100001";
			break;

		case 3://sub
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100010";
			break;

		case 4://subu
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100011";
			break;

		case 5://and
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100100";
			break;

		case 6://mult
			outcode+="000000";
			outcode+=operand[0];
			outcode+=operand[1];
			outcode+="0000000000011000";
			break;

		case 7://multu
			outcode+="000000";
			outcode+=operand[0];
			outcode+=operand[1];
			outcode+="0000000000011001";
			break;

		case 8://div
			outcode+="000000";
			outcode+=operand[0];
			outcode+=operand[1];
			outcode+="0000000000011010";
			break;

		case 9://divu
			outcode+="000000";
			outcode+=operand[0];
			outcode+=operand[1];
			outcode+="0000000000011011";
			break;

		case 10://mfhi
			outcode+="0000000000000000";
			outcode+=operand[0];
			outcode+="00000010000";
			break;

		case 11://mflo
			outcode+="0000000000000000";
			outcode+=operand[0];
			outcode+="00000010010";
			break;

		case 12://mthi
			outcode+="000000";
			outcode+=operand[0];
			outcode+="000000000000000010001";
			break;

		case 13://mtlo
			outcode+="000000";
			outcode+=operand[0];
			outcode+="000000000000000010011";
			break;

		case 14://mfc0
			outcode+="01000000000";
			outcode+=operand[0];
			outcode+=operand[1];
			outcode+="00000000";
			outcode+=operand[2];
			break;

		case 15://mtc0
			outcode+="01000000100";
			outcode+=operand[0];
			outcode+=operand[1];
			outcode+="00000000";
			outcode+=operand[2];
			break;

		case 16://or
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100101";
			break;
	
		case 17://xor
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100110";
			break;

		case 18://nor
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100111";
			break;

		case 19://slt
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000101010";
			break;

		case 20://sltu
			outcode+="000000";
			outcode+=operand[1];
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+="00000100111";
			break;

		case 21://sll
			outcode="00000000000";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			outcode+="000000";
			break;

		case 22://srl
			outcode="00000000000";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			outcode+="000010";
			break;

		case 23://sra
			outcode="00000000000";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			outcode+="000011";
			break;

		case 24://sllv
			outcode="000000";
			outcode+=operand[2];
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+="00000000100";
			break;

		case 25://srlv
			outcode="000000";
			outcode+=operand[2];
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+="00000000110";
			break;

		case 26://srav
			outcode="000000";
			outcode+=operand[2];
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+="00000000111";
			break;

		case 27://jr
			outcode+="000000";
			outcode+=operand[0];
			outcode+="000000000000000001000";
			break;

		case 28://jalr
			outcode+="000000";
			outcode+=operand[1];
			outcode+="00000";
			outcode+=operand[0];
			outcode+="00000001001";
			break;

		case 29://break
			break;

		case 30://syscall
			break;

		case 31://eret
			outcode="00000010000000000000000000011000";
			break;

		case 32://addi
			outcode+="001000";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 33://addiu
			outcode+="001001";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 34://andi
			outcode+="001100";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 35://ori
			outcode+="001101";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 36://xori
			outcode+="001110";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 37://lui
			outcode+="00111100000";
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 38://lb
			outcode+="100000";
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 39://lbu
			outcode+="100100";
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 40://lh
			outcode+="100001";
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 41://lhu
			outcode+="100101";
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 42://sb
			outcode+="101000";
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 43://sh
			outcode+="101001";
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 44://lw
			outcode+="100011";
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 45://sw
			outcode+="101011";
			outcode+=operand[2];
			outcode+=operand[0];
			outcode+=operand[1];
			break;

		case 46://beq
			outcode+="000100";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 47://bne
			outcode+="000101";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 48://bgez
			outcode+="000001";
			outcode+=operand[0];
			outcode+="00001";
			outcode+=operand[1];
			break;

		case 49://bgtz
			outcode+="000111";
			outcode+=operand[0];
			outcode+="00000";
			outcode+=operand[1];
			break;

		case 50://blez
			outcode+="000110";
			outcode+=operand[0];
			outcode+="00000";
			outcode+=operand[1];
			break;

		case 51://bltz
			outcode+="000111";
			outcode+=operand[0];
			outcode+="00000";
			outcode+=operand[1];
			break;

		case 52://bgezal
			outcode+="000001";
			outcode+=operand[0];
			outcode+="10001";
			outcode+=operand[1];
			break;

		case 53://bltzal
			outcode+="000001";
			outcode+=operand[0];
			outcode+="10000";
			outcode+=operand[1];
			break;

		case 54://slti
			outcode+="001010";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 55://sltiu
			outcode+="001011";
			outcode+=operand[1];
			outcode+=operand[0];
			outcode+=operand[2];
			break;

		case 56://j
			outcode+="000010";
			outcode+=operand[0];
			break;

		case 57://jal
			outcode+="000011";
			outcode+=operand[0];
			break;

		case 59:
			cout<<"����������"<<total_line<<"��!������в��ܳ��ֱ�������!"<<endl;
			//prog<<"����������"<<total_line<<"��!������в��ܳ��ֱ�������!"<<endl;
			exit(1);


		case 60://��ų�ͳһ�����nop
			outcode="00000000000000000000000000000000";
			break;

		case 61:
			outcode="00000000000000000000000000000000";
			break;

		default:
			break;

	}

	outcode+=",";
	prog<<outcode<<endl;
	line_prog++;
	cout<<outcode<<endl;

	}
}



int ident(string str)//1-57Ϊָ�58Ϊ�ؼ��֣�59Ϊ������60Ϊ���,61Ϊ������
{
	if(str=="add" || str=="ADD")
	{

		return 1;
	}

	if(str=="addu" || str=="ADDU")
	{
		return 2;
	}

	if(str=="sub" || str=="SUB")
	{
		return 3;
	}

	if(str=="subu" || str=="SUBU")
	{
		return 4;
	}

	if(str=="and" || str=="AND")
	{
		return 5;
	}

	if(str=="mult" || str=="MULT")
	{
		return 6;
	}

	if(str=="multu" || str=="MULTU")
	{
		return 7;
	}

	if(str=="div" || str=="DIV")
	{
		return 8;
	}

	if(str=="divu" || str=="DIVU")
	{
		return 9;
	}

	if(str=="mfhi" || str=="MFHI")
	{
		return 10;
	}

	if(str=="mflo" || str=="MFLO")
	{
		return 11;
	}

	if(str=="mthi" || str=="MTHI")
	{
		return 12;
	}

	if(str=="mtlo" ||str=="MTLO")
	{
		return 13;
	}

	if(str=="mfc0" || str=="MFC0")
	{
		return 14;
	}

	if(str=="mtc0" || str=="MTC0")
	{
		return 15;
	}

	if(str=="or" || str=="OR")
	{
		return 16;
	}

	if(str=="xor" || str=="XOR")
	{
		return 17;
	}

	if(str=="nor" || str=="NOR")
	{
		return 18;
	}

	if(str=="slt" || str=="SLT")
	{
		return 19;
	}

	if(str=="sltu" || str=="SLTU")
	{
		return 20;
	}

	if(str=="sll" || str=="SLL")
	{
		return 21;
	}

	if(str=="srl" || str=="SRL")
	{
		return 22;
	}

	if(str=="sra" || str=="SRA")
	{
		return 23;
	}

	if(str=="sllv" || str=="SLLV")
	{
		return 24;
	}

	if(str=="srlv" || str=="SRLV")
	{
		return 25;
	}

	if(str=="srav" || str=="SRAV")
	{
		return 26;
	}

	if(str=="jr" || str=="JR")
	{
		return 27;
	}

	if(str=="jalr" || str=="JALR")
	{
		return 28;
	}

	if(str=="break" || str=="BREAK")
	{
		return 29;
	}

	if(str=="syscall" || str=="SYSCALL")
	{
		return 30;
	}

	if(str=="eret" || str=="ERET")
	{
		return 31;
	}

	if(str=="addi" || str=="ADDI")
	{
		return 32;
	}

	if(str=="addiu" || str=="ADDIU")
	{
		return 33;
	}

	if(str=="andi" || str=="ANDI")
	{
		return 34;
	}

	if(str=="ori" || str=="ORI")
	{
		return 35;
	}

	if(str=="xori" || str=="XORI")
	{
		return 36;
	}

	if(str=="lui" || str=="LUI")
	{
		return 37;
	}

	if(str=="lb" || str=="LB")
	{
		return 38;
	}

	if(str=="lbu" || str=="LBU")
	{
		return 39;
	}

	if(str=="lh" || str=="LH")
	{
		return 40;
	}

	if(str=="lhu" || str=="LHU")
	{
		return 41;
	}

	if(str=="sb" || str=="SB")
	{
		return 42;
	}

	if(str=="sh" || str=="SH")
	{
		return 43;
	}

	if(str=="lw" || str=="LW")
	{
		return 44;
	}

	if(str=="sw" || str=="SW")
	{
		return 45;
	}

	if(str=="beq" || str=="BEQ")
	{
		return 46;
	}

	if(str=="bne" || str=="BNE")
	{
		return 47;
	}

	if(str=="bgez" || str=="BGEZ")
	{
		return 48;
	}

	if(str=="bgtz" || str=="BGTZ")
	{
		return 49;
	}

	if(str=="blez" || str=="BLEZ")
	{
		return 50;
	}

	if(str=="bltz" || str=="BLTZ")
	{
		return 51;
	}

	if(str=="bgezal" || str=="BGEZAL")
	{
		return 52;
	}

	if(str=="bltzal" || str=="BLTZAL")
	{
		return 53;
	}

	if(str=="slti" || str=="SLTI")
	{
		return 54;
	}

	if(str=="sltiu" || str=="SLTIU")
	{
		return 55;
	}

	if(str=="j" || str=="J")
	{
		return 56;
	}

	if(str=="jal" || str=="JAL")
	{
		return 57;
	}


	if(str.substr(0,5)==".code"||str.substr(0,5)==".CODE")//��.data��ͷ��˵���������ݶΣ���Ȼ������Խ�ָ���ĵ�ַ
	{
		reach_code=1;
		return 58;
	}
	
	if(str.substr(0,5)==".data"||str.substr(0,5)==".DATA")//��.code��ͷ��˵���������Σ���Ȼ������Խ�ָ���ĵ�ַ
	{
		reach_data=1;
		return 58;
	}
	if(str==".model"||str==".MODEL"||str==".stack"||str==".STACK")
	{
		return 58;
	}

	if(str=="END" || str=="end")
	{
		return 61;		//�����ǳ�����������õ�END
	}

	if(str==".space" || str==".align")
	{
		return 62;		//αָ��
	}

	if(str[0]<65||str[0]>122||(str[0]<=96&&str[0]>=91))
	{
		err="�������Ų����Ϲ淶��Ӧ������ĸΪ��ͷ��!";
		outputerror(err);
	}
	else
	{
		if(str[str.length()-1]==':')//���һ���ַ��ǣ���ʾ�Ǳ��
			return 60;		//���
		else
			return 59;		//����
	}
}


int ident_reg(string name)		//ʶ������Ϊname�ļĴ�����Ӧ�ļĴ������
{
	if(name == "0" || name == "zero")
	{
		return 0;
	}
	else if(name == "1" || name == "at")
	{
		return 1;
	}
	else if(name == "2" || name == "v0")
	{
		return 2;
	}
	else if(name == "3" || name == "v1")
	{
		return 3;
	}
	else if(name == "4" || name == "a0")
	{
		return 4;
	}
	else if(name == "5" || name == "a1")
	{
		return 5;
	}
	else if(name == "6" || name == "a2")
	{
		return 6;
	}
	else if(name == "7" || name == "a3")
	{
		return 7;
	}
	else if(name == "8" || name == "t0")
	{
		return 8;
	}
	else if(name == "9" || name == "t1")
	{
		return 9;
	}
	else if(name == "10" || name == "t2")
	{
		return 10;
	}
	else if(name == "11" || name == "t3")
	{
		return 11;
	}
	else if(name == "12" || name == "t4")
	{
		return 12;
	}
	else if(name == "13" || name == "t5")
	{
		return 13;
	}
	else if(name == "14" || name == "t6")
	{
		return 14;
	}
	else if(name == "15" || name == "t7")
	{
		return 15;
	}
	else if(name == "16" || name == "s0")
	{
		return 16;
	}
	else if(name == "17" || name == "s1")
	{
		return 17;
	}
	else if(name == "18" || name == "s2")
	{
		return 18;
	}
	else if(name == "19" || name == "s3")
	{
		return 19;
	}
	else if(name == "20" || name == "s4")
	{
		return 20;
	}
	else if(name == "21" || name == "s5")
	{
		return 21;
	}
	else if(name == "22" || name == "s6")
	{
		return 22;
	}
	else if(name == "23" || name == "s7")
	{
		return 23;
	}
	else if(name == "24" || name == "t8")
	{
		return 24;
	}
	else if(name == "25" || name == "t9")
	{
		return 25;
	}
	else if(name == "26" || name == "k0")
	{
		return 26;
	}
	else if(name == "27" || name == "k1")
	{
		return 27;
	}
	else if(name == "28" || name == "gp")
	{
		return 28;
	}
	else if(name == "29" || name == "sp")
	{
		return 29;
	}
	else if(name == "30" || name == "s8" || name=="fp")
	{
		return 30;
	}
	else if(name == "31" || name == "ra")
	{
		return 31;
	}
	else //δʶ��ļĴ���������
	{
		return -1;
	}

}


void get_p_data(string base,int radix,parameter * p)	//��ȡparameter��p_data���ԣ�����baseΪ�ַ�����radixΪ����
	//��radix���Ƶ��ַ���base����Ӧ�Ķ�����ֵ�ŵ�p��
{
		string temp ;
		if(p->p_type==1)
		{
			if(base == "?")				//ռλ��
			{
				temp = "00000000";
			}
			else
			{
				const char *ix=base.c_str();/*һ��radix�����ַ���*/
				long i8=strtol(ix,NULL,radix);/*��ȡ10���Ƶ�ֵ*/
				bitset<8> bb(i8);
				temp=bb.to_string();
			}
		}

		if(p->p_type==2)
		{
			if(base == "?")				//ռλ��
			{
				temp = "0000000000000000";
			}
			else
			{
				const char *ix=base.c_str();/*һ��radix�����ַ���*/
				long i16=strtol(ix,NULL,radix);/*��ȡ10���Ƶ�ֵ*/
				bitset<16> bb(i16);
				temp=bb.to_string();
			}
		}

		if(p->p_type==3)
		{
			if(base == "?")				//ռλ��
			{
				temp = "00000000000000000000000000000000";
			}
			else
			{
				const char *ix=base.c_str();/*һ��radix�����ַ���*/
				long i32=strtol(ix,NULL,radix);/*��ȡ10���Ƶ�ֵ*/
				bitset<32> bb(i32);
				temp=bb.to_string();
			}
		}

		//temp += p->p_data;				//���³��ֵ����ݷŵ�p->data��ǰ��
		//p->p_data = temp;
		p->p_data.push_back(temp);

}

void store_dataseg()
{
	//struct data_info				//�洢���ݶ���Ϣ
	//{
	//	string info;				//��¼���ݶ�ÿ�е���Ϣ
	//	int line;					//��¼�����ڴ����еľ��Ե�ַ���ڴ������붨λ
	//};


	/*vector<parameter>				//para_info�д���������ݵ���Ϣ
	struct parameter
	{
		int line;					//�����������ݶε��к�
		int address;				//���ݵ�ַ,ָ����������Ե�ַ
		string name;				//��������
		string p_data;				//�����ƣ���Ȼ������16λ����8λ��ֵ��
		int p_type;					//��������
	};
	*/


	itofdatainfo = dataseg.begin();
	//for(;itofdatainfo!=dataseg.end();itofdatainfo++)
	//{
	//	cout<<itofdatainfo->info<<"           "<<itofdatainfo->line<<endl;
	//}
	parameter temp_p;					//���ڴ洢һ����������Ϣ
	int offset;							//��¼���������ݶ��е�ƫ����

	for(itofdatainfo = dataseg.begin();itofdatainfo!=dataseg.end();itofdatainfo++)
	{
		total_line = itofdatainfo->line;//��λ���þ仰�ڴ����еľ����������Ա��������

		vector<string> split_str;		//���ڴ�Ű��տո�ָ���str
		string tempword;
		stringstream ss(itofdatainfo->info);
		while(ss>>tempword)
			split_str.push_back(tempword);
		ss.clear();
		it=split_str.begin();

		int idofstr=ident(*it);			//���ڼ�¼str�ı��

		if(idofstr==58)		//.DATAָ��
		{
			++it;
			if(it!=split_str.end())//.DATA���滹��ָ�������ݶο�ʼ�ĵ�ַ,��.data 0x10000200
			{
				int strlen = (*it).length();
				string a = (*it).substr(2,strlen-2);//��ȡ0x�����16�����ַ���
				if(((*it)[0]=='0')&&((*it)[1]=='x'))//ָ�������ݶεĻ�ַΪ16���Ʊ�ʾ��
				{
					const char *i=a.c_str();/*һ��16�����ַ���*/
					baseaddress=strtol(i,NULL,16);/*��ȡ10���Ƶ�ֵ*/
					cout<<"��ַΪ��"<<baseaddress<<endl;
				}
				else//ָ�������ݶεĻ�ַΪ10���Ʊ�ʾ��
				{
					baseaddress=atoi(a.c_str());
				}

			}
			else//.data����û������
			{
				baseaddress = 0;
			}
			offset = 0;								//��Ϊ�ֿ�ʼ��һ���µ����ݶΣ�����ƫ����������0
		}


		else//��.DATA���������ݶ������.spcae;.align���ݲ�֧��һ���������ж�����������
		{
			
			if(*it==".space")		//.space n�ճ�n���ֽڵĿռ�
			{
				++it;
				int space = atoi((*it).c_str());
				string temp;

				if(space == 1)
				{
					bitset<8> bit(0);
					temp=bit.to_string();
				}
				else if(space == 2)
				{
					bitset<16> bit(0);
					temp=bit.to_string();
				}
				else if(space == 3)
				{
					bitset<24> bit(0);
					temp=bit.to_string();
				}

				//temp += temp_p.p_data;				//���³��ֵ����ݷŵ�p->data��ǰ��
				//temp_p.p_data = temp;
				temp_p.p_data.push_back(temp);

				offset += space;	//����ƫ����
			}

			else if(*it=="align")	//.align n ����һ�������������2n�ֽڶ��롣�˴�n�������1
			{

			}


			///*���������������һ���������洢������������*/
			//else if(*it==".word")
			//{

			//}
			//
			//else if(*it==".half")
			//{

			//}
			//
			//else if(*it==".byte")
			//{

			//}

			else//����ֻ���Ǳ�������
			{
				if(((*it)[0]>='a'&&(*it)[0]<='z')||((*it)[0]>='A'&&(*it)[0]<='Z'))//ȷʵ�����±�����
				{
					
					if(temp_p.p_type==1||temp_p.p_type==2||temp_p.p_type==3)
						para_info.push_back(temp_p);

					//memset(&temp_p, 0, sizeof(parameter));//����һ���µı������������ԭ��������Ϣ
					//temp_p.p_data.clear();

					//���
					(temp_p.p_data).swap(vector<string>());

					temp_p.base = baseaddress;	//�����������ݶεĶλ�ַ
					temp_p.offset = offset;		//�����ڸ����ݶε���Ե�ַ
					//temp_p.line=d_line;		//�����������ݶε�����к�
					it=split_str.begin();
					temp_p.name=*it;


					it++;					//��λ����������
					if(*it==".byte")
						temp_p.p_type=1;
					else if(*it==".half")
						temp_p.p_type=2;
					else if(*it==".word")
						temp_p.p_type=3;
					//else if(*it==".ascii")
					//	temp_p.p_type=4;
					//else if(*it==".asciiz")
					//	temp_p.p_type=5;
					else
					{
						err="���ݶ����ʽ��������ֻ��Ϊ.byte��.half��.word�е�һ��!";
						outputerror(err);
					}

					it++;					//��λ������ֵ
					string check = *it;		

					//������ֵ�п��ܳ��ֵĶ����û��ɿո񣬱������ֿ�
					for(int i=0;i < check.length();i++)
					{
						if(check[i]==',')
						{
							check[i]=' ';
							//����һ��������ζ�Ŷ�һ�����ݣ���ôƫ����Ҫ����
							if(temp_p.p_type==1)//byte�͵����ݣ�һ������ռ1���ֽ�
								offset +=1;
							else if(temp_p.p_type==2)//half�͵����ݣ�һ������ռ2���ֽ�
								offset +=2;
							else//word�͵����ݣ�һ������ռ4���ֽ�
								offset +=4;
						}
					}
					//�����һ������
					if(temp_p.p_type==1)//byte�͵����ݣ�һ������ռ1���ֽ�
						offset +=1;
					else if(temp_p.p_type==2)//half�͵����ݣ�һ������ռ2���ֽ�
						offset +=2;
					else//word�͵����ݣ�һ������ռ4���ֽ�
						offset +=4;


					vector<string> split_data;		//���ڴ�Ű��տո�ָ�������
					string tempword;
					stringstream ss(check);
					while(ss>>tempword)
						split_data.push_back(tempword);
					ss.clear();
					it=split_data.begin();

					for(;it!=split_data.end();it++)
					{
						int radix =10;				//Ĭ�ϸ�������ʮ���Ƶ�
						string base = *it;
						if((*it).substr(0,2)=="0x")
						{
							radix = 16;				//0x��ͷΪʮ������
							base = base.substr(2,base.length()-2);
						}
						if((*it)[(*it).length()-1]=='h'||(*it)[(*it).length()-1]=='H')
						{
							radix = 16;				//b��β�Ƕ�����
							base = base.substr(0,base.length()-1);
						}
						if((*it)[(*it).length()-1]=='o'||(*it)[(*it).length()-1]=='O')
						{
							radix = 8;				//b��β�Ƕ�����
							base = base.substr(0,base.length()-1);
						}
						if((*it)[(*it).length()-1]=='b'||(*it)[(*it).length()-1]=='B')
						{
							radix = 2;				//b��β�Ƕ�����
							base = base.substr(0,base.length()-1);
						}
						get_p_data(base,radix,&temp_p);
					}
				}
				else//������������ĸ��ͷ
				{
						err="���ݶ����ʽ����������ֻ������ĸ��ͷ!";
						outputerror(err);
				}

			}//�Ƿ�Ϊ�������������

		}

	}//һ�����ݶδ������
	para_info.push_back(temp_p);//�������ݶ�������һ������������洢


}



void check_operand(vector<string> & operand,string check,int id)
{
	int numofoprd=0;				//��¼�������ĸ���
	int regnum=0;					//���ڼ�¼�Ĵ�����

	//�����Ϻ�Ĳ������еĶ��ţ������û��ɿո񣬱������ֿ�
	for(int i=0;i < check.length();i++)
	{
		if(check[i]==',')
		{
			check[i]=' ';
			numofoprd++;				//����һ��������ζ�Ŷ�һ��������
		}
	}
	numofoprd++;						//�����һ��������


	vector<string> split_oprd;			//���ڴ�Ű��տո�ָ��Ĳ�����
	string tempword;
	stringstream ss(check);
	while(ss>>tempword)
		split_oprd.push_back(tempword);
	ss.clear();
	it=split_oprd.begin();

	for(;it!=split_oprd.end();it++)
	{
		int pos_l=(*it).find('(',0);	//���Ҹò�������(��λ��
		int pos_r=(*it).find(')',0);	//���Ҹò�������)��λ��

		if(pos_l==string::npos && pos_r==string::npos)		//û�ҵ�'('Ҳû�ҵ�')'
		{
			no_bracket(*it,operand,id);	//����������ʱ�Ĵ�����
		}

		else if(pos_l!=string::npos && pos_r>pos_l)//�ҵ�����������
		{
			numofoprd++;				//�������Ѱַindex[base]��Ҫ���һ��������
			//int base;					//��ַ
			int index;					//��ַ
			if(pos_l==0)//���������û��ֵ
				index=0;
			else//�������������ֵ���������������߱�����
			{
				string temp=(*it).substr(0,pos_l);//ȡ��������֮ǰ�Ĳ���
				if(temp[0]>47 && temp[0]<58)//������
				{
					index=atoi(temp.c_str());
				}
				else//�����Ǳ�����
				{
					//vector<parameter> para_info;
					itofpara=para_info.begin();
					for(;itofpara!=para_info.end();itofpara++)
					{
						//cout<<"������"<<itofpara->name<<"��ֵΪ��"<<itofpara->p_data<<"��ַΪ��"<<itofpara->address<<endl;
						if(temp==(itofpara->name))
						{
							//const char *pp=(itofpara->p_data[0]).c_str();
							//index=strtol(pp,NULL,2);
							index = itofpara->base + itofpara->offset;
							break;
						}
					}
					if(itofpara==para_info.end())//���Ǳ���
					{
						err="��ʽ�����޷�ʶ��������ߵ�ƫ����,�Ȳ���������Ҳ���Ǳ�����";
						outputerror(err);
					}
				}
			}//���ˣ�index�����п�������������

			bitset<16> bb(index);
			operand.push_back(bb.to_string());
			//�������base�Ŀ������

			string l2r=(*it).substr(pos_l+1,pos_r-1-pos_l);//ȡ��������֮�󣬡�����֮ǰ�Ĳ���
			//base��ַֻ���ǼĴ��������ֻ������$��ͷ
			//�ò�������$��ͷ����ֻ����ж���Ĵ�����

			/*��������$��ͷ*/
			if(l2r[0]=='$')
			{
				string r=l2r.substr(1,l2r.length()-1);		//ȥ��$
				regnum=ident_reg(r.c_str());
				if(regnum>31||regnum<0)
				{
					err="�Ĵ�����Խ�磨�Ĵ����ŷ�Χ��0-31����";
					outputerror(err);
				}
				else//û��Խ�磬�򽫸üĴ�����¼����
				{
					bitset<5> bb(regnum);
					operand.push_back(bb.to_string());
				}
			}

			else
			{
				err="������ֻ�ܳ��ּĴ�����";
				outputerror(err);
			}

		}
		else
		{
			err="�������Ų�ƥ�䣡";
			outputerror(err);
		}

	}//һ��������������

}


void no_bracket(string name,vector<string> & operand,int id)	//����������Ŵ���ʽ

{
	int regnum;

	/*��������$��ͷ�����ǼĴ���*/
	if(name[0]=='$')
	{
		string r=name.substr(1,name.length()-1);		//ȥ��$
		regnum=ident_reg(r.c_str());
		if(regnum>31||regnum<0)
		{
			err="�Ĵ�����Խ�磨�Ĵ����ŷ�Χ��0-31����";
			outputerror(err);
		}
		else//û��Խ�磬�򽫸üĴ�����¼����
		{
			bitset<5> bb(regnum);
			operand.push_back(bb.to_string());
		}
	}


	//�ò����������ֿ�ͷ���������imm����shamt
	//���������֣�ֱ������32λ��˵
	else if(name[0]>47 && name[0]<58)//������
	{
		int j=atoi(name.c_str());

		//mfc0,mtc0ָ���е�sel�ֶ�ռ3λ
		if(id==14 || id==15)
		{
			bitset<3> bb(j);
			operand.push_back(bb.to_string());
		}

		//sll,srl,sra��3����λָ���е�����Ϊshamt��ռ5λ
		if(id == 21 || id == 22 || id == 23)
		{
			bitset<5> bb(j);
			operand.push_back(bb.to_string());
		}

		//����I��ָ���е����־�Ϊ16λ
		else if(id>=32 && id<=55)
		{
			bitset<16> bb(j);
			operand.push_back(bb.to_string());
		}

		//���������תָ��ж�ָ��ȵľ�Ϊ26λ
		else if(id == 56 || id == 57)
		{
			j/=4;
			bitset<26> bb(j);
			operand.push_back(bb.to_string());
		}
		else//break����syscall�Ĵ�����
		{

		}
	}

	else//�����Ǳ��������߱����
	{
		itofpara=para_info.begin();
		for(;itofpara!=para_info.end();itofpara++)
		{
			if(name==itofpara->name)//�ҵ��˱��������򷵻����Ӧ����ֵ��16λ��ʾ
			{
				string temp = itofpara->p_data[0];
				long num = strtol(temp.c_str(),NULL,2);

				if(id == 56 ||id == 57)//j����jalҪ����26λ��ַ
				{
					bitset<26> bb(num/4);
					temp = bb.to_string();
				}
				else
				{
					bitset<16> bb(num);
					temp = bb.to_string();
				}
				operand.push_back(temp);
				break;
			}
		}
		if(itofpara==para_info.end())//���Ǳ������������Ǳ��
		{
			if((itofmap=label.find(name))!=label.end())//�Ǳ��
			{
				if(id==56 || id==57)//j����jalҪ����26λ��ַ
				{
					bitset<26> bb((itofmap->second)/4);
					operand.push_back(bb.to_string());//ͳһ����26λ��ֵ
				}
				else if(id >=46 &&id <= 53)		//	beq��ָ���offsetҪ��������
				{
					int diff = (itofmap->second - p_line*4)/4;
					bitset<16> bb(diff);
					operand.push_back(bb.to_string());
				}
				else//�����õ���ת�ĵط�ֻҪ����16λ��ַ
				{
					bitset<16> bb(itofmap->second);
					operand.push_back(bb.to_string());//ͳһ����16λ��ֵ
				}
			}
			else
			{
				err="�������ĸ�ʽ�����⣡";
				outputerror(err);
			}
		}
	}
}


string to32(string & str)		//��str�Ƿ�����չ��32λ
{
	string z;
	if(str.length()==0)
		z="00000000000000000000000000000000";
	else if(str.length()==8)
	{
		z="000000000000000000000000";
		z+=str;
	}
	else if(str.length()==16)
	{
		z="0000000000000000";
		z+=str;
	}
	else if(str.length()==24)
	{
		z="00000000";
		z+=str;
	}
	else
	{
		z="";
		z+=str;
	}
	str=z;
	return str;
}



void write_data()
{
		//struct data_info				//�洢���ݶ���Ϣ
	//{
	//	string info;				//��¼���ݶ�ÿ�е���Ϣ
	//	int line;					//��¼�����ڴ����еľ��Ե�ַ���ڴ������붨λ
	//};


	/*vector<parameter>				//para_info�д���������ݵ���Ϣ
	struct parameter
	{
		int line;					//�����������ݶε��к�
		int address;				//���ݵ�ַ,ָ����������Ե�ַ
		string name;				//��������
		vector<string> p_data;		//�洢�����Ķ�������ʽ�����ñ����洢�˶������ʱ��һ�μӽ�ȥ
		int p_type;					//��������
	};

	vector<data_info> dataseg;			//�����ݶε�ÿһ�м�¼�����Ա��������
	vector<parameter> para_info;		//������б�������Ϣ
	*/

	cout<<"��ʼд��RAM..."<<endl;

	ram<<"memory_initialization_radix = 2;"<<endl;
	ram<<"memory_initialization_vector ="<<endl;
	ram0<<"memory_initialization_radix = 2;"<<endl;
	ram0<<"memory_initialization_vector ="<<endl;
	ram1<<"memory_initialization_radix = 2;"<<endl;
	ram1<<"memory_initialization_vector ="<<endl;
	ram2<<"memory_initialization_radix = 2;"<<endl;
	ram2<<"memory_initialization_vector ="<<endl;
	ram3<<"memory_initialization_radix = 2;"<<endl;
	ram3<<"memory_initialization_vector ="<<endl;

	int line_ram = 0;				//��¼ram������
	int line_ram0 = 0;
	int line_ram1 = 0;
	int line_ram2 = 0;
	int line_ram3 = 0;

	int mem = 0;					//��ROM��4RAM�����ڼ�¼��ǰд��ڼ���RAM
	//int row = 0;					//���ݱ����Ļ�ַ��offset�ɵ�������RAM�е��к�
	int column = 0;					//���ݱ����Ļ�ַ��offset�ɵ�������RAM�е��кţ������Ұ�һ�У�32λ������8λһ�зֳ�4��
	string line = "";				//��RAMģʽ�У�ÿ����4���ֽڵ����ݣ�����浽line�У�����32λ����д��һ��
	int size = 0;					//һ��������ռ���ֽ���
	for(itofpara = para_info.begin();itofpara != para_info.end();itofpara++)
	{//����ÿһ����������������洢
		if(itofpara->p_type == 1)
		{
			size = 1;
		}
		if(itofpara->p_type == 2)
		{
			size = 2;
		}
		if(itofpara->p_type == 3)
		{
			size = 4;
		}
		for(it = itofpara->p_data.begin();it != itofpara->p_data.end();it++)
		{//�����������Ͷ����ݽ��зֽ⣬һ���ֽ�һ���ֽڵ�ȡ�������洢��coe�У�ע������ֽ���itofpara->p_data�����Ҳ�
			for(int i = 0; i < size; i++)
			{
				int start_pos = (*it).length()-8*(i+1);//��i���ֽڿ�ʼ��λ��
				string temp = (*it).substr(start_pos,8);//ȡ����i���ֽ�


				/*��д��4RAM��*/
				if(mem == 0)
				{
					ram0<<temp<<","<<endl;					//д���0��RAM
					line_ram0++;
				}
				if(mem == 1)
				{
					ram1<<temp<<","<<endl;					//д���1��RAM
					line_ram1++;
				}
				if(mem == 2)
				{
					ram2<<temp<<","<<endl;					//д���2��RAM
					line_ram2++;
				}
				if(mem == 3)
				{
					ram3<<temp<<","<<endl;					//д���3��RAM
					line_ram3++;
				}

				mem++;									//д��һ��RAM����memָ����һ��
				if(mem == 4)
					mem = 0;

				/*����д�뵥RAM��*/
				if(column < 3)							//�������ݼ���tempҲ����32λ
				{
					temp = temp+line;					//�³��ֵ��ֽ�Ҫ���ڸ�λ
					line = temp;
					column ++;
				}
				else//column = 3 ��ζ�Ÿ������ݼ���temp�ﵽ32λ����˿������һ��
				{
					temp = temp+line;						//�³��ֵ��ֽ�Ҫ���ڸ�λ
					line = temp;
					line = line+",";
					ram<<line<<endl;
					line_ram++;

					line = "";								//�����line����Դ���һ������д��
					column = 0;
				}


			}//�����ڵ�һ���ֽڴ������

		}//�����ڵ�һ�����ݴ������

	}//һ�������������

	int ramline = pow(2,15);


	for(int i = line_ram0;i<ramline-1;i++)
		ram0<<"00000000,"<<endl;
	ram0 << "00000000;";

	for(int i = line_ram1;i<ramline-1;i++)
		ram1<<"00000000,"<<endl;
	ram1 << "00000000;";

	for(int i = line_ram2;i<ramline-1;i++)
		ram2<<"00000000,"<<endl;
	ram2 << "00000000;";

	for(int i = line_ram3;i<ramline-1;i++)
		ram3<<"00000000,"<<endl;
	ram3 << "00000000;";

	//���һ�����ݴ�����󣬵�RAM���һ�п�����Ϊ����32λ��δ���������
	if(line.length()==0)					//��һ����������꣬û�в���������Ҫ���
	{
	}
	else//���в�����һ������Ҫ���
	{
		line = to32(line);
		ram<<line<<","<<endl;
		line_ram++;
	}
	
	for(int i = line_ram;i<ramline-1;i++)
		ram<<"00000000000000000000000000000000,"<<endl;
	ram << "00000000000000000000000000000000;";

	cout<<"ŶҲ��RAMд�����!"<<endl<<endl;

}



void outputerror(string err)
{
	cout<<"����������"<<total_line<<"��!"<<err<<endl;
	//prog<<"����������"<<total_line<<"��!"<<err<<endl;
	exit(1);
}