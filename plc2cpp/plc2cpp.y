/*
 *  This file is part of Software PLC for RT-Linux.
 *
 *  Software PLC for RT-Linux is free software; you can redistribute
 *  it and/or modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Software PLC for RT-Linux is distributed in the hope that it
 *  will be useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Software PLC for RT-Linux; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Author: Vladimir Kloz <Vladimir.Kloz@dtg.cz>
 *  Project home: http://sourceforge.net/projects/softwareplc
 */

%{

	#include <stdio.h>
	#include <string>
	#include <vector>
	#include <iostream>
	#include <stdlib.h>
	#include <getopt.h>

	#include "ccodeprocessor.h"

	#include "plc2cpp_globals.h"

	using namespace std;

extern "C"
{
	int yyerror(const char *);
	int yywrap(void);
	int yyparse(void);

	static struct option options[] = {
		{"target", 1, NULL, 't'},
		{"source", 1, NULL, 's'},
		{"project", 1, NULL, 'p'},
		{"help", 0, NULL, 'h'},
		{"redget", 1, NULL, 'r'},
		{"notaskt", 1, NULL, 'n'},
		{"file-list", 1, NULL, 'l'},
		{NULL, 0, NULL, 0}
	};
}

/*
 * Source code lines counter
 */
int	iLineCount = 1;

/*
 * Source code file name
 */
string sActualFile;

string sFileList;

vector<string> vsGeneratedFiles;
/**
 * Contains vector with list of files, that
 * must be proceed (contain depending files)
 */
vector<string> vsParseFiles;

ProgramBlocks	CurrentCodeBlockType;

#ifndef PLC_SINGLE_PERIOD_DEFAULT
	#define PLC_SINGLE_PERIOD_DEFAULT	1000000
#endif

long long	llSinglePeriod = PLC_SINGLE_PERIOD_DEFAULT;

#ifndef PLC_NO_TASK_PERIOD_DEFAULT
	#define PLC_NO_TASK_PERIOD_DEFAULT	1000000
#endif

long long	llNoTaskPeriod = PLC_NO_TASK_PERIOD_DEFAULT;
/*
 * Name of target directory - converted sources
 * are stored here with headers in include subdirectory
 */
char	*pTargetDirectory = NULL;

/*
 * Name of PLC project
 */
string	sProjectName;

int iExitCode = 0;

int yylex(void);

int yyerror(const char *str)
{
	fprintf(stderr, "error: Line:%d - %s\n", iLineCount, str);
	return 0;
}

int yywrap(void)
{
	return 1;
}

extern FILE *yyin;

void help(void)
{
	cout << "plc2cpp 1.01\n\n";
	cout << "  --target [-t] directory  Target directory for converted PLC sources\n";
	cout << "  --source [-s] file       Name of source file in PLC IL language\n";
	cout << "  --project [-p] file      Name of project file\n";
	cout << "  --redget [-r] time       Period time in [ns] for detection of reising\n                           edge in single running tasks(default=" << PLC_SINGLE_PERIOD_DEFAULT << "ns)\n";
	cout << "  --notaskt [-n] time      Default period in [ns] for programm  with no task\n                           associated(default=" << PLC_NO_TASK_PERIOD_DEFAULT << "ns)\n";
	cout << "  --libdir [-L] dirs       Comma separated list of library directories\n";
	cout << "  --file-list  [-l] file   File with list of generated files\n";
	cout << "  --help [-h]              This help\n";
}

int main(int argc, char *argv[])
{
	int c;
	/*
	 * Vector of path's, where are searched sources
	 * from libraries. Is filled from command-line
	 * argument -l and from environment variable
	 * PLC_LIBDIR
	 */
	vector<string>	vsLibraryPath;

/* defines directory with system wide libraries */
#ifndef PLC_LIBDIRS
	ParsePathString(vsLibraryPath, PLC_LIBDIRS, ",");
#endif

	while(1)
	{
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;

		c = getopt_long(argc, argv, "ht:s:p:r:L:l:", options, &option_index);
		
		if (c == -1)
			break;

		switch(c)
		{
			case 'h':
				help();
				return -1;
				break;
			case 's':
				vsParseFiles.push_back(optarg);
				break;
			case 'p':
				sProjectName = optarg;
				break;
			case 't':
				pTargetDirectory = strdup(optarg);
				break;
			case 'r':
				llSinglePeriod = atoll(optarg);
				break;
			case 'n':
				llNoTaskPeriod = atoll(optarg);
				break;
			case 'L':
				ParsePathString(vsLibraryPath, optarg, ",");
				break;
			case 'l':
				sFileList = optarg;
				break;
			default:
				break;
		}
	}

	if (getenv("PLC_LIBDIR") != NULL)
		ParsePathString(vsLibraryPath, getenv("PLC_LIBDIR"), ":");

	// if target directory wasn't specified, use
	// default value
	if (!pTargetDirectory)
		pTargetDirectory = strdup("sources");
	
	for (int i = 0; i < vsParseFiles.size(); i++)
	{
		sActualFile = vsParseFiles[i];
		
		if (sActualFile.length() > 0)
		{
			// find source position. if not in current directory,
			// search library directories
			GetRealSourcePath(sActualFile, vsLibraryPath);

			if (!(yyin = fopen(sActualFile.c_str(), "r")))
			{
				cerr << "Can't open source file: " << sActualFile.c_str() << "\n";
				return 1;
			}
		}
		yyparse();

		fclose(yyin);
	}
	
	GenerateCode();

	if (sFileList.length() > 0)
		WriteListOfGeneratedFiles(sFileList);

	return iExitCode;
}

%}


%token	TOK_FB TOK_FB_END 
%token	TOK_VAR_INPUT TOK_VAR_OUTPUT TOK_VAR_INPUT_OUTPUT TOK_END_VAR
%token	TOK_VAR_GLOBAL TOK_VAR TOK_VAR_EXTERNAL
%token	TOK_FUNCTION TOK_FUNCTION_END
%token	TOK_PROGRAM TOK_PROGRAM_END
%token	TOK_CONFIGURATION TOK_CONFIGURATION_END
%token	TOK_RESOURCE TOK_RESOURCE_END TOK_ON TOK_WITH
%token	TOK_TASK TOK_PRIORITY TOK_SINGLE TOK_INTERVAL
%token	VAL_TRUE VAL_FALSE
%token	TOK_BOOL
%token	TOK_TIME TOK_DATE
%token	TOK_TOD TOK_DT TOK_STRING 
%token	TOK_AT TOK_COLON TOK_SEMICOLON TOK_OF TOK_SETVAL TOK_LPARENT TOK_RPARENT
%token	TOK_COMMA TOK_INSTRUCTION_EOLN
%token	TOK_CONSTANT
%token	IL_LD IL_ST IL_S IL_R IL_AND IL_OR IL_XOR IL_ADD IL_SUB IL_MUL IL_DIV
%token	IL_GT IL_GE IL_EQ IL_NE IL_LE IL_LT IL_JMP IL_CAL IL_RET
%token	IL_JMPC IL_JMPCN IL_CALC IL_CALCN IL_RETC IL_RETCN
%token	IL_LD_NEG IL_ST_NEG IL_AND_NEG IL_OR_NEG IL_XOR_NEG
%token	TOK_INCLUDE_BEGIN TOK_INCLUDE_END

%union
{
	int	number;
	char	*string;
	double	number_r;
}

	/* Identifier token is string */
%token <string>		NAME
%token <string>		LABEL_NAME
%token <string>		TOK_FB_DATATYPE
%token <string>		TOK_FUNCTION_DATATYPE
	/* Variable location token is string */
%token <string>		VAR_LOCATION
	/* Operand of type function block parameter */
%token <string>		FB_PARM

%token <string>		TOK_NUM_DATATYPE
%token <number>		VAL_INTEGER
%token <number_r>	VAL_REAL

	/* Parts of time constant */
%token <string>		VAL_TIME
	/* File name token is string */
%token <string>		FILE_NAME
%left ','

%%
blocks:
	| 
	Function blocks
	| 
	FunctionBlock blocks
	| 
	Program blocks
	|
	Configuration blocks
	|
	Include_files blocks
	;

Include_files:
	TOK_INCLUDE_BEGIN File_list TOK_INCLUDE_END
	;

File_list:
	FILE_NAME
	{
		AddDependFile($1);
	}
	|
	FILE_NAME TOK_COMMA File_list
	{
		AddDependFile($1);
	}
	;
	/* ***************************************** */
	/* Rule for numeric variables initialization */
	/* ***************************************** */
Numbers:
	Real
	|
	Integer
	;
	/* Whole numeric initialization (hex, octal, binary, integer) */
Integer:
	VAL_INTEGER
	{
		NewUseContainerVariable();
		pContainerVariable->SetInitialValue($1);
	}
	;
	/* Decimal numeric initialization (with or without exponent) */
Real:
	VAL_REAL
	{
		NewUseContainerVariable();
		pContainerVariable->SetInitialValue($1);
	}
	;

	/* **************************************** */
	/* Rule for boolean variable initialization */
	/* **************************************** */
Boolean:
	VAL_TRUE
	{
		NewUseContainerVariable();
		pContainerVariable->SetInitialValue(1);
	}
	|
	VAL_FALSE
	{
		NewUseContainerVariable();
		pContainerVariable->SetInitialValue(0);
	}
	;

	/* ******************************* */
	/* Configuration block declaration */
	/* ******************************* */
Configuration:
	TOK_CONFIGURATION
	{
		CurrentCodeBlockType = EConfiguration;
	}
	NAME 
	var_global Resource
	TOK_CONFIGURATION_END
	{
		FinishUseContainerBlock(EConfiguration, $3);
	}
	;
	/* *************************** */
	/* Function source declaration */
	/* *************************** */
Function:
	TOK_FUNCTION
	{
		CurrentCodeBlockType = EFunction;
	}
	NAME TOK_COLON 
	{
		NewUseContainerBlock(CurrentCodeBlockType, $3);
	}
	TOK_FUNCTION_DATATYPE
	FunctionVars
	code
	TOK_FUNCTION_END
	{
		pFunction->SetReturnDataType($6);
		FinishUseContainerBlock(EFunction, $3);
	}
	;

	/* ************************************** */
	/* Allowed variables blocks for function. */
	/* This rules allows random order of      */
	/* blocks in function declaration         */
	/* ************************************** */
FunctionVars:
	|
	var_input FunctionVars
	|
	var_external FunctionVars
	|
	var FunctionVars
	;

	/* ********************************* */
	/* Function block source declaration */
	/* ********************************* */
FunctionBlock:
	TOK_FB
	{
		CurrentCodeBlockType = EFunctionBlock;
	}
	NAME TOK_INSTRUCTION_EOLN
	{
		NewUseContainerBlock(CurrentCodeBlockType, $3);
	}
	FBProgramVars
	code
	TOK_FB_END
	{
		FinishUseContainerBlock(EFunctionBlock, $3);
	}
	;

	/* ********************************* */
	/* Programm block source declaration */
	/* ********************************* */
Program:
	TOK_PROGRAM 
	{
		CurrentCodeBlockType = EProgram;
	}
	NAME TOK_INSTRUCTION_EOLN
	{
		NewUseContainerBlock(CurrentCodeBlockType, $3);
	}
	FBProgramVars
	code
	TOK_PROGRAM_END
	{
		FinishUseContainerBlock(EProgram, $3);
	}
	;

	/* ********************************************* */
	/* Allowed variables blocks for functions blocks */
	/* and program blocks. This rule allows random   */
	/* order of variables blocks                     */
	/* ********************************************* */
FBProgramVars:
	|
	var_input FBProgramVars
	|
	var_output FBProgramVars
	|
	var_inputoutput FBProgramVars
	|
	var FBProgramVars
	|
	var_external FBProgramVars
	;

	/* ******************************************** */
	/* Resource block inside of configuration block */
	/* declaration - declares tasks and program     */
	/* executions                                   */
	/* ******************************************** */
Resource:
	|
	TOK_RESOURCE
	NAME TOK_ON NAME
	var_global resource_body
	TOK_RESOURCE_END
	;

	/* ***************************** */
	/* Variables blocks declarations */
	/* ***************************** */

	/* Input variables block */
var_input:
	TOK_VAR_INPUT
	{
		CurrentVarBlockType = EVarInput;
	}
	var_declaration
	TOK_END_VAR
	{
		FinishUseContainerContainerVars();
		CurrentVarBlockType = EVarUndefined;
	}
	;

	/* Output variables block */
var_output:
	TOK_VAR_OUTPUT
	{
		CurrentVarBlockType = EVarOutput;
	}
	var_declaration
	TOK_END_VAR
	{
		FinishUseContainerContainerVars();
		CurrentVarBlockType = EVarUndefined;
	}
	;

	/* Input/output variables block */
var_inputoutput:
	TOK_VAR_INPUT_OUTPUT
	{
		CurrentVarBlockType = EVarIO;
	}
	var_declaration
	TOK_END_VAR
	{
		FinishUseContainerContainerVars();
		CurrentVarBlockType = EVarUndefined;
	}
	;

	/* Local variables block */
var:
	TOK_VAR
	{
		CurrentVarBlockType = EVarLocal;
	}
	var_declaration_all
	TOK_END_VAR
	{
		FinishUseContainerContainerVars();
		CurrentVarBlockType = EVarUndefined;
	}
	;

	/* Global variables declaration */
var_global:
	|
	TOK_VAR_GLOBAL
	{
		CurrentVarBlockType = EVarGlobal;
	}
	var_declaration_all
	TOK_END_VAR
	{
		FinishUseContainerContainerVars();
		CurrentVarBlockType = EVarUndefined;
	}
	;
	/* ***************************************** */
	/* Declaration of external variables block   */
	/* External variables are scope declarations */
	/* for global variables declared in  config  */
	/* block                                     */
	/* ***************************************** */
var_external:
	TOK_VAR_EXTERNAL
	{
		CurrentVarBlockType = EVarExternal;
	}
	var_declaration_all
	TOK_END_VAR
	{
		FinishUseContainerContainerVars();
		CurrentVarBlockType = EVarUndefined;
	}
	;

	/* *********************************************************** */
	/* Declaration of variable (name, location, initial value ...) */
	/* *********************************************************** */
var_declaration:
	| var_declaration var_decl
	;

	/* ****************************************** */
	/* Variable consist from NAME [location] and  */
	/* datatype (with or without initilization)   */
	/* ****************************************** */
var_decl:
	NAME var_location TOK_COLON
	var_datatype
	TOK_SEMICOLON
	{
		NewUseContainerVariable();
		pContainerVariable->SetName($1);
		FinishUseContainerVariable();
	}
	;

var_declaration_all:
	| var_declaration_all var_decl_all
	;

var_decl_all:
	var_constant NAME var_location TOK_COLON
	var_datatype_all
	TOK_SEMICOLON
	{
		NewUseContainerVariable();
		pContainerVariable->SetName($2);
		FinishUseContainerVariable();
	}
	;

var_datatype_all:
	var_datatype
	|
	TOK_FB_DATATYPE
	{
		string sInclude;

		sInclude = string("plcfb_") + $1;
		sInclude += ".h";

		NewUseContainerVariable();
		pContainerVariable->SetFBDataType($1);

		NewUseContainerIncludes();
		(*pIncludes) += sInclude.c_str();
	}
	;

var_constant:
	|
	TOK_CONSTANT
	{
		NewUseContainerVariable();
		pContainerVariable->SetConstant();
	}
	;
	/* ************************************** */
	/* If variable is declared with specified */
	/* location, get location and save it     */
	/* ************************************** */
var_location:
	|
	TOK_AT VAR_LOCATION
	{
		NewUseContainerVariable();
		pContainerVariable->SetLocation($2);
	}
	;
	/* ***************************************** */
	/* Parse data type of variable - all numeric */
	/* formats are parsed with one rule. When    */
	/* initial value of variable is specified,   */
	/* save it                                   */
	/* ***************************************** */
var_datatype:
	TOK_NUM_DATATYPE var_init_number
	{
		NewUseContainerVariable();
		pContainerVariable->SetDataType($1);
	}
	|
	TOK_TIME
	{
		NewUseContainerVariable();
		pContainerVariable->SetDataType("TIME");
	}
	|
	TOK_DATE
	{
		NewUseContainerVariable();
		pContainerVariable->SetDataType("DATE");
	}
	|
	TOK_TOD
	{
		NewUseContainerVariable();
		pContainerVariable->SetDataType("TOD");
	}
	|
	TOK_DT
	{
		NewUseContainerVariable();
		pContainerVariable->SetDataType("DT");
	}
	|
	TOK_STRING
	{
		NewUseContainerVariable();
		pContainerVariable->SetDataType("STRING");
	}
	|
	TOK_BOOL var_init_bool
	{
		NewUseContainerVariable();
		pContainerVariable->SetDataType("BOOL");
	}
	;

	/* ************************************* */
	/* Rule for numeric value initialization */
	/* ************************************* */
var_init_number:
	|
	TOK_SETVAL Numbers
	;

	/* ********************************************** */
	/* Rule for boolean variable value initialization */
	/* ********************************************** */
var_init_bool:
	|
	TOK_SETVAL Boolean
	;

	/* ********************************************* */
	/* Recursive rule for IL language program source */
	/* Count lines for IL separately here            */
	/* ********************************************* */
code:
	| code IL
	;

	/* ********************************* */
	/* Single IL instruction declaration */
	/* ********************************* */
IL:
	label instruction TOK_INSTRUCTION_EOLN
	|
	label function_call TOK_INSTRUCTION_EOLN
	;

	/* **************************************** */
	/* Declaration of label on instruction line */
	/* **************************************** */
label:
	|
	LABEL_NAME TOK_COLON
	{
		NewUseContainerIL();
		pIL->Label($1);
	}
	;
	/* ************************************** */
	/* Declarations of all IL instructions    */
	/* with allowed operands. If instruction  */
	/* have modifiers (negation etc.), it's   */
	/* declared also for every allowed        */
	/* modifier                               */
	/* ************************************** */
instruction:
	IL_ST any_operand
	{
		NewUseContainerIL();
		pIL->ST();
		FinishUseContainerIL();
	}
	|
	IL_ST_NEG any_operand
	{
		NewUseContainerIL();
		pIL->ST_NEG();
		FinishUseContainerIL();
	}
	|
	IL_LD any_operand
	{
		NewUseContainerIL();
		pIL->LD();
		FinishUseContainerIL();
	}
	|
	IL_LD_NEG any_operand
	{
		NewUseContainerIL();
		pIL->LD_NEG();
		FinishUseContainerIL();
	}
	|
	IL_R any_operand
	{
		NewUseContainerIL();
		pIL->R();
		FinishUseContainerIL();
	}
	|
	IL_S any_operand
	{
		NewUseContainerIL();
		pIL->S();
		FinishUseContainerIL();
	}
	|
	IL_OR any_operand
	{
		NewUseContainerIL();
		pIL->OR();
		FinishUseContainerIL();
	}
	|
	IL_OR_NEG any_operand
	{
		NewUseContainerIL();
		pIL->OR_NEG();
		FinishUseContainerIL();
	}
	|
	IL_AND any_operand
	{
		NewUseContainerIL();
		pIL->AND();
		FinishUseContainerIL();
	}
	|
	IL_AND_NEG any_operand
	{
		NewUseContainerIL();
		pIL->AND_NEG();
		FinishUseContainerIL();
	}
	|
	IL_XOR any_operand
	{	
		NewUseContainerIL();
		pIL->XOR();
		FinishUseContainerIL();
	}
	|
	IL_XOR_NEG any_operand
	{
		NewUseContainerIL();
		pIL->XOR_NEG();
		FinishUseContainerIL();
	}
	|
	IL_ADD any_operand
	{
		NewUseContainerIL();
		pIL->ADD();
		FinishUseContainerIL();
	}
	|
	IL_SUB any_operand
	{
		NewUseContainerIL();
		pIL->SUB();
		FinishUseContainerIL();
	}
	|
	IL_MUL any_operand
	{
		NewUseContainerIL();
		pIL->MUL();
		FinishUseContainerIL();
	}
	|
	IL_DIV any_operand
	{
		NewUseContainerIL();
		pIL->DIV();
		FinishUseContainerIL();
	}
	|
	IL_GT any_operand
	{
		NewUseContainerIL();
		pIL->GT();
		FinishUseContainerIL();
	}
	|
	IL_GE any_operand
	{
		NewUseContainerIL();
		pIL->GE();
		FinishUseContainerIL();
	}
	|
	IL_EQ any_operand
	{
		NewUseContainerIL();
		pIL->EQ();
		FinishUseContainerIL();
	}
	|
	IL_NE any_operand
	{
		NewUseContainerIL();
		pIL->NE();
		FinishUseContainerIL();
	}
	|
	IL_LE any_operand
	{
		NewUseContainerIL();
		pIL->LE();
		FinishUseContainerIL();
	}
	|
	IL_LT any_operand
	{
		NewUseContainerIL();
		pIL->LT();
		FinishUseContainerIL();
	}
	|
	IL_JMP NAME
	{
		NewUseContainerIL();
		pIL->RightOperand($2);
		pIL->JMP();
		FinishUseContainerIL();
	}
	|
	IL_JMPC NAME
	{
		NewUseContainerIL();
		pIL->RightOperand($2);
		pIL->JMPC();
		FinishUseContainerIL();
	}
	|
	IL_JMPCN NAME
	{
		NewUseContainerIL();
		pIL->RightOperand($2);
		pIL->JMPCN();
		FinishUseContainerIL();
	}
	|
	IL_CAL	cal_operand
	{
		NewUseContainerIL();
		pIL->CAL();
		FinishUseContainerIL();
	}
	|
	IL_CALC	cal_operand
	{
		NewUseContainerIL();
		pIL->CALC();
		FinishUseContainerIL();
	}
	|
	IL_CALCN cal_operand
	{
		NewUseContainerIL();
		pIL->CALCN();
		FinishUseContainerIL();
	}
	|
	IL_RET
	{
		NewUseContainerIL();
		pIL->RET();
		FinishUseContainerIL();
	}
	|
	IL_RETC
	{
		NewUseContainerIL();
		pIL->RETC();
		FinishUseContainerIL();
	}
	|
	IL_RETCN
	{
		NewUseContainerIL();
		pIL->RETCN();
		FinishUseContainerIL();
	}
	;

function_call:
	/* **************************************** */
	/* Unknown instruction - specifies call for */
	/* function (return value is stored back    */
	/* into actual result)                      */
	/* **************************************** */
	NAME function_parm_list
	{
		string sInclude;

		sInclude = string("plcf_") + $1;
		sInclude += ".h";

		NewUseContainerIL();
		pIL->AddFunctionName($1);
		FinishUseContainerIL();

		NewUseContainerIncludes();
		(*pIncludes) += sInclude.c_str();
	}
	;

	/* *************************************** */
	/* Declaration for any instruction operand */
	/* Consists of constant, numeric operand,  */
	/* variable, boolean operand, boolean      */
	/* constant and function block parameter   */
	/* *************************************** */
any_operand:
	variable_operand
	|
	number_operand
	|
	function_block_operand
	|
	time_operand
	;

	/* ***************************** */
	/* Operand with type of function */
	/* block instance parameter      */
	/* ***************************** */
function_block_operand:
	FB_PARM
	{
		NewUseContainerIL();
		pIL->RightVariable($1);
	}
	;

	/* ******************************* */
	/* Operand is any numeric constant */
	/* ******************************* */
number_operand:
	int_oper
	|
	real_oper
	;
	/* Whole numeric constant */
int_oper:
	VAL_INTEGER
	{
		NewUseContainerIL();
		pIL->RightValue($1);
	}
	;
	/* Decimal numeric constant */
real_oper:
	VAL_REAL
	{
		NewUseContainerIL();
		pIL->RightValue($1);
	}
	;

time_operand:
	VAL_TIME
	{
		NewUseContainerIL();
		pIL->RightValue(ConvertString2Time($1));
	}
	;
	/* ************************************** */
	/* Variable operand specified by location */
	/* or by variable name                    */
	/* ************************************** */
variable_operand:
	VAR_LOCATION
	{
		NewUseContainerIL();
		pIL->RightLocation($1);
	}
	|
	NAME
	{
		NewUseContainerIL();
		pIL->RightVariable($1);
	}
	;

	/* ************************************** */
	/* Operand specificating call of function */
	/* block with or without given operands   */
	/* ************************************** */
cal_operand:
	NAME cal_operand_parm
	{
		NewUseContainerIL();
		pIL->AddFBName($1);
	}
	;

	/* ************************************* */
	/* This rule allows function block call  */
	/* without parenthesis given if no parms */
	/* are given                             */
	/* ************************************* */
cal_operand_parm:
	|
	TOK_LPARENT fb_parm_list TOK_RPARENT
	;

	/* ****************************************** */
	/* Proceed single argument or comma separated */
	/* argument list                              */
	/* ****************************************** */
fb_parm_list:
	|
	fb_parm
	|
	fb_parm TOK_COMMA fb_parm_list
	;
fb_parm:
	NAME TOK_SETVAL any_operand
	{
		NewUseContainerIL();
		pIL->AddFBParm($1);
	}
	;

	/* **************************************** */
	/* Declaration for function call parameters */
	/* list. First parameter is taken from      */
	/* actual result value                      */
	/* **************************************** */
function_parm_list:
	|
	function_parm
	|
	function_parm TOK_COMMA function_parm_list
	;

	/* ***************************************** */
	/* Declaration for single function parameter */
	/* ***************************************** */
function_parm:
	any_operand
	{
		NewUseContainerIL();
		pIL->AddFunctionParm();
	}
	;

	/* ******************************************* */
	/* Structure of RESOURCE END_RESOURCE body     */
	/* Can contain tasks declaration and execution */
	/* of program blocks with or without task      */
	/* association                                 */
	/* ******************************************* */
resource_body:
	tasks program_executions
	;

	/* ********************************************* */
	/* Recursive rule for multiple tasks declaration */
	/* ********************************************* */
tasks:
	| 
	task tasks
	;

	/* ********************************************** */
	/* Single task declaration, detects task name and */
	/* tasks parameters if they are given             */
	/* ********************************************** */
task:
	TOK_TASK NAME 
	{
		NewUseContainerTask($2);
	}
	TOK_LPARENT 
	task_parms
	TOK_RPARENT TOK_SEMICOLON
	{
		FinishUseContainerTask();
	}
	;

	/* ****************************************************** */
	/* Allowed task parameters are SINGLE, PRIORITY, INTERVAL */
	/* ****************************************************** */
task_parms:
	task_parms TOK_COMMA task_interval task_single task_priority
	|
	task_interval task_single task_priority
	;

	/* ****************************** */
	/* Detect period setting for task */
	/* ****************************** */
task_interval:
	|
	TOK_INTERVAL TOK_SETVAL VAL_TIME
	{
		pTask->SetInterval(ConvertString2Time($3));
	}
	;

	/* ***************************************** */
	/* Detect single input setting, if it exists */
	/* find address or variable name for input   */
	/* ***************************************** */
task_single:
	|
	TOK_SINGLE TOK_SETVAL task_single_input
	;

	/* ******************************************* */
	/* Find variable for single execution of task. */
	/* It may be also direct memory bit address    */
	/* ******************************************* */
task_single_input:
	NAME
	{
		pTask->SetSingleInputVariable($1);
	}
	|
	VAR_LOCATION
	{
		pTask->SetSingleInputLocation($1);
	}
	;

	/* **i************************* */
	/* Detect task priority setting */
        /* **************************** */
task_priority:
	|
	TOK_PRIORITY TOK_SETVAL VAL_INTEGER
	{
		pTask->SetPriority($3);
	}
	;

	/* **************************************************** */
	/* Rule for proceeding of program execution in resource */
	/* block. Task may have associated task for cyclic      */
	/* execution                                            */
	/* **************************************************** */
program_executions:
	|
	program_execution program_executions
	;

	/* ********************************* */
	/* Rule for single program execution */
	/* ********************************* */
program_execution:
	TOK_PROGRAM NAME 
	{
		NewUseContainerPCall($2);
	}
	program_task_assoc TOK_COLON NAME
	{
		string	sNewInclude;

		pPCall->SetProgramName($6);

		sNewInclude = string("plcp_") + $6;
		sNewInclude += ".h";

		NewUseContainerIncludes();
		(*pIncludes) += sNewInclude.c_str();
	}
	TOK_LPARENT program_parm_list TOK_RPARENT TOK_SEMICOLON
	{
		FinishUseContainerPCall();
	}
	;

	/* **************************************** */
	/* Association of task to program execution */
	/* **************************************** */
program_task_assoc:
	|
	TOK_WITH NAME
	{
		pPCall->SetTaskName($2);
	}
	;

	/* ******************************************** */
	/* Rules for parsing of program call parameters */
	/* ******************************************** */
program_parm_list:
	|
	program_parm
	|
	program_parm TOK_COMMA program_parm_list
	;

program_parm:
	NAME
	{
		pPCall->AddParmName($1);
	}
	TOK_SETVAL program_parm_operand
	;

program_parm_operand:
	NAME
	{
		pPCall->AddParmVariable($1);
	}
	|
	VAR_LOCATION
	{
		pPCall->AddParmLocation($1);
	}
	;
%%

