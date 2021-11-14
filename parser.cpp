// ddb.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <map>
#include <list>
#include<string>
#include<vector>
using namespace std;



class reserve
{
private:
	map<string, int> reserve_word;
public:	 
	reserve()
	{
		reserve_word["select"] = 0;
		reserve_word["from"] = 1;
		reserve_word["where"] = 2;
		reserve_word["update"] = 3;
		reserve_word["delete"] = 4;
		reserve_word["and"] = 5;
		reserve_word["or"] = 6;
		reserve_word["between"] = 7;
		reserve_word["count"] = 8;
		reserve_word["sum"] = 9;
		reserve_word["avg"] = 10;

		reserve_word["create"] = 11;
		reserve_word["insert"] = 12;
		reserve_word["primaryKey"] = 13;
		reserve_word["foreignKey"] = 14;
		reserve_word["unique"] = 15;
		reserve_word["table"] = 16;
		reserve_word["notNull"] = 17;
		reserve_word["references"] = 18;

		

		reserve_word["int"] = 4;
		reserve_word["tinyint"] = 1;
		reserve_word["smallint"] = 2;
		reserve_word["float"] = 4;
		reserve_word["double"] = 8;
		reserve_word["char"] = 1;
		reserve_word["varchar"] = 106;

	}
	bool is_reserved(string in)
	{
		
		if (reserve_word.count(in) > 0)
			return true;
		else
			return false;
	}
	int getReservedValue(string in)
	{
		if (is_reserved(in))
			return reserve_word[in];
		else
			return -1;
	}

}reserve_words;


//--------------------------------------------------------LEXER-----------------------------------------------------------------------//

class token
{
public:
	string tokenName;
	int tokenClass;
	token(string tokenName, int Class) { this->tokenName = tokenName; this->tokenClass = Class; }
	token(){}
	
};

enum token_s
{
	t_Reserve, t_String, t_Number, t_Character, t_Word,t_End
};
class lexer
{
private:
	list<token> tokens;
	int index;


	int getspace(const string &str, int i)
	{
		//cout << "space!";
		if (i >= str.length())
			return i;

		if (str[i] == ' ')
		{
			i++;
			//return i;
		}
		

		return i;
	}

	int getword(const string &str, int i)
	{
		//cout << "word!";
		if (i >= str.length())
			return i;

		int down = i;
		if (str[i] >= 'a'&&str[i] <= 'z' || str[i] >= 'A'&&str[i] <= 'Z'|| str[i]=='_') {

			while ((str[i] >= 'a'&&str[i] <= 'z' || str[i] >= 'A'&&str[i] <= 'Z' || str[i] == '_' || str[i] >= '0'&&str[i] <= '9') && i < str.length())
				i++;
			token tok(str.substr(down, i - down), t_Word);
			if (reserve_words.is_reserved(tok.tokenName))
				tok.tokenClass = t_Reserve;
			this->tokens.push_back(tok);
			return i;
		}
		else
			return i;

	}
	int getnumber(const string &str, int i)
	{
		//cout << "number!";
		if (i >= str.length())
			return i;
		int down = i;
		if (str[i] >= '0'&&str[i] <= '9') {
			while (str[i] >= '0'&&str[i] <= '9' && i < str.length())
				i++;
			token tok(str.substr(down, i - down), t_Number);
			this->tokens.push_back(tok);
			return i;
		}
		else
			return i;
	}
	int getstr(const string &str, int i)
	{
		//cout << "str!";
		if (i >= str.length())
			return i;
		int down = i;

		if (str[i] != '\'')
			return i;
		i++;
		while (str[i] != '\'' && i < str.length())
		{
			i++;
		}
		i++;
		token tok(str.substr(down, i - down), t_String);
		this->tokens.push_back(tok);
		return i;
	}

	int getother(const string &str, int i)
	{
		//cout << "other!";
		if (i >= str.length())
			return i;
		int down = i;
		if (str[i] == '*' || str[i] == '(' || str[i] == ')' || str[i] == ';' || str[i] == '='|| str[i] == '+' || str[i] == '-'|| str[i] == ',' || str[i] == '%' || str[i] == '@' || str[i] == '!' || str[i] == '^' || str[i] == '&' || str[i] == '\\' || str[i] == '/' || str[i] == '<' || str[i] == '>' || str[i] == ']' || str[i] == '[' || str[i] == '|' || str[i] == '?' || str[i] == '.' || str[i] == '~' || str[i] == '`' || str[i] == '#'|| str[i] == '$' || str[i] == '"')
		{
			token tok(str.substr(down, i - down + 1), t_Character);
			this->tokens.push_back(tok);
			i++;
			return i;
		}
		else {
			
			return i;
		}


	}
public:
	lexer()
	{
		index = 0;
	}
	lexer(string text)
	{
		getTokensFromStr(text);
		index = 0;

	}
	int getCurrentIndex()
	{
		return this->index;
	}

	int getTokensNum()
	{
		return tokens.size() - 1;
	}
	void getTokensFromStr(const string &str)
	{

		int state = 0;
		int up = 0;
		for (int i = 0; i < str.length(); )
		{
			if (i >= str.length())
				break;
			i = getspace(str, i); 
			i = getword(str, i); 
			i = getnumber(str, i);
			i = getstr(str, i); 
			i = getother(str, i); 
		}
		token tok;
		tok.tokenName = "";
		tok.tokenClass = t_End;
		this->tokens.push_back(tok);
	}
	token lex()
	{
		list<token>::iterator iter = tokens.begin();
		for (int ix = 0; ix < index; ++ix) ++iter;
		if (index < getTokensNum())
			index++;
		
		return *iter;
	}
	void indexBack(int num)
	{
		if (index - num >= 0)
			this->index -= num;
		else
			index = 0;
	}
	token peekCurrentToken()
	{
		list<token>::iterator iter = tokens.begin();

		int indexhead = index;
		if (indexhead >= getTokensNum())
			indexhead = getTokensNum() - 1;

		for (int ix = 0; ix < indexhead; ++ix) ++iter;

		return *iter;
	}
	token peekahead(int n)
	{

	
		list<token>::iterator iter = tokens.begin();

		int indexhead = index + n - 1;
		if (indexhead >= getTokensNum())
			indexhead = getTokensNum() - 1;

		for (int ix = 0; ix < indexhead; ++ix) ++iter;

		return *iter;
	}
	void printAll()
	{
		list<token>::iterator iter;

		for (iter = tokens.begin(); iter != tokens.end(); iter++)

		{
			//std::cout << iter->tokenName << "$"  << iter->tokenClass << std::endl;
			std::cout << iter->tokenName << std::endl;
			
		}
	}
	void printStrWithErrorClue(int index)
	{
		list<token>::iterator iter;

		int errorBlacklength = 0;
		int in = 0;
		for (iter = tokens.begin(); iter != tokens.end(); iter++,in++)
		{
			if (in < index) {
				errorBlacklength += (iter->tokenName.length());
				errorBlacklength += 1;
			}
			std::cout << iter->tokenName << " ";
		}
		cout << endl;

		for (int i = 0; i < errorBlacklength-1; i++)
			cout << " ";
		cout << "^" << endl;
		
	}
};

//--------------------------------------------------------PARSER-----------------------------------------------------------------------//
//translationUnit->select_s|delete_s|insert_s|update_s|create_s
//selects->$select Rows $from ID;|$select Rows $from ID $where expression;
//delete_s->$delete $from ID $where expression;
//Rows->IDlist|count(id)|sum(id)|*
//IDlist->IDlist,id|id
//expr->exprOr
//exprOr->exprOr or exprAnd|exprAnd
//exprAnd->exprAnd and exprRow|exprRow
//exprRow -> Row = Row|Row < Row|Row > Row | Row <> Row
//Row->string|number

class result
{
private:

public:
	string resultString;
	int code;
	result() { resultString = ""; code = 0; }
};


class parser
{
private:
	lexer *lex;
public:
	parser(string in)
	{
		lex = new lexer(in);
	}
	~parser()
	{
		free(lex);
	}

	void clueofError(int index)
	{
		cout << "语法错误！" << endl;
		lex->printStrWithErrorClue(index);
		//exit(0);
		return ;
	}


	token consumeToken()
	{
		token tok = lex->lex();
		//cout << "consume :" << tok.tokenName << endl;
			
		return tok;
	}
	int consumeToken(string t)
	{
		token tok = lex->lex();
		//cout << "consume :" << tok.tokenName << endl;
		if (t.compare(tok.tokenName) != 0)
		{
			//cout << "语法错误！" << tok.tokenName << "应该是:" << t<< endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		return 1;
	}
	
//---------------------------------------------------------------select_parser----------------------------------------//
	

	int parseTableIdList2(string &IdResults)
	{
		token tok = lex->peekahead(1);
		if (tok.tokenName.compare(",") == 0) {
			int re = consumeToken(",");
			if (re == -1)
				return re;


			tok = consumeToken();
			IdResults = IdResults + " " + tok.tokenName;

			if (tok.tokenClass != t_Word)
			{
				cout << "此处应该是单词！" << endl;
				clueofError(lex->getCurrentIndex() - 1);
				return -1;
			}
			re = parseTableIdList2(IdResults);
			if (re == -1)
				return re;
		}
		else if (tok.tokenName.compare(";") == 0|| tok.tokenName.compare("where") == 0)
		{
			return 0;
		}
		else {
			
			clueofError(lex->getCurrentIndex() - 1);
			return -1;
		}


		return 0;

	}
	int parseTableIdList(string &IdResults)
	{
		
		token tok = consumeToken();
		IdResults += (tok.tokenName);
		int re = parseTableIdList2(IdResults);
		if (re == -1)
			return re;

		return 0;

	}



	//PSRL->selectRowList|*
	//selectRowList->selectRowList,selectRow|selectRow
	//selectRow->word|$function(simpleRow)|word.word
	//funcParameter->word|*
	//function->sum|avg|count

	int parseFuncParameter(string &paramaterRe)
	{
		token peek = lex->peekahead(2);
		if (peek.tokenName.compare(".") == 0) {
			int re = parseExprRowRow(paramaterRe);
			if (re == -1)
				return re;
		}
		else 
		{
			token tok = consumeToken();
			if (tok.tokenClass == t_Word)
			{
				paramaterRe += tok.tokenName;
			}
			else if (tok.tokenName.compare("*") == 0)
			{
				paramaterRe += tok.tokenName;
			}
			else
			{
				cout << "语法错误！" << "函数参数输入不正确！" << endl;
				clueofError(lex->getCurrentIndex());
				return -1;
			}
		}
		
		return 0;
	}
	int parseFunction(string &Re)
	{

		string functionP;
		token functionName = consumeToken();
		int re = consumeToken("(");
		if (re == -1)
		{
			//clueofError(lex->getCurrentIndex());
			return -1;
		}
		re = parseFuncParameter(functionP);
		if (re == -1)
		{
			//clueofError(lex->getCurrentIndex());
			return -1;
		}
		re = consumeToken(")");
		if (re == -1)
		{
			//clueofError(lex->getCurrentIndex());
			return -1;
		}
		Re = "[Compute " + functionName.tokenName+" "+functionP+"]";

		return 0;
	}

	int parseSelectRow(string &sRe)
	{
		string RowResult;
		token peek = lex->peekahead(1);
		if (peek.tokenClass == t_Word)
		{
			token peek = lex->peekahead(2);
			if (peek.tokenName.compare(".") == 0) {
				int re = parseExprRowRow(RowResult);
				RowResult = "[Normal " + RowResult + "]";
				if (re == -1)
					return -1;
	

			}
			else {
				token tok = consumeToken();
				RowResult = "[Normal " + tok.tokenName + "]";
			}


		}
		else if (peek.tokenName.compare("sum") == 0 || peek.tokenName.compare("avg") == 0|| peek.tokenName.compare("count") == 0)
		{
			int re = parseFunction(RowResult);
			if (re == -1)
			{
				//clueofError(lex->getCurrentIndex());
				return -1;
			}
		}
		else
		{
			cout << "语法错误！" << "此处应该输入列名或函数调用！" << endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		sRe = RowResult;
		return 0;

	}

	int parseSelectRowList2(string &ResultAll)
	{
		string Re;
		token peek = lex->peekahead(1);
		if (peek.tokenName.compare(",") == 0)
		{
			int re = consumeToken(",");
			if (re == -1)
				return re;

			re = parseSelectRow(Re);
			if (re == -1)
				return re;
			ResultAll += Re;


			re = parseSelectRowList2(ResultAll);
			if (re == -1)
				return re;
		}
		else if(peek.tokenName.compare("from") == 0)
		{
			return 0;
		}
		else
		{
			clueofError(lex->getCurrentIndex() - 1);
			return -1;
		}
		return 0;
		
	}
	int parseSelectRowList(string &selectRowRe)
	{
		string ResultAll, Re;
		int re = parseSelectRow(Re);
		if (re == -1)
			return -1;
		ResultAll += Re;
		re = parseSelectRowList2(ResultAll);
		if (re == -1)
			return re;
		selectRowRe = ResultAll;
		return 0;
	}



	int parseRow(string &RowRe)
	{
		string RowResult;
		token tok = lex->peekahead(1);
		if (tok.tokenClass == t_Word|| tok.tokenClass == t_Reserve) {
			int re = parseSelectRowList(RowResult);
			if (re == -1)
				return re;
		}
		else if (tok.tokenClass == t_Character) {
			int re = consumeToken("*");
			if (re == -1)
				return re;
			RowResult += "[normal *]";
		}
		else
		{
			
			
		}
		RowRe = RowResult;
		return 0;
		
	}

	int parseSelect()
	{
		//lex->printAll();
		int re = consumeToken("select");
		if (re == -1)
			return -1;

		string RowRe;
		re = parseRow(RowRe);
		if (re == -1)
			return -1;
		
		re = consumeToken("from");
		if (re == -1)
			return re;

		string tablelist;
		token table_id = lex->peekahead(1);
		if (table_id.tokenClass != t_Word) {
				cout << "语法错误！应该后面跟表名\n" <<endl;
				clueofError(lex->getCurrentIndex());
				return -1;
			}
		else
		{
			int re = parseTableIdList(tablelist);
			if (re == -1)
				return re;
		}
			
		string whereRe;
		token tok = lex->peekahead(1);
		if (tok.tokenName.compare("where") == 0) {
			int re = parseWhereExpression(whereRe);
			if (re == -1)
				return re;
			re = consumeToken(";");//这里可以发现有多个consume ';'的情况。实际上可以把;放到外面，这样consume一次就行了。这个就是看文法怎么设计。
			if (re == -1)
				return re;
		}
		else if (tok.tokenName.compare(";") == 0) {
			int re = consumeToken(";");
			if (re == -1)
				return re;
		}
		else {
			cout << "语法错误！缺少;或者继续写where子句"<<endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}

		vector<string> semaResult;

		semaResult.push_back("GetTable '"+tablelist+"'");
		semaResult.push_back("GetRow '" + RowRe + "'");
		semaResult.push_back("Condition '" + whereRe + "'");

		cout << endl;
		for (int i = 0; i < semaResult.size(); i++)
		{
			cout << semaResult[i] << endl;
		}
		cout << endl;
		return 0;
	}

	int parseExprRowRow(string &RowRow)
	{
		token tokleft = consumeToken();
		if(tokleft.tokenClass!=t_Word)
		{
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		int re = consumeToken(".");
		if (re == -1)
			return re;

		token tokright = consumeToken();

		if (tokright.tokenClass != t_Word)
		{
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		RowRow = (tokleft.tokenName + "." + tokright.tokenName);
		return 0;
	}

	int parseExprRow(string &exprRowResult)
	{

		string tokenleft;
		string tokenright;
		string tokenop;

		token tokleft;
		token tokop;
		token tokright;

		token tokpeek;
		tokpeek = lex->peekahead(2);
		if (tokpeek.tokenName.compare(".") == 0)
		{
			int re = parseExprRowRow(tokenleft);
			if (re == -1)
				return re;
		}
		else {
			tokleft = consumeToken();
			tokenleft = tokleft.tokenName;
			if (tokleft.tokenClass != t_Word)
			{

				cout << "语法错误!" << tokleft.tokenName << "此处应该用单词写表名！" << endl;
				clueofError(lex->getCurrentIndex());
				return -1;
			}
		}
				
		tokop = consumeToken();			
		if(tokop.tokenName.compare("=") == 0)			
		{
			
		}			
		else if (tokop.tokenName.compare("<") == 0)		
		{				
			token tok = lex->peekahead(1);			
			if (tok.tokenClass == t_String) {		
				cout << "字符串不能参与大小比较！" << endl;		
				clueofError(lex->getCurrentIndex() + 1);	
				return -1;					
			}	
		}			
		else if (tokop.tokenName.compare(">") == 0)			
		{			
			token tok = lex->peekahead(1);			
			if (tok.tokenClass == t_String){			
				cout << "字符串不能参与大小比较！"<< endl;			
				clueofError(lex->getCurrentIndex() + 1);				
				return -1;		
			}	
		}			
		else if (tokop.tokenName.compare("!") == 0)		
		{
		
		}		
		else		
		{			
			if(tokop.tokenClass== t_Character)			
				cout << "语法错误!" << tokop.tokenName << "不支持的比较符！" << endl;			
			else			
				cout << "语法错误!" <<"需要在后面跟 > < = ! 这几个符号，而不是直接写新的单词！"<< endl;			
			clueofError(lex->getCurrentIndex());			
			return -1;
		
		}	
		tokpeek = lex->peekahead(2);		
		if (tokpeek.tokenName.compare(".") == 0)		
		{
				int re = parseExprRowRow(tokenright);	
				if (re == -1)
					return re;
		}			
		else {		
			tokright = consumeToken();			
			tokenright = tokright.tokenName;		
			if (tokright.tokenClass != t_Word && tokright.tokenClass != t_String && tokright.tokenClass != t_Number)	
			{	
				cout << "语法错误!" << tokright.tokenName << "应该是单词、字符串或者数字！" << endl;		
				clueofError(lex->getCurrentIndex());		
				return -1;			
			}
		}

		exprRowResult = "[compare " + tokop.tokenName + " " + tokenleft + " " +tokenright+"] ";
		return 0;
	}
	int parseExprAnd2(string &exprReAnd2)
	{
		int re = consumeToken("and");
		if (re == -1)
			return re;
		string exprRe;
		re = parseExprRow(exprRe);
		
		if (re == -1)
			return re;
		if (lex->peekahead(1).tokenName.compare("and") == 0) {

			int re = parseExprAnd2(exprReAnd2);
			if (re == -1)
				return -1;
		}
		else if (lex->peekahead(1).tokenName.compare(";") == 0|| lex->peekahead(1).tokenName.compare("or") == 0)
			;
		else {
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		exprReAnd2 = exprReAnd2 + "&" + exprRe;
		return 0;
		
	}
	int parseExprAnd(string &exprAndRe)
	{
		string Result;
		string exprRe;
		int re = parseExprRow(exprRe);
		
		if (re == -1)
			return re;
		token tok = lex->peekahead(1);
		if (tok.tokenName.compare("and") == 0) {
			re = parseExprAnd2(Result);
			if (re == -1)
				return re;
		}
		else if (tok.tokenName.compare(";") == 0 || lex->peekahead(1).tokenName.compare("or") == 0)
			;
		else {
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		Result = Result + exprRe;
		exprAndRe += Result;
		return 0;
			
	}
	int parseExprOr2(string &exprOr2Re)
	{
		string exprOrRe;
		int re = consumeToken("or");
		
		if (re == -1)
			return re;
		re = parseExprAnd(exprOrRe);

		
		if (re == -1)
			return re;
		if(lex->peekahead(1).tokenName.compare("or") == 0){
			int re = parseExprOr2(exprOr2Re);
			if (re == -1)
				return re;
		}
		else if (lex->peekahead(1).tokenName.compare(";") == 0)
			;
		else {
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		exprOr2Re = exprOr2Re + "|" + exprOrRe;
		return 0;
	}
	int parseExprOr(string &exprRe)
	{
		int re;
		string Result;
		string ExprOrRe;
		re = parseExprAnd(ExprOrRe);
		
		if (re == -1)
			return re;
		if (lex->peekahead(1).tokenName.compare("or") == 0) {
			int re = parseExprOr2(Result);
			if (re == -1)
				return re;
		}
		else if (lex->peekahead(1).tokenName.compare(";") == 0)
			;
		else {
			clueofError(lex->getCurrentIndex());
			return - 1;
		}

		Result += ExprOrRe;
		exprRe = Result;
		
		
		return 0;
	}

	int parseExpr(string &ExprRe)
	{
		int re = parseExprOr(ExprRe);
		
		return re;

	}

	int parseWhereExpression(string &whereRe)
	{

		int re = consumeToken("where");
		if (re == -1)
			return -1;

		return parseExpr(whereRe);
		
	
	}





//---------------------------------------------------------------create_parser----------------------------------------//
/*
//CREATE TABLE Orders
	(
		O_Id int NOT NULL,
		OrderNo int NOT NULL,
		P_Id int,
		PRIMARY KEY(O_Id),
		FOREIGN KEY(P_Id) REFERENCES Persons(P_Id)
		)
//

create_s->$create $table id(cParaList);
cParaList->cParaList,cPara
cPara->id specifier rowConstrain^opt|keyConstrain (id)[ references id(id)] 

*/
	int parseSpecifier()
	{
		token spec = consumeToken();
		if (spec.tokenClass != t_Reserve)
		{
			cout << "语法错误!" << "应输入类型名！" << endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		map<string,string> spe;
		spe["int"] = 4;
		spe["tinyint"] = 1;
		spe["smallint"] = 2;
		spe["float"] = 4;
		spe["double"] = 8;
		spe["char"] = 1;
		spe["varchar"] = -1;
		if (spe.find(spec.tokenName) == spe.end())
		{
			//cout << "语法错误!" << "应输入类型名！" << endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}

		if (spec.tokenName.compare("varchar") == 0)
		{
			int re = consumeToken("(");
			if(re == -1)
				return re;
		
			token length = consumeToken();
			if(length.tokenClass != t_Number)
			{
				cout << "语法错误!" << "应输入数字！" << endl;
				clueofError(lex->getCurrentIndex());
				return -1;
			}
			re = consumeToken(")");
			if (re == -1) 
				return re;
			
		}

		return 0;

	}
	

	int parseCPara()
	{
		token word = lex->peekahead(1);
		if (word.tokenClass == t_Word)
		{
			token id = consumeToken();

			int re = parseSpecifier();
			if (re == -1)
				return re;

			token cur = lex->peekCurrentToken();
			if (cur.tokenClass == t_Reserve)
			{
				if (cur.tokenName.compare("notNull") == 0 || cur.tokenName.compare("unique") == 0)
				{
					token RowConstrain = consumeToken();
				}
				else if(cur.tokenName.compare("primaryKey") == 0)
				{
					token keyConstrain = consumeToken();
				}
			}
			
		}
		else if(word.tokenClass == t_Reserve)
		{ 
			if (word.tokenName.compare("primaryKey") == 0)
			{
				consumeToken("primaryKey");
				int re;
				re = consumeToken("(");
				if (re == -1)
					return re;
				token id = consumeToken();
				if (id.tokenClass != t_Word)
				{
					clueofError(lex->getCurrentIndex());
					return -1;
				}

				re = consumeToken(")");
				if (re == -1)
					return re;

			}
			else if (word.tokenName.compare("foreignKey") == 0)
			{
				consumeToken("foreignKey");
				int re;
				re = consumeToken("(");
				if (re == -1)
					return re;
				token inRow = consumeToken();
				if (inRow.tokenClass != t_Word)
				{
					clueofError(lex->getCurrentIndex());
					return -1;
				}
				re = consumeToken(")");
				if (re == -1)
					return re;
				re = consumeToken("references");
				if (re == -1)
					return re;
				token outTable = consumeToken();
				if (outTable.tokenClass != t_Word)
				{
					clueofError(lex->getCurrentIndex());
					return -1;
				}
				re = consumeToken("(");
				if (re == -1)
					return re;
				token outRow = consumeToken();
				if (outRow.tokenClass != t_Word)
				{
					clueofError(lex->getCurrentIndex());
					return -1;
				}
				re = consumeToken(")");
				if (re == -1)
					return re;
			}
			else
			{
				cout << "语法错误!" << "应输入主键、外键约束关键字！" << endl;
				clueofError(lex->getCurrentIndex());
				return -1;
			}
		}
		else
		{
			cout << "语法错误!" << "应输入列名或者主键、外键约束关键字！" << endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}
		return 0;

	}

	int parseCParaList2()
	{
		token peek = lex->peekahead(1);
		if (peek.tokenName.compare(",") == 0)
		{
			int re;
			re = consumeToken(",");
			if (re == -1)
				return re;

			re = parseCPara();
			if (re == -1)
				return re;

			re = parseCParaList2();
			if (re == -1)
				return re;

		}
		else if (peek.tokenName.compare(")") == 0)
		{
			return 0;
		}
		else
		{
			
			cout << "语法错误!" << "要么继续加逗号写，要么就用）结束！" << endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}

	}

	int parseCParaList()
	{
		int re = parseCPara();
		if (re == -1)
			return re;

		re = parseCParaList2();
		if (re == -1)
			return re;
	
	}


	int parseCreate()
	{
		token create = consumeToken();
		token table = consumeToken();
		if (table.tokenClass == t_Reserve && table.tokenName.compare("table") == 0)
			;
		else
		{
			cout << "应输入table关键字！" << endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}

		token table_name = consumeToken();
		if (table_name.tokenClass != t_Word)
		{
			cout << "应输入表名！" << endl;
			clueofError(lex->getCurrentIndex());
			return -1;
		}

		int re = consumeToken("(");
		if (re == -1)
			return -1;


		re = parseCParaList();
		if (re == -1)
			return re;
		re = consumeToken(")");
		if (re == -1)
			return -1;
		re = consumeToken(";");
		if (re == -1)
			return -1;



		return 0;
	}








	int parse()
	{
		do 
		{

			token tok = lex->peekCurrentToken();
			///cout << "["<< tok.tokenName<<"]";
			
			if (tok.tokenName.compare("select") == 0) {
				int re = parseSelect();
				if (re == -1)
					return -1;
			}
			else if (tok.tokenName.compare("create") == 0) {
				int re = parseCreate();
				if (re == -1)
					return -1;
			}
			else {
				cout << "不支持的操作.[" << tok.tokenName << "]" << endl;
				
				return -1;
			}
			
			//cout << lex->getCurrentIndex() << "|" << lex->getTokensNum();
			
		} while (lex->getCurrentIndex() < lex->getTokensNum() );
		return 0;

	}

};


int main()
{
	//parser Parser("select name,age from ddd where 342424=3 or > eee or fff;");
	string input;
	
	//parser Parser("select erer,d2223,zx from rgeg where ddd_ggg or fff=32  and frf>5;");

	for (;;) {
		cout << "sql>";
		getline(cin,input);
		if (input.length() > 0) {
			parser Parser(input);
			if (Parser.parse() >= 0)cout << "语句正常执行！\n";
			//Parser.~parser();
		}
	}
	cout << "语句正常通过!\n";
	//lexer lex("select count(id) from ddd where id=('2342424')and4>43or5+33=43;");
	

	
    return 0;
}

