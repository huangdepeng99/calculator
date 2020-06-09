#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


#define MAX_TOKEN 20		//标记的最大长度(字符数)
#define MAX_EXPR 100		//后缀表达式的最大长度(标记数)
#define PI 3.14159265		//圆周率


enum TokenTypes {
	OPERAND,			//数值操作数
	ARITH_OPERATOR,		//算数运算符
	FUNCT_OPERATOR,		//函数运算符
	PAREN_LEFT,			//左圆括号
	PAREN_RIGHT,		//右圆括号
	INVALID				//无效标记
};

typedef enum Errors {
	OPEN_FILE = 1,		//文件打开错误
	CLI_SYNTAX,			//命令行语法错误
	INVALID_TOKEN,		//无效的标记
	TOO_LONG_TOKEN,		//标记(数值操作数)太长
	TOO_LONG_EXPR,		//(后缀)表达式太长
	PAREN_MISMATCH,		//圆括号不匹配
	MISSING_OPERAND,	//运算符缺少操作数
	FUNCT_CALL,			//函数调用错误
	DIV_ZERO,			//除数为零
	UNKNOWN_ERROR		//未知错误
} Error;


typedef struct {
	union {
		char s[MAX_TOKEN];
		double d;
	} value;			//标记的值：运算符为字符串，数值操作数则为浮点数
	int type;			//标记的类型
} Token;


/*
 * 函数isAriop判断一个字符是否是算数运算符，如果是，则返回1，否则，
 * 该函数将返回0；其中合法的算数运算符包括：+、-、*、/
 */
int isAriop(int c)
{
	return c == '+' || c == '-' || c == '*' || c == '/';
}

/*
 * 函数isFunct判断一个字符串是否是函数运算符，如果是，则返回1，否则，
 * 该函数将返回0；其中合法的函数运算符包括：sin、cos、sqrt
 */
int isFunct(const char *s)
{
	return strcmp(s, "sin") == 0 || strcmp(s, "cos") == 0 || strcmp(s, "sqrt") == 0;
}


/*
 * 函数getToken从fp指向的流中获取下一个标记(token)，并将取到的标记
 * 存放在大小为size的字符数组token中；当遇到EOF或发生错误时，该函
 * 返回EOF，当遇到换行符('\n')时，该函数返回换行符，否则该函数将返
 * 回标记的类型
 */
int getToken(FILE *fp, char *token, int size)
{
	int c;
	char *tp = token;		//保存token的初始值
	while((c = fgetc(fp)) == ' ' || c == '\t')		//跳过空格和制表符
		;

	if(c == EOF || c == '\n'){		//EOF或者换行符
		;
	}else if(c == '(' || c == ')'){		//圆括号
		*token++ = c;
		c =  (c == '(') ? PAREN_LEFT : PAREN_RIGHT;
	}else if(isdigit(c)){		//数值操作数
		*token++ = c;
		--size;
		int dn = 0;		//小数点计数器
		while(--size > 0)
		{
			if(!isdigit(c = fgetc(fp)) &&  c != '.'){
				ungetc(c, fp);
				break;
			}
			if(c == '.'){
				if(dn == 0){
					++dn;
				}else if(dn == 1){
					ungetc(c, fp);
					break;
				}
			}
			*token++ = c;
		}
		if(size == 0){
			if(isdigit(c = fgetc(fp))){
				c = EOF;		//数值操作数长度超过MAX_TOKEN - 1
			}else if(c == '.'){
				if(dn == 0){
					c = EOF;
				}else if(dn == 1){
					ungetc(c, fp);
					c = OPERAND;
				}
			}else{
				ungetc(c, fp);
				c = OPERAND;
			}
		}else{
			c = OPERAND;
		}
	}else if(isAriop(c)){		//算数运算符
		*token++ = c;
		c = ARITH_OPERATOR;
	}else if(isalpha(c)){		//函数运算符
		*token++ = c;
		--size;
		while(--size > 0)
		{
			if(!isalpha(c = fgetc(fp))){
				ungetc(c, fp);
				break;
			}
			*token++ = c;
		}
		*token = '\0';
		if(isFunct(tp)){
			c = FUNCT_OPERATOR;
		}else{
			c = INVALID;
		}
	}else{		//无效标记
		*token++ = c;
		c = INVALID;
	}

	*token = '\0';

	return c;
}


/*
 * 函数printErr根据参数err来向标准错误(stderr)中输出错误信息，其中
 * err的值应为枚举Errors中的值
 */
void printErr(Error err, char *argv[], const char *token)
{
	int exitCode;
	switch(err)
	{
		case OPEN_FILE:
			fprintf(stderr, "%s: can\'t open file %s\n", argv[0], argv[1]);
			exitCode = OPEN_FILE;
			break;
		case CLI_SYNTAX:
			fprintf(stderr, "%s: command line option syntax error\n", argv[0]);
			exitCode = CLI_SYNTAX;
			break;
		case INVALID_TOKEN:
			fprintf(stderr, "%s: invalid token %s\n", argv[0], token);
			exitCode = INVALID_TOKEN;
			break;
		case TOO_LONG_TOKEN:
			fprintf(stderr, "%s: the token %s is too long\n", argv[0], token);
			exitCode = TOO_LONG_TOKEN;
			break;
		case TOO_LONG_EXPR:
			fprintf(stderr, "%s: the mathematical expression is too long\n", argv[0]);
			exitCode = TOO_LONG_EXPR;
			break;
		case PAREN_MISMATCH:
			fprintf(stderr, "%s: mismatched parentheses\n", argv[0]);
			exitCode = PAREN_MISMATCH;
			break;
		case MISSING_OPERAND:
			fprintf(stderr, "%s: the operator %s is missing operands\n", argv[0], token);
			exitCode = MISSING_OPERAND;
			break;
		case FUNCT_CALL:
			fprintf(stderr, "%s: error calling function %s\n", argv[0], token);
			exitCode = FUNCT_CALL;
			break;
		case DIV_ZERO:
			fprintf(stderr, "%s: divide by zero\n", argv[0]);
			exitCode = DIV_ZERO;
			break;
		case UNKNOWN_ERROR:
			fprintf(stderr, "%s: an error occurred, but the error could not be precisely located\n", argv[0]);
			exitCode = UNKNOWN_ERROR;
			break;
		default:
			fprintf(stderr, "%s: invalid error type\n", argv[0]);
			exitCode = -1;
			break;
	}

	fprintf(stderr, "Usage: %s [<filename>]\n", argv[0]);
	exit(exitCode);
}


/*
 * 函数getPrec得到运算符的优先级代码，运算符的优先级代码都是正整数，数字越小优先级越高
 * 运算符的优先级代码如下：
 *  1 - 函数运算(sin、cos、sqrt)
 *  2 - 乘法(*)、除法(/)
 *  3 - 加法(+)、减法(-)
 */
int getPrec(const char *op)
{
	int precCode;
	if(strcmp(op, "sin") == 0 || strcmp(op, "cos") == 0 || strcmp(op, "sqrt") == 0){
		precCode = 1;
	}else if(strcmp(op, "*") == 0 || strcmp(op, "/") == 0){
		precCode = 2;
	}else if(strcmp(op, "+") == 0 || strcmp(op, "-") == 0){
		precCode = 3;
	}

	return precCode;
}


/*
 * 函数opCmp按照优先级对运算符(包括算数运算符和函数运算符)进行比较，若运算符op1的优先级高于
 * 运算符op2，则返回一个正整数；若op1低于op2，则返回一个负整数；若op1与op2相同，则返回0。
 * 运算符的优先级如下：
 *  函数运算(sin、cos、sqrt)
 *  乘法(*)、除法(/)
 *  加法(+)、减法(-)
 */
int opCmp(const char *op1, const char *op2)
{
	return getPrec(op2) - getPrec(op1);
}


/*
 * 函数getExpr从fp指向的流中获取下一个数学表达式(一行)，并将取到的数学表达
 * 式(中缀表达式)转换为后缀表达式，存放在大小为size的标记数组posExp中；该函
 * 数遇到任何错误都将导致程序终止，并输出错误信息；在没有发生错误时，该函数
 * 若遇到EOF，将返回EOF，否则该函数将返回后缀表达式的长度(标记数)
 */
int getExpr(FILE *fp, Token *posExp, int size, char *argv[])
{
	char token[MAX_TOKEN];		//标记(token)
	int tokenType;				//标记的类型

	int preTokenType = INVALID;	//前一个标记的类型
	int parenInd = 0;			//括号指示器：每遇到一个左圆括号，加1；每遇到一个右圆括号，减1

	int i = 0;					//数组posExp的索引

	char oprtStack[MAX_EXPR][MAX_TOKEN];		//运算符栈(包括圆括号)
	int j = 0;					//栈的索引


	while(i < size && (tokenType = getToken(fp, token, MAX_TOKEN)) != EOF && tokenType != '\n')
	{
		if(tokenType == OPERAND){	//数值操作数
operand:	posExp[i].value.d = atof(token);
			posExp[i++].type = preTokenType = tokenType;
		}else if(tokenType == ARITH_OPERATOR){	//算数运算符
			if(strcmp(token, "+") == 0 || strcmp(token, "-") == 0){
				if(preTokenType == INVALID || preTokenType == PAREN_LEFT){		//单目运算符(正号/负号)
					//将单目运算符转换成双目运算符：在正号/负号前面加上0.0
					posExp[i].value.d = 0.0;
					posExp[i++].type = preTokenType = OPERAND;
					goto arith_operator;
				}else if(preTokenType == PAREN_RIGHT || preTokenType == OPERAND){		//双目运算符(加号/减号)
					goto arith_operator;
				}else{		//错误
					printErr(MISSING_OPERAND, argv, token);
				}
			}else{
arith_operator:	if(preTokenType != PAREN_RIGHT && preTokenType != OPERAND){
					printErr(MISSING_OPERAND, argv, token);
				}
				for(--j; i < size && j >= 0 && strcmp(oprtStack[j], "(") != 0 && opCmp(token, oprtStack[j]) <= 0; ++i, --j)
				{
					strcpy(posExp[i].value.s, oprtStack[j]);
					posExp[i].type = isFunct(oprtStack[j]) ? FUNCT_OPERATOR : ARITH_OPERATOR;
				}

				if(i >= size){
					printErr(TOO_LONG_EXPR, argv, NULL);
				}
				if(j < 0 || strcmp(oprtStack[j], "(") == 0 || opCmp(token, oprtStack[j]) > 0){
					strcpy(oprtStack[++j], token);
				}
				++j;
				preTokenType = tokenType;

				tokenType = getToken(fp, token, MAX_TOKEN);
				if(tokenType != PAREN_LEFT && tokenType != OPERAND && tokenType != FUNCT_OPERATOR){
					printErr(MISSING_OPERAND, argv, oprtStack[j - 1]);
				}else{
					if(tokenType == PAREN_LEFT){
						goto paren_left;
					}else if(tokenType == OPERAND){
						goto operand;
					}else if(tokenType == FUNCT_OPERATOR){
						goto funct_operator;
					}
				}
			}

		}else if(tokenType == FUNCT_OPERATOR){		//函数运算符
funct_operator:
			for(--j; i < size && j >= 0 && strcmp(oprtStack[j], "(") != 0 && opCmp(token, oprtStack[j]) <= 0; ++i, --j)
			{
				strcpy(posExp[i].value.s, oprtStack[j]);
				posExp[i].type = isFunct(oprtStack[j]) ? FUNCT_OPERATOR : ARITH_OPERATOR;
			}

			if(i >= size){
				printErr(TOO_LONG_EXPR, argv, NULL);
			}
			if(j < 0 || strcmp(oprtStack[j], "(") == 0 || opCmp(token, oprtStack[j]) > 0){
				strcpy(oprtStack[++j], token);
			}
			++j;
			preTokenType = tokenType;

			tokenType = getToken(fp, token, MAX_TOKEN);
			if(tokenType != PAREN_LEFT){
				printErr(FUNCT_CALL, argv, oprtStack[j - 1]);
			}else{
				goto paren_left;
			}

		}else if(tokenType == PAREN_LEFT){	//左圆括号
paren_left:	++parenInd;
			strcpy(oprtStack[j++], token);
			preTokenType = tokenType;
		}else if(tokenType == PAREN_RIGHT){	//右圆括号
			if(parenInd <= 0){
				printErr(PAREN_MISMATCH, argv, NULL);
			}
			--parenInd;
			for(--j; i < size && strcmp(oprtStack[j], "(") != 0; ++i, --j)
			{
				strcpy(posExp[i].value.s, oprtStack[j]);
				posExp[i].type = isFunct(oprtStack[j]) ? FUNCT_OPERATOR : ARITH_OPERATOR;
			}
			if(i >= size){
				printErr(TOO_LONG_EXPR, argv, NULL);
			}
			preTokenType = tokenType;
		}else if(tokenType == INVALID){	//无效标记
			printErr(INVALID_TOKEN, argv, token);
		}
	}		//while结束


	if(i >= size){			//(后缀)表达式太长
		printErr(TOO_LONG_EXPR, argv, NULL);
	}
	if(tokenType == EOF){
		if(strlen(token) > 0){			//标记(数值操作数)太长
			printErr(TOO_LONG_TOKEN, argv, token);
		}else{
			return EOF;
		}
	}

	if(parenInd){
		printErr(PAREN_MISMATCH, argv, NULL);
	}
	for(--j; i < size && j >= 0; ++i, --j)
	{
		strcpy(posExp[i].value.s, oprtStack[j]);
		posExp[i].type = isFunct(oprtStack[j]) ? FUNCT_OPERATOR : ARITH_OPERATOR;
	}
	if(i >= size){
		printErr(TOO_LONG_EXPR, argv, NULL);
	}


	return i; 		//返回标记数组posExp中的标记个数
}


/*
 * 函数calcExpr对长度(标记数)为len的后缀表达式posExp进行计算；该函数遇到
 * 任何错误都将导致程序终止，并输出错误信息；在没有发生错误时，该函数将返
 * 回最终的计算结果
 */
double calcExpr(Token *posExp, int len, char *argv[])
{
	int i;							//数组posExp的索引

	double numStack[MAX_EXPR];		//数值操作数栈
	int j = 0;						//栈的索引


	for(i = 0; i < len; ++i)
	{
		if(posExp[i].type == OPERAND){
			numStack[j++] = posExp[i].value.d;
		}else if(posExp[i].type == ARITH_OPERATOR){
			if(j < 2){
				printErr(UNKNOWN_ERROR, argv, NULL);
			}
			double d1 = numStack[--j];
			double d2 = numStack[--j];

			if(strcmp(posExp[i].value.s, "+") == 0){
				numStack[j++] = d2 + d1;
			}else if(strcmp(posExp[i].value.s, "-") == 0){
				numStack[j++] = d2 - d1;
			}else if(strcmp(posExp[i].value.s, "*") == 0){
				numStack[j++] = d2 * d1;
			}else if(strcmp(posExp[i].value.s, "/") == 0){
				if(d1 == 0.0){
					printErr(DIV_ZERO, argv, NULL);
				}
				numStack[j++] = d2 / d1;
			}

		}else if(posExp[i].type == FUNCT_OPERATOR){
			if(j < 1){
				printErr(UNKNOWN_ERROR, argv, NULL);
			}
			double d = numStack[--j];

			if(strcmp(posExp[i].value.s, "sin") == 0){
				numStack[j++] = sin(d * PI / 180);
			}else if(strcmp(posExp[i].value.s, "cos") == 0){
				numStack[j++] = cos(d * PI / 180);
			}else if(strcmp(posExp[i].value.s, "sqrt") == 0){
				numStack[j++] = sqrt(d);
			}
		}
	}

	if(j != 1){
		printErr(UNKNOWN_ERROR, argv, NULL);
	}


	return numStack[--j];		//返回计算结果
}


/*
 * 文本流中可以存在多行，其中每一行代表一个数学表达式，若
 * 行数大于1，则对每一行分别进行计算，并将结果分行输出
 */
int main(int argc, char *argv[])
{
	Token posExp[MAX_EXPR];			//后缀表达式
	int len;						//后缀表达式的长度(标记数)

	double result;					//数学表达式的计算结果


	//第1步：读取用户输入的数学表达式
	if(argc == 1 || argc == 2){
		FILE *fp;
		if(argc == 1){		//情况1：从标准输入中读取表达式
			printf("Please enter mathematical expressions: \n");
			fp = stdin;
		}else if(argc == 2){		//情况2：从文件中读取表达式
			fp = fopen(argv[1], "r");
			if(fp == NULL){
				printErr(OPEN_FILE, argv, NULL);
			}
		}

		while((len = getExpr(fp, posExp, MAX_EXPR, argv)) != EOF)		//第2步：将中缀表达式转换成后缀表达式
		{
			//第3步：对后缀表达式进行计算并立即输出结果
			result = calcExpr(posExp, len, argv);
			printf("%f\n", result);
		}
		
		fclose(fp);

	}else{		//情况3：命令行语法错误
		printErr(CLI_SYNTAX, argv, NULL);
	}


	return 0;
}

