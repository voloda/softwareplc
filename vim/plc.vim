" Vim syntax file
" Language:	PLC IEC-1131-3 Instruction list source file
" Maintainer:	Voloda (vladimir.kloz@dtg.cz)
" URL:		http://www.zotikos.com/downloads/cvs.vim
" Last Change:	Sat Nov 24 23:25:11 CET 2001

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
	syntax clear
elseif exists("b:current_syntax")
	finish
endif


syn region plcVar start="\<VAR\>" end="\<END_VAR\>" contains=plcNum,plcBin,plHex,plcOct,plcAddr,plcVarKeywords,plcComment,plcDataTypes,plcIdentifier
syn region plcVarGlobal start="\<VAR_GLOBAL\>" end="\<END_VAR\>" contains=plcNum,plcBin,plHex,plcOct,plcAddr,plcVarKeywords,plcComment,plcDataTypes,plcIdentifier
syn region plcVarOutput start="\<VAR_OUTPUT\>" end="\<END_VAR\>" contains=plcNum,plcBin,plHex,plcOct,plcAddr,plcVarKeywords,plcComment,plcDataTypes,plcIdentifier
syn region plcVarInput start="\<VAR_INPUT\>" end="\<END_VAR\>" contains=plcNum,plcBin,plHex,plcOct,plcAddr,plcVarKeywords,plcComment,plcDataTypes,plcIdentifier
syn region plcComment start="(\*" end="\*)"
syn region plcFB start="\<FUNCTION_BLOCK\>" end="\<END_FUNCTION_BLOCK\>" contains=plcInstruction,plcHex,plcOct,plcNum,plcBin,plcVar,plcAddr,plcComment,plcIdentifier,plcVarOutput
syn region plcFunction start="\<FUNCTION\>" end="\<END_FUNCTION\>" contains=plcInstruction,plcHex,plcOct,plcNum,plcBin,plcVar,plcAddr,plcComment,plcIdentifier
syn region plcProgram start="\<PROGRAM\>" end="\<END_PROGRAM\>" contains=plcInstruction,plcHex,plcOct,plcNum,plcBin,plcVar,plcAddr,plcComment,plcIdentifier

syn keyword plcInstruction ADD DIV MUL LD ST LDN STN R S SUB JMP JMPC JMPCN 
syn keyword plcInstruction CAL CALC CALCN RET RETC RETCN GT GE LT LE EQ NE
syn keyword plcInstruction AND OR XOR AND ORN XORN STN LDN
syn keyword plcVarKeywords AT
syn keyword plcDataTypes BOOL TIME SINT INT DINT LINT USINT UINT UDINT ULINT 
syn keyword plcDataTypes REAL LREAL BYTE WORD DWORD LWORD

syn match plcHex "\<16#[a-fA-F0-9]\+\>"
syn match plcOct "\<8#[0-7]\+\>"
syn match plcBin "\<2#[01]\+\>"
syn match plcNum "\<[-+]\=[0-9]\+\.\=[0-9]*[^#]\>"

syn match plcLabel "^[\ \t]*[@a-z_$][a-z0-9_$@]*\ *:"
syn match plcAddr "%[IQM][XBWD]\=[0-9]\+\(.[0-9]\+\)\="

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_plc_syn_inits")
	if version < 508
		let did_plc_syn_inits = 1
		command -nargs=+ HiLink hi link <args>
	else
		command -nargs=+ HiLink hi def link <args>
	endif

	HiLink plcComment	Comment
	HiLink plcInstruction	KeyWord
	HiLink plcHex		Number
	HiLink plcBin		Number
	HiLink plcOct		Number
	HiLink plcNum		Number
	HiLink plcLabel		Label
	HiLink plcAddr		Identifier
	HiLink plcDataTypes	Type
	HiLink plcIdentifier	Special

	hi plcVar		ctermfg=blue
	hi plcVarOutput		ctermfg=blue
	hi plcVarInput		ctermfg=blue
	hi plcVarGlobal		ctermfg=blue
	hi plcFB		ctermfg=green
	hi plcFunction		ctermfg=green
	hi plcProgram		ctermfg=green
	hi plcVarKeywords	ctermfg=red


	delcommand HiLink
endif

let b:current_syntax = "plc"


