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
IDENTIFIER	([a-zA-Z_]{1}[a-zA-Z0-9_]*)
INTEGER		([0-9]+)
FP		({INTEGER}(.{INTEGER})?)
TIME_CONSTANT	({FP}[dD])?(_?{FP}[hH])?(_?{FP}[mM])?(_?{FP}[sS])?(_?{FP}((ms)|(MS)))?
NUM_DATATYPE	(SINT)|(INT)|(DINT)|(LINT)|(USINT)|(UINT)|(UDINT)|(ULINT)|(REAL)|(LREAL)|(BYTE)|(WORD)|(DWORD)|(LWORD)
	/* Space definition */
SP		[ \t]+
	/* Space or newline */
SPN		[ \t\n]+
%{
#include <stdio.h>
#include <string.h>
#include "plc2cpp.tab.h"

#include "plc2cpp_globals.h"

%}
/*
 * Use stack for start conditions
 */
%option stack

/*
 * Used start conditions:
 *   var_block           Variable block definition state
 *   var_local_block     Block of local variables
 *   var_external_block  External variables block definition
 *   instruction         Instruction detected - end of line is required
 *   p_block             Programm block context
 *   f_block             Function block context
 *   fb_block            FunctionBlock block context
 *   config_block        Configuration block context
 *   f_declaration       Function name is required
 *   f_data_type         Function return data type is required
 *   var_global_block    Scope of global variables
 *   time_constant       Begin of time constant
 *   resource_block      Resource block in configuration
 *   include_files       List of include files in comment section
 *   comment_state       State of comment
 */
%s var_block instruction p_block f_block fb_block 
%s f_declaration f_data_type var_external_block
%s config_block var_global_block resource_block
%s var_local_block var_bool include_files comment_state
%%
	/* ***************************** */
	/* *** Program blocks tokens *** */
	/* ***************************** */
	/* Token declaration for FUNCTION unit declaration */
<INITIAL>FUNCTION/{SP}			yy_push_state(f_block); yy_push_state(f_declaration); return TOK_FUNCTION;
<f_block>END_FUNCTION		yy_pop_state(); return TOK_FUNCTION_END;

	/* Token declaration for PROGRAM unit declaration */
<INITIAL,resource_block>PROGRAM/{SP}		{

	if (YY_START == INITIAL)
	{
		yy_push_state(p_block); 
	}
	return TOK_PROGRAM;
}

	/* ********************************************* */
	/* *** If comment at the top level of source *** */
	/* *** contains keyword Include:, proceed it *** */
	/* *** as list of files on which this file   *** */
	/* *** depends on                            *** */
	/* ********************************************* */
<INITIAL>\(\*{SP}*Include:{SP}* {
	yy_push_state(include_files);
	return TOK_INCLUDE_BEGIN;
}

	/* ******************************************** */
	/* *** Waiting for list of files, each file *** */
        /* *** may contain part of path and file    *** */
	/* *** name with or without extension       *** */
	/* ******************************************** */
<include_files>(([.]*[\/]*)*{IDENTIFIER}\/)*{IDENTIFIER}[.]?{IDENTIFIER}	{
	yylval.string = strdup(yytext); 
	return FILE_NAME;
}
	/* *************************************** */
	/* *** Detect end of include file list *** */
	/* *************************************** */
<include_files>\*\) {
	yy_pop_state();
	return TOK_INCLUDE_END;
}

<p_block>END_PROGRAM		yy_pop_state(); return TOK_PROGRAM_END;

	/* Token declaration for FUNCTION BLOCK unit declaration */
<INITIAL>FUNCTION_BLOCK/{SP}			yy_push_state(fb_block); return TOK_FB;
<fb_block>END_FUNCTION_BLOCK	yy_pop_state(); return TOK_FB_END;

	/* Token declaration for CONFIGURATION unit declaration */
<INITIAL>CONFIGURATION/{SP}		yy_push_state(config_block); return TOK_CONFIGURATION;
<config_block>END_CONFIGURATION	yy_pop_state(); return TOK_CONFIGURATION_END;

<config_block>RESOURCE/{SP}			yy_push_state(resource_block); return TOK_RESOURCE;
<resource_block>END_RESOURCE		yy_pop_state(); return TOK_RESOURCE_END;

	/* ******************************* */
	/* *** Variables blocks tokens *** */
	/* ******************************* */
	/* Token declaration for INPUT variables block */
VAR_INPUT/{SPN}			yy_push_state(var_block);return TOK_VAR_INPUT;
	/* Token declaration for OUTPUT variables block */
VAR_OUTPUT/{SPN}		yy_push_state(var_block);return TOK_VAR_OUTPUT;
	/* Token declaration for INPUT-OUTPUT variables block */
VAR_IN_OUT/{SPN}		yy_push_state(var_block);return TOK_VAR_INPUT_OUTPUT;
	/* Token declaration for LOCAL variables block */
VAR/{SPN}			yy_push_state(var_local_block);return TOK_VAR;
	/* Token declaration for GLOBAL variables block */
<config_block,resource_block>VAR_GLOBAL		yy_push_state(var_global_block);return TOK_VAR_GLOBAL;
VAR_EXTERNAL		yy_push_state(var_external_block); return TOK_VAR_EXTERNAL;
	/* Token declaration for termination of variables block */
	/* May be fired only when block starts before */
<var_local_block,var_block,var_external_block,var_global_block>END_VAR		yy_pop_state();return TOK_END_VAR;

	/* Ignore RETAIN keyword in variable declaration */
<var_local_block,var_global_block>RETAIN/{SP}	
	/* Constant declaration of variable */
<var_local_block,var_global_block>CONSTANT/{SP}		return TOK_CONSTANT;

<resource_block>
{
TASK		return TOK_TASK;
INTERVAL	return TOK_INTERVAL;
PRIORITY	return TOK_PRIORITY;
SINGLE		return TOK_SINGLE;
}
	/* **************************************** */
	/* *** Variables basic datatypes tokens *** */
	/* **************************************** */
<var_local_block,var_block,var_external_block,var_global_block>
{
BOOL/([ \t]*[;:])			yy_push_state(var_bool); return TOK_BOOL;
TIME/([ \t]*[;:])			return TOK_TIME;
DATE/([ \t]*[;:])			return TOK_DATE;
TIME_OF_DAY|TOD/([ \t]*[:;])		return TOK_TOD;
(DATE_AND_TIME|DT)/([ \t]*[:;])		return TOK_DT;
(STRING)/([ \t]*[:;])			return TOK_STRING;
	/* Numeric datatypes declaration */
	/* (also BYTE, WORD, DWORD, LWORD */
{NUM_DATATYPE}/([ \t]*[:;])		{
		yylval.string = strdup(yytext); 
		return TOK_NUM_DATATYPE;
	}
}
<resource_block>ON				return TOK_ON;
<resource_block>WITH				return TOK_WITH;
<var_bool,var_local_block,var_global_block>AT/{SP}	return TOK_AT;	/* declaration of variable location */
OF			return TOK_OF;
:			{ 
	// YY_START hold information about actual start condition
	// if am I in function declaration phase, 
	// the colon means delimiter between function name
	// and return value datatype -> I switch into new state
	if (YY_START == f_declaration) 
	{
		yy_pop_state();
		yy_push_state(f_data_type);
	}
	return TOK_COLON;
}
;				{
	if (YY_START == var_bool)
		yy_pop_state();

	return TOK_SEMICOLON; /* end of code line */
}
<resource_block,var_local_block,var_block,var_global_block,instruction,var_bool>:=	return TOK_SETVAL; /* assign value into variable */
\(				return TOK_LPARENT; /* left parent */
\)				return TOK_RPARENT; /* right parent */
,				return TOK_COMMA;
	/* *********************************************** */
	/* *** Basic IL instructions tokens definition *** */
	/* *********************************************** */
<f_block,p_block,fb_block>
{
	/* Data storage instructions */
LD			yy_push_state(instruction); return IL_LD;
LDN			yy_push_state(instruction); return IL_LD_NEG;

ST			yy_push_state(instruction); return IL_ST;
STN			yy_push_state(instruction); return IL_ST_NEG;

S			yy_push_state(instruction); return IL_S;
R			yy_push_state(instruction); return IL_R;
	/* Logical instructions */
AND|&			yy_push_state(instruction); return IL_AND;
ANDN|&N			yy_push_state(instruction); return IL_AND_NEG;

OR			yy_push_state(instruction); return IL_OR;
ORN			yy_push_state(instruction); return IL_OR_NEG;

XOR			yy_push_state(instruction); return IL_XOR;
XORN			yy_push_state(instruction); return IL_XOR_NEG;
	/* arithmetical instructions */
ADD			yy_push_state(instruction); return IL_ADD;
SUB			yy_push_state(instruction); return IL_SUB;
MUL			yy_push_state(instruction); return IL_MUL;
DIV			yy_push_state(instruction); return IL_DIV;
	/* Comparation instructions */
GT			yy_push_state(instruction); return IL_GT;
GE			yy_push_state(instruction); return IL_GE;
EQ			yy_push_state(instruction); return IL_EQ;
NE			yy_push_state(instruction); return IL_NE;
LE			yy_push_state(instruction); return IL_LE;
LT			yy_push_state(instruction); return IL_LT;

	/* Label jump instructions */
JMP			yy_push_state(instruction); return IL_JMP;
JMPC			yy_push_state(instruction); return IL_JMPC;
JMPCN			yy_push_state(instruction); return IL_JMPCN;

	/* function block call instructions */
CAL			yy_push_state(instruction); return IL_CAL;
CALC			yy_push_state(instruction); return IL_CALC;
CALCN			yy_push_state(instruction); return IL_CALCN;

	/* yy_push_state(instruction); return from code instruction */
RET			yy_push_state(instruction); return IL_RET;
RETC			yy_push_state(instruction); return IL_RETC;
RETCN			yy_push_state(instruction); return IL_RETCN;
}

	/* ********************************************** */
	/* *** Regular expression for memory location *** */
	/* ********************************************** */
%[IQM]{1}[XBWDL]?[0-9]+(.[0-9]+)?	yylval.string = strdup(yytext + 1); return VAR_LOCATION;	

	/* *************************************** */
	/* *** Boolean value token declaration *** */
	/* *************************************** */
<var_bool>
{
(TRUE)|(1)/([ \t]*;)		return VAL_TRUE; /* bool TRUE declaration */
(FALSE)|(0)/([ \t]*;)		return VAL_FALSE; /* bool FALSE declaration */
}

	/* ********************************* */
	/* *** Integer numeric constants *** */
	/* ********************************* */

	/* Integer constant */
[-+]?([0-9]+)/[^#]		StripUnderlines(yytext); yylval.number = atoi(yytext); return VAL_INTEGER;
	/* Hexadecimal constant expression */
16#[0-9a-fA-F]((_?[0-9a-fA-F])*)		StripUnderlines(yytext); yylval.number = strtol(yytext + 3, NULL, 16); return VAL_INTEGER;
	/* Octal constant expression */
8#[0-7]((_?[0-7])*)			StripUnderlines(yytext); yylval.number = strtol(yytext + 2, NULL, 8); return VAL_INTEGER;
	/* Binary constant expression */
2#[01]((_?[01])*)			StripUnderlines(yytext); yylval.number = strtol(yytext + 2, NULL, 2); return VAL_INTEGER;

	/* time constant */
((t)|(time)|(T)|(TIME))#{TIME_CONSTANT}	StripUnderlines(yytext); yylval.string = strdup(yytext); return VAL_TIME;

	/* ********************************** */
	/* *** Decimal numeric constants **** */ 
	/* *** (with or without exponent) *** */
	/* ********************************** */
[-+]?[0-9]+\.[0-9]+((e[-+]?[0-9]+)?)	StripUnderlines(yytext); yylval.number_r = strtod(yytext, NULL); return VAL_REAL; 

	/* **************************************** */
	/* *** Definition of data types for use *** */
	/* *** as function return value         *** */
	/* **************************************** */
<f_data_type>(BOOL)|(TIME)|(DATE)|(TIME_OF_DAY)|(TOD)|(DATE_AND_TIME)|(DT)|(STRING)|({NUM_DATATYPE})	{
	yylval.string = strdup(yytext);
	yy_pop_state();
	return TOK_FUNCTION_DATATYPE;
}
	/* Detection for FunctionBlock variable name */
<var_local_block,var_block,var_external_block,var_global_block>([a-zA-Z_]{1}[a-zA-Z0-9_]*)/([ \t]*;)	{
	yylval.string = strdup(yytext);
	return TOK_FB_DATATYPE;
}


	/* *************************************** */
	/* *** This rule detects label in code *** */
	/* *** which is returned as NAME       *** */
	/* *************************************** */
<p_block,f_block,fb_block>^[ \t]*{IDENTIFIER}/: yylval.string = strdup(yytext); return LABEL_NAME;

	/* ******************************************** */
	/* *** Identifier expression for variables, *** */
	/* *** functions, function blocks naming    *** */
	/* ******************************************** */
{IDENTIFIER} {
	yylval.string = strdup(yytext); 

	if (YY_START == f_data_type)
	{
		yyterminate();
	}
	else
	{
		/* *********************************************** */
		/* *** Detection of function call instruction  *** */
		/* *** or name of block. End of line detection *** */
		/* *** is required                             *** */
		/* *********************************************** */
		if ((YY_START == p_block) || (YY_START == f_block) || (YY_START == fb_block))
			yy_push_state(instruction);
	}

	return NAME;
}

	/* ************************************************** */
	/* *** Operand accessing function block parameter *** */
	/* ************************************************** */
{IDENTIFIER}\.{IDENTIFIER} yylval.string = strdup(yytext); return FB_PARM; 

	/* **************************************** */
	/* *** Detection for end of instruction *** */
	/* *** New line is only detected and    *** */
	/* *** later used for line counting     *** */
	/* **************************************** */
<instruction>[.]?/\n		{
	yy_pop_state(); 
	return TOK_INSTRUCTION_EOLN;
}

	/* ******************************** */
	/* *** Source file line counter *** */
	/* ******************************** */
\n                      ++iLineCount;/* ignore end of line */
{SP}                  /* ignore whitespace */

	/* ****************************************** */
	/* *** Comments may contain include files *** */
	/* *** which are detected before, rule    *** */
	/* *** must be separated in two rules     *** */
        /* ****************************************** */
\(\*			yy_push_state(comment_state);

<comment_state>.*\*\)	yy_pop_state();	/* ignore comments */
%%

