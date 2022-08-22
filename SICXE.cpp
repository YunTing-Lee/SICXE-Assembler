#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stack>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <iomanip>

using namespace std;

string hexLOCCTR ; // 程式位置計數器
string PC ; // 下一道指令的位置
int gColumnWalk ;
string startAddr = "0000" ;

struct TokenInfo{
    string name ;
    string type ;
    int entry ;   // 是table裡的第幾個token
};


struct Information{

    string insLocation ;
    string insObjcode ;
    string errorMessage ;
    string source ;
    int length ;
    int lineTokenNum ;

};

char gNextChar = ' ';  // 儲存檔案中的還沒存的下一個字元
string gToken = "" ;  // 儲存目前讀到的這個token
string gPreToken = "";  // 前一個token
string gPrePreToken = "" ;  // 前前token，用來判斷C'EOF'
bool hasLeftQuote = false ;
bool gEnd = false ;
bool gIsFirstLine = false ;

void readUntilEnter();
bool isDigit( string str ) ;
string decimalToHex( int decimal) ;
void calculatePC( int length) ;
int hexToDecimal( string hex ) ;
int hexToInt(char a) ;
string intTohex( int a ) ;
string hexToBinary( string hex ) ;
string decimalToBin( int decimal ) ;
string binToHex( string bin ) ;
string hexCal( string a, string b, string calType ) ;
string twoComplement( string bin) ;
int calDisp( string a, string b) ;
string calOpcodeni( string opcode, string n, string i) ;
bool isHexInt( char ch ) ;

struct Optable{
  string name ;
  string operandNum ;
  string opcode ;
  string format ;
};

struct PseudoTable{
    string name ;
};


class RegTable{
public:
    string name ;
    string num ;

    string getRegnum(string reg){
		if(strcasecmp(reg.c_str(), "A") == 0 )  return "0";
		else if(strcasecmp(reg.c_str(), "B") == 0)  return "3";
		else if(strcasecmp(reg.c_str(), "F") == 0)  return "6";
		else if(strcasecmp(reg.c_str(), "L") == 0)  return "2";
		else if(strcasecmp(reg.c_str(), "S") == 0)  return "4";
		else if(strcasecmp(reg.c_str(), "T") == 0)  return "5";
		else if(strcasecmp(reg.c_str(), "X") == 0)  return "1";
		else if(strcasecmp(reg.c_str(), "PC") == 0) return "8";
		else if(strcasecmp(reg.c_str(), "SW") == 0) return "9";
		return NULL;
	}
} ; // class RegTable


struct DelimeterTable{
    string name ;
};

struct VarTable{
    string name ;
    string address ;
    string value ;
};

struct LiteralTable{
    string name ;
    string address ;
    string value ;
    string type ;
};


class SICXEAssembler{

public :
    // 指令   後面個數   OP碼     格式
    vector<Optable> optab = {{"ADD", "1","18","3/4"}, {"ADDF", "1","58","3/4"}, {"ADDR", "2","90","2"  }, {"AND", "1","40","3/4"},
    {"CLEAR","1","B4","2"  }, {"COMP","1","28","3/4"}, {"COMPF","1","88","3/4"}, {"COMPR", "2","A0","2"  }, {"DIV", "1","24","3/4"},
    {"DIVF", "1","64","3/4"}, {"DIVR","2","9C","2"  }, {"FIX",   "0","C4","1"  }, {"FLOAT", "0","C0","1"  }, {"HIO", "0","F4","1"  },
    {"J",    "1","3C","3/4"}, {"JEQ", "1","30","3/4"}, {"JGT",   "1","34","3/4"}, {"JLT",   "1","38","3/4"}, {"JSUB","1","48","3/4"},
    {"LDA",  "1","00","3/4"}, {"LDB", "1","68","3/4"}, {"LDCH",  "1","50","3/4"}, {"LDF",   "1","70","3/4"}, {"LDL", "1","08","3/4"},
    {"LDS",  "1","6C","3/4"}, {"LDT", "1","74","3/4"}, {"LDX",   "1","04","3/4"}, {"LPS",   "1","D0","3/4"}, {"MUL", "1","20","3/4"},
    {"MULF", "1","60","3/4"}, {"MULR","2","98","2"  }, {"NORM",  "0","C8","1"  }, {"OR",    "1","44","3/4"}, {"RD",  "1","D8","3/4"},
    {"RMO",  "2","AC","2"  }, {"RSUB","0","4C","3/4"}, {"SHIFTL","2","A4","2"  }, {"SHIFTR","2","A8","2"  }, {"SIO", "0","F0","1"  },
    {"SSK",  "1","EC","3/4"}, {"STA", "1","0C","3/4"}, {"STB",   "1","78","3/4"}, {"STCH",  "1","54","3/4"}, {"STF", "1","80","3/4"},
    {"STI",  "1","D4","3/4"}, {"STL", "1","14","3/4"}, {"STS",   "1","7C","3/4"}, {"STSW",  "1","E8","3/4"}, {"STT", "1","84","3/4"},
    {"STX",  "1","10","3/4"}, {"SUB", "1","1C","3/4"}, {"SUBF",  "1","5C","3/4"}, {"SUBR",  "2","94","2"  }, {"SVC", "1","B0","2"  },
    {"TD",   "1","E0","3/4"}, {"TIO", "0","F8","1"  }, {"TIX", "1","2C","3/4"}, {"TIXR",  "1","B8","2"  }, {"WD",  "1","DC","3/4"} };

    vector<PseudoTable> pseudo ; // defalut
    vector<RegTable> regtab ; // defalut
    vector<DelimeterTable> delimtab ; // defalut
    vector<VarTable> symtab ;
    vector<VarTable> litNumTab ;
    vector<VarTable> litStrTab ;
    vector<LiteralTable> littab ;
    vector<Information> info ;
    vector<vector<TokenInfo>> tokens ;
    vector<TokenInfo> newLine ;
    vector<string> undefinedSymbol ;
    vector<string> duplicatedSymbol ;
    fstream sicFile ;
    string sicFileName ;
    void readSICFile() ;
    string BASE ;

    void readALineFromFile() ;
    void setTable() ;
    void readTable( string fileName, string tableType ) ;
    void readFileName() ;
    void openSicFile() ;
    bool isDelimeter( char aChar) ;
    void getUntilWhiteOrDel(string aLine, string & aToken) ;
    void getNotWhite( string aLine) ;
    void storeDelimeter( string aLine, TokenInfo & newToken ) ;
    void getUntilQuote( string aLine, string & tokenName ) ;
    void readFirstLine() ;



    bool isInstruc( TokenInfo & aTokenn) ;
    bool isPseudo( TokenInfo & aToken ) ;
    bool isRegister( TokenInfo & aToken) ;
    bool isDelimeter( TokenInfo & aToken ) ;
    bool isInTable( string name, vector<VarTable> & table ) ;
    void processPseudo( int pseudoIndex, bool & hasLocation ) ;
    string calEQUValue(vector<TokenInfo> aLine, string & errorSymbol) ;


    void findTokenType( TokenInfo & aToken , string preToken, string prePreToken, string prePrePreToken) ;
    void findTokenEntry( TokenInfo & aToken, vector<VarTable> & table ) ;
    int hashFunction( string tokenName ) ;
    void putInLittab(  TokenInfo & aToken, string preToken, string prePreToken)  ;
    string findSymbolAddr( TokenInfo & symbol ) ;
    void writeOutputFile() ;
    void defineLiteral() ;

    void processALine() ;
    void analysisInstruction( int insIndex ) ;
    void pass2() ;
    void calInsOpcode( int i, int j) ;
    void calPseudoOpcode( int i, int j ) ;
    string createFormat3Machine( int index, int insIndex, int operandIndex,  string opcode, string n, string i, string & x, string & b, string & p, string e) ;
    string combineOPnixbpe( string opcode, string n, string i, string x, string b, string p, string e) ;
    string calculateDisp(int index, int j, string & b, string & p) ;
    string getPC(int i) ;
    string findLiteralAddr() ;
    string findLiteralAddr( TokenInfo & literal) ;

    void findSymbolEntry( TokenInfo & aToken) ;
    bool isCorrectDelimeter( int i ) ;
    void processEQU() ;
    string processFormat4(int index, int j, string opcode);
    string processFormat3(int index, int j, string opcode);
    string choosePcOrBase( int index,string targetAddr, string & p, string & b) ;
    bool isInsCorrect( int insIndex ) ;

    void addError() ;
    bool isOperandCorrect( int i ) ;
};

int main(){

    SICXEAssembler sicxeAsm  ;
    sicxeAsm.setTable();
    sicxeAsm.openSicFile();
    sicxeAsm.readSICFile() ;
    sicxeAsm.pass2();
    sicxeAsm.writeOutputFile() ;
    return 0;
}

void SICXEAssembler :: setTable(){

    string fileName = "Table2.table" ;
    readTable(fileName, "pseudo") ;

    fileName = "Table3.table" ;
    readTable(fileName, "register") ;

    fileName = "Table4.table" ;
    readTable(fileName, "delimeter") ;

    symtab.resize(100) ;
    litNumTab.resize(100) ;
    litStrTab.resize(100) ;
    littab.resize(100) ;

} // setTable()


void SICXEAssembler :: readTable( string fileName, string tableType ){
    fstream table ;
    table.open( fileName.c_str(), ios::in ) ;
    if( ! table ) {
        cout << "### " << fileName << " does not exist ### " << endl ;
        exit(EXIT_SUCCESS);  // 有缺少table檔案則直接退出程式
    } // if
    else {
        while( ! table.eof() ){
            string temp ;
            table >> temp ;
            if( tableType == "pseudo"){
                PseudoTable t ;
                t.name = temp ;
                pseudo.push_back(t) ;
            } // else
            else if ( tableType == "register"){
                RegTable r ;
                r.name = temp ;
                r.num = r.getRegnum(r.name) ;
                regtab.push_back(r) ;
            } // else if
            else if ( tableType == "delimeter" ){
                DelimeterTable d ;
                d.name = temp ;
                delimtab.push_back(d) ;
            }
        } // while
        table.close();
    } // else
} // readTable

void SICXEAssembler :: openSicFile(){

    readFileName();  // 讀檔案名字
    sicFile.open( sicFileName.c_str(), ios::in ) ;
    while( ! sicFile ) {  // 若檔案不存在，則會叫使用者再次輸入檔案名
        cout << "### " << sicFileName << " does not exist ### " << endl ;
        readUntilEnter();
        readFileName();
        sicFile.open( sicFileName.c_str(), ios::in ) ;
    } // while
} // openSicFile

void SICXEAssembler ::readFileName(){
    cout << "Input a file name : " ;
    cin >> sicFileName ;
    sicFileName = sicFileName + ".txt" ;
} // readFileName()

void readUntilEnter(){
    char enter ;
    cin.get(enter) ;
    while( enter != '\n' )
        cin.get(enter) ;
} // readUntilEnter()

void SICXEAssembler :: readSICFile(){

    readFirstLine() ; // pass1讀取第一行
    gIsFirstLine = true ;
    hexLOCCTR = startAddr ;
    PC = hexLOCCTR ;
    tokens.push_back(newLine) ; // 新增一行
    processALine();
    vector<TokenInfo>().swap(newLine) ;

    while ( !sicFile.eof() ) {

        hexLOCCTR = PC ;
        readALineFromFile();
        gIsFirstLine = false ;
        tokens.push_back(newLine) ;

        if( newLine.size() == 0 || newLine[0].name == ".") {
            vector<TokenInfo>().swap(newLine) ;
            continue ;
        } // if
        else if ( gEnd ){
            info.back().errorMessage = "It's END error!!!" ;
            info.back().length = 0 ;
            info.back().insLocation = "" ;
            vector<TokenInfo>().swap(newLine) ;
            continue ;
        }

        processALine();
        vector<TokenInfo>().swap(newLine) ;

    } // while

    sicFile.close() ;

} //readSICFile()

void SICXEAssembler :: readFirstLine(){
    // 讀第一行不是空白且註解的

    readALineFromFile();
    while(  newLine.size() == 0 || newLine[0].name == "."  ){ // 若是空白或是整行為註解
        tokens.push_back(newLine) ;
        vector<TokenInfo>().swap(newLine) ; // 清空
        readALineFromFile(); // 在繼續讀下一行
    } // while

    int numOfTokens = newLine.size() ;
    int tokenIndex = 0 ;

    for( tokenIndex ; tokenIndex < info.back().lineTokenNum ; tokenIndex++ ){
        if( strcasecmp( newLine[tokenIndex].name.c_str(), "START") == 0 ){  // {label} START hex_num

            if( info.back().lineTokenNum != 3 && info.back().lineTokenNum != 2 ){
                info.back().errorMessage = "syntax error!!!" ;
                return ;
            }
            else if ( tokenIndex != info.back().lineTokenNum-2 ){
                info.back().errorMessage = "syntax error!!!" ;
                return ;
            }
            else{
                for( int j = 0 ; j < newLine[tokenIndex+1].name.size() ; j++ ){
                    char ch = newLine[tokenIndex+1].name[j] ;
                    if( ! isHexInt(ch) ){
                        info.back().errorMessage = "syntax error!!!" ;
                        return ;
                    } // if
                } // for


                if( tokenIndex == 1 && newLine[0].type != "symbol" ){
                    info.back().errorMessage = "syntax error!!!" ;
                    return ;
                }
                else{
                    startAddr = newLine[tokenIndex+1].name ;
                    while( startAddr.size() < 4 )  // 補足4位元
                        startAddr = "0" + startAddr ;
                }

            } // else
        } // if
    } // for
} // readFirstLine()

void SICXEAssembler :: readALineFromFile(){

    string preToken ;  // 前一個token
    string prePreToken ;  // 前前token，用來判斷C'EOF'
    string prePrePreToken ; // 判斷=3277、=C'EOF'

    if ( !sicFile.eof() ) {
        string aLine ;
        getline(sicFile , aLine) ; // 讀檔案裡的一行

        Information newInfo ;
        newInfo.source = aLine ;
        info.push_back(newInfo) ;

        for( gColumnWalk = 0 ; gColumnWalk < aLine.size() ; gColumnWalk++ ){
            if( aLine[gColumnWalk] != ' ' && aLine[gColumnWalk] != '\t' )
                break ;
        } // for

        if ( gColumnWalk == aLine.size() || aLine[gColumnWalk]=='.') {  // 是空白，則繼續讀下一行
            info.back().errorMessage = "" ;
            info.back().insLocation = "" ;
            info.back().length = 0 ;
            info.back().lineTokenNum = 0 ;
            return ;
        }// if

        for( gColumnWalk ; gColumnWalk < aLine.size() ; ){  // 切一行

            TokenInfo newToken ;
            if ( isDelimeter( aLine[gColumnWalk]) ) // 若是分隔符
                storeDelimeter( aLine, newToken ); // 將newToken設為此分隔符，並將gColumnWalk+1
            else if( newLine.size() >= 1 && preToken == "'" && hasLeftQuote ){
                getUntilQuote(aLine, newToken.name) ; // 是字串或數字，讀到單引號為止
            }
            else{
                getNotWhite(aLine) ;  // 將gColumnWalk走到不是空白為止
                if( gColumnWalk == aLine.size() ) // 這一行已經沒有token了
                    break ;
                getUntilWhiteOrDel( aLine, newToken.name ) ;
            } //  else

            if( info.back().errorMessage != "" )
                break ;

            if( ( newToken.name == "x" || newToken.name == "X" || newToken.name == "c" || newToken.name == "C") && aLine[gColumnWalk] == '\'' )
                ; //不用翻譯
            else{
                findTokenType( newToken, preToken, prePreToken, prePrePreToken ) ; // 將token的type及entry設定好
                newLine.push_back(newToken) ;
            } // else

            if( newToken.name == "." || info.back().errorMessage != "")  // 後面都是註解
                break ;

            prePrePreToken = prePreToken ;
            prePreToken = preToken ;
            preToken = newToken.name ;

        } // for

        gColumnWalk = 0 ;
        if( newLine[newLine.size()-1].name == "." )
            info.back().lineTokenNum = newLine.size()-1 ;
        else
            info.back().lineTokenNum = newLine.size() ;
    } // if
} // readALineFromFile()


void SICXEAssembler :: processALine(){
    bool hasLocation = true ;

    if( info.back().errorMessage != "" ){
        info.back().insLocation = "" ;
        info.back().length = 0 ;
        return ;
    }
/*
    for( int i = 0 ; i < newLine.size() ; i++ )
        cout << newLine[i].name << "(" << newLine[i].type << ")" << "\t" ;
    cout << endl ;
*/
    for( int i = 0 ; i < newLine.size() ; i++ ){

        if( newLine[i].name == "." ){
            if( i == 0 ){ // 整行都為註解
                info.back().errorMessage = "" ;
                info.back().insLocation = "" ;
                info.back().length = 0 ;
                return ;
            }
        }
        else if( newLine[i].type == "instruction" ){  // 要檢查後面接的operand對不對
            analysisInstruction( i );
            break ;
        } // else if
        else if( newLine[i].type == "pseudo" ){
            processPseudo( i , hasLocation) ;
            if(  strcasecmp( newLine[i].name.c_str(),"EQU") == 0 || strcasecmp( newLine[i].name.c_str(),"LTORG") == 0
                || strcasecmp( newLine[i].name.c_str(),"END") == 0 ) {
                return ;
            } // if
            break ;
        }
        else if( newLine[i].type == "symbol" ){
            if( i == 0 && info.back().lineTokenNum  == 1 ){
                info.back().errorMessage = "syntax error!!!" ;
                info.back().insLocation ="" ;
                info.back().length = 0  ;
                return ;
            }
            else if( i == 0 && info.back().lineTokenNum  >= 2 && newLine[i+1].type != "instruction"
                    &&  newLine[i+1].type != "pseudo" && newLine[i+1].name != "+" ){
                info.back().errorMessage = "syntax error!!!" ;
                info.back().insLocation ="" ;
                info.back().length = 0  ;
                return ;
            }
            else if( ( i == 0 && info.back().lineTokenNum  >= 2 && strcasecmp( newLine[1].name.c_str(),"EQU") != 0 ) ) {
                symtab[newLine[i].entry].address = hexLOCCTR ;
            } // if
        }
        else if( newLine[i].type == "delimeter" && newLine[i].name != "." && newLine[i].name != "+" ){
            if( i == 0 ){
                info.back().errorMessage = "syntax error!!!" ;
                info.back().insLocation ="" ;
                info.back().length = 0  ;
                return ;
            }
        } // else if
        else if( newLine[i].name == "+"){
            if( i < newLine.size()-1 ){
                if( newLine[i+1].type == "instruction" && optab[newLine[i+1].entry-1].format == "3/4")
                    ;
                else{
                    info.back().errorMessage = "syntax error!!!" ;
                    info.back().insLocation ="" ;
                    info.back().length = 0  ;
                    return ;
                }
            }
        }

    } // for

    if( info.back().errorMessage != "" ){
        info.back().insLocation ="" ;
        info.back().length = 0  ;
    }
    else if( newLine[0].name != "." && hasLocation )
        info.back().insLocation = hexLOCCTR ;
    else
        info.back().insLocation = "" ;

    calculatePC( info.back().length );


//    for( int i = 0 ; i < info.size() ; i++ )
//        cout << "[" << i << "] " << info[i].insLocation << "\t" << info[i].length << "\t" << info[i].lineTokenNum << "\t" << info[i].errorMessage << endl ;
} // processALine()

bool SICXEAssembler :: isInsCorrect( int insIndex ){

    if( optab[ newLine[insIndex].entry- 1 ].format == "1" ){
        if( ( insIndex == 1 && newLine[0].type != "symbol") || insIndex > 1 || info.back().lineTokenNum > insIndex+1 ){
            info.back().errorMessage = "syntax error!!!" ;
            return false;
        }
    }
    else if ( optab[ newLine[insIndex].entry- 1 ].format == "2" ){
        if( ( insIndex == 1 && newLine[0].type != "symbol") || insIndex > 1 ){
            info.back().errorMessage = "syntax error!!!" ;
            return false;
        } // if
    } // else if
    else{ // format3/4
        if( insIndex > 2 ){
            info.back().errorMessage = "syntax error!!!" ;
            return false;
        } // if
        else if( insIndex == 2 ){ // format 4
            if( newLine[1].name != "+" || newLine[0].type != "symbol" ){
                info.back().errorMessage = "syntax error!!!" ;
                return false;
            }
        } // else if
        else if( insIndex == 1 ){
            if( newLine[0].name != "+" && newLine[0].type != "symbol" ){
                info.back().errorMessage = "syntax error!!!" ;
                return false;
            } // if
        } // else if
    } // else

    return true ;
} //

bool SICXEAssembler :: isOperandCorrect( int i ){

    if( newLine[i].name == "#"){
        if( i == info.back().lineTokenNum - 2 ){
            if( newLine[i+1].type != "symbol" && newLine[i+1].type != "integer" ) {
                info.back().errorMessage = "syntax error(wrong operand)" ;
                return false;
            }
        }
        else{
            info.back().errorMessage = "syntax error!!!" ;
            return false;
        }
    } // if
    else if( newLine[i].name == "@" ){
        if( i == info.back().lineTokenNum - 2 ){
            if( newLine[i+1].type != "symbol" ) {
                info.back().errorMessage = "syntax error(wrong operand)" ;
                return false;
            }
        } // if
        else{
            info.back().errorMessage = "syntax error!!!" ;
            return false;
        } // else
    } // else
    else if( newLine[i].name == "=" ){
        if( i == info.back().lineTokenNum - 4 ){

            if( newLine[i+1].name != "'" || newLine[i+3].name != "'" ){ //LDA	 =C'EOF'
                info.back().errorMessage = "syntax error!!!" ;
                return false;
            }
            if( newLine[i+2].type != "literalString" && newLine[i+2].type != "literalHexInt" ){
                info.back().errorMessage = "syntax error!!!" ;
                return false;
            }
        }
        else if( i == info.back().lineTokenNum -2 ){
            if( newLine[i+1].type != "literalDecInt" ){
                info.back().errorMessage = "syntax error!!!" ;
                return false;
            }
        } //else
        else{
            info.back().errorMessage = "syntax error!!!" ;
            return false;
        } // else
    } // else if

    return true ;
} // insOperandCorrect()

void SICXEAssembler :: analysisInstruction( int insIndex ){
    int operandNum = 0 ;
    if( ! isInsCorrect(insIndex) ){
        return ;
    }

    for( int i = insIndex+1 ; i < newLine.size() ; i++ ){

        if( optab[ newLine[insIndex].entry- 1 ].format == "2" ){

            if( strcasecmp( newLine[insIndex].name.c_str(),"SHIFTL") == 0 || strcasecmp( newLine[insIndex].name.c_str(),"SHIFTR") == 0 ||
                strcasecmp( newLine[insIndex].name.c_str(),"SVC") == 0){
                if( newLine[i].type == "register" || newLine[i].type == "integer" )
                    operandNum++ ;
                if( newLine[i].type != "register" && newLine[i].name != "," && newLine[i].type != "integer" && newLine[i].name != "."){
                    info.back().errorMessage = "syntax error(wrong operand)" ;
                    return ;
                }
            } // if
            else{
                if( newLine[i].type == "register" )
                    operandNum++ ;
                if( newLine[i].type != "register" && newLine[i].name != "," && newLine[i].name != "."){
                    info.back().errorMessage = "syntax error(wrong operand)" ;
                    return ;
                } // if
            } // else it is not SHIFTL SHIFTR
        } // if it is format2
        else{
            if( newLine[i].type == "symbol"  || newLine[i].type == "integer" ||
                newLine[i].type == "literalHexInt" || newLine[i].type == "literalDecInt" || newLine[i].type == "literalString")
                operandNum++ ;
            if( newLine[i].type == "instruction" || newLine[i].type == "pseudo" ||
               ( newLine[i].type == "register" && strcasecmp( newLine[i].name.c_str(),"X") != 0) ||
               ( newLine[i].name == "," && ( i != info.back().lineTokenNum-2 || strcasecmp( newLine[i+1].name.c_str(),"X") != 0 ))) {
                info.back().errorMessage = "syntax error(wrong operand)" ;
                return ;
            } // if

            if( ! isOperandCorrect(i) ){
                return ;
            }
        } // else
    } // for

    if( operandNum != stoi ( optab[newLine[insIndex].entry-1].operandNum ) ) {  // 檢查operandNum
        info.back().errorMessage = "syntax error!!!" ;
        return ;
    } // if
    else{
        info.back().errorMessage = "" ;
    } // else

    if( optab[newLine[insIndex].entry-1].format == "1" ){
        info.back().length = 1 ;
    }
    else if( optab[newLine[insIndex].entry-1].format == "2" ){
        info.back().length = 2 ;
    }
    else if ( optab[newLine[insIndex].entry-1].format == "3/4" ){
        if( insIndex > 0 && newLine[insIndex-1].name == "+" )
            info.back().length = 4 ;
        else
            info.back().length = 3 ;
    } // else if
} // analysisInstruction

void SICXEAssembler :: processPseudo( int pseudoIndex, bool & hasLocation ){

    if( strcasecmp( newLine[pseudoIndex].name.c_str(), "EQU") == 0 ){
        if( pseudoIndex != 1 || newLine[0].type != "symbol" ){
            info.back().insLocation = "" ;
            info.back().length = 0 ;
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        }
    }
    else if( pseudoIndex > 1 || (pseudoIndex == 1 && newLine[0].type != "symbol") ){
       info.back().errorMessage = "syntax error!!!" ;
        if( strcasecmp( newLine[pseudoIndex].name.c_str(), "END") == 0 || strcasecmp( newLine[pseudoIndex].name.c_str(), "LTORG") == 0){
            info.back().insLocation = "" ;
            info.back().length = 0 ;
        }
        return ;
    } // else if


    if( strcasecmp( newLine[pseudoIndex].name.c_str(), "BYTE") == 0){
        if( pseudoIndex == info.back().lineTokenNum-4 && newLine[pseudoIndex+2].type == "string") // EOF	BYTE	C'EOF'
            info.back().length = newLine[pseudoIndex+2].name.size() ;
        else if( pseudoIndex == info.back().lineTokenNum-4 && newLine[pseudoIndex+2].type == "integer" ) // INPUT	BYTE	X'F1'
            info.back().length = ceil( ( newLine[pseudoIndex+2].name.size() ) / 2 ) ;
        else{
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        }
    } // if
    else if(  strcasecmp( newLine[pseudoIndex].name.c_str(),"WORD") == 0 ){ // MAXLEN	WORD	4096
        if( pseudoIndex == info.back().lineTokenNum-2 && newLine[pseudoIndex+1].type == "integer" )
            info.back().length = 3 ;
        else{
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        }
    } // else if
    else if(  strcasecmp( newLine[pseudoIndex].name.c_str(), "RESB") == 0 ){  // BUFFER	RESB	4096
        if( pseudoIndex ==  info.back().lineTokenNum -2 &&  newLine[pseudoIndex+1].type == "integer" )
           info.back().length =  stoi(newLine[pseudoIndex+1].name)  ;
        else{
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        }
    } // else if
    else if(  strcasecmp( newLine[pseudoIndex].name.c_str(), "RESW") == 0 ){  // RETADR	RESW	1
        if(  pseudoIndex ==  info.back().lineTokenNum -2 && newLine[pseudoIndex+1].type == "integer" )
            info.back().length = stoi(newLine[pseudoIndex+1].name) * 3  ;
        else{
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        }
    } // else if
    else if(  strcasecmp( newLine[pseudoIndex].name.c_str(), "EQU") == 0 ){ // location會在裡面設定
        processEQU();
        return ;
    }
    else if(  strcasecmp( newLine[pseudoIndex].name.c_str(), "BASE") == 0 ){ // BASE LENGTH
        if( pseudoIndex == info.back().lineTokenNum -2 && newLine[pseudoIndex+1].type == "symbol" ){
            if( isInTable( newLine[pseudoIndex+1].name, symtab) )
                BASE = symtab[ newLine[pseudoIndex+1].entry ].address ;
        } // if
        else if( pseudoIndex == info.back().lineTokenNum -2 && newLine[pseudoIndex+1].type == "integer" ){
            string hex = decimalToHex( stoi(newLine[pseudoIndex+1].name) ) ;
            BASE = hex ;
        }
        else{
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        }
        info.back().length = 0 ;
        hasLocation = false ;
    }
    else if(  strcasecmp( newLine[pseudoIndex].name.c_str(), "LTORG") == 0 ){
        if( pseudoIndex == info.back().lineTokenNum - 1 ){
            info.back().length = 0 ;
            info.back().insLocation = "";
            info.back().errorMessage = "";
            defineLiteral();
            hasLocation = false ;
            return ;
        } // if
        else{
            info.back().length = 0 ;
            info.back().insLocation = "";
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        } // else
    } // else if
    else if(  strcasecmp( newLine[pseudoIndex].name.c_str(),"END") == 0 ){
        if( pseudoIndex == info.back().lineTokenNum-1 || ( pseudoIndex == info.back().lineTokenNum-2 && newLine[pseudoIndex+1].type == "symbol" ) ){
            gEnd = true ;
            info.back().length = 0 ;
            info.back().insLocation = "";
            info.back().errorMessage = "";
            defineLiteral();
            return ;
        }
        else{
            info.back().length = 0 ;
            info.back().insLocation = "";
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        }

        hasLocation = false ;
    } // else if
    else if (strcasecmp( newLine[pseudoIndex].name.c_str(),"START") == 0){
        if( ! gIsFirstLine ) {
            info.back().errorMessage = "START MUST BE IN FIRST LINE ERROR!!" ;
            return ;
        } // if
        info.back().length = 0 ;
    } // else if

    info.back().errorMessage = "";
} // processPseudo()

void SICXEAssembler :: defineLiteral(){

    for( int i = 0 ; i < littab.size() ; i++ ){
        if( ! littab[i].name.empty() && littab[i].address.empty() ){
            hexLOCCTR = PC ;
            vector<TokenInfo> addLine ;
            TokenInfo add ;
            Information newInfo ;
            string defineLiterl = "=" +  littab[i].name  ;

            if( littab[i].type == "literalHexInt" ){
                defineLiterl = defineLiterl + " BYTE X'" + littab[i].name + "'" ;
                newInfo.length = ceil(littab[i].name.size()/2)  ;
            }
            else if( littab[i].type == "literalString" ){
                defineLiterl = defineLiterl + " BYTE C'" + littab[i].name + "'" ;
                 newInfo.length = littab[i].name.size() ;
            } // else
            else if( littab[i].type == "literalDecInt"){
                defineLiterl = defineLiterl + " WORD " + littab[i].name ;
                 newInfo.length  = 3  ;
            }

            add.name = littab[i].name ;
            add.type = littab[i].type ;
            addLine.push_back(add) ;
            tokens.push_back(addLine) ;
            newInfo.source = defineLiterl ;
            newInfo.lineTokenNum = 0 ;
            newInfo.insLocation = hexLOCCTR  ;
            newInfo.errorMessage = "" ;
            info.push_back(newInfo);
            littab[i].address = hexLOCCTR ;
            calculatePC( info.back().length);
        } // if
    } // for
} // defineLiteral

void SICXEAssembler :: processEQU(){

    if( info.back().lineTokenNum == 3 && newLine[0].type == "symbol" && newLine[2].name == "*") {// BUFFEND EQU	*
        symtab[ newLine[0].entry].address = PC ;
        info.back().insLocation = PC  ;
        info.back().length = 0 ;
        info.back().errorMessage = "";
    }
    else if( info.back().lineTokenNum == 3 && newLine[0].type == "symbol" && newLine[2].type == "integer"){ // 	var3  EQU  12
        string address = decimalToHex( stoi(newLine[2].name) ) ;
        while( address.size() < 4 )
            address = "0" + address ;
        symtab[ newLine[0].entry].address = address ;
        info.back().insLocation = address;
        info.back().length = 0 ;
        info.back().errorMessage = "";
    } // else if
    else if (  info.back().lineTokenNum == 3 && newLine[0].type == "symbol" && newLine[2].type == "symbol" ){ // ALPHA EQU BETA
        info.back().length = 0 ;
        info.back().errorMessage = "";
        if( isInTable( newLine[2].name, symtab ) ){
            string address = symtab[ newLine[2].entry].address ;
            symtab[ newLine[0].entry ].address = address ;
            info.back().insLocation = address ;
        }
        else{ // pass2才能解決
            info.back().insLocation = "xxxx" ;
            symtab[ newLine[0].entry ].address = "xxxx" ;
        }
    }
    else if( info.back().lineTokenNum > 3 ){// MAXLEN EQU BUFFEND-BUFFER
        string errorSymbol ;
        string address = calEQUValue(newLine, errorSymbol) ;
        if( address == "ERROR" ){
            info.back().insLocation = "";
            info.back().length = 0 ;
            info.back().errorMessage = "syntax error!!!" ;
            return ;
        }
        symtab[ newLine[0].entry ].address = address ;
        info.back().insLocation = address;
        info.back().length = 0 ;
        info.back().errorMessage = "";
    } // else if
    else{
        info.back().errorMessage = "syntax error!!!" ;
        info.back().insLocation = "";
        info.back().length = 0 ;
        return ;
    } // else
} // processEQU

string SICXEAssembler :: calEQUValue( vector<TokenInfo> aLine, string & errorSymbol){ // MAXLEN EQU BUFFEND-BUFFER
    string result ;

    int tokenNum ;
    if( aLine.back().name == "." )
        tokenNum = aLine.size() -1 ;
    else
        tokenNum = aLine.size() ;

    for( int i = 2 ; i < tokenNum ; i++ ){
        if( i % 2 == 0 ){
            if( aLine[i].type == "symbol"){
                if( isInTable( aLine[i].name, symtab ) ){
                    string symAddr = findSymbolAddr(aLine[i]) ;
                    if( symAddr == "xxxx" || symAddr == "" ){
                        errorSymbol = aLine[i].name ;
                        return "xxxx";
                    }
                    if( i == 2 )
                        result = symAddr ;
                    else if ( aLine[i-1].name == "+" || aLine[i-1].name == "-" || aLine[i-1].name == "*" || aLine[i-1].name == "/") {
                        result = hexCal( result, symAddr, aLine[i-1].name ) ;
                    }
                } // if
                else{
                    errorSymbol = aLine[i].name ;
                    return "xxxx"; // pass2才能算出來
                }
            } // if
            else{
                return "ERROR";
            }
        } // if
        else if ( aLine[i].name != "+" && aLine[i].name != "-" && aLine[i].name != "*" && aLine[i].name != "/"){
            return "ERROR";
        }
    } // for
    return result ;
} // calEQUValue

void SICXEAssembler :: pass2(){

    for( int i = 0 ; i < tokens.size() ; i++ ){

/*
        for( int j = 0 ; j < tokens[i].size() ; j++ )
            cout << tokens[i][j].name << "(" << tokens[i][j].type << ")" << "\t" ;
        cout << endl ;
*/
        if( info[i].errorMessage != "" || tokens[i].size() == 0 || tokens[i][0].name == "." ){// errrorMessage不為空
            info[i].insObjcode = "" ;
            continue ;
        }

        for( int j = 0 ; j < tokens[i].size() ; j++ ){
            if( tokens[i][j].type == "instruction" ){
                calInsOpcode(i,j) ;
                break ;
            } // if
            else if( tokens[i][j].type == "pseudo" ){
                calPseudoOpcode(i, j) ;
                break ;
            } // else
            else if( tokens[i][j].type == "literalHexInt" || tokens[i][j].type == "literalDecInt"){
                string value = tokens[i][j].name ;
                info[i].insObjcode = value ;
            }
            else if( tokens[i][j].type == "literalString" ){
                string asciiCode ;
                for( int m = 0 ; m < tokens[i][j].name.size() ; m++ ){
                    int ascii = tokens[i][j].name[m] ;
                    asciiCode = asciiCode + decimalToHex(ascii) ;
                }
                info[i].insObjcode = asciiCode ;
            }
        } // for

        //cout << "object : " << info[i].insObjcode << endl << endl ;
    }//for
}//pass2()

void SICXEAssembler :: calInsOpcode( int index, int j){

    string machineCode ;
    string opcode = optab[ tokens[index][j].entry -1].opcode;
    string r1, r2 ;

    if( optab[ tokens[index][j].entry -1].format == "1"){
        machineCode = opcode ;
    }
    else if(optab[ tokens[index][j].entry -1].format == "2" ){
        if( strcasecmp( tokens[index][j].name.c_str(),"SVC") == 0 )
            r1 = tokens[index][j+1].name ;
        else
            r1 = regtab[ tokens[index][j+1].entry - 1].num ;
        if( strcasecmp( tokens[index][j].name.c_str(),"SHIFTL") == 0 || strcasecmp( tokens[index][j].name.c_str(),"SHIFTR") == 0 )
            r2 = to_string( stoi( tokens[index][j+3].name ) -1 );
        else if( optab[ tokens[index][j].entry -1].operandNum == "2" )
            r2 = regtab[ tokens[index][j+3].entry - 1].num ;
        else r2 = "0" ;
        machineCode = opcode + r1 + r2 ;
    }
    else if(optab[ tokens[index][j].entry -1].format == "3/4" ){
        if( j >= 1 && tokens[index][j-1].name == "+" ){ // format4
            machineCode = processFormat4(index, j, opcode) ;
        }
        else{ // format3
            machineCode = processFormat3(index, j, opcode) ;
        } // else
    } // else format is 3/4

    if ( machineCode == "ERROR" )
        info[index].insObjcode = "" ;
    else
        info[index].insObjcode = machineCode ;
} // calOpcode

string SICXEAssembler :: processFormat4(int index, int j, string opcode){

    string n, i, x, b, p, e ;
    string addr ;
    int symIndex;
    e = "1" ;
    x = "0" ;
    b = "0" ;
    p = "0" ;

    if( optab[tokens[index][j].entry-1].operandNum == "0" ){
        n = "1" ;
        i = "1" ;
        addr = "00000" ;
    } // if
    else if( tokens[index][j+1].name == "=" ){
        n = "1" ;
        i = "1" ;
        int operandIndex ;
        if( tokens[index][j+2].type == "literalDecInt" )
            operandIndex = j+2 ;
        else
            operandIndex = j+3 ;
        addr = findLiteralAddr(tokens[index][operandIndex]);
    } // else if
    else if( tokens[index][j+1].name != "@" &&  tokens[index][j+1].name != "#" ){
        symIndex = j+1 ;
        n = "1" ;
        i = "1" ;
        if( tokens[index][j+1].type == "symbol" && j+1 == info[index].lineTokenNum-1 )
            addr = findSymbolAddr(tokens[index][j+1] ) ;
        else if( tokens[index][j+1].type == "integer" && j+1 == info[index].lineTokenNum-1 )
            addr = decimalToHex( stoi(tokens[index][j+1].name) ) ;
        else if( tokens[index][j+1].type == "symbol" && j+1 == info[index].lineTokenNum-3 && tokens[index][j+3].type == "register"
                        &&  strcasecmp( tokens[index][j+3].name.c_str(), "X") == 0) { // use index
            x = "1" ;
            addr = findSymbolAddr(tokens[index][j+1] ) ;
        }
    } // if
    else if( tokens[index][j+1].name == "#" || tokens[index][j+1].name == "@" ){
        symIndex = j+2 ;
        if( tokens[index][j+2].type == "symbol" )
            addr = findSymbolAddr(tokens[index][j+2] ) ;
        else if ( tokens[index][j+2].type == "integer" )
            addr = decimalToHex( stoi(tokens[index][j+2].name) ) ;

        if( tokens[index][j+1].name == "#" ){
            n = "0" ;
            i = "1" ;
        }
        else if( tokens[index][j+1].name == "@" ){
            n = "1" ;
            i = "0" ;
        }
    } // else if

    string opnixbpe = combineOPnixbpe(opcode, n, i, x, b, p, e) ;
    if( addr == "" || addr == "xxxx"){
        info[index].errorMessage = " undefined symbol : " + tokens[index][symIndex].name ;
        return "ERROR" ;
    }
    else{
        while( addr.size() < 5 )
            addr = "0" + addr ;
        string machineCode = opnixbpe + addr ;
        return machineCode ;
    }

} // processFormat4

string SICXEAssembler :: processFormat3(int index, int j, string opcode){

    string n, i, x, b, p, e ;
    string disp ;
    string machineCode ;
    e = "0" ;
    if( j < info[index].lineTokenNum-1 && tokens[index][j+1].name == "=" ){ // literal
        n = "1" ;
        i = "1" ;
        x = "0" ;
        int operandIndex ;
        if( tokens[index][j+2].type == "literalDecInt" ) // LDA =3277
            operandIndex = j+2 ;
        else  // TD	=X'05'
            operandIndex = j+3 ;
        string literalAddr = findLiteralAddr(tokens[index][operandIndex]);
        disp = choosePcOrBase( index, literalAddr, p, b) ;
        if( disp == "" ){
            return "ERROR";
        }
        else{
            while( disp.size() < 3 )
                disp = "0" + disp ;
            string opnixbpe = combineOPnixbpe( opcode, n, i, x, b, p,  e) ;
            machineCode = opnixbpe + disp ;
        } // else
    }//if
    else if( optab[tokens[index][j].entry-1].operandNum == "0" ||
            ( j < info[index].lineTokenNum-1 && tokens[index][j+1].name != "@" &&  tokens[index][j+1].name != "#") ){  // FIRST	STL	RETADR
        n = "1" ;
        i = "1" ;
        machineCode = createFormat3Machine(index, j, j+1, opcode, n, i, x, b, p ,e ) ;
    } // if
    else if( tokens[index][j+1].name == "#" || tokens[index][j+1].name == "@" ){  // 		LDB #LENGTH
        if( tokens[index][j+1].name == "#" ){
            n = "0" ;
            i = "1" ;
            x = "0" ;
        } // if
        else if( tokens[index][j+1].name == "@" ){
            n = "1" ;
            i = "0" ;
            x = "0" ;
        } // else
        machineCode = createFormat3Machine(index, j, j+2, opcode, n, i, x, b, p ,e ) ;
    } // else

    return machineCode ;
} // processFormat3

string SICXEAssembler :: choosePcOrBase( int index, string targetAddr, string & p, string & b){
    int decimalDisp = calDisp( targetAddr, getPC(index)) ;
    if( decimalDisp <= 2047 && decimalDisp >= -2048 ){
        p = "1" ;
        b = "0" ;
    }
    else if( BASE != "" ){// 用BASE REGISTER
        decimalDisp = calDisp( targetAddr, BASE) ;
        if( decimalDisp >= 0 && decimalDisp <= 4095 ){
            p = "0" ;
            b = "1" ;
        }
        else{
            info[index].errorMessage = "error distance" ;
            return "" ;
        }
    } // else
    else{
        info[index].errorMessage = "error distance" ;
        return "" ;
    }
    string disp = decimalToHex(decimalDisp);
    return disp ;
} // choosePcOrBase

int calDisp( string a, string b){

    int decimalResult ;
    int decimalA = hexToDecimal(a) ;
    int decimalB = hexToDecimal(b) ;
    decimalResult = decimalA - decimalB ;
    return decimalResult ;
} // calDisp

string SICXEAssembler :: getPC(int i){
    for( i = i + 1; i < info.size() ; i++ ){
        if( info[i].insLocation != "" ){
            return info[i].insLocation ;
        }
    }
} //getPC(int i)


string SICXEAssembler :: combineOPnixbpe( string opcode, string n, string i, string x, string b, string p, string e){
    string opcodeni = calOpcodeni( opcode, n, i) ;
    string xbpe = x + b + p + e ;
    return opcodeni + binToHex(xbpe) ;
} // combineOPnixbpe

string calOpcodeni( string opcode, string n, string i){

    string bin = decimalToBin(hexToInt(opcode[1]) ) ;
    while( bin.size() < 4 )
        bin = "0" + bin ;
    bin = bin.substr(0,2) + n + i;
    string hex = opcode[0] + binToHex(bin);
    return hex ;
} // calOpcodeni

string SICXEAssembler :: createFormat3Machine( int index, int insIndex, int operandIndex, string opcode, string n, string i, string & x, string & b, string & p, string e){
    string machineCode;
    string disp ;

    if( optab[ tokens[index][insIndex].entry -1].operandNum == "0" ){
        x = "0" ;
        b = "0" ;
        p = "0" ;
        disp="000" ;
    }
    else if( tokens[index][operandIndex].type == "symbol" && operandIndex == info[index].lineTokenNum-1 ){ // FIRST	STL	RETADR
        x = "0" ;
        string symAddr = findSymbolAddr( tokens[index][operandIndex]) ;
        if( symAddr == "" || symAddr == "xxxx" )
            disp ="" ;
        else{
            disp = choosePcOrBase( index, symAddr, p, b) ;
            if( disp == "")
                return "ERROR";
        }
    } // if
    else if ( tokens[index][operandIndex].type == "integer" && operandIndex == info[index].lineTokenNum-1 ){
        x = "0" ;
        b = "0" ;
        p = "0" ;
        disp = decimalToHex( stoi(tokens[index][operandIndex].name)) ;
    }
    else if( tokens[index][operandIndex].type == "symbol" && operandIndex < info[index].lineTokenNum-1 && tokens[index][operandIndex+2].type == "register"
                        &&  strcasecmp( tokens[index][operandIndex+2].name.c_str(), "X") == 0) { // 用 index register
        x = "1" ;
        string symAddr = findSymbolAddr( tokens[index][operandIndex]) ;
        if( symAddr == "" || symAddr == "xxxx" ){
            disp ="" ;
        }
        else{
            disp = choosePcOrBase(  index, symAddr, p, b) ;
            if( disp == "")
                return "ERROR" ;
        }
    } // else if


    if( disp == "" ){
        info[index].errorMessage = "undefined symbol :  " + tokens[index][operandIndex].name;
        return "ERROR" ;
    }
    else{
        string opnixbpe = combineOPnixbpe( opcode, n, i, x, b, p,  e) ;

        while( disp.size() < 3 )
            disp = "0" + disp ;
        machineCode = opnixbpe + disp ;
        return machineCode ;
    } // else

} // createFormat3Machine


void SICXEAssembler :: calPseudoOpcode( int i, int j ){

    string asciiCode ;
    if( strcasecmp( tokens[i][j].name.c_str(), "BYTE") == 0 ){ // EOF	BYTE	C'EOF'
        if( tokens[i][j+2].type == "string" ){
            for( int m = 0 ; m < tokens[i][j+2].name.size() ; m++ ){
                int ascii = tokens[i][j+2].name[m] ;
                asciiCode = asciiCode + decimalToHex(ascii) ;
            }
            info[i].insObjcode = asciiCode ;
        }
        else if( tokens[i][j+2].type == "integer" ){ // INPUT	BYTE	X'F1'
            info[i].insObjcode = tokens[i][j+2].name ;
        } // else if
    } // if
    else if( strcasecmp( tokens[i][j].name.c_str(), "WORD") == 0 ){ //MAXLEN	WORD	4096
        string hex = decimalToHex( stoi(tokens[i][j+1].name) ) ;
        while( hex.size() < 6 )
            hex = "0" + hex ;
        info[i].insObjcode = hex ;

    } // else if
    else if (  strcasecmp( tokens[i][j].name.c_str(), "BASE") == 0 ){
        info[i].insObjcode = "" ;
        if( tokens[i][j+1].type == "symbol"){
            string symAddr = findSymbolAddr( tokens[i][j+1] ) ;
            if( symAddr == "" || symAddr == "xxxx" )
                info[i].errorMessage = "undefined symbol : " + tokens[i][j+1].name;
            else
                BASE = symAddr ;
        }
    } // else if
    else if( strcasecmp( tokens[i][j].name.c_str(), "EQU") == 0 ){
        info[i].insObjcode = "" ;

        if( info[i].insLocation == "xxxx" || info[i].insLocation == "" ){
            if (  info[i].lineTokenNum == 3 && tokens[i][0].type == "symbol" && tokens[i][2].type == "symbol" ){ // ALPHA EQU BETA
                string address = findSymbolAddr(tokens[i][2]) ;
                if( address == "xxxx" || address == "" ){
                    info[i].errorMessage = " undefined symbol : " + tokens[i][2].name ;
                }
                symtab[ tokens[i][0].entry ].address = address ;
                info[i].insLocation = address ;
            }
            else if( info[i].lineTokenNum > 3 ){// MAXLEN EQU BUFFEND-BUFFER
                string errorSymbol ;
                string address = calEQUValue(tokens[i],errorSymbol) ;

                if( address == "xxxx")
                    info[i].errorMessage = " undefined symbol : " + errorSymbol ;

                symtab[ tokens[i][0].entry ].address = address ;
                info[i].insLocation = address ;
            } // else if
        } // if

    } // else if
    else{
        info[i].insObjcode = "" ;
    }
} //calPseudoOpcode


string SICXEAssembler :: findSymbolAddr( TokenInfo & symbol ){

    int hashValue = hashFunction( symbol.name )  ; // 計算它的hashValue雜湊值
    int key = hashValue ;

    while( ! symtab[key].name.empty() ){
        if( symtab[key].name == symbol.name ){
            string address = symtab[key].address ;
            symbol.entry = key ;
            if( address == "" || address == "xxxx" )
                break ;
            return symtab[key].address;
        }
        key = ( key + 1 ) % 100 ;
    } // while

    for( int i = 0 ; i < undefinedSymbol.size() ; i++ ){
        if( undefinedSymbol[i] == symbol.name )
            return "" ;
    }
    undefinedSymbol.push_back(symbol.name) ;
    return "" ;
} // findSymbolAddr

string SICXEAssembler :: findLiteralAddr( TokenInfo & literal){

    int hashValue = hashFunction( literal.name )  ; // 計算它的hashValue雜湊值
    int key = hashValue ;

    while( ! littab[key].name.empty() ){
        if( littab[key].name == literal.name ){
            literal.entry = key ;
            return littab[key].address;
        }
        key = ( key + 1 ) % 100 ;
    } // while
    return "" ;
} // findLiteralAddr()

string hexCal( string a, string b, string calType ){
    string hexResult ;
    int decimalResult ;
    int decimalA = hexToDecimal(a) ;
    int decimalB = hexToDecimal(b) ;
    if ( calType == "+")
        decimalResult = decimalA + decimalB ;
    else if( calType == "-" )
        decimalResult = decimalA - decimalB ;
    else if( calType == "*" )
        decimalResult = decimalA * decimalB ;
    else if( calType == "/" )
        decimalResult = decimalA / decimalB ;

    if( decimalResult >= 0 ){
        hexResult = decimalToHex(decimalResult) ;
    }
    else{
        int positive = -1 * decimalResult ;
        string bin = decimalToBin(positive) ;
        while( bin.size() < 12 )
            bin = "0" + bin ;
        bin = twoComplement(bin) ;
        for( int i = 0 ; i < bin.size() ; i=i+4){
            string hex = binToHex( bin.substr(i,4)) ;
            hexResult = hexResult + hex ;
        }

    }
    while( hexResult.size() < 4 )
        hexResult = "0" + hexResult;
    return hexResult ;
} // hexCal

string twoComplement( string bin){
    for( int i = 0 ; i < bin.size() ; i++ ){
        if( bin[i] == '1')
            bin[i] = '0' ;
        else if ( bin[i] == '0' )
            bin[i] = '1' ;
    } // for


    if( bin[bin.size()-1] == '0' )
        bin[bin.size()-1] = '1' ;
    else{
        bin[bin.size()-1] = '0' ;
        for( int i = bin.size()-2 ; i >= 0 ; i-- ){
            if( bin[i] == '0' ){
                bin[i] = '1' ;
                break ;
            } // if
            else
                bin[i] = '0' ;
        }
    }
    return bin ;
} // twoComplement


void calculatePC( int length ){
    //cout << "length = " << length << endl ;
    int decimalPC = hexToDecimal(PC) ;
    decimalPC = decimalPC + length ;
    PC = decimalToHex(decimalPC) ;
    while( PC.size() < 4 )
        PC = "0" + PC ;
} // calculatePC()

int hexToDecimal( string hex ){

    int decimal = 0;

    for( int i = hex.size()-1, j=0 ; i >=0 ; i--, j++ ){
        decimal = decimal + hexToInt(hex[i])* pow( 16, j) ;
    } // for
    return decimal ;
} // hexToDecimal

string hexToBinary( string hex ){
    string bin ;
    for( int i = 0 ; i < hex.size() ; i++ ){
        if( hex[i] == '0') bin = bin + "0000" ;
        else if( hex[i] == '1') bin = bin + "0001" ;
        else if( hex[i] == '2') bin = bin + "0010" ;
        else if( hex[i] == '3') bin = bin + "0011" ;
        else if( hex[i] == '4') bin = bin + "0100" ;
        else if( hex[i] == '5') bin = bin + "0101" ;
        else if( hex[i] == '6') bin = bin + "0110" ;
        else if( hex[i] == '7') bin = bin + "0111" ;
        else if( hex[i] == '8') bin = bin + "1000" ;
        else if( hex[i] == '9') bin = bin + "1001" ;
        else if( hex[i] == 'A') bin = bin + "1010" ;
        else if( hex[i] == 'B') bin = bin + "1011" ;
        else if( hex[i] == 'C') bin = bin + "1100" ;
        else if( hex[i] == 'D') bin = bin + "1101" ;
        else if( hex[i] == 'E') bin = bin + "1110" ;
        else if( hex[i] == 'F') bin = bin + "1111" ;
    }

    //cout << "bin " << bin << endl ;
    return bin ;
} // hexToBinary

string binToHex( string bin ){

    if( bin == "0000" ) return "0" ;
    else if( bin == "0001" ) return "1" ;
    else if( bin == "0010" ) return "2" ;
    else if( bin == "0011" ) return "3" ;
    else if( bin == "0100" ) return "4" ;
    else if( bin == "0101" ) return "5" ;
    else if( bin == "0110" ) return "6" ;
    else if( bin == "0111" ) return "7" ;
    else if( bin == "1000" ) return "8" ;
    else if( bin == "1001" ) return "9" ;
    else if( bin == "1010" ) return "A" ;
    else if( bin == "1011" ) return "B" ;
    else if( bin == "1100" ) return "C" ;
    else if( bin == "1101" ) return "D" ;
    else if( bin == "1110" ) return "E" ;
    else if( bin == "1111" ) return "F" ;

} // binToHex

string decimalToBin( int decimal){

    //cout << "decimal : " << decimal << endl ;
    int quotient = decimal ;
    string bin ;
    while( quotient != 0 ){
        int remainder = quotient % 2 ;
        quotient = quotient / 2 ;
        bin = intTohex(remainder) + bin ;
    } // while
    return bin ;
} // decimalToBin()

string decimalToHex( int decimal){

    if( decimal == 0 ){
        return "0" ;
    }
    else if( decimal > 0 ){
        int quotient = decimal ;
        string hex ;
        while( quotient != 0 ){
            int remainder = quotient % 16 ;
            quotient = quotient / 16 ;
            hex = intTohex(remainder) + hex ;
        } // while
        return hex ;
    } // if
    else{
        string hexResult ;
        int positive = -1 * decimal ;
        string bin = decimalToBin(positive) ;
        while( bin.size() < 12 )
            bin = "0" + bin ;
        bin = twoComplement(bin) ;
        for( int i = 0 ; i < bin.size() ; i=i+4){
            string hex = binToHex( bin.substr(i,4)) ;
            hexResult = hexResult + hex ;
        }
    return hexResult ;
    }
} // decimalToHex

string intTohex( int a ){

    if( a == 0) return "0" ;
    else if ( a == 1 ) return "1" ;
    else if ( a == 2 ) return "2" ;
    else if ( a == 3 ) return "3" ;
    else if ( a == 4 ) return "4" ;
    else if ( a == 5 ) return "5" ;
    else if ( a == 6 ) return "6" ;
    else if ( a == 7 ) return "7" ;
    else if ( a == 8 ) return "8" ;
    else if ( a == 9 ) return "9" ;
    else if ( a == 10 ) return "A" ;
    else if ( a == 11 ) return "B" ;
    else if ( a == 12 ) return "C" ;
    else if ( a == 13 ) return "D" ;
    else if ( a == 14 ) return "E" ;
    else if ( a == 15 ) return "F" ;
} // intTohex

int hexToInt(char a){

    switch(a){
        case '0':return 0;break;
        case '1':return 1;break;
        case '2':return 2;break;
        case '3':return 3;break;
        case '4':return 4;break;
        case '5':return 5;break;
        case '6':return 6;break;
        case '7':return 7;break;
        case '8':return 8;break;
        case '9':return 9;break;
        case 'A':return 10;break;
        case 'B':return 11;break;
        case 'C':return 12;break;
        case 'D':return 13;break;
        case 'E':return 14;break;
        case 'F':return 15;break;
    }
    return -1;
} // hexToInt

void SICXEAssembler :: storeDelimeter( string aLine , TokenInfo & newToken ){
    newToken.name = newToken.name + aLine[gColumnWalk] ;
    newToken.type = "delimeter" ;
    gColumnWalk++ ;
    if ( newToken.name == "'"){
        if( gColumnWalk == aLine.size() && !hasLeftQuote ){
            hasLeftQuote = false ;
            info.back().errorMessage = "quote error !" ;
        }
        else
            hasLeftQuote = !hasLeftQuote ;
    }
} // storeDelimeter

void SICXEAssembler :: getNotWhite( string aLine ){
    while(  aLine[gColumnWalk] == ' ' || aLine[gColumnWalk] == '\t' )
       gColumnWalk++ ;
} // getNotWhite()

void SICXEAssembler :: getUntilWhiteOrDel(string aLine, string & tokenName){

    // 還沒讀到空白、結束、分隔符
    if( isDelimeter(aLine[gColumnWalk])){
        tokenName = tokenName + aLine[gColumnWalk] ;
        gColumnWalk++ ;
        if( tokenName == "'")
            hasLeftQuote = !hasLeftQuote ;
    }
    else{
        while(  gColumnWalk < aLine.size() && aLine[gColumnWalk] != ' ' && aLine[gColumnWalk] != '\t' && aLine[gColumnWalk] != '\n'
              && !isDelimeter(aLine[gColumnWalk])) {
            tokenName = tokenName + aLine[gColumnWalk] ;
            gColumnWalk++ ;
        } // while  gColumn會走到下一個還沒儲存的字元
    } // else
} // getUntilWhite()

void SICXEAssembler :: getUntilQuote( string aLine, string & tokenName ){

    while( gColumnWalk < aLine.size() && aLine[gColumnWalk] != '\''){
        tokenName = tokenName + aLine[gColumnWalk] ;
        gColumnWalk++ ;
        if( gColumnWalk == aLine.size() ) { // ERROR
            hasLeftQuote = false ;
            info.back().errorMessage = "quote error !" ;
            break ;
        } // if
    } // while
} // getUntilQuote()

bool SICXEAssembler :: isDelimeter( char aChar){
    for( int i = 0 ; i < delimtab.size() ; i++ ) {
        if ( aChar == delimtab[i].name[0]) {
            return true ;
        } // if
    } // for
    return false ;
} // IsDelimeter()

bool isDigit( string str ){
    for( int i = 0 ; i < str.length() ; i++ ){
        if( str[i]  < '0' || str[i] > '9')
            return false ;
    } // for
    return true ;
} // isDigit()


void SICXEAssembler :: findTokenType( TokenInfo & aToken, string preToken, string prePreToken, string prePrePreToken ){

    int numOfThisLineToken = newLine.size() ;

    if( isDelimeter(aToken))
        aToken.type = "delimeter" ;
    else if( isInstruc(aToken) )
        aToken.type = "instruction" ;
    else if( isPseudo(aToken) )
        aToken.type = "pseudo" ;
    else if( isRegister(aToken) )
        aToken.type = "register" ;
    else if(( numOfThisLineToken >=2 && preToken == "'" && ( prePreToken == "X" || prePreToken == "x") ) ){ // integer
        if( prePrePreToken == "=" ) {
            putInLittab( aToken, preToken, prePreToken ) ;
        }
        else{
            aToken.type = "integer" ;
            findTokenEntry( aToken, litNumTab ) ;
        }
    } // else if
    else if( numOfThisLineToken >=2 && preToken == "'" && ( prePreToken == "C" || prePreToken == "c") ){   // 是STRING
        if( prePrePreToken == "=" ) {
            putInLittab( aToken, preToken, prePreToken ) ;
        }
        else{
            aToken.type = "string" ;
            findTokenEntry( aToken, litStrTab ) ;
        }
    } // else if
    else if( (isDigit(aToken.name))){
        if( preToken == "=" ){
            putInLittab( aToken, preToken, prePreToken ) ;
        }
        else{
            aToken.type = "integer" ;
            findTokenEntry( aToken, litNumTab ) ;
        }
    }
    else if(numOfThisLineToken >=1 && preToken == "'"){
        info.back().errorMessage = "syntax error!!!" ;
        return ;
    }
    else if( (aToken.name[0]  >= 'a' && aToken.name[0] <= 'z') || (aToken.name[0]  >= 'A' && aToken.name[0] <= 'Z') ){  // 是SYMBOL
        aToken.type = "symbol" ;
        if( newLine.size() == 0 || isInTable( aToken.name, symtab ) )
            findSymbolEntry( aToken ) ;
    }// else if
    else {  // 其他狀況
        info.back().errorMessage = "syntax error!!!" ;
        return ;
    } // else
} //  findTokenType

bool SICXEAssembler :: isInstruc( TokenInfo & aToken ){
    for( int i = 0 ; i < optab.size() ; i++ ){
        if( strcasecmp( aToken.name.c_str(), optab[i].name.c_str()) == 0 ){
            aToken.entry = i+1 ;
            return true ;
        }
    } // for
    return false ;
} //isInstruc

bool SICXEAssembler :: isPseudo( TokenInfo & aToken ){
    for( int i = 0 ; i < pseudo.size() ; i++ ){
        if( strcasecmp( aToken.name.c_str(), pseudo[i].name.c_str()) == 0 ){
            aToken.entry = i+1 ;
            return true ;
        } // if
    } // for
    return false ;
} //isPseudo

bool SICXEAssembler :: isRegister(TokenInfo & aToken){
    for( int i = 0 ; i < regtab.size() ; i++ ){
        if( strcasecmp( aToken.name.c_str(), regtab[i].name.c_str()) == 0 ){
            aToken.entry = i+1 ;
            return true ;
        }
    } // for
    return false ;
} // isRegister

bool SICXEAssembler :: isDelimeter( TokenInfo & aToken ){
    for( int i = 0 ; i < delimtab.size() ; i++ ){
        if( strcasecmp( aToken.name.c_str(), delimtab[i].name.c_str()) == 0 ){
            aToken.entry = i+1 ;
            return true ;
        }
    } // for
    return false ;
} //isDelimeter


bool SICXEAssembler :: isInTable( string name, vector<VarTable> & table ){
    int hashValue = hashFunction( name )  ; // 計算它的hashValue雜湊值
    int key = hashValue ;

    while( ! table[key].name.empty() ){
        if( table[key].name == name ){  // 在pass1的symtab找到相同名稱的symbol-->error
            return true;
        } // if

        key = ( key + 1 ) % 100 ;
    } // while

    return false ;
} // isInTable

bool isHexInt( char ch ){

    if( (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))
        return true ;
    else
        return false ;

} //isHexIn

void SICXEAssembler ::  findSymbolEntry( TokenInfo & aToken ){

    int hashValue = hashFunction( aToken.name )  ; // 計算它的hashValue雜湊值
    int key = hashValue ;

    while( ! symtab[key].name.empty() ){
        if( symtab[key].name == aToken.name ){  // 在pass1的symtab找到相同名稱的symbol-->error
            if( newLine.size() == 0 ){
                info.back().errorMessage = "duplicate definition : " + aToken.name ;
                for( int i = 0 ; i < duplicatedSymbol.size() ; i++ ){
                    if( duplicatedSymbol[i] == aToken.name )
                        return ;
                }
                duplicatedSymbol.push_back(aToken.name) ;
                return ;
            }
            aToken.entry = key ;
            return ;
        } // if

        key = ( key + 1 ) % 100 ;
    } // while

    aToken.entry = key ;
    symtab[key].name = aToken.name ;

} // findSymbolEntry

void SICXEAssembler ::  findTokenEntry( TokenInfo & aToken, vector<VarTable> & table ){

    int hashValue = hashFunction( aToken.name )  ; // 計算它的hashValue雜湊值
    int key = hashValue ;

    while( ! table[key].name.empty() ){
        if( table[key].name == aToken.name ){  // 在pass1的symtab找到相同名稱的symbol-->error
            aToken.entry = key ;
            return ;
        } // if

        key = ( key + 1 ) % 100 ;
    } // while

    aToken.entry = key ;
    table[key].name = aToken.name ;

} // findTokenEntry

void SICXEAssembler :: putInLittab( TokenInfo & aToken, string preToken, string prePreToken){

    int hashValue = hashFunction( aToken.name )  ; // 計算它的hashValue雜湊值
    int key = hashValue ;

    while( ! littab[key].name.empty() ){
        if( littab[key].name == aToken.name ){
            aToken.entry = key ;
            aToken.type = littab[key].type ;
            return ;
        } // if

        key = ( key + 1 ) % 100 ;
    } // while

    aToken.entry = key ;
    littab[key].name = aToken.name ;
    littab[key].value = aToken.name ;

    if( preToken == "'" && ( prePreToken == "X" || prePreToken == "x") )
        littab[key].type = "literalHexInt" ;
    else if( preToken == "'" && ( prePreToken == "C" || prePreToken == "c"))
        littab[key].type = "literalString" ;
    else
        littab[key].type = "literalDecInt" ;
    aToken.type = littab[key].type ;

} // putInLittab

int SICXEAssembler ::  hashFunction( string tokenName ){ // y雜湊函數:將每個字元的ASCII相加再mod 100
    int hashValue = 0 ;
    for ( int i = 0 ; tokenName[i] != NULL  ; i++ ) {
        hashValue = ( hashValue + tokenName[i] ) % 100 ;
    } // for
    return hashValue ;
} // SICXEAssembler ::  hashFunction()

void SICXEAssembler :: writeOutputFile(){

    fstream sicObjectFile ;
    int pos = sicFileName.find(".") ;
    string outfileName = sicFileName ;
    outfileName = outfileName.insert(pos, "_output") ;
    sicObjectFile.open( outfileName.c_str() , ios :: out ) ;

    string line ;
    sicFile.open( sicFileName.c_str(), ios::in ) ;
    sicObjectFile << "Line  Location          Source code                  Object code" << endl ;
    sicObjectFile << "----  --------   -------------------------        -----------------" << endl ;
    for( int i = 0 ; i < info.size() ; i++ ) {
        sicObjectFile << setw(3) << 5*(i+1) << "  " << setw(8) << info[i].insLocation << "   " ;
        sicObjectFile << setw(20) << info[i].source ;
        //cout << info[i].source << "\t" << info[i].insLocation << "\t" << info[i].insObjcode << endl ;
        //

        if ( info[i].insObjcode.size() > 0 )
          sicObjectFile << "        " <<  setw(20) <<info[i].insObjcode << endl  ;
        else
          sicObjectFile << "        " <<  setw(20) << info[i].errorMessage << endl ;

    } // for

    sicObjectFile.close();
    sicFile.close();

} // writeOutputFile
