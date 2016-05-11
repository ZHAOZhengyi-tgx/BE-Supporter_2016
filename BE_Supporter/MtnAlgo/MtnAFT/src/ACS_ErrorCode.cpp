//The MIT License (MIT)
//
//Copyright (c) 2016 ZHAOZhengyi-tgx
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
// (c)All right reserved, sg.LongRenE@gmail.com



#include "stdafx.h"

#include "MtnApi.h"

#define __OFFSET_ACS_ERROR_CODE_1000					1022
#define __OFFSET_ACS_ERROR_CODE_2000					2002
#define __OFFSET_ACS_ERROR_CODE_3000					3023
#define __OFFSET_ACS_ERROR_CODE_5000					5001

#define __MAX_OFFSET_ACS_ERROR_CODE_1000				1253
#define __MAX_OFFSET_ACS_ERROR_CODE_2000			    2096
#define __MAX_OFFSET_ACS_ERROR_CODE_3000			    3265
#define __MAX_OFFSET_ACS_ERROR_CODE_5000			    5104

#ifdef __ACS_ERR_STRING
char *strACS_ErrorCode_1000[] = {
	"Illegal command                                                                  ", //1022
	"Read-only variable cannot be assigned                                            ", //1023
    "NULL"                                                                             ,     //1024
	"Time interval between two characters in command is more than 2 seconds           ", //1025
    "NULL"                                                                             ,     //1026
    "NULL"                                                                             ,     //1027
	"Scalar variable cannot accept axis specification                                 ", //1028
	"Extra characters after the command                                               ", //1029
    "NULL"                                                                             ,     //1030
    "NULL"                                                                             ,     //1031
    "NULL"                                                                             ,     //1032
    "NULL"                                                                             ,     //1033
	"Illegal index value                                                              ", //1034
	"Index is out of range                                                            ", //1035
    "NULL"                                                                             ,     //1036
	"Illegal variable name                                                            ", //1037
	"Wrong checksum in the command                                                    ", //1038
    "NULL"                                                                             ,     //1039
	"Unable to open file                                                              ", //1040
	"Assigned value is out of range                                                   ", //1041
    "NULL"                                                                             ,     //1042
	"Program cannot start because the buffer was not compiled                         ", //1043
	"Command cannot be executed while the program is running                          ", //1044
	"Numerical error in standard function                                             ", //1045
	"Write file error                                                                 ", //1046
	"Read file error                                                                  ", //1047
	"More axes than were defined in the motion                                        ", //1048
    "NULL"                                                                             ,     //1049   
	"Conflict with user-defined axis group                                            ", //1050
	"Line number is out of range                                                      ", //1051
	"Buffer number is out of range                                                    ", //1052
	"Wrong type                                                                       ", //1053
    "NULL"                                                                             ,     //1054   
	"Command requires line number specification                                       ", //1055
    "NULL"                                                                             ,     //1056   
    "NULL"                                                                             ,     //1057   
    "NULL"                                                                             ,     //1058   
    "NULL"                                                                             ,     //1059   
	"Illegal memory command                                                           ", //1060
	"')' wasn't found                                                                 ", //1061
    "NULL"                                                                             ,     //1062   
	"Variable is not defined in the buffer                                            ", //1063
	"Undefined global variable                                                        ", //1064
	"Command cannot be executed while the current motion is in progress               ", //1065
    "NULL"                                                                             ,     //1066
	"GO command failed                                                                ", //1067
	"Referenced axis does not execute a motion (motion was terminated?)               ", //1068
	"This command can be used only with MPTP, PATH or PVSPLINE motion                 ", //1069
	"Attempt to add segment after ENDS command                                        ", //1070
	"File name was expected                                                           ", //1071
	"Wrong array size                                                                 ", //1072
	"Text for search is not specified                                                 ", //1073
	"Only standard or SP variable is allowed in the command                           ", //1074
    "NULL"                                                                             ,     //1075   
	"Undefined label                                                                  ", //1076
	"Protection violation                                                             ", //1077
	"Variable can be changed only while the motor is disabled                         ", //1078
    "NULL"                                                                             ,     //1079   
    "NULL"                                                                             ,     //1080   
	"Incompatible suffixes                                                            ", //1081
    "NULL"                                                                             ,     //1082   
    "NULL"                                                                             ,     //1083   
    "NULL"                                                                             ,     //1084   
	"Extra number after the command                                                   ", //1085
	"Variable name must be specified                                                  ", //1086
	"Command cannot be executed while the axis is moving                              ", //1087
	"Variable can be queried only in compiled buffer                                  ", //1088
	"Label can be referenced only in compiled buffer                                  ", //1089
	"This type of motion is not allowed for grouped axis                              ", //1090
	"Less arguments than required                                                     ", //1091
	"More arguments than required                                                     ", //1092
	"Bit selector value is greater than 31 or less than 0                             ", //1093
    "NULL"                                                                             ,     //1094   
    "NULL"                                                                             ,     //1095   
	"'}' was not found                                                                ", //1096
	"Previous data collection is in progress                                          ", //1097
	"Stalled motion requires limits specification                                     ", //1098
	"Extra numbers after the command                                                  ", //1099
    "NULL"                                                                             ,     //1100   
	"The program is suspended, the start line number is not allowed                   ", //1101
    "NULL"                                                                             ,     //1102   
    "NULL"                                                                             ,     //1103   
    "NULL"                                                                             ,     //1104   
	"Format error                                                                     ", //1105
	"SP function failed                                                               ", //1106
    "NULL"                                                                             ,     //1107   
	"Invalid real number                                                              ", //1108
	"The command is not allowed in SAFE mode                                          ", //1109
    "NULL"                                                                             ,     //1110   
    "NULL"                                                                             ,     //1111   
	"No matches were found                                                            ", //1112
    "NULL"                                                                             ,     //1113   
	"The program finished without STOPcommand                                         ", //1114
	"Stack underflow (RET without CALL)                                               ", //1115
	"Stack overflow (too many nested CALLs)                                           ", //1116
	"Attempt to enter autoroutine directly                                            ", //1117
	"Illegal axis number                                                              ", //1118
    "NULL"                                                                             ,     //1119
	"The motion must involve the first two axes from the group                        ", //1120
	"Unknown #-constant                                                               ", //1121
	"Bit selection is not applicable                                                  ", //1122
	"Illegal bit selector                                                             ", //1123
	"Attempt to enable motor failed                                                   ", //1124
	"Error in SP program                                                              ", //1125
	"Illegal SP number                                                                ", //1126
	"Editing of this buffer is disabled                                               ", //1127
    "NULL"                                                                             ,     //1128
	"In binary command the name specification must end with /                         ", //1129
  "Segment sequence for the previous motion were not terminated with ends command.  "  ,     //1130
    "NULL"                                                                             ,     //1131
    "NULL"                                                                             ,     //1132
    "NULL"                                                                             ,     //1133
    "NULL"                                                                             ,     //1134
    "NULL"                                                                             ,     //1135
    "NULL"                                                                             ,     //1136
    "NULL"                                                                             ,     //1137
    "NULL"                                                                             ,     //1138
	"Illegal input                                                                    ", //1139
    "NULL"                                                                             ,     //1140
    "NULL"                                                                             ,     //1141
    "NULL"                                                                             ,     //1142
    "NULL"                                                                             ,     //1143
	"Simulator does not support this command                                          ", //1144
    "NULL"                                                                             ,     //1145
    "NULL"                                                                             ,     //1146
    "NULL"                                                                             ,     //1147
	"Requested more SINCOS encoder multipliers than installed                         ", //1148
    "NULL"                                                                             ,     //1149
    "NULL"                                                                             ,     //1150
	"This is a DEMO version of Simulator. 5 minutes of work remains.                  ", //1151
	"The DEMO version of Simulator has terminated                                     ", //1152
	"Illegal query command                                                            ", //1153
    "NULL"                                ,                                                  //1154
    "NULL"                                ,                                                  //1155
    "NULL"                                ,                                                  //1156
    "NULL"                                ,                                                  //1157
    "NULL"                                ,                                                  //1158
    "NULL"                                ,                                                  //1159
    "NULL"                                ,                                                  //1160
    "NULL"                                ,                                                  //1161
    "NULL"                                ,                                                  //1162
    "NULL"                                ,                                                  //1163
    "NULL"                                ,                                                  //1164
    "NULL"                                ,                                                  //1165
    "NULL"                                ,                                                  //1166
    "NULL"                                ,                                                  //1167
    "NULL"                                ,                                                  //1168
    "NULL"                                ,                                                  //1169
    "NULL"                                ,                                                  //1170
    "NULL"                                ,                                                  //1171
    "NULL"                                ,                                                  //1172
    "NULL"                                ,                                                  //1173
    "NULL"                                ,                                                  //1174
    "NULL"                                ,                                                  //1175
    "NULL"                                ,                                                  //1176
    "NULL"                                ,                                                  //1177
    "NULL"                                ,                                                  //1178
    "NULL"                                ,                                                  //1179
    "NULL"                                ,                                                  //1180
    "NULL"                                ,                                                  //1181
    "NULL"                                ,                                                  //1182
    "NULL"                                ,                                                  //1183
    "NULL"                                ,                                                  //1184
    "NULL"                                ,                                                  //1185
    "NULL"                                ,                                                  //1186
    "NULL"                                ,                                                  //1187
    "NULL"                                ,                                                  //1188
    "NULL"                                ,                                                  //1189
    "NULL"                                ,                                                  //1190
    "NULL"                                ,                                                  //1191
    "NULL"                                ,                                                  //1192
    "NULL"                                ,                                                  //1193
    "NULL"                                ,                                                  //1194
    "NULL"                                ,                                                  //1195
    "NULL"                                ,                                                  //1196
    "NULL"                                ,                                                  //1197
    "NULL"                                ,                                                  //1198
    "NULL"                                ,                                                  //1199
    "NULL"                                ,                                                  //1200
    "NULL"                                ,                                                  //1201
    "NULL"                                ,                                                  //1202
    "NULL"                                ,                                                  //1203
    "NULL"                                ,                                                  //1204
    "NULL"                                ,                                                  //1205
    "NULL"                                ,                                                  //1206
    "NULL"                                ,                                                  //1207
    "NULL"                                ,                                                  //1208
    "NULL"                                ,                                                  //1209
    "NULL"                                ,                                                  //1210
    "NULL"                                ,                                                  //1211
    "NULL"                                ,                                                  //1212
    "NULL"                                ,                                                  //1213
    "NULL"                                ,                                                  //1214
    "NULL"                                ,                                                  //1215
    "NULL"                                ,                                                  //1216
    "NULL"                                ,                                                  //1217
    "NULL"                                ,                                                  //1218
    "NULL"                                ,                                                  //1219
    "NULL"                                ,                                                  //1220
    "NULL"                                ,                                                  //1221
    "NULL"                                ,                                                  //1222
    "NULL"                                ,                                                  //1223
    "NULL"                                ,                                                  //1224
    "NULL"                                ,                                                  //1225
    "NULL"                                ,                                                  //1226
    "NULL"                                ,                                                  //1227
    "NULL"                                ,                                                  //1228
    "NULL"                                ,                                                  //1229
    "NULL"                                ,                                                  //1230
    "NULL"                                ,                                                  //1231
    "NULL"                                ,                                                  //1232
    "NULL"                                ,                                                  //1233
    "NULL"                                ,                                                  //1234
    "NULL"                                ,                                                  //1235
    "NULL"                                ,                                                  //1236
    "NULL"                                ,                                                  //1237
    "NULL"                                ,                                                  //1238
    "NULL"                                ,                                                  //1239
    "NULL"                                ,                                                  //1240
    "NULL"                                ,                                                  //1241
    "NULL"                                ,                                                  //1242
    "NULL"                                ,                                                  //1243
    "NULL"                                ,                                                  //1244
    "NULL"                                ,                                                  //1245
    "NULL"                                ,                                                  //1246
    "NULL"                                ,                                                  //1247
    "NULL"                                ,                                                  //1248
    "NULL"                                ,                                                  //1249
    "NULL"                                ,                                                  //1250
    "NULL"                                ,                                                  //1251
    "NULL"                                ,                                                  //1252
	"Unable to work with dummy motor"                                                    //1253
};

char *strACS_ErrorCode_2000[] = {                                                                                                                           
	"Unrecognized command                                              ",                                       //2002 
	"Unexpected delimiter, incomplete command                          ",                                       //2003 
   "NULL"                                                              ,                                            //2004 
   "NULL"                                                              ,                                            //2005 
	"Unexpected END                                                    ",                                       //2006 
	"Two adjacent operators                                            ",                                       //2007 
	"Left bracket was expected                                         ",                                       //2008 
	"Right bracket was expected                                        ",                                       //2009 
	"Comma was expected                                                ",                                       //2010 
	"Equals sign was expected                                          ",                                       //2011 
   "NULL"                                                              ,                                            //2012 
   "NULL"                                                              ,                                            //2013 
   "NULL"                                                              ,                                            //2014 
	"Integer positive constant is expected                             ",                                       //2015 
	"Only label or line number is allowed as start point               ",                                       //2016 
   "NULL"                                                              ,                                            //2017 
	"Scalar variable cannot be indexed or used with axis specification ",                                       //2018 
   "NULL"                                                              ,                                            //2019 
	"Read-only variable cannot be assigned                             ",                                       //2020 
	"Label was expected                                                ",                                       //2021 
	"Array name was expected                                           ",                                       //2022 
	"Variable name was expected                                        ",                                       //2023 
	"Undefined label                                                   ",                                       //2024 
	"Duplicated label                                                  ",                                       //2025 
	"Undefined variable name                                           ",                                       //2026 
	"Duplicated variable name                                          ",                                       //2027 
   "NULL"                                                              ,                                            //2028 
   "NULL"                                                              ,                                            //2029 
	"User array is limited to 30000 elements                           ",                                       //2030 
	"Global of different type/dimension was defined in other buffer    ",                                       //2031 
   "NULL"                                                              ,                                            //2032 
	"Mandatory argument is omitted                                     ",                                       //2033 
	"More arguments than required                                      ",                                       //2034 
	"Wrong argument type                                               ",                                       //2035 
	"Function that not returns value cannot be used in expression      ",                                       //2036 
	"Axis specification was expected                                   ",                                       //2037 
   "NULL"                                                              ,                                            //2038 
   "NULL"                                                              ,                                            //2039 
   "NULL"                                                              ,                                            //2040 
   "NULL"                                                              ,                                            //2041 
   "NULL"                                                              ,                                            //2042 
   "NULL"                                                              ,                                            //2043 
	"Index is out of range                                             ",                                       //2044 
	"Illegal axis value                                                ",                                       //2045 
   "NULL"                                                              ,                                            //2046 
   "NULL"                                                              ,                                            //2047 
	"Argument must be specified as + or - sign                         ",                                       //2048 
	"Illegal suffix for this command                                   ",                                       //2049 
	"Name of standard variable was expected                            ",                                       //2050 
	"Only APOS, RPOS, FPOS and F2POS are allowed in SET command        ",                                       //2051 
	"Variable name was expected                                        ",                                       //2052 
	"Constant argument was expected                                    ",                                       //2053 
	"Illegal buffer number                                             ",                                       //2054 
	"Assigned value is out of range                                    ",                                       //2055 
	"Zero divide                                                       ",                                       //2056 
	"Only VEL, ACC, DEC, JERK, KDECare allowed in IMMcommand           ",                                       //2057 
	"Bit selection cannot be applied to real variable                  ",                                       //2058 
	"ELSEwithout IF                                                    ",                                       //2059 
	"ELSEIFwithout IF                                                  ",                                       //2060 
	"LOOPwithout END                                                   ",                                       //2061 
	"DOwithout END                                                     ",                                       //2062 
	"IFwithout END                                                     ",                                       //2063 
	"Memory overflow                                                   ",                                       //2064 
	"Axis constant or axis expression in brackets expected             ",                                       //2065 
	"Too many axis specifiers                                          ",                                       //2066 
	"An axis is specified more than once                               ",                                       //2067 
	"Sign constant or sign expression in brackets expected             ",                                       //2068 
	"Too many sign specifiers                                          ",                                       //2069 
	"Unknown #-constant                                                ",                                       //2070 
	"Local variable is not allowed in this command                     ",                                       //2071 
	"WHILEwithout END                                                  ",                                       //2072 
	"WAIT, TILL, GOTO, CALL, LOOP...END, WHILE...END, Description, INPUTare not allowed in immediate command ", //2073 
	"Only RPOSis allowed after CONNECT                                           ",                             //2074 
	"Only MPOSvariable is allowed after MASTER                                   ",                             //2075 
	"Constant bit selector is greater than 31 or less than 0                     ",                             //2076 
	"Array name requires indexing                                                ",                             //2077 
	"Current empty space in the dynamic buffer is not enough for the command     ",                             //2078 
	"GOTO,CALL,RET,LOOP, WHILE,IF,ON are not allowed in dynamic buffer           ",                             //2079 
	"Local variable declaration is not allowed in a immediate command.           ",                             //2080 
	"Variable declaration is not allowed in dynamic buffer                       ",                             //2081 
	"Illegal file name                                                           ",                             //2082 
	"Integer overflow                                                            ",                             //2083 
	"Integer constants are allowed in the range from -2147483648 to +2147483647  ",                             //2084 
	"Protection violation                                                        ",                             //2085 
	"Protection attribute cannot be changed for this parameter                   ",                             //2086 
	"Only constant 0 or 1 is allowed at the right side                           ",                             //2087 
	"No dual-port RAM in this controller                                         ",                             //2088 
	"Bit selection is not available for DPR variable                             ",                             //2089 
	"Only global variables can be defined in DPR                                 ",                             //2090 
	"DPR address must be specified                                               ",                             //2091 
	"Only even numbers from 128 to 504 are allowed as DPR address                ",                             //2092 
	"Collision with other variable in DPR                                        ",                             //2093 
	"DPR variable is not allowed in this command                                 ",                             //2094 
	"Illegal line number                                                         ",                             //2095 
	"Only even numbers from 128 to 1016 are allowed as DPR address               "                              //2096 
};

char *strACS_ErrorCode_3000[] = {  
	 	"Read-only variable cannot be assigned                                    ",  //3023
    "NULL"                                                                        ,      //3024
    "NULL"                                                                        ,      //3025
    "NULL"                                                                        ,      //3026
    "NULL"                                                                        ,      //3027
 	"Scalar variable cannot accept axis specification                            ",  //3028
    "NULL"                                                                        ,      //3029
    "NULL"                                                                        ,      //3030
    "NULL"                                                                        ,      //3031
    "NULL"                                                                        ,      //3032
    "NULL"                                                                        ,      //3033
 	"Illegal index value                                                         ",  //3034
 	"Index is out of range                                                       ",  //3035
    "NULL"                                                                        ,      //3036
 	"Illegal variable name                                                       ",  //3037
    "NULL"                                                                        ,      //3038
    "NULL"                                                                        ,      //3039
 	"Unable to open file                                                         ",  //3040
 	"Assigned value is out of range                                              ",  //3041
    "NULL"                                                                        ,      //3042
 	"Program cannot start because the buffer was not compiled                    ",  //3043
 	"Command cannot be executed while the program is running                     ",  //3044
 	"Numerical error in standard function                                        ",  //3045
 	"Write file error                                                            ",  //3046
 	"Read file error                                                             ",  //3047
 	"More axes than were defined in the motion                                   ",  //3048
    "NULL"                                                                        ,      //3049
 	"Conflict with user-defined axis group                                       ",  //3050
 	"Line number is out of range                                                 ",  //3051
 	"Buffer number is out of range                                               ",  //3052
 	"Wrong type                                                                  ",  //3053
    "NULL"                                                                        ,      //3054
 	"Command requires line number specification                                  ",  //3055
    "NULL"                                                                        ,      //3056
    "NULL"                                                                        ,      //3057
    "NULL"                                                                        ,      //3058
    "NULL"                                                                        ,      //3059
 	"Illegal memory command                                                      ",  //3060
 	"')' wasn't found                                                            ",  //3061
    "NULL"                                                                        ,      //3062
 	"Variable is not defined in the buffer                                       ",  //3063
 	"Undefined global variable                                                   ",  //3064
 	"Command cannot be executed while the current motion is in progress          ",  //3065
    "NULL"                                                                        ,      //3066
 	"GO command failed                                                           ",  //3067
 	"Referenced axis does not execute a motion (motion was terminated?)          ",  //3068
 	"This command can be used only with MPTP, PATH, or PVSPLINE motion           ",  //3069
 	"Attempt to add segment after ENDScommand                                    ",  //3070
 	"File name was expected                                                      ",  //3071
 	"Wrong array size                                                            ",  //3072
 	"Text for search is not specified                                            ",  //3073
 	"Only standard or SP variable is allowed in the command                      ",  //3074
    "NULL"                                                                        ,      //3075
 	"Undefined label                                                             ",  //3076
 	"Protection violation                                                        ",  //3077
 	"Variable can be changed only while the motor is disabled                    ",  //3078
 	"Motion cannot start because one or more motors are disabled                 ",  //3079
 	"Default Connection flag is set for the motor                                ",  //3080
 	"Incompatible suffixes                                                       ",  //3081
    "NULL"                                                                        ,      //3082
    "NULL"                                                                        ,      //3083
    "NULL"                                                                        ,      //3084
 	"Extra number after the command                                              ",  //3085
 	"Variable name must be specified                                             ",  //3086
 	"Command cannot be executed while the axis is moving                         ",  //3087
 	"Variable can be queried only in compiled buffer                             ",  //3088
 	"Label can be referenced only in compiled buffer                             ",  //3089
 	"This type of motion is not allowed for grouped axis                         ",  //3090
 	"Less arguments than required                                                ",  //3091
 	"More arguments than required                                                ",  //3092
 	"Bit selector value is greater than 31 or less than 0                        ",  //3093
    "NULL"                                                                        ,      //3094
    "NULL"                                                                        ,      //3095
 	"'}' was not found                                                           ",  //3096
 	"Previous data collection is in progress                                     ",  //3097
 	"Stalled motion requires limits specification                                ",  //3098
 	"Extra numbers after the command                                             ",  //3099
    "NULL"                                                                        ,      //3100
 	"The program is suspended, the start line number is not allowed              ",  //3101
    "NULL"                                                                        ,      //3102
    "NULL"                                                                        ,      //3103
    "NULL"                                                                        ,      //3104
 	"Format error                                                                ",  //3105
 	"SP function failed                                                          ",  //3106
    "NULL"                                                                        ,      //3107
 	"Invalid real number                                                         ",  //3108
    "NULL"                                                                        ,      //3109
    "NULL"                                                                        ,      //3110
    "NULL"                                                                        ,      //3111
 	"No matches were found                                                       ",  //3112
    "NULL"                                                                        ,      //3113
 	"The program finished without a STOP command                                 ",  //3114
 	"Stack underflow (RETwithout CALL)                                           ",  //3115
 	"Stack overflow (too many nested CALLs)                                      ",  //3116
 	"Attempt to enter autoroutine directly                                       ",  //3117
 	"Illegal axis number                                                         ",  //3118
    "NULL"                                                                        ,      //3119
 	"The motion must involve the first two axes from the group                   ",  //3120
 	"Unknown #-constant                                                          ",  //3121
 	"Bit selection is not applicable                                             ",  //3122
 	"Illegal bit selector                                                        ",  //3123
 	"Attempt to enable motor failed                                              ",  //3124
 	"Error in SP program                                                         ",  //3125
 	"Illegal SP number                                                           ",  //3126
 	"Editing of this buffer is disabled                                          ",  //3127
    "NULL"                                                                        ,      //3128
 	"In binary command the name specification must end with /                    ",  //3129
 	"Segment sequence for the previous motion was not terminated with ENDS       ",  //3130
    "NULL"                                                                        ,      //3131
 	"The file is not a legal user data file                                      ",  //3132
 	"Discrepancy in types of the saved and restored arrays                       ",  //3133
 	"Discrepancy in sizes of the saved and restored arrays                       ",  //3134
    "NULL"                                                                        ,      //3135
 	"Wrong relation between first point, last point and interval                 ",  //3136
 	"Illegal analog output number                                                ",  //3137
 	"Incompatible SP and analog output                                           ",  //3138
 	"Illegal input                                                               ",  //3139
    "NULL"                                                                        ,      //3140
    "NULL"                                                                        ,      //3141
 	"Arguments are inconsistent                                                  ",  //3142
 	"Simulator does not support this command                                     ",  //3143
 	"The specified DPR address is less than 128 or exceeds the DPR size          ",  //3144
 	"Collision with other variable in DPR                                        ",  //3145
    "NULL"                                                                        ,      //3146
    "NULL"                                                                        ,      //3147
    "NULL"                                                                        ,      //3148
 	"Illegal SP address                                                          ",  //3149
 	"Only even numbers are allowed as DPR address                                ",  //3150
 	"This is a DEMO version of Simulator. 5 minutes of work remains.             ",  //3151
 	"The DEMO version of Simulator has terminated                                ",  //3152
    "NULL"                                                                        ,      //3153
 	"This command can be used only with MSEGmotion                               ",  //3154
 	"Motion cannot start because the previous motion failed. Use FCLEAR command. ",  //3155
    "NULL"                                                                        ,      //3156
    "NULL"                                                                        ,      //3157
    "NULL"                                                                        ,      //3158
    "NULL"                                                                        ,      //3159
    "NULL"                                                                        ,      //3160
    "NULL"                                                                        ,      //3161
    "NULL"                                                                        ,      //3162
    "NULL"                                                                        ,      //3163
    "NULL"                                                                        ,      //3164
    "NULL"                                                                        ,      //3165
    "NULL"                                                                        ,      //3166
    "NULL"                                                                        ,      //3167
    "NULL"                                                                        ,      //3168
    "NULL"                                                                        ,      //3169
    "NULL"                                                                        ,      //3170
    "NULL"                                                                        ,      //3171
    "NULL"                                                                        ,      //3172
    "NULL"                                                                        ,      //3173
    "NULL"                                                                        ,      //3174
    "NULL"                                                                        ,      //3175
    "NULL"                                                                        ,      //3176
    "NULL"                                                                        ,      //3177
    "NULL"                                                                        ,      //3178
    "NULL"                                                                        ,      //3179
    "NULL"                                                                        ,      //3180
    "NULL"                                                                        ,      //3181
    "NULL"                                                                        ,      //3182
    "NULL"                                                                        ,      //3183
    "NULL"                                                                        ,      //3184
    "NULL"                                                                        ,      //3185
    "NULL"                                                                        ,      //3186
    "NULL"                                                                        ,      //3187
    "NULL"                                                                        ,      //3188
    "NULL"                                                                        ,      //3189
    "NULL"                                                                        ,      //3190
    "NULL"                                                                        ,      //3191
    "NULL"                                                                        ,      //3192
    "NULL"                                                                        ,      //3193
    "NULL"                                                                        ,      //3194
    "NULL"                                                                        ,      //3195
    "NULL"                                                                        ,      //3196
    "NULL"                                                                        ,      //3197
    "NULL"                                                                        ,      //3198
    "NULL"                                                                        ,      //3199
    "NULL"                                                                        ,      //3200
 	"End-of-Sequence is illegal for this motion                                  ",  //3201
    "NULL"                                                                        ,      //3202
    "NULL"                                                                        ,      //3203
    "NULL"                                                                        ,      //3204
    "NULL"                                                                        ,      //3205
    "NULL"                                                                        ,      //3206
    "NULL"                                                                        ,      //3207
    "NULL"                                                                        ,      //3208
    "NULL"                                                                        ,      //3209
    "NULL"                                                                        ,      //3210
    "NULL"                                                                        ,      //3211
 	"ARC arguments are inconsistent                                              ",  //3212
 	"Stopper is prohibited for master-slave motion                               ",  //3213
 	"Adjacent stoppers are prohibited                                            ",  //3214
 	"In cyclic path the first and the last points must coincide                  ",  //3215
 	"Velocity is specified, but the motion command had no V command option       ",  //3216
 	"Segment of zero length                                                      ",  //3217
    "NULL"                                                                        ,      //3218
    "NULL"                                                                        ,      //3219
    "NULL"                                                                        ,      //3220
    "NULL"                                                                        ,      //3221
    "NULL"                                                                        ,      //3222
    "NULL"                                                                        ,      //3223
    "NULL"                                                                        ,      //3224
    "NULL"                                                                        ,      //3225
    "NULL"                                                                        ,      //3226
    "NULL"                                                                        ,      //3227
    "NULL"                                                                        ,      //3228
    "NULL"                                                                        ,      //3229
    "NULL"                                                                        ,      //3230
 	"Illegal key                                                                 ",  //3231
 	"Illegal index                                                               ",  //3232
 	"Illegal value                                                               ",  //3233
 	"Value in SETCONF must be 0 or 1                                             ",  //3234
 	"SETCONF function is disabled                                                ",  //3235
 	"SETCONF cannot be executed while the motor is enabled                       ",  //3236
    "NULL"                                                                        ,      //3237
    "NULL"                                                                        ,      //3238
    "NULL"                                                                        ,      //3239
    "NULL"                                                                        ,      //3240
    "NULL"                                                                        ,      //3241
    "NULL"                                                                        ,      //3242
    "NULL"                                                                        ,      //3243
    "NULL"                                                                        ,      //3244
    "NULL"                                                                        ,      //3245
    "NULL"                                                                        ,      //3246
    "NULL"                                                                        ,      //3247
    "NULL"                                                                        ,      //3248
    "NULL"                                                                        ,      //3249
    "NULL"                                                                        ,      //3250
    "NULL"                                                                        ,      //3251
    "NULL"                                                                        ,      //3252
 	"Unable to work with Dummy motor                                             ",  //3253
 	"The operation requires the motor to be enabled                              ",  //3254
 	"The operation requires the motor to be disabled                             ",  //3255
 	"The operation is valid only for brushless motor without Hall sensor         ",  //3256
 	"The operation failed because the brushless motor is not commutated          ",  //3257
 	"The operation failed because the motor is in open-loop mode                 ",  //3258
 	"Motion cannot start because the motor is defined as dummy                   ",  //3259
 	"Motion cannot start because the motor is disabled                           ",  //3260
 	"Motion cannot start because the brushless motor is not commutated           ",  //3261
 	"Motion cannot start because the motor is in open-loop mode                  ",  //3262
 	"Motion cannot start because the previous motion failed. Use FCLEAR command. ",  //3263
    "NULL"                                                                        ,      //3264
 	"SP program does not support this operation                                  "  //3265
};

char *strACS_ErrorCode_5000[] = {
"Motion generation finished                                 ", //5001
"NULL"                                                       , //5002
"Motion was terminated by user                              ", //5003
"Motor was disabled by user                                 ", //5004
"Motion was terminated because a motor was disabled         ", //5005
"Motion was killed                                          ", //5006
"Motor was disabled due to another motor becoming disabled  ", //5007
"Motion was killed due to another motion being killed       ", //5008
"NULL"                                                       , //5009
"Right Limit                                                ", //5010
"Left Limit                                                 ", //5011
"Preliminary Right Limit                                    ", //5012
"Preliminary Left Limit                                     ", //5013
"Overheat                                                   ", //5014
"Software Right Limit                                       ", //5015
"Software Left Limit                                        ", //5016
"Encoder Not Connected                                      ", //5017
"Encoder 2 Not Connected                                    ", //5018
"Drive Alarm                                                ", //5019
"Encoder Error                                              ", //5020
"Encoder 2 Error                                            ", //5021
"Position Error                                             ", //5022
"Critical Position Error                                    ", //5023
"Velocity Limit                                             ", //5024
"Acceleration Limit                                         ", //5025
"Overcurrent                                                ", //5026
"Servo Processor Alarm                                      ", //5027
"NULL"                                                       , //5028
"NULL"                                                       , //5029
"NULL"                                                       , //5030
"NULL"                                                       , //5031
"NULL"                                                       , //5032
"Attempt of Motion in Disabled Direction                    ", //5033
"NULL"                                                       , //5034
"Program Error                                              ", //5035
"Memory Overuse                                             ", //5036
"Time Overuse                                               ", //5037
"Emergency Stop                                             ", //5038
"Servo Interrupt                                            ", //5039
"NULL"                                                       , //5040
"NULL"                                                       , //5041
"NULL"                                                       , //5042
"NULL"                                                       , //5043
"NULL"                                                       , //5044
"NULL"                                                       , //5045
"NULL"                                                       , //5046
"NULL"                                                       , //5047
"NULL"                                                       , //5048
"NULL"                                                       , //5049
"NULL"                                                       , //5050
"NULL"                                                       , //5051
"NULL"                                                       , //5052
"NULL"                                                       , //5053
"NULL"                                                       , //5054
"NULL"                                                       , //5055
"NULL"                                                       , //5056
"NULL"                                                       , //5057
"NULL"                                                       , //5058
"NULL"                                                       , //5059
"Driver Alarm                                               ", //5060
"Drive Alarm: Short Circuit                                 ", //5061
"Drive Alarm: External Protection Activated                 ", //5062
"Drive Alarm: Power Supply Too Low                          ", //5063
"Drive Alarm: Power Supply Too High                         ", //5064
"Drive Alarm: Temperature Too High                          ", //5065
"Drive Alarm: Power Supply 24VF1                            ", //5066
"Drive Alarm: Power Supply 24VF2                            ", //5067
"Drive Alarm: Emergency Stop                                ", //5068
"Drive alarm: Power-down                                    ", //5069
"NULL"                                                       , //5070
"NULL"                                                       , //5071
"NULL"                                                       , //5072
"NULL"                                                       , //5073
"NULL"                                                       , //5074
"Drive Alarm: Digital Drive Interface not Connected         ", //5075
"NULL"                                                       , //5076
"NULL"                                                       , //5077
"NULL"                                                       , //5078
"NULL"                                                       , //5079
"NULL"                                                       , //5080
"NULL"                                                       , //5081
"NULL"                                                       , //5082
"NULL"                                                       , //5083
"NULL"                                                       , //5084
"NULL"                                                       , //5085
"NULL"                                                       , //5086
"NULL"                                                       , //5087
"NULL"                                                       , //5088
"NULL"                                                       , //5089
"NULL"                                                       , //5090
"NULL"                                                       , //5091
"NULL"                                                       , //5092
"NULL"                                                       , //5093
"NULL"                                                       , //5094
"NULL"                                                       , //5095
"NULL"                                                       , //5096
"NULL"                                                       , //5097
"NULL"                                                       , //5098
"NULL"                                                       , //5099
"Current bias measured is out of range                      ", //5100
"Autocommutation failure (phase error)                      ", //5101
"Autocommutation failure (position error)                   ", //5102
"NULL"                                                       , //5103
"ENABLE failed because the motor is moving                  " //5104
};

#endif  // __ACS_ERR_STRING

char *strNullErrorCode = "NULL";

short acs_get_error_code_string(char * strErrorText, int iErrorCode)
{
	short sRet = MTN_API_OK_ZERO;

#ifdef __ACS_ERR_STRING

	if(iErrorCode < __OFFSET_ACS_ERROR_CODE_1000)
	{
		strErrorText = strNullErrorCode;
		sRet = MTN_API_ERROR;
	}
	else if(iErrorCode >= __OFFSET_ACS_ERROR_CODE_1000 && iErrorCode <= __MAX_OFFSET_ACS_ERROR_CODE_1000)
	{
		strErrorText = strACS_ErrorCode_1000[iErrorCode - __OFFSET_ACS_ERROR_CODE_1000];
	}
	else if(iErrorCode >= __OFFSET_ACS_ERROR_CODE_2000 && iErrorCode <= __MAX_OFFSET_ACS_ERROR_CODE_2000)
	{
		strErrorText = strACS_ErrorCode_2000[iErrorCode - __OFFSET_ACS_ERROR_CODE_2000];
	}
	else if(iErrorCode >= __OFFSET_ACS_ERROR_CODE_3000 && iErrorCode <= __MAX_OFFSET_ACS_ERROR_CODE_3000)
	{
		strErrorText = strACS_ErrorCode_3000[iErrorCode - __OFFSET_ACS_ERROR_CODE_3000];
	}
	else if(iErrorCode >= __OFFSET_ACS_ERROR_CODE_5000 && iErrorCode <= __MAX_OFFSET_ACS_ERROR_CODE_5000)
	{
		strErrorText = strACS_ErrorCode_5000[iErrorCode - __OFFSET_ACS_ERROR_CODE_5000];
	}
	else
	{
		strErrorText = strNullErrorCode;
		sRet = MTN_API_ERROR;
	}
#endif  // __ACS_ERR_STRING

	return sRet;
}