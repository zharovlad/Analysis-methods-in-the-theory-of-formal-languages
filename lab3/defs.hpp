/* Общие типы данных и их макроопределения */
#ifndef __DEFS
#define __DEFS
#include <iostream>
#include <string>
#include <map>
using namespace std;

const int MAX_LEX = 100;         // максимальная длина лексемы

typedef string TypeLex;		// для лексемы
typedef string TypeMod;		// для модуля

// ключевые слова
const int Tfor = 1;      // for
const int Tint = 10;     // int
const int Tshort = 11;   // short
const int Tlong = 12;    // long
const int Tdouble = 13;  // double
const int Tstruct = 14;  // struct
const int Tmain = 15;	 // main

// идентификаторы и константы
const int Tident = 20;       // идентификатор
const int TConstInt = 30;    // константа целая
const int TConstExp = 31;    // константа экспоненциальная
const int TConstDouble = 32; // константа с плавающей точкой

// специальные знаки
const int TDot = 40; // .
const int TCom = 41; // ,
const int TDC = 42;  // ;
const int TLB = 43;  // (
const int TRB = 44;  // )
const int TFLB = 45; // {
const int TFRB = 46; // }

// знаки операций 
const int TLT = 50;      // < 
const int TLE = 51;      // <= 
const int TGT = 52;      // > 
const int TGE = 53;      // >= 
const int TEQ = 54;      // == 
const int TNEQ = 55;     // != 
const int TPlus = 56;    // +
const int TMinus = 57;   // -
const int TMult = 58;    // *
const int TDiv = 59;     // /
const int TSave = 60;    // = 
const int TBLS = 61;     // << 
const int TBRS = 62;     // >> 
const int TBOR = 63;     // |
const int TBAnd = 64;    // &
const int TBExOr = 65;   // ^
const int TBNot = 66;	 // ~

// конец исходного модуля
const int TEnd = 100;

// ошибочный символ
const int TErr = 200;

// лексические ошибки
const string errorConst = "Incorrect constant";
const string errorFile = "Input file not found";
const string errorSymbol = "Incorrect symbol";
const string errorComment = "Unclosed comment";
const string errorTruncated = "Truncated constant";

// семантические ошибки
const string dual = "dual defenition";
const string notDefine = "not defined";
const string notInit = ". Trying to access to uninitialized variable";
#endif