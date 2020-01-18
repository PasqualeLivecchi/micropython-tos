#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/objstr.h"
#include "py/obj.h"

#define __NR_tos_get_rnd 3000
#define __NR_tos_god_word_pick 3001
#define __NR_tos_god_word_rnd 3002
#define __NR_tos_get_input 3003
#define __NR_tos_popup_get_input 3004
#define __NR_tos_exec_buf 3005
#define __NR_tos_bible_view 3006

#define __NR_tos_bible_passage 3007
#define __NR_tos_bible_passage_pick 3008

#define __NR_tos_popup_yn 3099

/*
x86_64 Syscall Info

Inputs:
Syscall # 	Param 1 	Param 2 	Param 3 	Param 4 	Param 5 	Param 6
rax 		rdi 		rsi 		rdx 		r10 		r8 		r9

Return value: rax 

No clobber registers: rcx and r11
*/

char fxsave_region[512] __attribute__((aligned(16)));

/*
include/asm/fpu/internal.h:             asm volatile( "fxsave %[fx]" : [fx] "=m" (fpu->state.fxsave));
include/asm/fpu/internal.h:     else if (IS_ENABLED(CONFIG_AS_FXSAVEQ))
include/asm/fpu/internal.h:             asm volatile("fxsaveq %[fx]" : [fx] "=m" (fpu->state.fxsave));
*/

int64_t syscall0(unsigned int syscall_num)
{
        int64_t result;
        asm volatile( "fxsave %[fx]" : [fx] "=m" (fxsave_region));
        __asm__ (".global _tos_syscall\ncallq _tos_syscall\n"
                : "=a" (result)
                : "0" (syscall_num)
                : "r11"
        );
        asm volatile( "frstor %[fx]" : [fx] "=m" (fxsave_region));
        return result;
}

int64_t syscall1(unsigned int syscall_num, void *p0)
{
        int64_t result;
        asm volatile( "fxsave %[fx]" : [fx] "=m" (fxsave_region));
        __asm__ volatile (".global _tos_syscall\ncallq _tos_syscall\n"
                : "=a" (result)
                : "0" (syscall_num), "D" (p0)
                : "r11", "rcx"
        );
        asm volatile( "frstor %[fx]" : [fx] "=m" (fxsave_region));
        return result;
}

int64_t syscall2(unsigned int syscall_num, void *p0, void *p1)
{
        int64_t result;
        asm volatile( "fxsave %[fx]" : [fx] "=m" (fxsave_region));
        __asm__ volatile (".global _tos_syscall\ncallq _tos_syscall\n"
                : "=a" (result)
                : "0" (syscall_num), "D" (p0), "S" (p1)
                : "r11", "rcx"
        );
        asm volatile( "frstor %[fx]" : [fx] "=m" (fxsave_region));
        return result;
}

int64_t syscall3(unsigned int syscall_num, void *p0, void *p1, void *p2)
{
        int64_t result;
        asm volatile( "fxsave %[fx]" : [fx] "=m" (fxsave_region));
        __asm__ volatile (".global _tos_syscall\ncallq _tos_syscall\n"
                : "=a" (result)
                : "0" (syscall_num), "D" (p0), "S" (p1), "d" (p2)
                : "r11", "rcx"
        );
        asm volatile( "frstor %[fx]" : [fx] "=m" (fxsave_region));
        return result;
}

int64_t syscall4(unsigned int syscall_num, void *p0, void *p1, void *p2, void *p3)
{
        int64_t result;
        asm volatile( "fxsave %[fx]" : [fx] "=m" (fxsave_region));
        register int64_t r10 asm("r10") = p3;   \
        __asm__ volatile (".global _tos_syscall\ncallq _tos_syscall\n"
                : "=a" (result)
                : "0" (syscall_num), "D" (p0), "S" (p1), "d" (p2), "r" (r10)
                : "r11", "rcx"
        );
        asm volatile( "frstor %[fx]" : [fx] "=m" (fxsave_region));
        return result;
}

STATIC mp_obj_t tos_get_rnd_int(void) {
    return mp_obj_new_int(syscall0(__NR_tos_get_rnd));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_get_rnd_int_obj, tos_get_rnd_int);

STATIC mp_obj_t tos_god_word(void) {
    char * godword=(char*)syscall0(__NR_tos_god_word_rnd);
    return mp_obj_new_str(godword,strlen(godword));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_god_word_obj, tos_god_word);

STATIC mp_obj_t tos_god_word_pick(void) {
    char * godword=(char*)syscall0(__NR_tos_god_word_pick);
    return mp_obj_new_str(godword,strlen(godword));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_god_word_pick_obj, tos_god_word_pick);

STATIC mp_obj_t tos_get_input(void) {
    char * input =(char*)syscall0(__NR_tos_get_input);
    return mp_obj_new_str(input,strlen(input));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_get_input_obj, tos_get_input);

STATIC mp_obj_t tos_popup_get_input(mp_obj_t header_in) {
    char * header = mp_obj_str_get_str(header_in);
    char * input=(char*)syscall1(__NR_tos_popup_get_input, (void*)header);
    return mp_obj_new_str(input,strlen(input));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_popup_get_input_obj, tos_popup_get_input);

STATIC mp_obj_t tos_exec_buf(mp_obj_t buf_in) {
    char * buf = mp_obj_str_get_str(buf_in);
    int64_t res=syscall1(__NR_tos_exec_buf, (void*)buf);
    return mp_obj_new_int(res);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_exec_buf_obj, tos_exec_buf);

STATIC mp_obj_t tos_popup_yn(mp_obj_t header_in, mp_obj_t footer_in) {
    char * header = mp_obj_str_get_str(header_in);
    char * footer = mp_obj_str_get_str(footer_in);
    int64_t result=syscall2(__NR_tos_popup_yn, (void*)header, (void*)footer);
    return mp_obj_new_int(result);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_popup_yn_obj, tos_popup_yn);

STATIC mp_obj_t tos_get_str(mp_obj_t addr) {
    const char * str = (const char*)mp_obj_get_int(addr);
    return mp_obj_new_str(str,strlen(str));
}


STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_get_str_obj, tos_get_str);


STATIC mp_obj_t tos_make_str(mp_obj_t buf);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_make_str_obj, tos_make_str);


STATIC mp_obj_t tos_get_uint64(mp_obj_t addr) {
    uint64_t result = *((uint64_t*)mp_obj_get_int(addr));
    return mp_obj_new_int(result);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_get_uint64_obj, tos_get_uint64);

STATIC mp_obj_t tos_put_uint64(mp_obj_t addr, mp_obj_t value_in) {
    uint64_t value = mp_obj_get_int(value_in);
    *((uint64_t*)mp_obj_get_int(addr)) = value;
    return 0;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_put_uint64_obj, tos_put_uint64);

STATIC mp_obj_t tos_get_uint8(mp_obj_t addr) {
    uint64_t result = *((uint8_t*)mp_obj_get_int(addr));
    return mp_obj_new_int(result);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_get_uint8_obj, tos_get_uint8);

STATIC mp_obj_t tos_put_uint8(mp_obj_t addr, mp_obj_t value_in) {
    uint8_t value = (uint8_t)mp_obj_get_int(value_in);
    *((uint8_t*)mp_obj_get_int(addr)) = value;
    return 0;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_put_uint8_obj, tos_put_uint8);

STATIC mp_obj_t tos_ptr_to_addr(mp_obj_t addr_in) {
    char *addr = (char*)mp_obj_get_int(addr_in);
    uint64_t result = (uint64_t)addr;
    return mp_obj_new_int(result);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ptr_to_addr_obj, tos_ptr_to_addr);


STATIC mp_obj_t tos_peek_uint8(mp_obj_t addr_in) {
    uint8_t *addr;
    addr = mp_obj_get_int(addr_in);
    uint8_t result = *addr;
    return mp_obj_new_int(result);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_peek_uint8_obj, tos_peek_uint8);

STATIC mp_obj_t tos_poke_uint8(mp_obj_t addr_in, mp_obj_t value_in) {
    char *addr;
    addr = mp_obj_get_int(addr_in);
    *addr = (uint8_t)mp_obj_get_int(value_in);
    return 0;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_poke_uint8_obj, tos_poke_uint8);

/*
STATIC mp_obj_t tos_bible_view(void) {
    syscall0(__NR_tos_bible_view);
    return 0;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_bible_view_obj, tos_bible_view);
*/

STATIC mp_obj_t tos_bible_passage(void) {
    syscall0(__NR_tos_bible_passage);
    return 0;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_bible_passage_obj, tos_bible_passage);

STATIC mp_obj_t tos_bible_passage_pick(void) {
    syscall0(__NR_tos_bible_passage_pick);
    return 0;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_bible_passage_pick_obj, tos_bible_passage_pick);

#define __NR__tos_ArcFinishCompression 4000
#define __NR__tos_AreYouSure 4001
#define __NR__tos_Blink 4002
#define __NR__tos_BlkDevLock 4003
#define __NR__tos_BlkDevUnlock 4004
#define __NR__tos_BlkRead 4005
#define __NR__tos_BlkWrite 4006
#define __NR__tos_BptR 4007
#define __NR__tos_BptS 4008
#define __NR__tos_BreakLock 4009
#define __NR__tos_BreakUnlock 4010
#define __NR__tos_ChkCodePtr 4011
#define __NR__tos_ChkOnStk 4012
#define __NR__tos_ChkPtr 4013
#define __NR__tos_DbgMode 4014
#define __NR__tos_DrvEnable 4015
#define __NR__tos_DrvLock 4016
#define __NR__tos_DrvMap 4017
#define __NR__tos_Drv 4018
#define __NR__tos_DrvUnlock 4019
#define __NR__tos_Echo 4020
#define __NR__tos_EdLite 4021
#define __NR__tos_E 4022
#define __NR__tos_FBlkRead 4023
#define __NR__tos_FBlkWrite 4024
#define __NR__tos_FifoI64Ins 4025
#define __NR__tos_FifoI64Peek 4026
#define __NR__tos_FifoI64Rem 4027
#define __NR__tos_FifoU8Ins 4028
#define __NR__tos_FifoU8Peek 4029
#define __NR__tos_FifoU8Rem 4030
#define __NR__tos_FileFind 4031
#define __NR__tos_FileNameChk 4032
#define __NR__tos_Fix 4033
#define __NR__tos_GetOption 4034
#define __NR__tos_IsDbgMode 4035
#define __NR__tos_IsDir 4036
#define __NR__tos_IsDotC 4037
#define __NR__tos_IsDotZ 4038
#define __NR__tos_IsMute 4039
#define __NR__tos_IsRaw 4040
#define __NR__tos_IsSilent 4041
#define __NR__tos_IsSingleUser 4042
#define __NR__tos_IsSuspended 4043
#define __NR__tos_IsSysDbg 4044
#define __NR__tos_JobResScan 4045
#define __NR__tos_Kill 4046
#define __NR__tos_Man 4047
#define __NR__tos_Mute 4048
#define __NR__tos_PutSrcLink 4049
#define __NR__tos_Raw 4050
#define __NR__tos_ScanKey 4051
#define __NR__tos_Silent 4052
#define __NR__tos_SingleUser 4053
#define __NR__tos_Suspend 4054
#define __NR__tos_SysDbg 4055
#define __NR__tos_TaskValidate 4056
#define __NR__tos_Trace 4057
#define __NR__tos_WildMatch 4058
#define __NR__tos_WinInside 4059
#define __NR__tos_YorN 4060
#define __NR__tos_CompressBuf 4061
#define __NR__tos_ArcCtrlNew 4062
#define __NR__tos_BlkDevChk 4063
#define __NR__tos_BlkDevNextFreeSlot 4064
#define __NR__tos_DrvIsWritable 4065
#define __NR__tos_Let2BlkDev 4066
#define __NR__tos_Str2ColorU16 4067
#define __NR__tos_Str2ColorU32 4068
#define __NR__tos_Now 4069
#define __NR__tos_Str2Date 4070
#define __NR__tos_Struct2Date 4071
#define __NR__tos_FilesFind 4072
#define __NR__tos_DrvChk 4073
#define __NR__tos_DrvMakeFreeSlot 4074
#define __NR__tos_Let2Drv 4075
#define __NR__tos_FifoI64New 4076
#define __NR__tos_FifoU8New 4077
#define __NR__tos_FOpen 4078
#define __NR__tos_DefineLoad 4079
#define __NR__tos_DefineLstLoad 4080
#define __NR__tos_FunSegFind 4081
#define __NR__tos_FramePtrAdd 4082
#define __NR__tos_HashPublic 4083
#define __NR__tos_HashTableNew 4084
#define __NR__tos_MHeapCtrl 4085
#define __NR__tos_TaskExe 4086
#define __NR__tos_TaskText 4087
#define __NR__tos_QueCopy 4088
#define __NR__tos_QueVectU8New 4089
#define __NR__tos_User 4090
#define __NR__tos_Clamp 4091
#define __NR__tos_Max 4092
#define __NR__tos_Min 4093
#define __NR__tos_Ona2Freq 4094
#define __NR__tos_Pow10I64 4095
#define __NR__tos_Str2F64 4096
#define __NR__tos_RandI16 4097
#define __NR__tos_RandI32 4098
#define __NR__tos_Adam 4099
#define __NR__tos_ATARep 4100
#define __NR__tos_B2 4101
#define __NR__tos_BCnt 4102
#define __NR__tos_BirthWait 4103
#define __NR__tos_CeilI64 4104
#define __NR__tos_Char2ScanCode 4105
#define __NR__tos_Clus2Blk 4106
#define __NR__tos_ClusBlkRead 4107
#define __NR__tos_ClusBlkWrite 4108
#define __NR__tos_ClusNumNext 4109
#define __NR__tos_ClusRead 4110
#define __NR__tos_ClusWrite 4111
#define __NR__tos_DayOfWeek 4112
#define __NR__tos_DefineCnt 4113
#define __NR__tos_DefineMatch 4114
#define __NR__tos_EndianI64 4115
#define __NR__tos_ExeFile 4116
#define __NR__tos_ExePrint 4117
#define __NR__tos_FifoI64Cnt 4118
#define __NR__tos_FifoU8Cnt 4119
#define __NR__tos_FirstDayOfMon 4120
#define __NR__tos_FirstDayOfYear 4121
#define __NR__tos_FloorI64 4122
#define __NR__tos_FlushMsgs 4123
#define __NR__tos_FramePtrDel 4124
#define __NR__tos_FramePtrSet 4125
#define __NR__tos_FramePtr 4126
#define __NR__tos_FSize 4127
#define __NR__tos_GetS 4128
#define __NR__tos_HashDefineLstAdd 4129
#define __NR__tos_HashEntrySize2 4130
#define __NR__tos_HashEntrySize 4131
#define __NR__tos_HashLstAdd 4132
#define __NR__tos_HashStr 4133
#define __NR__tos_HashTablePurge 4134
#define __NR__tos_HashTableSize2 4135
#define __NR__tos_HashTypeNum 4136
#define __NR__tos_HashVal 4137
#define __NR__tos_JobResGet 4138
#define __NR__tos_JobsHndlr 4139
#define __NR__tos_KbdMsEvtTime 4140
#define __NR__tos_LastDayOfMon 4141
#define __NR__tos_LastDayOfYear 4142
#define __NR__tos_Let2BlkDevType 4143
#define __NR__tos_LinkedLstCnt 4144
#define __NR__tos_LinkedLstSize 4145
#define __NR__tos_LstMatch 4146
#define __NR__tos_MemPageTable 4147
#define __NR__tos_MountIDEAuto 4148
#define __NR__tos_PopUpPrint 4149
#define __NR__tos_PopUp 4150
#define __NR__tos_PressAKey 4151
#define __NR__tos_QueCnt 4152
#define __NR__tos_QueSize 4153
#define __NR__tos_QueVectU8Get 4154
#define __NR__tos_RandI64 4155
#define __NR__tos_RoundI64 4156
#define __NR__tos_RunFile2 4157
#define __NR__tos_RunFile 4158
#define __NR__tos_Scale2Mem 4159
#define __NR__tos_ScanChar 4160
#define __NR__tos_Seed 4161
#define __NR__tos_Spaces2Tabs 4162
#define __NR__tos_SrcLineNum 4163
#define __NR__tos_Str2I64 4164
#define __NR__tos_StrCmp 4165
#define __NR__tos_StreamExePrint 4166
#define __NR__tos_StrICmp 4167
#define __NR__tos_StrNCmp 4168
#define __NR__tos_StrNICmp 4169
#define __NR__tos_StrOcc 4170
#define __NR__tos_Un 4171
#define __NR__tos_UnusedStk 4172
#define __NR__tos_YearStartDate 4173
#define __NR__tos_Freq2Ona 4174
#define __NR__tos_AdamErr 4175
#define __NR__tos_AdamLog 4176
#define __NR__tos_ArcCompressBuf 4177
#define __NR__tos_ArcCtrlDel 4178
#define __NR__tos_ArcExpandBuf 4179
#define __NR__tos_Beep 4180
#define __NR__tos_BlkDevDel 4181
#define __NR__tos_BlkDevsRelease 4182
#define __NR__tos_BlkPoolAdd 4183
#define __NR__tos_BlkPoolInit 4184
#define __NR__tos_BlkWriteZero 4185
#define __NR__tos_Break 4186
#define __NR__tos_CallerRep 4187
#define __NR__tos_Date2Struct 4188
#define __NR__tos_Da 4189
#define __NR__tos_Dbg 4190
#define __NR__tos_DeathWait 4191
#define __NR__tos_DefinePrint 4192
#define __NR__tos_DirContextDel 4193
#define __NR__tos_DirEntryDel2 4194
#define __NR__tos_DirEntryDel 4195
#define __NR__tos_DirTreeDel2 4196
#define __NR__tos_DirTreeDel 4197
#define __NR__tos_Dm 4198
#define __NR__tos_Dr 4199
#define __NR__tos_DrvDel 4200
#define __NR__tos_DrvRep 4201
#define __NR__tos_DrvsRelease 4202
#define __NR__tos_DskCacheInvalidate 4203
#define __NR__tos_DskChg 4204
#define __NR__tos_D 4205
#define __NR__tos_DVDImageRead 4206
#define __NR__tos_Exit 4207
#define __NR__tos_FClose 4208
#define __NR__tos_FifoI64Del 4209
#define __NR__tos_FifoI64Flush 4210
#define __NR__tos_FifoU8Del 4211
#define __NR__tos_FifoU8Flush 4212
#define __NR__tos_FixSet 4213
#define __NR__tos_Free 4214
#define __NR__tos_G2 4215
#define __NR__tos_GetOutOfDollar 4216
#define __NR__tos_GridInit 4217
#define __NR__tos_G 4218
#define __NR__tos_HashAdd 4219
#define __NR__tos_HashDel 4220
#define __NR__tos_HashTableDel 4221
#define __NR__tos_HeapCtrlDel 4222
#define __NR__tos_Help 4223
#define __NR__tos_HomeSet 4224
#define __NR__tos_InFile 4225
#define __NR__tos_InStr 4226
#define __NR__tos_In 4227
#define __NR__tos_JobDel 4228
#define __NR__tos_KbdTypeMatic 4229
#define __NR__tos_KeyDescSet 4230
#define __NR__tos_KeyDevRem 4231
#define __NR__tos_LinkedLstDel 4232
#define __NR__tos_Mem32DevFree 4233
#define __NR__tos_Mem64DevFree 4234
#define __NR__tos_MemPagFree 4235
#define __NR__tos_MPHalt 4236
#define __NR__tos_MPIntAll 4237
#define __NR__tos_MPInt 4238
#define __NR__tos_MPNMInt 4239
#define __NR__tos_Msg 4240
#define __NR__tos_Panic 4241
#define __NR__tos_PrintErr 4242
#define __NR__tos_Print 4243
#define __NR__tos_PrintWarn 4244
#define __NR__tos_ProgressBarsRst 4245
#define __NR__tos_PutChars 4246
#define __NR__tos_PutDirLink 4247
#define __NR__tos_PutExcept 4248
#define __NR__tos_PutKey 4249
#define __NR__tos_QueDel 4250
#define __NR__tos_QueVectU8Del 4251
#define __NR__tos_QueVectU8Put 4252
#define __NR__tos_RawD 4253
#define __NR__tos_RawDm 4254
#define __NR__tos_RawPrint 4255
#define __NR__tos_RawPutChar 4256
#define __NR__tos_Reboot 4257
#define __NR__tos_ScanFlags 4258
#define __NR__tos_Sleep 4259
#define __NR__tos_SleepUntil 4260
#define __NR__tos_Snd 4261
#define __NR__tos_SndRst 4262
#define __NR__tos_SrvTaskCont 4263
#define __NR__tos_StkRep 4264
#define __NR__tos_StrCpy 4265
#define __NR__tos_StreamPrint 4266
#define __NR__tos_S 4267
#define __NR__tos_TaskDerivedValsUpdate 4268
#define __NR__tos_TaskQueIns 4269
#define __NR__tos_TaskQueRem 4270
#define __NR__tos_TaskRstAwaitingMsg 4271
#define __NR__tos_TaskWait 4272
#define __NR__tos_throw 4273
#define __NR__tos_ToFileLine 4274
#define __NR__tos_TSSBusy 4275
#define __NR__tos_UserCmdLine 4276
#define __NR__tos_UserTaskCont 4277
#define __NR__tos_VGAFlush 4278
#define __NR__tos_WinDerivedValsUpdate 4279
#define __NR__tos_XTalk 4280
#define __NR__tos_XTalkStr 4281
#define __NR__tos_XTalkStrWait 4282
#define __NR__tos_XTalkWait 4283
#define __NR__tos_Yield 4284
#define __NR__tos_EndianU16 4285
#define __NR__tos_PCIReadU16 4286
#define __NR__tos_RandU16 4287
#define __NR__tos_EndianU32 4288
#define __NR__tos_PCIReadU32 4289
#define __NR__tos_RandU32 4290
#define __NR__tos_CeilU64 4291
#define __NR__tos_FloorU64 4292
#define __NR__tos_RandU64 4293
#define __NR__tos_ACAlloc 4294
#define __NR__tos_AMAlloc 4295
#define __NR__tos_AMAllocIdent 4296
#define __NR__tos_AStrNew 4297
#define __NR__tos_Caller 4298
#define __NR__tos_CAlloc 4299
#define __NR__tos_CatPrint 4300
#define __NR__tos_Char2KeyName 4301
#define __NR__tos_Color2Str 4302
#define __NR__tos_DefineSub 4303
#define __NR__tos_Define 4304
#define __NR__tos_DirFile 4305
#define __NR__tos_DirNameAbs 4306
#define __NR__tos_Drv2Let 4307
#define __NR__tos_DrvModelNum 4308
#define __NR__tos_DrvNextFreeLet 4309
#define __NR__tos_DrvSerialNum 4310
#define __NR__tos_DrvTextAttrGet 4311
#define __NR__tos_ExpandBuf 4312
#define __NR__tos_ExtChg 4313
#define __NR__tos_ExtDft 4314
#define __NR__tos_FileExtDot 4315
#define __NR__tos_FileExtRem 4316
#define __NR__tos_FileNameAbs 4317
#define __NR__tos_FileNameTmpTxt 4318
#define __NR__tos_FileRead 4319
#define __NR__tos_GetStr 4320
#define __NR__tos_IntEntryGet 4321
#define __NR__tos_Let2Let 4322
#define __NR__tos_LinkedLstCopy 4323
#define __NR__tos_LstSub 4324
#define __NR__tos_MAlloc 4325
#define __NR__tos_MAllocIdent 4326
#define __NR__tos_Mem32DevAlloc 4327
#define __NR__tos_Mem64DevAlloc 4328
#define __NR__tos_MemPagAlloc 4329
#define __NR__tos_MStrPrint 4330
#define __NR__tos_MStrUtil 4331
#define __NR__tos_PCIReadU8 4332
#define __NR__tos_ScaleIndent 4333
#define __NR__tos_ScanCode2Char 4334
#define __NR__tos_ScanCode2KeyName 4335
#define __NR__tos_SrcEdLink 4336
#define __NR__tos_SrcFileName 4337
#define __NR__tos_StrFind 4338
#define __NR__tos_StrFirstOcc 4339
#define __NR__tos_StrFirstRem 4340
#define __NR__tos_StrIMatch 4341
#define __NR__tos_StrLastOcc 4342
#define __NR__tos_StrLastRem 4343
#define __NR__tos_StrMatch 4344
#define __NR__tos_StrPrintFlags 4345
#define __NR__tos_StrPrintJoin 4346
#define __NR__tos_StrPrint 4347
#define __NR__tos_StrScan 4348
#define __NR__tos_StrUtil 4349
#define __NR__tos_Tabs2Spaces 4350
#define __NR__tos_ToggleZorNotZ 4351
#define __NR__tos_MemCmp 4352
#define __NR__tos_MemCpy 4353
#define __NR__tos_MemSet 4354
#define __NR__tos_MemSetI64 4355
#define __NR__tos_MemSetU16 4356
#define __NR__tos_MemSetU32 4357
#define __NR__tos_MemSetU8 4358
#define __NR__tos_StrLen 4359
#define __NR__tos_InU8 4360
#define __NR__tos_OutU8 4361
#define __NR__tos_Uf 4362
#define __NR__tos_U 4363
#define __NR__tos_StrNew 4364
#define __NR__tos_DelTree 4365
#define __NR__tos_CopyTree 4366
#define __NR__tos_Move 4367
#define __NR__tos_Copy 4368
#define __NR__tos_Del 4369
#define __NR__tos_Cd 4370
#define __NR__tos_Ed 4371
#define __NR__tos_Dir 4372
#define __NR__tos_DirMk 4373
#define __NR__tos_DirCur 4374
#define __NR__tos_DocWrite 4375


STATIC mp_obj_t tos_ArcFinishCompression(mp_obj_t c) {
void* p0 = mp_obj_get_int(c);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_ArcFinishCompression,p0));
}

STATIC mp_obj_t tos_AreYouSure(void) {
return mp_obj_new_int((unsigned char)syscall0(__NR__tos_AreYouSure));
}

STATIC mp_obj_t tos_Blink(mp_obj_t Hz) {
int64_t p0 = mp_obj_get_int(Hz);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Blink,p0));
}

STATIC mp_obj_t tos_BlkDevLock(mp_obj_t bd) {
void* p0 = mp_obj_get_int(bd);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_BlkDevLock,p0));
}

STATIC mp_obj_t tos_BlkDevUnlock(mp_obj_t bd) {
void* p0 = mp_obj_get_int(bd);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_BlkDevUnlock,p0));
}

STATIC mp_obj_t tos_BlkRead(mp_obj_t dv,mp_obj_t buf,mp_obj_t blk,mp_obj_t cnt) {
void* p0 = mp_obj_get_int(dv);
int64_t *p1;
if (mp_obj_is_str_or_bytes(buf)) {
p1 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p1 = (int64_t *)mp_obj_get_int(buf);
}
int64_t p2 = mp_obj_get_int(blk);
int64_t p3 = mp_obj_get_int(cnt);
return mp_obj_new_int((unsigned char)syscall4(__NR__tos_BlkRead,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_BlkWrite(mp_obj_t dv,mp_obj_t buf,mp_obj_t blk,mp_obj_t cnt) {
void* p0 = mp_obj_get_int(dv);
int64_t *p1;
if (mp_obj_is_str_or_bytes(buf)) {
p1 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p1 = (int64_t *)mp_obj_get_int(buf);
}
int64_t p2 = mp_obj_get_int(blk);
int64_t p3 = mp_obj_get_int(cnt);
return mp_obj_new_int((unsigned char)syscall4(__NR__tos_BlkWrite,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_BptR(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_BptR,p0));
}

STATIC mp_obj_t tos_BptS(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_BptS,p0));
}

STATIC mp_obj_t tos_BreakLock(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_BreakLock,p0));
}

STATIC mp_obj_t tos_BreakUnlock(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_BreakUnlock,p0));
}

STATIC mp_obj_t tos_ChkCodePtr(mp_obj_t ptr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(ptr)) {
p0 = (int64_t *)mp_obj_str_get_str(ptr);
} else {
p0 = (int64_t *)mp_obj_get_int(ptr);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_ChkCodePtr,p0));
}

STATIC mp_obj_t tos_ChkOnStk(mp_obj_t ptr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(ptr)) {
p0 = (int64_t *)mp_obj_str_get_str(ptr);
} else {
p0 = (int64_t *)mp_obj_get_int(ptr);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_ChkOnStk,p0));
}

STATIC mp_obj_t tos_ChkPtr(mp_obj_t ptr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(ptr)) {
p0 = (int64_t *)mp_obj_str_get_str(ptr);
} else {
p0 = (int64_t *)mp_obj_get_int(ptr);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_ChkPtr,p0));
}

STATIC mp_obj_t tos_DbgMode(mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_DbgMode,p0));
}

STATIC mp_obj_t tos_DrvEnable(mp_obj_t drv_let,mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(drv_let);
unsigned char p1 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_DrvEnable,p0,p1));
}

STATIC mp_obj_t tos_DrvLock(mp_obj_t dv) {
void* p0 = mp_obj_get_int(dv);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_DrvLock,p0));
}

STATIC mp_obj_t tos_DrvMap(mp_obj_t drv_let,mp_obj_t dv) {
unsigned char p0 = mp_obj_get_int(drv_let);
void* p1 = mp_obj_get_int(dv);
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_DrvMap,p0,p1));
}

STATIC mp_obj_t tos_Drv(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Drv,p0));
}

STATIC mp_obj_t tos_DrvUnlock(mp_obj_t dv) {
void* p0 = mp_obj_get_int(dv);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_DrvUnlock,p0));
}

STATIC mp_obj_t tos_Echo(mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Echo,p0));
}

STATIC mp_obj_t tos_EdLite(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_EdLite,p0));
}

STATIC mp_obj_t tos_E(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_E,p0));
}

STATIC mp_obj_t tos_FBlkRead(mp_obj_t f,mp_obj_t buf) {
void* p0 = mp_obj_get_int(f);
int64_t *p1;
if (mp_obj_is_str_or_bytes(buf)) {
p1 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p1 = (int64_t *)mp_obj_get_int(buf);
}
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_FBlkRead,p0,p1));
}

STATIC mp_obj_t tos_FBlkWrite(mp_obj_t f,mp_obj_t buf) {
void* p0 = mp_obj_get_int(f);
int64_t *p1;
if (mp_obj_is_str_or_bytes(buf)) {
p1 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p1 = (int64_t *)mp_obj_get_int(buf);
}
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_FBlkWrite,p0,p1));
}

STATIC mp_obj_t tos_FifoI64Ins(mp_obj_t f,mp_obj_t q) {
void* p0 = mp_obj_get_int(f);
int64_t p1 = mp_obj_get_int(q);
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_FifoI64Ins,p0,p1));
}

STATIC mp_obj_t tos_FifoI64Peek(mp_obj_t f,mp_obj_t _q) {
void* p0 = mp_obj_get_int(f);
void* p1 = mp_obj_get_int(_q);
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_FifoI64Peek,p0,p1));
}

STATIC mp_obj_t tos_FifoI64Rem(mp_obj_t f,mp_obj_t _q) {
void* p0 = mp_obj_get_int(f);
void* p1 = mp_obj_get_int(_q);
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_FifoI64Rem,p0,p1));
}

STATIC mp_obj_t tos_FifoU8Ins(mp_obj_t f,mp_obj_t b) {
void* p0 = mp_obj_get_int(f);
unsigned char p1 = mp_obj_get_int(b);
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_FifoU8Ins,p0,p1));
}

STATIC mp_obj_t tos_FifoU8Peek(mp_obj_t f,mp_obj_t _b) {
void* p0 = mp_obj_get_int(f);
int64_t *p1;
if (mp_obj_is_str_or_bytes(_b)) {
p1 = (int64_t *)mp_obj_str_get_str(_b);
} else {
p1 = (int64_t *)mp_obj_get_int(_b);
}
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_FifoU8Peek,p0,p1));
}

STATIC mp_obj_t tos_FifoU8Rem(mp_obj_t f,mp_obj_t _b) {
void* p0 = mp_obj_get_int(f);
int64_t *p1;
if (mp_obj_is_str_or_bytes(_b)) {
p1 = (int64_t *)mp_obj_str_get_str(_b);
} else {
p1 = (int64_t *)mp_obj_get_int(_b);
}
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_FifoU8Rem,p0,p1));
}

STATIC mp_obj_t tos_FileFind(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_FileFind,p0));
}

STATIC mp_obj_t tos_FileNameChk(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_FileNameChk,p0));
}

STATIC mp_obj_t tos_Fix(mp_obj_t edf_dof_flags) {
int64_t p0 = mp_obj_get_int(edf_dof_flags);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Fix,p0));
}

STATIC mp_obj_t tos_GetOption(mp_obj_t num) {
int64_t p0 = mp_obj_get_int(num);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_GetOption,p0));
}

STATIC mp_obj_t tos_IsDbgMode(void) {
return mp_obj_new_int((unsigned char)syscall0(__NR__tos_IsDbgMode));
}

STATIC mp_obj_t tos_IsDir(mp_obj_t dir_name) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dir_name)) {
p0 = (int64_t *)mp_obj_str_get_str(dir_name);
} else {
p0 = (int64_t *)mp_obj_get_int(dir_name);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_IsDir,p0));
}

STATIC mp_obj_t tos_IsDotC(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_IsDotC,p0));
}

STATIC mp_obj_t tos_IsDotZ(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_IsDotZ,p0));
}

STATIC mp_obj_t tos_IsMute(void) {
return mp_obj_new_int((unsigned char)syscall0(__NR__tos_IsMute));
}

STATIC mp_obj_t tos_IsRaw(void) {
return mp_obj_new_int((unsigned char)syscall0(__NR__tos_IsRaw));
}

STATIC mp_obj_t tos_IsSilent(void) {
return mp_obj_new_int((unsigned char)syscall0(__NR__tos_IsSilent));
}

STATIC mp_obj_t tos_IsSingleUser(void) {
return mp_obj_new_int((unsigned char)syscall0(__NR__tos_IsSingleUser));
}

STATIC mp_obj_t tos_IsSuspended(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_IsSuspended,p0));
}

STATIC mp_obj_t tos_IsSysDbg(void) {
return mp_obj_new_int((unsigned char)syscall0(__NR__tos_IsSysDbg));
}

STATIC mp_obj_t tos_JobResScan(mp_obj_t rqst) {
void* p0 = mp_obj_get_int(rqst);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_JobResScan,p0));
}

STATIC mp_obj_t tos_Kill(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Kill,p0));
}

STATIC mp_obj_t tos_Man(mp_obj_t st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st)) {
p0 = (int64_t *)mp_obj_str_get_str(st);
} else {
p0 = (int64_t *)mp_obj_get_int(st);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Man,p0));
}

STATIC mp_obj_t tos_Mute(mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Mute,p0));
}

STATIC mp_obj_t tos_PutSrcLink(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_PutSrcLink,p0));
}

STATIC mp_obj_t tos_Raw(mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Raw,p0));
}

STATIC mp_obj_t tos_ScanKey(mp_obj_t _ch) {
void* p0 = mp_obj_get_int(_ch);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_ScanKey,p0));
}

STATIC mp_obj_t tos_Silent(mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Silent,p0));
}

STATIC mp_obj_t tos_SingleUser(mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_SingleUser,p0));
}

STATIC mp_obj_t tos_Suspend(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Suspend,p0));
}

STATIC mp_obj_t tos_SysDbg(mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_SysDbg,p0));
}

STATIC mp_obj_t tos_TaskValidate(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_TaskValidate,p0));
}

STATIC mp_obj_t tos_Trace(mp_obj_t val) {
unsigned char p0 = mp_obj_get_int(val);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Trace,p0));
}

STATIC mp_obj_t tos_WildMatch(mp_obj_t test_str,mp_obj_t wild_str) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(test_str)) {
p0 = (int64_t *)mp_obj_str_get_str(test_str);
} else {
p0 = (int64_t *)mp_obj_get_int(test_str);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(wild_str)) {
p1 = (int64_t *)mp_obj_str_get_str(wild_str);
} else {
p1 = (int64_t *)mp_obj_get_int(wild_str);
}
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_WildMatch,p0,p1));
}

STATIC mp_obj_t tos_WinInside(mp_obj_t x,mp_obj_t y) {
int64_t p0 = mp_obj_get_int(x);
int64_t p1 = mp_obj_get_int(y);
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_WinInside,p0,p1));
}

STATIC mp_obj_t tos_YorN(void) {
return mp_obj_new_int((unsigned char)syscall0(__NR__tos_YorN));
}

STATIC mp_obj_t tos_CompressBuf(mp_obj_t src,mp_obj_t size) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
int64_t p1 = mp_obj_get_int(size);
return mp_obj_new_int((int64_t*)syscall2(__NR__tos_CompressBuf,p0,p1));
}

STATIC mp_obj_t tos_ArcCtrlNew(mp_obj_t expand) {
unsigned char p0 = mp_obj_get_int(expand);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_ArcCtrlNew,p0));
}

STATIC mp_obj_t tos_BlkDevChk(mp_obj_t bd) {
void* p0 = mp_obj_get_int(bd);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_BlkDevChk,p0));
}

STATIC mp_obj_t tos_BlkDevNextFreeSlot(mp_obj_t first_drv_let,mp_obj_t type) {
unsigned char p0 = mp_obj_get_int(first_drv_let);
int64_t p1 = mp_obj_get_int(type);
return mp_obj_new_int((int64_t*)syscall2(__NR__tos_BlkDevNextFreeSlot,p0,p1));
}

STATIC mp_obj_t tos_DrvIsWritable(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_DrvIsWritable,p0));
}

STATIC mp_obj_t tos_Let2BlkDev(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_Let2BlkDev,p0));
}

STATIC mp_obj_t tos_Str2ColorU16(mp_obj_t st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st)) {
p0 = (int64_t *)mp_obj_str_get_str(st);
} else {
p0 = (int64_t *)mp_obj_get_int(st);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Str2ColorU16,p0));
}

STATIC mp_obj_t tos_Str2ColorU32(mp_obj_t st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st)) {
p0 = (int64_t *)mp_obj_str_get_str(st);
} else {
p0 = (int64_t *)mp_obj_get_int(st);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Str2ColorU32,p0));
}

STATIC mp_obj_t tos_Now(void) {
return mp_obj_new_int((int64_t)syscall0(__NR__tos_Now));
}

STATIC mp_obj_t tos_Str2Date(mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Str2Date,p0));
}

STATIC mp_obj_t tos_Struct2Date(mp_obj_t _ds) {
void* p0 = mp_obj_get_int(_ds);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Struct2Date,p0));
}

STATIC mp_obj_t tos_FilesFind(mp_obj_t files_find_mask) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(files_find_mask)) {
p0 = (int64_t *)mp_obj_str_get_str(files_find_mask);
} else {
p0 = (int64_t *)mp_obj_get_int(files_find_mask);
}
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_FilesFind,p0));
}

STATIC mp_obj_t tos_DrvChk(mp_obj_t dv) {
void* p0 = mp_obj_get_int(dv);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_DrvChk,p0));
}

STATIC mp_obj_t tos_DrvMakeFreeSlot(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_DrvMakeFreeSlot,p0));
}

STATIC mp_obj_t tos_Let2Drv(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_Let2Drv,p0));
}

STATIC mp_obj_t tos_FifoI64New(mp_obj_t size) {
int64_t p0 = mp_obj_get_int(size);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_FifoI64New,p0));
}

STATIC mp_obj_t tos_FifoU8New(mp_obj_t size) {
int64_t p0 = mp_obj_get_int(size);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_FifoU8New,p0));
}

STATIC mp_obj_t tos_FOpen(mp_obj_t filename,mp_obj_t flags) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(flags)) {
p1 = (int64_t *)mp_obj_str_get_str(flags);
} else {
p1 = (int64_t *)mp_obj_get_int(flags);
}
return mp_obj_new_int((int64_t*)syscall2(__NR__tos_FOpen,p0,p1));
}

STATIC mp_obj_t tos_DefineLoad(mp_obj_t dname,mp_obj_t st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dname)) {
p0 = (int64_t *)mp_obj_str_get_str(dname);
} else {
p0 = (int64_t *)mp_obj_get_int(dname);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(st)) {
p1 = (int64_t *)mp_obj_str_get_str(st);
} else {
p1 = (int64_t *)mp_obj_get_int(st);
}
return mp_obj_new_int((int64_t*)syscall2(__NR__tos_DefineLoad,p0,p1));
}

STATIC mp_obj_t tos_DefineLstLoad(mp_obj_t dname,mp_obj_t lst) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dname)) {
p0 = (int64_t *)mp_obj_str_get_str(dname);
} else {
p0 = (int64_t *)mp_obj_get_int(dname);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(lst)) {
p1 = (int64_t *)mp_obj_str_get_str(lst);
} else {
p1 = (int64_t *)mp_obj_get_int(lst);
}
return mp_obj_new_int((int64_t*)syscall2(__NR__tos_DefineLstLoad,p0,p1));
}

STATIC mp_obj_t tos_FunSegFind(mp_obj_t addr,mp_obj_t _offset) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
void* p1 = mp_obj_get_int(_offset);
return mp_obj_new_int((int64_t*)syscall2(__NR__tos_FunSegFind,p0,p1));
}

STATIC mp_obj_t tos_FramePtrAdd(mp_obj_t name) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(name)) {
p0 = (int64_t *)mp_obj_str_get_str(name);
} else {
p0 = (int64_t *)mp_obj_get_int(name);
}
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_FramePtrAdd,p0));
}

STATIC mp_obj_t tos_HashPublic(mp_obj_t st,mp_obj_t mask) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st)) {
p0 = (int64_t *)mp_obj_str_get_str(st);
} else {
p0 = (int64_t *)mp_obj_get_int(st);
}
int64_t p1 = mp_obj_get_int(mask);
return mp_obj_new_int((int64_t*)syscall2(__NR__tos_HashPublic,p0,p1));
}

STATIC mp_obj_t tos_HashTableNew(mp_obj_t size) {
int64_t p0 = mp_obj_get_int(size);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_HashTableNew,p0));
}

STATIC mp_obj_t tos_MHeapCtrl(mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_MHeapCtrl,p0));
}

STATIC mp_obj_t tos_TaskExe(mp_obj_t srv,mp_obj_t master,mp_obj_t data,mp_obj_t flags) {
void* p0 = mp_obj_get_int(srv);
void* p1 = mp_obj_get_int(master);
int64_t *p2;
if (mp_obj_is_str_or_bytes(data)) {
p2 = (int64_t *)mp_obj_str_get_str(data);
} else {
p2 = (int64_t *)mp_obj_get_int(data);
}
int64_t p3 = mp_obj_get_int(flags);
return mp_obj_new_int((int64_t*)syscall4(__NR__tos_TaskExe,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_TaskText(mp_obj_t srv,mp_obj_t master,mp_obj_t data,mp_obj_t flags) {
void* p0 = mp_obj_get_int(srv);
void* p1 = mp_obj_get_int(master);
int64_t *p2;
if (mp_obj_is_str_or_bytes(data)) {
p2 = (int64_t *)mp_obj_str_get_str(data);
} else {
p2 = (int64_t *)mp_obj_get_int(data);
}
int64_t p3 = mp_obj_get_int(flags);
return mp_obj_new_int((int64_t*)syscall4(__NR__tos_TaskText,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_QueCopy(mp_obj_t head) {
void* p0 = mp_obj_get_int(head);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_QueCopy,p0));
}

STATIC mp_obj_t tos_QueVectU8New(mp_obj_t min_idx) {
int64_t p0 = mp_obj_get_int(min_idx);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_QueVectU8New,p0));
}

STATIC mp_obj_t tos_User(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_User,p0));
}

STATIC mp_obj_t tos_Clamp(mp_obj_t d,mp_obj_t lo,mp_obj_t hi) {
int64_t p0 = mp_obj_get_int(d);
int64_t p1 = mp_obj_get_int(lo);
int64_t p2 = mp_obj_get_int(hi);
return mp_obj_new_int((int64_t)syscall3(__NR__tos_Clamp,p0,p1,p2));
}

STATIC mp_obj_t tos_Max(mp_obj_t n1,mp_obj_t n2) {
int64_t p0 = mp_obj_get_int(n1);
int64_t p1 = mp_obj_get_int(n2);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_Max,p0,p1));
}

STATIC mp_obj_t tos_Min(mp_obj_t n1,mp_obj_t n2) {
int64_t p0 = mp_obj_get_int(n1);
int64_t p1 = mp_obj_get_int(n2);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_Min,p0,p1));
}

STATIC mp_obj_t tos_Ona2Freq(mp_obj_t ona) {
int64_t p0 = mp_obj_get_int(ona);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Ona2Freq,p0));
}

STATIC mp_obj_t tos_Pow10I64(mp_obj_t i) {
int64_t p0 = mp_obj_get_int(i);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Pow10I64,p0));
}

STATIC mp_obj_t tos_Str2F64(mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Str2F64,p0));
}

STATIC mp_obj_t tos_RandI16(void) {
return mp_obj_new_int((int16_t)syscall0(__NR__tos_RandI16));
}

STATIC mp_obj_t tos_RandI32(void) {
return mp_obj_new_int((int32_t)syscall0(__NR__tos_RandI32));
}

STATIC mp_obj_t tos_Adam(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Adam,p0));
}

STATIC mp_obj_t tos_ATARep(mp_obj_t pmt) {
unsigned char p0 = mp_obj_get_int(pmt);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_ATARep,p0));
}

STATIC mp_obj_t tos_B2(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_B2,p0));
}

STATIC mp_obj_t tos_BCnt(mp_obj_t d) {
int64_t p0 = mp_obj_get_int(d);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_BCnt,p0));
}

STATIC mp_obj_t tos_BirthWait(mp_obj_t _task) {
void* p0 = mp_obj_get_int(_task);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_BirthWait,p0));
}

STATIC mp_obj_t tos_CeilI64(mp_obj_t num,mp_obj_t to) {
int64_t p0 = mp_obj_get_int(num);
int64_t p1 = mp_obj_get_int(to);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_CeilI64,p0,p1));
}

STATIC mp_obj_t tos_Char2ScanCode(mp_obj_t ch) {
int64_t p0 = mp_obj_get_int(ch);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Char2ScanCode,p0));
}

STATIC mp_obj_t tos_Clus2Blk(mp_obj_t dv,mp_obj_t c) {
void* p0 = mp_obj_get_int(dv);
int64_t p1 = mp_obj_get_int(c);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_Clus2Blk,p0,p1));
}

STATIC mp_obj_t tos_ClusBlkRead(mp_obj_t dv,mp_obj_t buf,mp_obj_t c,mp_obj_t blks) {
void* p0 = mp_obj_get_int(dv);
int64_t *p1;
if (mp_obj_is_str_or_bytes(buf)) {
p1 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p1 = (int64_t *)mp_obj_get_int(buf);
}
int64_t p2 = mp_obj_get_int(c);
int64_t p3 = mp_obj_get_int(blks);
return mp_obj_new_int((int64_t)syscall4(__NR__tos_ClusBlkRead,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_ClusBlkWrite(mp_obj_t dv,mp_obj_t buf,mp_obj_t c,mp_obj_t blks) {
void* p0 = mp_obj_get_int(dv);
int64_t *p1;
if (mp_obj_is_str_or_bytes(buf)) {
p1 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p1 = (int64_t *)mp_obj_get_int(buf);
}
int64_t p2 = mp_obj_get_int(c);
int64_t p3 = mp_obj_get_int(blks);
return mp_obj_new_int((int64_t)syscall4(__NR__tos_ClusBlkWrite,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_ClusNumNext(mp_obj_t dv,mp_obj_t c) {
void* p0 = mp_obj_get_int(dv);
int64_t p1 = mp_obj_get_int(c);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_ClusNumNext,p0,p1));
}

STATIC mp_obj_t tos_ClusRead(mp_obj_t dv,mp_obj_t buf,mp_obj_t c,mp_obj_t cnt) {
void* p0 = mp_obj_get_int(dv);
int64_t *p1;
if (mp_obj_is_str_or_bytes(buf)) {
p1 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p1 = (int64_t *)mp_obj_get_int(buf);
}
int64_t p2 = mp_obj_get_int(c);
int64_t p3 = mp_obj_get_int(cnt);
return mp_obj_new_int((int64_t)syscall4(__NR__tos_ClusRead,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_ClusWrite(mp_obj_t dv,mp_obj_t buf,mp_obj_t c,mp_obj_t cnt) {
void* p0 = mp_obj_get_int(dv);
int64_t *p1;
if (mp_obj_is_str_or_bytes(buf)) {
p1 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p1 = (int64_t *)mp_obj_get_int(buf);
}
int64_t p2 = mp_obj_get_int(c);
int64_t p3 = mp_obj_get_int(cnt);
return mp_obj_new_int((int64_t)syscall4(__NR__tos_ClusWrite,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_DayOfWeek(mp_obj_t i) {
int64_t p0 = mp_obj_get_int(i);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_DayOfWeek,p0));
}

STATIC mp_obj_t tos_DefineCnt(mp_obj_t dname) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dname)) {
p0 = (int64_t *)mp_obj_str_get_str(dname);
} else {
p0 = (int64_t *)mp_obj_get_int(dname);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_DefineCnt,p0));
}

STATIC mp_obj_t tos_DefineMatch(mp_obj_t needle,mp_obj_t haystack_lst_dname) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(needle)) {
p0 = (int64_t *)mp_obj_str_get_str(needle);
} else {
p0 = (int64_t *)mp_obj_get_int(needle);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(haystack_lst_dname)) {
p1 = (int64_t *)mp_obj_str_get_str(haystack_lst_dname);
} else {
p1 = (int64_t *)mp_obj_get_int(haystack_lst_dname);
}
return mp_obj_new_int((int64_t)syscall2(__NR__tos_DefineMatch,p0,p1));
}

STATIC mp_obj_t tos_EndianI64(mp_obj_t d) {
int64_t p0 = mp_obj_get_int(d);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_EndianI64,p0));
}

STATIC mp_obj_t tos_ExeFile(mp_obj_t name) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(name)) {
p0 = (int64_t *)mp_obj_str_get_str(name);
} else {
p0 = (int64_t *)mp_obj_get_int(name);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_ExeFile,p0));
}

STATIC mp_obj_t tos_ExePrint(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_ExePrint,p0));
}

STATIC mp_obj_t tos_FifoI64Cnt(mp_obj_t f) {
void* p0 = mp_obj_get_int(f);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_FifoI64Cnt,p0));
}

STATIC mp_obj_t tos_FifoU8Cnt(mp_obj_t f) {
void* p0 = mp_obj_get_int(f);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_FifoU8Cnt,p0));
}

STATIC mp_obj_t tos_FirstDayOfMon(mp_obj_t i) {
int64_t p0 = mp_obj_get_int(i);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_FirstDayOfMon,p0));
}

STATIC mp_obj_t tos_FirstDayOfYear(mp_obj_t i) {
int64_t p0 = mp_obj_get_int(i);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_FirstDayOfYear,p0));
}

STATIC mp_obj_t tos_FloorI64(mp_obj_t num,mp_obj_t to) {
int64_t p0 = mp_obj_get_int(num);
int64_t p1 = mp_obj_get_int(to);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_FloorI64,p0,p1));
}

STATIC mp_obj_t tos_FlushMsgs(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_FlushMsgs,p0));
}

STATIC mp_obj_t tos_FramePtrDel(mp_obj_t name) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(name)) {
p0 = (int64_t *)mp_obj_str_get_str(name);
} else {
p0 = (int64_t *)mp_obj_get_int(name);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_FramePtrDel,p0));
}

STATIC mp_obj_t tos_FramePtrSet(mp_obj_t name,mp_obj_t val) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(name)) {
p0 = (int64_t *)mp_obj_str_get_str(name);
} else {
p0 = (int64_t *)mp_obj_get_int(name);
}
int64_t p1 = mp_obj_get_int(val);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_FramePtrSet,p0,p1));
}

STATIC mp_obj_t tos_FramePtr(mp_obj_t name) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(name)) {
p0 = (int64_t *)mp_obj_str_get_str(name);
} else {
p0 = (int64_t *)mp_obj_get_int(name);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_FramePtr,p0));
}

STATIC mp_obj_t tos_FSize(mp_obj_t f) {
void* p0 = mp_obj_get_int(f);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_FSize,p0));
}

STATIC mp_obj_t tos_GetS(mp_obj_t buf,mp_obj_t size) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(buf)) {
p0 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p0 = (int64_t *)mp_obj_get_int(buf);
}
int64_t p1 = mp_obj_get_int(size);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_GetS,p0,p1));
}

STATIC mp_obj_t tos_HashDefineLstAdd(mp_obj_t dname,mp_obj_t type,mp_obj_t table) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dname)) {
p0 = (int64_t *)mp_obj_str_get_str(dname);
} else {
p0 = (int64_t *)mp_obj_get_int(dname);
}
int64_t p1 = mp_obj_get_int(type);
void* p2 = mp_obj_get_int(table);
return mp_obj_new_int((int64_t)syscall3(__NR__tos_HashDefineLstAdd,p0,p1,p2));
}

STATIC mp_obj_t tos_HashEntrySize2(mp_obj_t tmph) {
void* p0 = mp_obj_get_int(tmph);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_HashEntrySize2,p0));
}

STATIC mp_obj_t tos_HashEntrySize(mp_obj_t tmph) {
void* p0 = mp_obj_get_int(tmph);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_HashEntrySize,p0));
}

STATIC mp_obj_t tos_HashLstAdd(mp_obj_t lst,mp_obj_t type,mp_obj_t table) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(lst)) {
p0 = (int64_t *)mp_obj_str_get_str(lst);
} else {
p0 = (int64_t *)mp_obj_get_int(lst);
}
int64_t p1 = mp_obj_get_int(type);
void* p2 = mp_obj_get_int(table);
return mp_obj_new_int((int64_t)syscall3(__NR__tos_HashLstAdd,p0,p1,p2));
}

STATIC mp_obj_t tos_HashStr(mp_obj_t st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st)) {
p0 = (int64_t *)mp_obj_str_get_str(st);
} else {
p0 = (int64_t *)mp_obj_get_int(st);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_HashStr,p0));
}

STATIC mp_obj_t tos_HashTablePurge(mp_obj_t table) {
void* p0 = mp_obj_get_int(table);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_HashTablePurge,p0));
}

STATIC mp_obj_t tos_HashTableSize2(mp_obj_t table) {
void* p0 = mp_obj_get_int(table);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_HashTableSize2,p0));
}

STATIC mp_obj_t tos_HashTypeNum(mp_obj_t tmph) {
void* p0 = mp_obj_get_int(tmph);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_HashTypeNum,p0));
}

STATIC mp_obj_t tos_HashVal(mp_obj_t tmph) {
void* p0 = mp_obj_get_int(tmph);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_HashVal,p0));
}

STATIC mp_obj_t tos_JobResGet(mp_obj_t rqst) {
void* p0 = mp_obj_get_int(rqst);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_JobResGet,p0));
}

STATIC mp_obj_t tos_JobsHndlr(mp_obj_t run_flags) {
int64_t p0 = mp_obj_get_int(run_flags);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_JobsHndlr,p0));
}

STATIC mp_obj_t tos_KbdMsEvtTime(void) {
return mp_obj_new_int((int64_t)syscall0(__NR__tos_KbdMsEvtTime));
}

STATIC mp_obj_t tos_LastDayOfMon(mp_obj_t i) {
int64_t p0 = mp_obj_get_int(i);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_LastDayOfMon,p0));
}

STATIC mp_obj_t tos_LastDayOfYear(mp_obj_t i) {
int64_t p0 = mp_obj_get_int(i);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_LastDayOfYear,p0));
}

STATIC mp_obj_t tos_Let2BlkDevType(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Let2BlkDevType,p0));
}

STATIC mp_obj_t tos_LinkedLstCnt(mp_obj_t _lst) {
void* p0 = mp_obj_get_int(_lst);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_LinkedLstCnt,p0));
}

STATIC mp_obj_t tos_LinkedLstSize(mp_obj_t _lst) {
void* p0 = mp_obj_get_int(_lst);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_LinkedLstSize,p0));
}

STATIC mp_obj_t tos_LstMatch(mp_obj_t needle,mp_obj_t haystack_lst) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(needle)) {
p0 = (int64_t *)mp_obj_str_get_str(needle);
} else {
p0 = (int64_t *)mp_obj_get_int(needle);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(haystack_lst)) {
p1 = (int64_t *)mp_obj_str_get_str(haystack_lst);
} else {
p1 = (int64_t *)mp_obj_get_int(haystack_lst);
}
return mp_obj_new_int((int64_t)syscall2(__NR__tos_LstMatch,p0,p1));
}

STATIC mp_obj_t tos_MemPageTable(mp_obj_t a) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(a)) {
p0 = (int64_t *)mp_obj_str_get_str(a);
} else {
p0 = (int64_t *)mp_obj_get_int(a);
}
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_MemPageTable,p0));
}

STATIC mp_obj_t tos_MountIDEAuto(void) {
return mp_obj_new_int((int64_t)syscall0(__NR__tos_MountIDEAuto));
}

STATIC mp_obj_t tos_PopUpPrint(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_PopUpPrint,p0));
}

STATIC mp_obj_t tos_PopUp(mp_obj_t buf) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(buf)) {
p0 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p0 = (int64_t *)mp_obj_get_int(buf);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_PopUp,p0));
}

STATIC mp_obj_t tos_PressAKey(void) {
return mp_obj_new_int((int64_t)syscall0(__NR__tos_PressAKey));
}

STATIC mp_obj_t tos_QueCnt(mp_obj_t head) {
void* p0 = mp_obj_get_int(head);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_QueCnt,p0));
}

STATIC mp_obj_t tos_QueSize(mp_obj_t head) {
void* p0 = mp_obj_get_int(head);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_QueSize,p0));
}

STATIC mp_obj_t tos_QueVectU8Get(mp_obj_t v,mp_obj_t idx) {
void* p0 = mp_obj_get_int(v);
int64_t p1 = mp_obj_get_int(idx);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_QueVectU8Get,p0,p1));
}

STATIC mp_obj_t tos_RandI64(void) {
return mp_obj_new_int((int64_t)syscall0(__NR__tos_RandI64));
}

STATIC mp_obj_t tos_RoundI64(mp_obj_t num,mp_obj_t to) {
int64_t p0 = mp_obj_get_int(num);
int64_t p1 = mp_obj_get_int(to);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_RoundI64,p0,p1));
}

STATIC mp_obj_t tos_RunFile2(mp_obj_t name) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(name)) {
p0 = (int64_t *)mp_obj_str_get_str(name);
} else {
p0 = (int64_t *)mp_obj_get_int(name);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_RunFile2,p0));
}

STATIC mp_obj_t tos_RunFile(mp_obj_t name) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(name)) {
p0 = (int64_t *)mp_obj_str_get_str(name);
} else {
p0 = (int64_t *)mp_obj_get_int(name);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_RunFile,p0));
}

STATIC mp_obj_t tos_Scale2Mem(mp_obj_t min,mp_obj_t max) {
int64_t p0 = mp_obj_get_int(min);
int64_t p1 = mp_obj_get_int(max);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_Scale2Mem,p0,p1));
}

STATIC mp_obj_t tos_ScanChar(void) {
return mp_obj_new_int((int64_t)syscall0(__NR__tos_ScanChar));
}

STATIC mp_obj_t tos_Seed(mp_obj_t seed) {
int64_t p0 = mp_obj_get_int(seed);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Seed,p0));
}

STATIC mp_obj_t tos_Spaces2Tabs(mp_obj_t dst,mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(src)) {
p1 = (int64_t *)mp_obj_str_get_str(src);
} else {
p1 = (int64_t *)mp_obj_get_int(src);
}
return mp_obj_new_int((int64_t)syscall2(__NR__tos_Spaces2Tabs,p0,p1));
}

STATIC mp_obj_t tos_SrcLineNum(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_SrcLineNum,p0));
}

STATIC mp_obj_t tos_Str2I64(mp_obj_t st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st)) {
p0 = (int64_t *)mp_obj_str_get_str(st);
} else {
p0 = (int64_t *)mp_obj_get_int(st);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Str2I64,p0));
}

STATIC mp_obj_t tos_StrCmp(mp_obj_t st1,mp_obj_t st2) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st1)) {
p0 = (int64_t *)mp_obj_str_get_str(st1);
} else {
p0 = (int64_t *)mp_obj_get_int(st1);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(st2)) {
p1 = (int64_t *)mp_obj_str_get_str(st2);
} else {
p1 = (int64_t *)mp_obj_get_int(st2);
}
return mp_obj_new_int((int64_t)syscall2(__NR__tos_StrCmp,p0,p1));
}

STATIC mp_obj_t tos_StreamExePrint(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_StreamExePrint,p0));
}

STATIC mp_obj_t tos_StrICmp(mp_obj_t st1,mp_obj_t st2) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st1)) {
p0 = (int64_t *)mp_obj_str_get_str(st1);
} else {
p0 = (int64_t *)mp_obj_get_int(st1);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(st2)) {
p1 = (int64_t *)mp_obj_str_get_str(st2);
} else {
p1 = (int64_t *)mp_obj_get_int(st2);
}
return mp_obj_new_int((int64_t)syscall2(__NR__tos_StrICmp,p0,p1));
}

STATIC mp_obj_t tos_StrNCmp(mp_obj_t st1,mp_obj_t st2,mp_obj_t n) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st1)) {
p0 = (int64_t *)mp_obj_str_get_str(st1);
} else {
p0 = (int64_t *)mp_obj_get_int(st1);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(st2)) {
p1 = (int64_t *)mp_obj_str_get_str(st2);
} else {
p1 = (int64_t *)mp_obj_get_int(st2);
}
int64_t p2 = mp_obj_get_int(n);
return mp_obj_new_int((int64_t)syscall3(__NR__tos_StrNCmp,p0,p1,p2));
}

STATIC mp_obj_t tos_StrNICmp(mp_obj_t st1,mp_obj_t st2,mp_obj_t n) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st1)) {
p0 = (int64_t *)mp_obj_str_get_str(st1);
} else {
p0 = (int64_t *)mp_obj_get_int(st1);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(st2)) {
p1 = (int64_t *)mp_obj_str_get_str(st2);
} else {
p1 = (int64_t *)mp_obj_get_int(st2);
}
int64_t p2 = mp_obj_get_int(n);
return mp_obj_new_int((int64_t)syscall3(__NR__tos_StrNICmp,p0,p1,p2));
}

STATIC mp_obj_t tos_StrOcc(mp_obj_t src,mp_obj_t ch) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
unsigned char p1 = mp_obj_get_int(ch);
return mp_obj_new_int((int64_t)syscall2(__NR__tos_StrOcc,p0,p1));
}

STATIC mp_obj_t tos_Un(mp_obj_t rip) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(rip)) {
p0 = (int64_t *)mp_obj_str_get_str(rip);
} else {
p0 = (int64_t *)mp_obj_get_int(rip);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Un,p0));
}

STATIC mp_obj_t tos_UnusedStk(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_UnusedStk,p0));
}

STATIC mp_obj_t tos_YearStartDate(mp_obj_t year) {
int64_t p0 = mp_obj_get_int(year);
return mp_obj_new_int((int64_t)syscall1(__NR__tos_YearStartDate,p0));
}

STATIC mp_obj_t tos_Freq2Ona(mp_obj_t freq) {
int64_t p0 = mp_obj_get_int(freq);
return mp_obj_new_int((char)syscall1(__NR__tos_Freq2Ona,p0));
}

STATIC mp_obj_t tos_AdamErr(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_AdamErr,p0));
}

STATIC mp_obj_t tos_AdamLog(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_AdamLog,p0));
}

STATIC mp_obj_t tos_ArcCompressBuf(mp_obj_t c) {
void* p0 = mp_obj_get_int(c);
return mp_obj_new_int(syscall1(__NR__tos_ArcCompressBuf,p0));
}

STATIC mp_obj_t tos_ArcCtrlDel(mp_obj_t c) {
void* p0 = mp_obj_get_int(c);
return mp_obj_new_int(syscall1(__NR__tos_ArcCtrlDel,p0));
}

STATIC mp_obj_t tos_ArcExpandBuf(mp_obj_t c) {
void* p0 = mp_obj_get_int(c);
return mp_obj_new_int(syscall1(__NR__tos_ArcExpandBuf,p0));
}

STATIC mp_obj_t tos_Beep(mp_obj_t ona) {
char p0 = mp_obj_get_int(ona);
return mp_obj_new_int(syscall1(__NR__tos_Beep,p0));
}

STATIC mp_obj_t tos_BlkDevDel(mp_obj_t bd) {
void* p0 = mp_obj_get_int(bd);
return mp_obj_new_int(syscall1(__NR__tos_BlkDevDel,p0));
}

STATIC mp_obj_t tos_BlkDevsRelease(void) {
return mp_obj_new_int(syscall0(__NR__tos_BlkDevsRelease));
}

STATIC mp_obj_t tos_BlkPoolAdd(mp_obj_t bp,mp_obj_t m,mp_obj_t pags) {
void* p0 = mp_obj_get_int(bp);
void* p1 = mp_obj_get_int(m);
int64_t p2 = mp_obj_get_int(pags);
return mp_obj_new_int(syscall3(__NR__tos_BlkPoolAdd,p0,p1,p2));
}

STATIC mp_obj_t tos_BlkPoolInit(mp_obj_t bp,mp_obj_t pags) {
void* p0 = mp_obj_get_int(bp);
int64_t p1 = mp_obj_get_int(pags);
return mp_obj_new_int(syscall2(__NR__tos_BlkPoolInit,p0,p1));
}

STATIC mp_obj_t tos_BlkWriteZero(mp_obj_t dv,mp_obj_t blk,mp_obj_t cnt) {
void* p0 = mp_obj_get_int(dv);
int64_t p1 = mp_obj_get_int(blk);
int64_t p2 = mp_obj_get_int(cnt);
return mp_obj_new_int(syscall3(__NR__tos_BlkWriteZero,p0,p1,p2));
}

STATIC mp_obj_t tos_Break(void) {
return mp_obj_new_int(syscall0(__NR__tos_Break));
}

STATIC mp_obj_t tos_CallerRep(mp_obj_t rbp) {
void* p0 = mp_obj_get_int(rbp);
return mp_obj_new_int(syscall1(__NR__tos_CallerRep,p0));
}

STATIC mp_obj_t tos_Date2Struct(mp_obj_t _ds,mp_obj_t cdt) {
void* p0 = mp_obj_get_int(_ds);
int64_t p1 = mp_obj_get_int(cdt);
return mp_obj_new_int(syscall2(__NR__tos_Date2Struct,p0,p1));
}

STATIC mp_obj_t tos_Da(mp_obj_t addr) {
void* p0 = mp_obj_get_int(addr);
return mp_obj_new_int(syscall1(__NR__tos_Da,p0));
}

STATIC mp_obj_t tos_Dbg(mp_obj_t msg) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(msg)) {
p0 = (int64_t *)mp_obj_str_get_str(msg);
} else {
p0 = (int64_t *)mp_obj_get_int(msg);
}
return mp_obj_new_int(syscall1(__NR__tos_Dbg,p0));
}

STATIC mp_obj_t tos_DeathWait(mp_obj_t _task) {
void* p0 = mp_obj_get_int(_task);
return mp_obj_new_int(syscall1(__NR__tos_DeathWait,p0));
}

STATIC mp_obj_t tos_DefinePrint(mp_obj_t dname,mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dname)) {
p0 = (int64_t *)mp_obj_str_get_str(dname);
} else {
p0 = (int64_t *)mp_obj_get_int(dname);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(src)) {
p1 = (int64_t *)mp_obj_str_get_str(src);
} else {
p1 = (int64_t *)mp_obj_get_int(src);
}
return mp_obj_new_int(syscall2(__NR__tos_DefinePrint,p0,p1));
}

STATIC mp_obj_t tos_DirContextDel(mp_obj_t dirc) {
void* p0 = mp_obj_get_int(dirc);
return mp_obj_new_int(syscall1(__NR__tos_DirContextDel,p0));
}

STATIC mp_obj_t tos_DirEntryDel2(mp_obj_t tmpde) {
void* p0 = mp_obj_get_int(tmpde);
return mp_obj_new_int(syscall1(__NR__tos_DirEntryDel2,p0));
}

STATIC mp_obj_t tos_DirEntryDel(mp_obj_t tmpde) {
void* p0 = mp_obj_get_int(tmpde);
return mp_obj_new_int(syscall1(__NR__tos_DirEntryDel,p0));
}

STATIC mp_obj_t tos_DirTreeDel2(mp_obj_t tmpde) {
void* p0 = mp_obj_get_int(tmpde);
return mp_obj_new_int(syscall1(__NR__tos_DirTreeDel2,p0));
}

STATIC mp_obj_t tos_DirTreeDel(mp_obj_t tmpde) {
void* p0 = mp_obj_get_int(tmpde);
return mp_obj_new_int(syscall1(__NR__tos_DirTreeDel,p0));
}

STATIC mp_obj_t tos_Dm(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int(syscall1(__NR__tos_Dm,p0));
}

STATIC mp_obj_t tos_Dr(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int(syscall1(__NR__tos_Dr,p0));
}

STATIC mp_obj_t tos_DrvDel(mp_obj_t dv) {
void* p0 = mp_obj_get_int(dv);
return mp_obj_new_int(syscall1(__NR__tos_DrvDel,p0));
}

STATIC mp_obj_t tos_DrvRep(void) {
return mp_obj_new_int(syscall0(__NR__tos_DrvRep));
}

STATIC mp_obj_t tos_DrvsRelease(void) {
return mp_obj_new_int(syscall0(__NR__tos_DrvsRelease));
}

STATIC mp_obj_t tos_DskCacheInvalidate(mp_obj_t dv) {
void* p0 = mp_obj_get_int(dv);
return mp_obj_new_int(syscall1(__NR__tos_DskCacheInvalidate,p0));
}

STATIC mp_obj_t tos_DskChg(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int(syscall1(__NR__tos_DskChg,p0));
}

STATIC mp_obj_t tos_D(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int(syscall1(__NR__tos_D,p0));
}

STATIC mp_obj_t tos_DVDImageRead(mp_obj_t dvd_drv_let,mp_obj_t out_name) {
unsigned char p0 = mp_obj_get_int(dvd_drv_let);
int64_t *p1;
if (mp_obj_is_str_or_bytes(out_name)) {
p1 = (int64_t *)mp_obj_str_get_str(out_name);
} else {
p1 = (int64_t *)mp_obj_get_int(out_name);
}
return mp_obj_new_int(syscall2(__NR__tos_DVDImageRead,p0,p1));
}

STATIC mp_obj_t tos_Exit(void) {
return mp_obj_new_int(syscall0(__NR__tos_Exit));
}

STATIC mp_obj_t tos_FClose(mp_obj_t f) {
void* p0 = mp_obj_get_int(f);
return mp_obj_new_int(syscall1(__NR__tos_FClose,p0));
}

STATIC mp_obj_t tos_FifoI64Del(mp_obj_t f) {
void* p0 = mp_obj_get_int(f);
return mp_obj_new_int(syscall1(__NR__tos_FifoI64Del,p0));
}

STATIC mp_obj_t tos_FifoI64Flush(mp_obj_t f) {
void* p0 = mp_obj_get_int(f);
return mp_obj_new_int(syscall1(__NR__tos_FifoI64Flush,p0));
}

STATIC mp_obj_t tos_FifoU8Del(mp_obj_t f) {
void* p0 = mp_obj_get_int(f);
return mp_obj_new_int(syscall1(__NR__tos_FifoU8Del,p0));
}

STATIC mp_obj_t tos_FifoU8Flush(mp_obj_t f) {
void* p0 = mp_obj_get_int(f);
return mp_obj_new_int(syscall1(__NR__tos_FifoU8Flush,p0));
}

STATIC mp_obj_t tos_FixSet(mp_obj_t filename,mp_obj_t line) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
int64_t p1 = mp_obj_get_int(line);
return mp_obj_new_int(syscall2(__NR__tos_FixSet,p0,p1));
}

STATIC mp_obj_t tos_Free(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int(syscall1(__NR__tos_Free,p0));
}

STATIC mp_obj_t tos_G2(mp_obj_t ms) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(ms)) {
p0 = (int64_t *)mp_obj_str_get_str(ms);
} else {
p0 = (int64_t *)mp_obj_get_int(ms);
}
return mp_obj_new_int(syscall1(__NR__tos_G2,p0));
}

STATIC mp_obj_t tos_GetOutOfDollar(void) {
return mp_obj_new_int(syscall0(__NR__tos_GetOutOfDollar));
}

STATIC mp_obj_t tos_GridInit(void) {
return mp_obj_new_int(syscall0(__NR__tos_GridInit));
}

STATIC mp_obj_t tos_G(mp_obj_t ms) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(ms)) {
p0 = (int64_t *)mp_obj_str_get_str(ms);
} else {
p0 = (int64_t *)mp_obj_get_int(ms);
}
return mp_obj_new_int(syscall1(__NR__tos_G,p0));
}

STATIC mp_obj_t tos_HashAdd(mp_obj_t tmph,mp_obj_t table) {
void* p0 = mp_obj_get_int(tmph);
void* p1 = mp_obj_get_int(table);
return mp_obj_new_int(syscall2(__NR__tos_HashAdd,p0,p1));
}

STATIC mp_obj_t tos_HashDel(mp_obj_t tmph) {
void* p0 = mp_obj_get_int(tmph);
return mp_obj_new_int(syscall1(__NR__tos_HashDel,p0));
}

STATIC mp_obj_t tos_HashTableDel(mp_obj_t table) {
void* p0 = mp_obj_get_int(table);
return mp_obj_new_int(syscall1(__NR__tos_HashTableDel,p0));
}

STATIC mp_obj_t tos_HeapCtrlDel(mp_obj_t hc) {
void* p0 = mp_obj_get_int(hc);
return mp_obj_new_int(syscall1(__NR__tos_HeapCtrlDel,p0));
}

STATIC mp_obj_t tos_Help(void) {
return mp_obj_new_int(syscall0(__NR__tos_Help));
}

STATIC mp_obj_t tos_HomeSet(mp_obj_t dirname) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dirname)) {
p0 = (int64_t *)mp_obj_str_get_str(dirname);
} else {
p0 = (int64_t *)mp_obj_get_int(dirname);
}
return mp_obj_new_int(syscall1(__NR__tos_HomeSet,p0));
}

STATIC mp_obj_t tos_InFile(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
return mp_obj_new_int(syscall1(__NR__tos_InFile,p0));
}

STATIC mp_obj_t tos_InStr(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_InStr,p0));
}

STATIC mp_obj_t tos_In(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_In,p0));
}

STATIC mp_obj_t tos_JobDel(mp_obj_t tmpc) {
void* p0 = mp_obj_get_int(tmpc);
return mp_obj_new_int(syscall1(__NR__tos_JobDel,p0));
}

STATIC mp_obj_t tos_KbdTypeMatic(mp_obj_t delay) {
unsigned char p0 = mp_obj_get_int(delay);
return mp_obj_new_int(syscall1(__NR__tos_KbdTypeMatic,p0));
}

STATIC mp_obj_t tos_KeyDescSet(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_KeyDescSet,p0));
}

STATIC mp_obj_t tos_KeyDevRem(mp_obj_t tmpk) {
void* p0 = mp_obj_get_int(tmpk);
return mp_obj_new_int(syscall1(__NR__tos_KeyDevRem,p0));
}

STATIC mp_obj_t tos_LinkedLstDel(mp_obj_t _lst) {
void* p0 = mp_obj_get_int(_lst);
return mp_obj_new_int(syscall1(__NR__tos_LinkedLstDel,p0));
}

STATIC mp_obj_t tos_Mem32DevFree(mp_obj_t base) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(base)) {
p0 = (int64_t *)mp_obj_str_get_str(base);
} else {
p0 = (int64_t *)mp_obj_get_int(base);
}
return mp_obj_new_int(syscall1(__NR__tos_Mem32DevFree,p0));
}

STATIC mp_obj_t tos_Mem64DevFree(mp_obj_t base,mp_obj_t pages1Gig) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(base)) {
p0 = (int64_t *)mp_obj_str_get_str(base);
} else {
p0 = (int64_t *)mp_obj_get_int(base);
}
int64_t p1 = mp_obj_get_int(pages1Gig);
return mp_obj_new_int(syscall2(__NR__tos_Mem64DevFree,p0,p1));
}

STATIC mp_obj_t tos_MemPagFree(mp_obj_t m) {
void* p0 = mp_obj_get_int(m);
return mp_obj_new_int(syscall1(__NR__tos_MemPagFree,p0));
}

STATIC mp_obj_t tos_MPHalt(void) {
return mp_obj_new_int(syscall0(__NR__tos_MPHalt));
}

STATIC mp_obj_t tos_MPIntAll(mp_obj_t num) {
unsigned char p0 = mp_obj_get_int(num);
return mp_obj_new_int(syscall1(__NR__tos_MPIntAll,p0));
}

STATIC mp_obj_t tos_MPInt(mp_obj_t num) {
unsigned char p0 = mp_obj_get_int(num);
return mp_obj_new_int(syscall1(__NR__tos_MPInt,p0));
}

STATIC mp_obj_t tos_MPNMInt(void) {
return mp_obj_new_int(syscall0(__NR__tos_MPNMInt));
}

STATIC mp_obj_t tos_Msg(mp_obj_t msg_code,mp_obj_t arg1,mp_obj_t arg2) {
int64_t p0 = mp_obj_get_int(msg_code);
int64_t p1 = mp_obj_get_int(arg1);
int64_t p2 = mp_obj_get_int(arg2);
return mp_obj_new_int(syscall3(__NR__tos_Msg,p0,p1,p2));
}

STATIC mp_obj_t tos_Panic(mp_obj_t msg) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(msg)) {
p0 = (int64_t *)mp_obj_str_get_str(msg);
} else {
p0 = (int64_t *)mp_obj_get_int(msg);
}
return mp_obj_new_int(syscall1(__NR__tos_Panic,p0));
}

STATIC mp_obj_t tos_PrintErr(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_PrintErr,p0));
}

STATIC mp_obj_t tos_Print(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_Print,p0));
}

STATIC mp_obj_t tos_PrintWarn(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_PrintWarn,p0));
}

STATIC mp_obj_t tos_ProgressBarsRst(mp_obj_t path) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(path)) {
p0 = (int64_t *)mp_obj_str_get_str(path);
} else {
p0 = (int64_t *)mp_obj_get_int(path);
}
return mp_obj_new_int(syscall1(__NR__tos_ProgressBarsRst,p0));
}

STATIC mp_obj_t tos_PutChars(mp_obj_t ch) {
uint64_t p0 = mp_obj_get_int(ch);
return mp_obj_new_int(syscall1(__NR__tos_PutChars,p0));
}

STATIC mp_obj_t tos_PutDirLink(mp_obj_t dirname) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dirname)) {
p0 = (int64_t *)mp_obj_str_get_str(dirname);
} else {
p0 = (int64_t *)mp_obj_get_int(dirname);
}
return mp_obj_new_int(syscall1(__NR__tos_PutDirLink,p0));
}

STATIC mp_obj_t tos_PutExcept(mp_obj_t catch_it) {
unsigned char p0 = mp_obj_get_int(catch_it);
return mp_obj_new_int(syscall1(__NR__tos_PutExcept,p0));
}

STATIC mp_obj_t tos_PutKey(mp_obj_t ch) {
int64_t p0 = mp_obj_get_int(ch);
return mp_obj_new_int(syscall1(__NR__tos_PutKey,p0));
}

STATIC mp_obj_t tos_QueDel(mp_obj_t head) {
void* p0 = mp_obj_get_int(head);
return mp_obj_new_int(syscall1(__NR__tos_QueDel,p0));
}

STATIC mp_obj_t tos_QueVectU8Del(mp_obj_t v) {
void* p0 = mp_obj_get_int(v);
return mp_obj_new_int(syscall1(__NR__tos_QueVectU8Del,p0));
}

STATIC mp_obj_t tos_QueVectU8Put(mp_obj_t v,mp_obj_t idx,mp_obj_t ch) {
void* p0 = mp_obj_get_int(v);
int64_t p1 = mp_obj_get_int(idx);
unsigned char p2 = mp_obj_get_int(ch);
return mp_obj_new_int(syscall3(__NR__tos_QueVectU8Put,p0,p1,p2));
}

STATIC mp_obj_t tos_RawD(mp_obj_t mS,mp_obj_t addr) {
int64_t p0 = mp_obj_get_int(mS);
int64_t *p1;
if (mp_obj_is_str_or_bytes(addr)) {
p1 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p1 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int(syscall2(__NR__tos_RawD,p0,p1));
}

STATIC mp_obj_t tos_RawDm(mp_obj_t mS,mp_obj_t addr) {
int64_t p0 = mp_obj_get_int(mS);
int64_t *p1;
if (mp_obj_is_str_or_bytes(addr)) {
p1 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p1 = (int64_t *)mp_obj_get_int(addr);
}
return mp_obj_new_int(syscall2(__NR__tos_RawDm,p0,p1));
}

STATIC mp_obj_t tos_RawPrint(mp_obj_t mS,mp_obj_t fmt) {
int64_t p0 = mp_obj_get_int(mS);
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall2(__NR__tos_RawPrint,p0,p1));
}

STATIC mp_obj_t tos_RawPutChar(mp_obj_t ch) {
int64_t p0 = mp_obj_get_int(ch);
return mp_obj_new_int(syscall1(__NR__tos_RawPutChar,p0));
}

STATIC mp_obj_t tos_Reboot(void) {
return mp_obj_new_int(syscall0(__NR__tos_Reboot));
}

STATIC mp_obj_t tos_ScanFlags(mp_obj_t _dst_flags,mp_obj_t lst,mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(_dst_flags)) {
p0 = (int64_t *)mp_obj_str_get_str(_dst_flags);
} else {
p0 = (int64_t *)mp_obj_get_int(_dst_flags);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(lst)) {
p1 = (int64_t *)mp_obj_str_get_str(lst);
} else {
p1 = (int64_t *)mp_obj_get_int(lst);
}
int64_t *p2;
if (mp_obj_is_str_or_bytes(src)) {
p2 = (int64_t *)mp_obj_str_get_str(src);
} else {
p2 = (int64_t *)mp_obj_get_int(src);
}
return mp_obj_new_int(syscall3(__NR__tos_ScanFlags,p0,p1,p2));
}

STATIC mp_obj_t tos_Sleep(mp_obj_t mS) {
int64_t p0 = mp_obj_get_int(mS);
return mp_obj_new_int(syscall1(__NR__tos_Sleep,p0));
}

STATIC mp_obj_t tos_SleepUntil(mp_obj_t wake_jiffy) {
int64_t p0 = mp_obj_get_int(wake_jiffy);
return mp_obj_new_int(syscall1(__NR__tos_SleepUntil,p0));
}

STATIC mp_obj_t tos_Snd(mp_obj_t ona) {
char p0 = mp_obj_get_int(ona);
return mp_obj_new_int(syscall1(__NR__tos_Snd,p0));
}

STATIC mp_obj_t tos_SndRst(void) {
return mp_obj_new_int(syscall0(__NR__tos_SndRst));
}

STATIC mp_obj_t tos_SrvTaskCont(void) {
return mp_obj_new_int(syscall0(__NR__tos_SrvTaskCont));
}

STATIC mp_obj_t tos_StkRep(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int(syscall1(__NR__tos_StkRep,p0));
}

STATIC mp_obj_t tos_StrCpy(mp_obj_t dst,mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(src)) {
p1 = (int64_t *)mp_obj_str_get_str(src);
} else {
p1 = (int64_t *)mp_obj_get_int(src);
}
return mp_obj_new_int(syscall2(__NR__tos_StrCpy,p0,p1));
}

STATIC mp_obj_t tos_StreamPrint(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall1(__NR__tos_StreamPrint,p0));
}

STATIC mp_obj_t tos_S(mp_obj_t ms) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(ms)) {
p0 = (int64_t *)mp_obj_str_get_str(ms);
} else {
p0 = (int64_t *)mp_obj_get_int(ms);
}
return mp_obj_new_int(syscall1(__NR__tos_S,p0));
}

STATIC mp_obj_t tos_TaskDerivedValsUpdate(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int(syscall1(__NR__tos_TaskDerivedValsUpdate,p0));
}

STATIC mp_obj_t tos_TaskQueIns(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int(syscall1(__NR__tos_TaskQueIns,p0));
}

STATIC mp_obj_t tos_TaskQueRem(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int(syscall1(__NR__tos_TaskQueRem,p0));
}

STATIC mp_obj_t tos_TaskRstAwaitingMsg(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int(syscall1(__NR__tos_TaskRstAwaitingMsg,p0));
}

STATIC mp_obj_t tos_TaskWait(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int(syscall1(__NR__tos_TaskWait,p0));
}

STATIC mp_obj_t tos_throw(mp_obj_t ch) {
int64_t p0 = mp_obj_get_int(ch);
return mp_obj_new_int(syscall1(__NR__tos_throw,p0));
}

STATIC mp_obj_t tos_ToFileLine(mp_obj_t _fl_file_line,mp_obj_t _filename,mp_obj_t _linenum) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(_fl_file_line)) {
p0 = (int64_t *)mp_obj_str_get_str(_fl_file_line);
} else {
p0 = (int64_t *)mp_obj_get_int(_fl_file_line);
}
void* p1 = mp_obj_get_int(_filename);
void* p2 = mp_obj_get_int(_linenum);
return mp_obj_new_int(syscall3(__NR__tos_ToFileLine,p0,p1,p2));
}

STATIC mp_obj_t tos_TSSBusy(mp_obj_t tr) {
int64_t p0 = mp_obj_get_int(tr);
return mp_obj_new_int(syscall1(__NR__tos_TSSBusy,p0));
}

STATIC mp_obj_t tos_UserCmdLine(void) {
return mp_obj_new_int(syscall0(__NR__tos_UserCmdLine));
}

STATIC mp_obj_t tos_UserTaskCont(void) {
return mp_obj_new_int(syscall0(__NR__tos_UserTaskCont));
}

STATIC mp_obj_t tos_VGAFlush(void) {
return mp_obj_new_int(syscall0(__NR__tos_VGAFlush));
}

STATIC mp_obj_t tos_WinDerivedValsUpdate(mp_obj_t task) {
void* p0 = mp_obj_get_int(task);
return mp_obj_new_int(syscall1(__NR__tos_WinDerivedValsUpdate,p0));
}

STATIC mp_obj_t tos_XTalk(mp_obj_t task,mp_obj_t fmt) {
void* p0 = mp_obj_get_int(task);
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall2(__NR__tos_XTalk,p0,p1));
}

STATIC mp_obj_t tos_XTalkStr(mp_obj_t task,mp_obj_t fmt) {
void* p0 = mp_obj_get_int(task);
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall2(__NR__tos_XTalkStr,p0,p1));
}

STATIC mp_obj_t tos_XTalkStrWait(mp_obj_t task,mp_obj_t fmt) {
void* p0 = mp_obj_get_int(task);
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall2(__NR__tos_XTalkStrWait,p0,p1));
}

STATIC mp_obj_t tos_XTalkWait(mp_obj_t task,mp_obj_t fmt) {
void* p0 = mp_obj_get_int(task);
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
return mp_obj_new_int(syscall2(__NR__tos_XTalkWait,p0,p1));
}

STATIC mp_obj_t tos_Yield(void) {
return mp_obj_new_int(syscall0(__NR__tos_Yield));
}

STATIC mp_obj_t tos_EndianU16(mp_obj_t d) {
uint16_t p0 = mp_obj_get_int(d);
return mp_obj_new_int((uint16_t)syscall1(__NR__tos_EndianU16,p0));
}

STATIC mp_obj_t tos_PCIReadU16(mp_obj_t bus,mp_obj_t dev,mp_obj_t fun,mp_obj_t rg) {
int64_t p0 = mp_obj_get_int(bus);
int64_t p1 = mp_obj_get_int(dev);
int64_t p2 = mp_obj_get_int(fun);
int64_t p3 = mp_obj_get_int(rg);
return mp_obj_new_int((uint16_t)syscall4(__NR__tos_PCIReadU16,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_RandU16(void) {
return mp_obj_new_int((uint16_t)syscall0(__NR__tos_RandU16));
}

STATIC mp_obj_t tos_EndianU32(mp_obj_t d) {
uint32_t p0 = mp_obj_get_int(d);
return mp_obj_new_int((uint32_t)syscall1(__NR__tos_EndianU32,p0));
}

STATIC mp_obj_t tos_PCIReadU32(mp_obj_t bus,mp_obj_t dev,mp_obj_t fun,mp_obj_t rg) {
int64_t p0 = mp_obj_get_int(bus);
int64_t p1 = mp_obj_get_int(dev);
int64_t p2 = mp_obj_get_int(fun);
int64_t p3 = mp_obj_get_int(rg);
return mp_obj_new_int((uint32_t)syscall4(__NR__tos_PCIReadU32,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_RandU32(void) {
return mp_obj_new_int((uint32_t)syscall0(__NR__tos_RandU32));
}

STATIC mp_obj_t tos_CeilU64(mp_obj_t num,mp_obj_t to) {
uint64_t p0 = mp_obj_get_int(num);
uint64_t p1 = mp_obj_get_int(to);
return mp_obj_new_int((uint64_t)syscall2(__NR__tos_CeilU64,p0,p1));
}

STATIC mp_obj_t tos_FloorU64(mp_obj_t num,mp_obj_t to) {
uint64_t p0 = mp_obj_get_int(num);
uint64_t p1 = mp_obj_get_int(to);
return mp_obj_new_int((uint64_t)syscall2(__NR__tos_FloorU64,p0,p1));
}

STATIC mp_obj_t tos_RandU64(void) {
return mp_obj_new_int((uint64_t)syscall0(__NR__tos_RandU64));
}

STATIC mp_obj_t tos_ACAlloc(mp_obj_t size) {
int64_t p0 = mp_obj_get_int(size);
uint64_t *tmp=syscall1(__NR__tos_ACAlloc,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_AMAlloc(mp_obj_t size) {
int64_t p0 = mp_obj_get_int(size);
uint64_t *tmp=syscall1(__NR__tos_AMAlloc,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_AMAllocIdent(mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
uint64_t *tmp=syscall1(__NR__tos_AMAllocIdent,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_AStrNew(mp_obj_t buf) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(buf)) {
p0 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p0 = (int64_t *)mp_obj_get_int(buf);
}
uint64_t *tmp=syscall1(__NR__tos_AStrNew,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Caller(mp_obj_t num) {
int64_t p0 = mp_obj_get_int(num);
uint64_t *tmp=syscall1(__NR__tos_Caller,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_CAlloc(mp_obj_t size) {
int64_t p0 = mp_obj_get_int(size);
uint64_t *tmp=syscall1(__NR__tos_CAlloc,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_CatPrint(mp_obj_t dst,mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
uint64_t *tmp=syscall2(__NR__tos_CatPrint,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Char2KeyName(mp_obj_t ch) {
int64_t p0 = mp_obj_get_int(ch);
uint64_t *tmp=syscall1(__NR__tos_Char2KeyName,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Color2Str(mp_obj_t buf,mp_obj_t c) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(buf)) {
p0 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p0 = (int64_t *)mp_obj_get_int(buf);
}
int64_t p1 = mp_obj_get_int(c);
uint64_t *tmp=syscall2(__NR__tos_Color2Str,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_DefineSub(mp_obj_t sub,mp_obj_t dname) {
int64_t p0 = mp_obj_get_int(sub);
int64_t *p1;
if (mp_obj_is_str_or_bytes(dname)) {
p1 = (int64_t *)mp_obj_str_get_str(dname);
} else {
p1 = (int64_t *)mp_obj_get_int(dname);
}
uint64_t *tmp=syscall2(__NR__tos_DefineSub,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Define(mp_obj_t dname) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dname)) {
p0 = (int64_t *)mp_obj_str_get_str(dname);
} else {
p0 = (int64_t *)mp_obj_get_int(dname);
}
uint64_t *tmp=syscall1(__NR__tos_Define,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_DirFile(mp_obj_t dirname) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dirname)) {
p0 = (int64_t *)mp_obj_str_get_str(dirname);
} else {
p0 = (int64_t *)mp_obj_get_int(dirname);
}
uint64_t *tmp=syscall1(__NR__tos_DirFile,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_DirNameAbs(mp_obj_t dirname) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dirname)) {
p0 = (int64_t *)mp_obj_str_get_str(dirname);
} else {
p0 = (int64_t *)mp_obj_get_int(dirname);
}
uint64_t *tmp=syscall1(__NR__tos_DirNameAbs,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Drv2Let(mp_obj_t dv) {
void* p0 = mp_obj_get_int(dv);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Drv2Let,p0));
}

STATIC mp_obj_t tos_DrvModelNum(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
uint64_t *tmp=syscall1(__NR__tos_DrvModelNum,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_DrvNextFreeLet(mp_obj_t first_drv_let) {
unsigned char p0 = mp_obj_get_int(first_drv_let);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_DrvNextFreeLet,p0));
}

STATIC mp_obj_t tos_DrvSerialNum(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
uint64_t *tmp=syscall1(__NR__tos_DrvSerialNum,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_DrvTextAttrGet(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_DrvTextAttrGet,p0));
}

STATIC mp_obj_t tos_ExpandBuf(mp_obj_t arc) {
void* p0 = mp_obj_get_int(arc);
uint64_t *tmp=syscall1(__NR__tos_ExpandBuf,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_ExtChg(mp_obj_t filename,mp_obj_t extension) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(extension)) {
p1 = (int64_t *)mp_obj_str_get_str(extension);
} else {
p1 = (int64_t *)mp_obj_get_int(extension);
}
uint64_t *tmp=syscall2(__NR__tos_ExtChg,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_ExtDft(mp_obj_t filename,mp_obj_t extension) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(extension)) {
p1 = (int64_t *)mp_obj_str_get_str(extension);
} else {
p1 = (int64_t *)mp_obj_get_int(extension);
}
uint64_t *tmp=syscall2(__NR__tos_ExtDft,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_FileExtDot(mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
uint64_t *tmp=syscall1(__NR__tos_FileExtDot,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_FileExtRem(mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
uint64_t *tmp=syscall1(__NR__tos_FileExtRem,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_FileNameAbs(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
uint64_t *tmp=syscall1(__NR__tos_FileNameAbs,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_FileNameTmpTxt(void) {
uint64_t *tmp=syscall0(__NR__tos_FileNameTmpTxt);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_FileRead(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
uint64_t *tmp=syscall1(__NR__tos_FileRead,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_GetStr(mp_obj_t msg) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(msg)) {
p0 = (int64_t *)mp_obj_str_get_str(msg);
} else {
p0 = (int64_t *)mp_obj_get_int(msg);
}
uint64_t *tmp=syscall1(__NR__tos_GetStr,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_IntEntryGet(mp_obj_t irq) {
int64_t p0 = mp_obj_get_int(irq);
uint64_t *tmp=syscall1(__NR__tos_IntEntryGet,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Let2Let(mp_obj_t drv_let) {
unsigned char p0 = mp_obj_get_int(drv_let);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Let2Let,p0));
}

STATIC mp_obj_t tos_LinkedLstCopy(mp_obj_t _lst) {
void* p0 = mp_obj_get_int(_lst);
return mp_obj_new_int((int64_t*)syscall1(__NR__tos_LinkedLstCopy,p0));
}

STATIC mp_obj_t tos_LstSub(mp_obj_t sub,mp_obj_t lst) {
int64_t p0 = mp_obj_get_int(sub);
int64_t *p1;
if (mp_obj_is_str_or_bytes(lst)) {
p1 = (int64_t *)mp_obj_str_get_str(lst);
} else {
p1 = (int64_t *)mp_obj_get_int(lst);
}
uint64_t *tmp=syscall2(__NR__tos_LstSub,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_MAlloc(mp_obj_t size) {
int64_t p0 = mp_obj_get_int(size);
uint64_t *tmp=syscall1(__NR__tos_MAlloc,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_MAllocIdent(mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
uint64_t *tmp=syscall1(__NR__tos_MAllocIdent,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Mem32DevAlloc(mp_obj_t size,mp_obj_t alignment) {
int64_t p0 = mp_obj_get_int(size);
int64_t p1 = mp_obj_get_int(alignment);
uint64_t *tmp=syscall2(__NR__tos_Mem32DevAlloc,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Mem64DevAlloc(mp_obj_t _pages1Gig) {
void* p0 = mp_obj_get_int(_pages1Gig);
uint64_t *tmp=syscall1(__NR__tos_Mem64DevAlloc,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_MemPagAlloc(mp_obj_t pags) {
int64_t p0 = mp_obj_get_int(pags);
uint64_t *tmp=syscall1(__NR__tos_MemPagAlloc,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_MStrPrint(mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(fmt)) {
p0 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p0 = (int64_t *)mp_obj_get_int(fmt);
}
uint64_t *tmp=syscall1(__NR__tos_MStrPrint,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_MStrUtil(mp_obj_t src,mp_obj_t flags) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
int64_t p1 = mp_obj_get_int(flags);
uint64_t *tmp=syscall2(__NR__tos_MStrUtil,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_PCIReadU8(mp_obj_t bus,mp_obj_t dev,mp_obj_t fun,mp_obj_t rg) {
int64_t p0 = mp_obj_get_int(bus);
int64_t p1 = mp_obj_get_int(dev);
int64_t p2 = mp_obj_get_int(fun);
int64_t p3 = mp_obj_get_int(rg);
return mp_obj_new_int((unsigned char)syscall4(__NR__tos_PCIReadU8,p0,p1,p2,p3));
}

STATIC mp_obj_t tos_ScaleIndent(mp_obj_t src,mp_obj_t indent_scale_factor) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
int64_t p1 = mp_obj_get_int(indent_scale_factor);
uint64_t *tmp=syscall2(__NR__tos_ScaleIndent,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_ScanCode2Char(mp_obj_t sc) {
int64_t p0 = mp_obj_get_int(sc);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_ScanCode2Char,p0));
}

STATIC mp_obj_t tos_ScanCode2KeyName(mp_obj_t sc) {
int64_t p0 = mp_obj_get_int(sc);
uint64_t *tmp=syscall1(__NR__tos_ScanCode2KeyName,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_SrcEdLink(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
uint64_t *tmp=syscall1(__NR__tos_SrcEdLink,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_SrcFileName(mp_obj_t addr) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(addr)) {
p0 = (int64_t *)mp_obj_str_get_str(addr);
} else {
p0 = (int64_t *)mp_obj_get_int(addr);
}
uint64_t *tmp=syscall1(__NR__tos_SrcFileName,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrFind(mp_obj_t needle,mp_obj_t haystack_str) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(needle)) {
p0 = (int64_t *)mp_obj_str_get_str(needle);
} else {
p0 = (int64_t *)mp_obj_get_int(needle);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(haystack_str)) {
p1 = (int64_t *)mp_obj_str_get_str(haystack_str);
} else {
p1 = (int64_t *)mp_obj_get_int(haystack_str);
}
uint64_t *tmp=syscall2(__NR__tos_StrFind,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrFirstOcc(mp_obj_t src,mp_obj_t marker) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(marker)) {
p1 = (int64_t *)mp_obj_str_get_str(marker);
} else {
p1 = (int64_t *)mp_obj_get_int(marker);
}
uint64_t *tmp=syscall2(__NR__tos_StrFirstOcc,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrFirstRem(mp_obj_t src,mp_obj_t marker) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(marker)) {
p1 = (int64_t *)mp_obj_str_get_str(marker);
} else {
p1 = (int64_t *)mp_obj_get_int(marker);
}
uint64_t *tmp=syscall2(__NR__tos_StrFirstRem,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrIMatch(mp_obj_t needle,mp_obj_t haystack_str) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(needle)) {
p0 = (int64_t *)mp_obj_str_get_str(needle);
} else {
p0 = (int64_t *)mp_obj_get_int(needle);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(haystack_str)) {
p1 = (int64_t *)mp_obj_str_get_str(haystack_str);
} else {
p1 = (int64_t *)mp_obj_get_int(haystack_str);
}
uint64_t *tmp=syscall2(__NR__tos_StrIMatch,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrLastOcc(mp_obj_t src,mp_obj_t marker) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(marker)) {
p1 = (int64_t *)mp_obj_str_get_str(marker);
} else {
p1 = (int64_t *)mp_obj_get_int(marker);
}
uint64_t *tmp=syscall2(__NR__tos_StrLastOcc,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrLastRem(mp_obj_t src,mp_obj_t marker) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(marker)) {
p1 = (int64_t *)mp_obj_str_get_str(marker);
} else {
p1 = (int64_t *)mp_obj_get_int(marker);
}
uint64_t *tmp=syscall2(__NR__tos_StrLastRem,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrMatch(mp_obj_t needle,mp_obj_t haystack_str) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(needle)) {
p0 = (int64_t *)mp_obj_str_get_str(needle);
} else {
p0 = (int64_t *)mp_obj_get_int(needle);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(haystack_str)) {
p1 = (int64_t *)mp_obj_str_get_str(haystack_str);
} else {
p1 = (int64_t *)mp_obj_get_int(haystack_str);
}
uint64_t *tmp=syscall2(__NR__tos_StrMatch,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrPrintFlags(mp_obj_t dst,mp_obj_t lst,mp_obj_t flags) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(lst)) {
p1 = (int64_t *)mp_obj_str_get_str(lst);
} else {
p1 = (int64_t *)mp_obj_get_int(lst);
}
int64_t p2 = mp_obj_get_int(flags);
uint64_t *tmp=syscall3(__NR__tos_StrPrintFlags,p0,p1,p2);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrPrintJoin(mp_obj_t dst,mp_obj_t fmt,mp_obj_t argc,mp_obj_t argv) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
int64_t p2 = mp_obj_get_int(argc);
void* p3 = mp_obj_get_int(argv);
uint64_t *tmp=syscall4(__NR__tos_StrPrintJoin,p0,p1,p2,p3);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrPrint(mp_obj_t dst,mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
uint64_t *tmp=syscall2(__NR__tos_StrPrint,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrScan(mp_obj_t src,mp_obj_t fmt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(fmt)) {
p1 = (int64_t *)mp_obj_str_get_str(fmt);
} else {
p1 = (int64_t *)mp_obj_get_int(fmt);
}
uint64_t *tmp=syscall2(__NR__tos_StrScan,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrUtil(mp_obj_t _src,mp_obj_t flags) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(_src)) {
p0 = (int64_t *)mp_obj_str_get_str(_src);
} else {
p0 = (int64_t *)mp_obj_get_int(_src);
}
int64_t p1 = mp_obj_get_int(flags);
uint64_t *tmp=syscall2(__NR__tos_StrUtil,p0,p1);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_Tabs2Spaces(mp_obj_t src) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src)) {
p0 = (int64_t *)mp_obj_str_get_str(src);
} else {
p0 = (int64_t *)mp_obj_get_int(src);
}
uint64_t *tmp=syscall1(__NR__tos_Tabs2Spaces,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_ToggleZorNotZ(mp_obj_t name) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(name)) {
p0 = (int64_t *)mp_obj_str_get_str(name);
} else {
p0 = (int64_t *)mp_obj_get_int(name);
}
uint64_t *tmp=syscall1(__NR__tos_ToggleZorNotZ,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_MemCmp(mp_obj_t ptr1,mp_obj_t ptr2,mp_obj_t cnt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(ptr1)) {
p0 = (int64_t *)mp_obj_str_get_str(ptr1);
} else {
p0 = (int64_t *)mp_obj_get_int(ptr1);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(ptr2)) {
p1 = (int64_t *)mp_obj_str_get_str(ptr2);
} else {
p1 = (int64_t *)mp_obj_get_int(ptr2);
}
int64_t p2 = mp_obj_get_int(cnt);
return mp_obj_new_int((int64_t)syscall3(__NR__tos_MemCmp,p0,p1,p2));
}

STATIC mp_obj_t tos_MemCpy(mp_obj_t dst,mp_obj_t src,mp_obj_t cnt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(src)) {
p1 = (int64_t *)mp_obj_str_get_str(src);
} else {
p1 = (int64_t *)mp_obj_get_int(src);
}
int64_t p2 = mp_obj_get_int(cnt);
uint64_t *tmp=syscall3(__NR__tos_MemCpy,p0,p1,p2);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_MemSet(mp_obj_t dst,mp_obj_t val,mp_obj_t cnt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t p1 = mp_obj_get_int(val);
int64_t p2 = mp_obj_get_int(cnt);
uint64_t *tmp=syscall3(__NR__tos_MemSet,p0,p1,p2);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_MemSetI64(mp_obj_t dst,mp_obj_t val,mp_obj_t I64cnt) {
void* p0 = mp_obj_get_int(dst);
int64_t p1 = mp_obj_get_int(val);
int64_t p2 = mp_obj_get_int(I64cnt);
return mp_obj_new_int((int64_t*)syscall3(__NR__tos_MemSetI64,p0,p1,p2));
}

STATIC mp_obj_t tos_MemSetU16(mp_obj_t dst,mp_obj_t val,mp_obj_t U16cnt) {
void* p0 = mp_obj_get_int(dst);
int64_t p1 = mp_obj_get_int(val);
int64_t p2 = mp_obj_get_int(U16cnt);
return mp_obj_new_int((int64_t*)syscall3(__NR__tos_MemSetU16,p0,p1,p2));
}

STATIC mp_obj_t tos_MemSetU32(mp_obj_t dst,mp_obj_t val,mp_obj_t U32cnt) {
void* p0 = mp_obj_get_int(dst);
int64_t p1 = mp_obj_get_int(val);
int64_t p2 = mp_obj_get_int(U32cnt);
return mp_obj_new_int((int64_t*)syscall3(__NR__tos_MemSetU32,p0,p1,p2));
}

STATIC mp_obj_t tos_MemSetU8(mp_obj_t dst,mp_obj_t val,mp_obj_t U8cnt) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dst)) {
p0 = (int64_t *)mp_obj_str_get_str(dst);
} else {
p0 = (int64_t *)mp_obj_get_int(dst);
}
int64_t p1 = mp_obj_get_int(val);
int64_t p2 = mp_obj_get_int(U8cnt);
uint64_t *tmp=syscall3(__NR__tos_MemSetU8,p0,p1,p2);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_StrLen(mp_obj_t st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st)) {
p0 = (int64_t *)mp_obj_str_get_str(st);
} else {
p0 = (int64_t *)mp_obj_get_int(st);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_StrLen,p0));
}

STATIC mp_obj_t tos_InU8(mp_obj_t port) {
int64_t p0 = mp_obj_get_int(port);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_InU8,p0));
}

STATIC mp_obj_t tos_OutU8(mp_obj_t port,mp_obj_t val) {
int64_t p0 = mp_obj_get_int(port);
int64_t p1 = mp_obj_get_int(val);
return mp_obj_new_int(syscall2(__NR__tos_OutU8,p0,p1));
}

STATIC mp_obj_t tos_Uf(mp_obj_t st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(st)) {
p0 = (int64_t *)mp_obj_str_get_str(st);
} else {
p0 = (int64_t *)mp_obj_get_int(st);
}
return mp_obj_new_int(syscall1(__NR__tos_Uf,p0));
}

STATIC mp_obj_t tos_U(mp_obj_t rip) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(rip)) {
p0 = (int64_t *)mp_obj_str_get_str(rip);
} else {
p0 = (int64_t *)mp_obj_get_int(rip);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_U,p0));
}

STATIC mp_obj_t tos_StrNew(mp_obj_t buf) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(buf)) {
p0 = (int64_t *)mp_obj_str_get_str(buf);
} else {
p0 = (int64_t *)mp_obj_get_int(buf);
}
uint64_t *tmp=syscall1(__NR__tos_StrNew,p0);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_DelTree(mp_obj_t files_find_mask) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(files_find_mask)) {
p0 = (int64_t *)mp_obj_str_get_str(files_find_mask);
} else {
p0 = (int64_t *)mp_obj_get_int(files_find_mask);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_DelTree,p0));
}

STATIC mp_obj_t tos_CopyTree(mp_obj_t src_files_find_mask,mp_obj_t dst_files_find_mask) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src_files_find_mask)) {
p0 = (int64_t *)mp_obj_str_get_str(src_files_find_mask);
} else {
p0 = (int64_t *)mp_obj_get_int(src_files_find_mask);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(dst_files_find_mask)) {
p1 = (int64_t *)mp_obj_str_get_str(dst_files_find_mask);
} else {
p1 = (int64_t *)mp_obj_get_int(dst_files_find_mask);
}
return mp_obj_new_int((int64_t)syscall2(__NR__tos_CopyTree,p0,p1));
}

STATIC mp_obj_t tos_Move(mp_obj_t f1,mp_obj_t f2) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(f1)) {
p0 = (int64_t *)mp_obj_str_get_str(f1);
} else {
p0 = (int64_t *)mp_obj_get_int(f1);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(f2)) {
p1 = (int64_t *)mp_obj_str_get_str(f2);
} else {
p1 = (int64_t *)mp_obj_get_int(f2);
}
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_Move,p0,p1));
}

STATIC mp_obj_t tos_Copy(mp_obj_t src_files_find_mask,mp_obj_t dst_files_find_mask) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(src_files_find_mask)) {
p0 = (int64_t *)mp_obj_str_get_str(src_files_find_mask);
} else {
p0 = (int64_t *)mp_obj_get_int(src_files_find_mask);
}
int64_t *p1;
if (mp_obj_is_str_or_bytes(dst_files_find_mask)) {
p1 = (int64_t *)mp_obj_str_get_str(dst_files_find_mask);
} else {
p1 = (int64_t *)mp_obj_get_int(dst_files_find_mask);
}
return mp_obj_new_int((unsigned char)syscall2(__NR__tos_Copy,p0,p1));
}

STATIC mp_obj_t tos_Del(mp_obj_t files_find_mask) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(files_find_mask)) {
p0 = (int64_t *)mp_obj_str_get_str(files_find_mask);
} else {
p0 = (int64_t *)mp_obj_get_int(files_find_mask);
}
return mp_obj_new_int((int64_t)syscall1(__NR__tos_Del,p0));
}

STATIC mp_obj_t tos_Cd(mp_obj_t dirname) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(dirname)) {
p0 = (int64_t *)mp_obj_str_get_str(dirname);
} else {
p0 = (int64_t *)mp_obj_get_int(dirname);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Cd,p0));
}

STATIC mp_obj_t tos_Ed(mp_obj_t link_st) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(link_st)) {
p0 = (int64_t *)mp_obj_str_get_str(link_st);
} else {
p0 = (int64_t *)mp_obj_get_int(link_st);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_Ed,p0));
}

STATIC mp_obj_t tos_Dir(void) {
return mp_obj_new_int((int64_t)syscall0(__NR__tos_Dir));
}

STATIC mp_obj_t tos_DirMk(mp_obj_t filename) {
int64_t *p0;
if (mp_obj_is_str_or_bytes(filename)) {
p0 = (int64_t *)mp_obj_str_get_str(filename);
} else {
p0 = (int64_t *)mp_obj_get_int(filename);
}
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_DirMk,p0));
}

STATIC mp_obj_t tos_DirCur(void) {
uint64_t *tmp=syscall0(__NR__tos_DirCur);
return mp_obj_new_int(tmp);
}

STATIC mp_obj_t tos_DocWrite(mp_obj_t doc) {
void* p0 = mp_obj_get_int(doc);
return mp_obj_new_int((unsigned char)syscall1(__NR__tos_DocWrite,p0));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ArcFinishCompression_obj, tos_ArcFinishCompression);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_AreYouSure_obj, tos_AreYouSure);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Blink_obj, tos_Blink);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BlkDevLock_obj, tos_BlkDevLock);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BlkDevUnlock_obj, tos_BlkDevUnlock);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_BlkRead_obj, tos_BlkRead);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_BlkWrite_obj, tos_BlkWrite);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BptR_obj, tos_BptR);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BptS_obj, tos_BptS);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BreakLock_obj, tos_BreakLock);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BreakUnlock_obj, tos_BreakUnlock);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ChkCodePtr_obj, tos_ChkCodePtr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ChkOnStk_obj, tos_ChkOnStk);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ChkPtr_obj, tos_ChkPtr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DbgMode_obj, tos_DbgMode);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_DrvEnable_obj, tos_DrvEnable);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvLock_obj, tos_DrvLock);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_DrvMap_obj, tos_DrvMap);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Drv_obj, tos_Drv);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvUnlock_obj, tos_DrvUnlock);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Echo_obj, tos_Echo);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_EdLite_obj, tos_EdLite);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_E_obj, tos_E);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FBlkRead_obj, tos_FBlkRead);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FBlkWrite_obj, tos_FBlkWrite);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FifoI64Ins_obj, tos_FifoI64Ins);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FifoI64Peek_obj, tos_FifoI64Peek);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FifoI64Rem_obj, tos_FifoI64Rem);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FifoU8Ins_obj, tos_FifoU8Ins);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FifoU8Peek_obj, tos_FifoU8Peek);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FifoU8Rem_obj, tos_FifoU8Rem);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FileFind_obj, tos_FileFind);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FileNameChk_obj, tos_FileNameChk);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Fix_obj, tos_Fix);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_GetOption_obj, tos_GetOption);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_IsDbgMode_obj, tos_IsDbgMode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_IsDir_obj, tos_IsDir);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_IsDotC_obj, tos_IsDotC);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_IsDotZ_obj, tos_IsDotZ);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_IsMute_obj, tos_IsMute);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_IsRaw_obj, tos_IsRaw);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_IsSilent_obj, tos_IsSilent);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_IsSingleUser_obj, tos_IsSingleUser);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_IsSuspended_obj, tos_IsSuspended);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_IsSysDbg_obj, tos_IsSysDbg);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_JobResScan_obj, tos_JobResScan);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Kill_obj, tos_Kill);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Man_obj, tos_Man);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Mute_obj, tos_Mute);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PutSrcLink_obj, tos_PutSrcLink);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Raw_obj, tos_Raw);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ScanKey_obj, tos_ScanKey);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Silent_obj, tos_Silent);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_SingleUser_obj, tos_SingleUser);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Suspend_obj, tos_Suspend);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_SysDbg_obj, tos_SysDbg);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_TaskValidate_obj, tos_TaskValidate);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Trace_obj, tos_Trace);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_WildMatch_obj, tos_WildMatch);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_WinInside_obj, tos_WinInside);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_YorN_obj, tos_YorN);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_CompressBuf_obj, tos_CompressBuf);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ArcCtrlNew_obj, tos_ArcCtrlNew);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BlkDevChk_obj, tos_BlkDevChk);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_BlkDevNextFreeSlot_obj, tos_BlkDevNextFreeSlot);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvIsWritable_obj, tos_DrvIsWritable);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Let2BlkDev_obj, tos_Let2BlkDev);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Str2ColorU16_obj, tos_Str2ColorU16);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Str2ColorU32_obj, tos_Str2ColorU32);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_Now_obj, tos_Now);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Str2Date_obj, tos_Str2Date);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Struct2Date_obj, tos_Struct2Date);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FilesFind_obj, tos_FilesFind);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvChk_obj, tos_DrvChk);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvMakeFreeSlot_obj, tos_DrvMakeFreeSlot);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Let2Drv_obj, tos_Let2Drv);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FifoI64New_obj, tos_FifoI64New);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FifoU8New_obj, tos_FifoU8New);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FOpen_obj, tos_FOpen);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_DefineLoad_obj, tos_DefineLoad);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_DefineLstLoad_obj, tos_DefineLstLoad);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FunSegFind_obj, tos_FunSegFind);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FramePtrAdd_obj, tos_FramePtrAdd);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_HashPublic_obj, tos_HashPublic);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashTableNew_obj, tos_HashTableNew);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MHeapCtrl_obj, tos_MHeapCtrl);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_TaskExe_obj, tos_TaskExe);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_TaskText_obj, tos_TaskText);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_QueCopy_obj, tos_QueCopy);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_QueVectU8New_obj, tos_QueVectU8New);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_User_obj, tos_User);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_Clamp_obj, tos_Clamp);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Max_obj, tos_Max);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Min_obj, tos_Min);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Ona2Freq_obj, tos_Ona2Freq);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Pow10I64_obj, tos_Pow10I64);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Str2F64_obj, tos_Str2F64);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_RandI16_obj, tos_RandI16);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_RandI32_obj, tos_RandI32);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Adam_obj, tos_Adam);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ATARep_obj, tos_ATARep);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_B2_obj, tos_B2);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BCnt_obj, tos_BCnt);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BirthWait_obj, tos_BirthWait);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_CeilI64_obj, tos_CeilI64);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Char2ScanCode_obj, tos_Char2ScanCode);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Clus2Blk_obj, tos_Clus2Blk);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_ClusBlkRead_obj, tos_ClusBlkRead);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_ClusBlkWrite_obj, tos_ClusBlkWrite);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_ClusNumNext_obj, tos_ClusNumNext);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_ClusRead_obj, tos_ClusRead);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_ClusWrite_obj, tos_ClusWrite);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DayOfWeek_obj, tos_DayOfWeek);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DefineCnt_obj, tos_DefineCnt);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_DefineMatch_obj, tos_DefineMatch);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_EndianI64_obj, tos_EndianI64);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ExeFile_obj, tos_ExeFile);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ExePrint_obj, tos_ExePrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FifoI64Cnt_obj, tos_FifoI64Cnt);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FifoU8Cnt_obj, tos_FifoU8Cnt);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FirstDayOfMon_obj, tos_FirstDayOfMon);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FirstDayOfYear_obj, tos_FirstDayOfYear);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FloorI64_obj, tos_FloorI64);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FlushMsgs_obj, tos_FlushMsgs);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FramePtrDel_obj, tos_FramePtrDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FramePtrSet_obj, tos_FramePtrSet);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FramePtr_obj, tos_FramePtr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FSize_obj, tos_FSize);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_GetS_obj, tos_GetS);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_HashDefineLstAdd_obj, tos_HashDefineLstAdd);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashEntrySize2_obj, tos_HashEntrySize2);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashEntrySize_obj, tos_HashEntrySize);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_HashLstAdd_obj, tos_HashLstAdd);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashStr_obj, tos_HashStr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashTablePurge_obj, tos_HashTablePurge);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashTableSize2_obj, tos_HashTableSize2);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashTypeNum_obj, tos_HashTypeNum);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashVal_obj, tos_HashVal);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_JobResGet_obj, tos_JobResGet);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_JobsHndlr_obj, tos_JobsHndlr);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_KbdMsEvtTime_obj, tos_KbdMsEvtTime);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_LastDayOfMon_obj, tos_LastDayOfMon);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_LastDayOfYear_obj, tos_LastDayOfYear);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Let2BlkDevType_obj, tos_Let2BlkDevType);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_LinkedLstCnt_obj, tos_LinkedLstCnt);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_LinkedLstSize_obj, tos_LinkedLstSize);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_LstMatch_obj, tos_LstMatch);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MemPageTable_obj, tos_MemPageTable);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_MountIDEAuto_obj, tos_MountIDEAuto);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PopUpPrint_obj, tos_PopUpPrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PopUp_obj, tos_PopUp);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_PressAKey_obj, tos_PressAKey);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_QueCnt_obj, tos_QueCnt);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_QueSize_obj, tos_QueSize);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_QueVectU8Get_obj, tos_QueVectU8Get);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_RandI64_obj, tos_RandI64);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_RoundI64_obj, tos_RoundI64);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_RunFile2_obj, tos_RunFile2);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_RunFile_obj, tos_RunFile);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Scale2Mem_obj, tos_Scale2Mem);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_ScanChar_obj, tos_ScanChar);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Seed_obj, tos_Seed);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Spaces2Tabs_obj, tos_Spaces2Tabs);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_SrcLineNum_obj, tos_SrcLineNum);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Str2I64_obj, tos_Str2I64);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrCmp_obj, tos_StrCmp);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_StreamExePrint_obj, tos_StreamExePrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrICmp_obj, tos_StrICmp);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_StrNCmp_obj, tos_StrNCmp);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_StrNICmp_obj, tos_StrNICmp);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrOcc_obj, tos_StrOcc);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Un_obj, tos_Un);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_UnusedStk_obj, tos_UnusedStk);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_YearStartDate_obj, tos_YearStartDate);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Freq2Ona_obj, tos_Freq2Ona);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_AdamErr_obj, tos_AdamErr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_AdamLog_obj, tos_AdamLog);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ArcCompressBuf_obj, tos_ArcCompressBuf);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ArcCtrlDel_obj, tos_ArcCtrlDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ArcExpandBuf_obj, tos_ArcExpandBuf);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Beep_obj, tos_Beep);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_BlkDevDel_obj, tos_BlkDevDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_BlkDevsRelease_obj, tos_BlkDevsRelease);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_BlkPoolAdd_obj, tos_BlkPoolAdd);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_BlkPoolInit_obj, tos_BlkPoolInit);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_BlkWriteZero_obj, tos_BlkWriteZero);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_Break_obj, tos_Break);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_CallerRep_obj, tos_CallerRep);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Date2Struct_obj, tos_Date2Struct);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Da_obj, tos_Da);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Dbg_obj, tos_Dbg);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DeathWait_obj, tos_DeathWait);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_DefinePrint_obj, tos_DefinePrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DirContextDel_obj, tos_DirContextDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DirEntryDel2_obj, tos_DirEntryDel2);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DirEntryDel_obj, tos_DirEntryDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DirTreeDel2_obj, tos_DirTreeDel2);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DirTreeDel_obj, tos_DirTreeDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Dm_obj, tos_Dm);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Dr_obj, tos_Dr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvDel_obj, tos_DrvDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_DrvRep_obj, tos_DrvRep);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_DrvsRelease_obj, tos_DrvsRelease);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DskCacheInvalidate_obj, tos_DskCacheInvalidate);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DskChg_obj, tos_DskChg);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_D_obj, tos_D);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_DVDImageRead_obj, tos_DVDImageRead);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_Exit_obj, tos_Exit);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FClose_obj, tos_FClose);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FifoI64Del_obj, tos_FifoI64Del);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FifoI64Flush_obj, tos_FifoI64Flush);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FifoU8Del_obj, tos_FifoU8Del);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FifoU8Flush_obj, tos_FifoU8Flush);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FixSet_obj, tos_FixSet);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Free_obj, tos_Free);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_G2_obj, tos_G2);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_GetOutOfDollar_obj, tos_GetOutOfDollar);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_GridInit_obj, tos_GridInit);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_G_obj, tos_G);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_HashAdd_obj, tos_HashAdd);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashDel_obj, tos_HashDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HashTableDel_obj, tos_HashTableDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HeapCtrlDel_obj, tos_HeapCtrlDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_Help_obj, tos_Help);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_HomeSet_obj, tos_HomeSet);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_InFile_obj, tos_InFile);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_InStr_obj, tos_InStr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_In_obj, tos_In);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_JobDel_obj, tos_JobDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_KbdTypeMatic_obj, tos_KbdTypeMatic);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_KeyDescSet_obj, tos_KeyDescSet);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_KeyDevRem_obj, tos_KeyDevRem);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_LinkedLstDel_obj, tos_LinkedLstDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Mem32DevFree_obj, tos_Mem32DevFree);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Mem64DevFree_obj, tos_Mem64DevFree);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MemPagFree_obj, tos_MemPagFree);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_MPHalt_obj, tos_MPHalt);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MPIntAll_obj, tos_MPIntAll);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MPInt_obj, tos_MPInt);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_MPNMInt_obj, tos_MPNMInt);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_Msg_obj, tos_Msg);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Panic_obj, tos_Panic);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PrintErr_obj, tos_PrintErr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Print_obj, tos_Print);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PrintWarn_obj, tos_PrintWarn);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ProgressBarsRst_obj, tos_ProgressBarsRst);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PutChars_obj, tos_PutChars);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PutDirLink_obj, tos_PutDirLink);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PutExcept_obj, tos_PutExcept);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_PutKey_obj, tos_PutKey);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_QueDel_obj, tos_QueDel);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_QueVectU8Del_obj, tos_QueVectU8Del);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_QueVectU8Put_obj, tos_QueVectU8Put);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_RawD_obj, tos_RawD);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_RawDm_obj, tos_RawDm);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_RawPrint_obj, tos_RawPrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_RawPutChar_obj, tos_RawPutChar);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_Reboot_obj, tos_Reboot);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_ScanFlags_obj, tos_ScanFlags);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Sleep_obj, tos_Sleep);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_SleepUntil_obj, tos_SleepUntil);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Snd_obj, tos_Snd);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_SndRst_obj, tos_SndRst);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_SrvTaskCont_obj, tos_SrvTaskCont);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_StkRep_obj, tos_StkRep);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrCpy_obj, tos_StrCpy);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_StreamPrint_obj, tos_StreamPrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_S_obj, tos_S);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_TaskDerivedValsUpdate_obj, tos_TaskDerivedValsUpdate);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_TaskQueIns_obj, tos_TaskQueIns);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_TaskQueRem_obj, tos_TaskQueRem);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_TaskRstAwaitingMsg_obj, tos_TaskRstAwaitingMsg);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_TaskWait_obj, tos_TaskWait);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_throw_obj, tos_throw);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_ToFileLine_obj, tos_ToFileLine);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_TSSBusy_obj, tos_TSSBusy);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_UserCmdLine_obj, tos_UserCmdLine);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_UserTaskCont_obj, tos_UserTaskCont);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_VGAFlush_obj, tos_VGAFlush);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_WinDerivedValsUpdate_obj, tos_WinDerivedValsUpdate);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_XTalk_obj, tos_XTalk);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_XTalkStr_obj, tos_XTalkStr);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_XTalkStrWait_obj, tos_XTalkStrWait);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_XTalkWait_obj, tos_XTalkWait);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_Yield_obj, tos_Yield);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_EndianU16_obj, tos_EndianU16);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_PCIReadU16_obj, tos_PCIReadU16);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_RandU16_obj, tos_RandU16);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_EndianU32_obj, tos_EndianU32);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_PCIReadU32_obj, tos_PCIReadU32);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_RandU32_obj, tos_RandU32);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_CeilU64_obj, tos_CeilU64);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_FloorU64_obj, tos_FloorU64);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_RandU64_obj, tos_RandU64);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ACAlloc_obj, tos_ACAlloc);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_AMAlloc_obj, tos_AMAlloc);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_AMAllocIdent_obj, tos_AMAllocIdent);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_AStrNew_obj, tos_AStrNew);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Caller_obj, tos_Caller);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_CAlloc_obj, tos_CAlloc);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_CatPrint_obj, tos_CatPrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Char2KeyName_obj, tos_Char2KeyName);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Color2Str_obj, tos_Color2Str);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_DefineSub_obj, tos_DefineSub);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Define_obj, tos_Define);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DirFile_obj, tos_DirFile);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DirNameAbs_obj, tos_DirNameAbs);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Drv2Let_obj, tos_Drv2Let);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvModelNum_obj, tos_DrvModelNum);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvNextFreeLet_obj, tos_DrvNextFreeLet);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvSerialNum_obj, tos_DrvSerialNum);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DrvTextAttrGet_obj, tos_DrvTextAttrGet);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ExpandBuf_obj, tos_ExpandBuf);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_ExtChg_obj, tos_ExtChg);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_ExtDft_obj, tos_ExtDft);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FileExtDot_obj, tos_FileExtDot);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FileExtRem_obj, tos_FileExtRem);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FileNameAbs_obj, tos_FileNameAbs);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_FileNameTmpTxt_obj, tos_FileNameTmpTxt);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_FileRead_obj, tos_FileRead);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_GetStr_obj, tos_GetStr);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_IntEntryGet_obj, tos_IntEntryGet);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Let2Let_obj, tos_Let2Let);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_LinkedLstCopy_obj, tos_LinkedLstCopy);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_LstSub_obj, tos_LstSub);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MAlloc_obj, tos_MAlloc);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MAllocIdent_obj, tos_MAllocIdent);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Mem32DevAlloc_obj, tos_Mem32DevAlloc);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Mem64DevAlloc_obj, tos_Mem64DevAlloc);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MemPagAlloc_obj, tos_MemPagAlloc);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_MStrPrint_obj, tos_MStrPrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_MStrUtil_obj, tos_MStrUtil);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_PCIReadU8_obj, tos_PCIReadU8);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_ScaleIndent_obj, tos_ScaleIndent);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ScanCode2Char_obj, tos_ScanCode2Char);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ScanCode2KeyName_obj, tos_ScanCode2KeyName);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_SrcEdLink_obj, tos_SrcEdLink);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_SrcFileName_obj, tos_SrcFileName);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrFind_obj, tos_StrFind);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrFirstOcc_obj, tos_StrFirstOcc);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrFirstRem_obj, tos_StrFirstRem);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrIMatch_obj, tos_StrIMatch);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrLastOcc_obj, tos_StrLastOcc);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrLastRem_obj, tos_StrLastRem);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrMatch_obj, tos_StrMatch);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_StrPrintFlags_obj, tos_StrPrintFlags);
STATIC MP_DEFINE_CONST_FUN_OBJ_4(tos_StrPrintJoin_obj, tos_StrPrintJoin);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrPrint_obj, tos_StrPrint);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrScan_obj, tos_StrScan);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_StrUtil_obj, tos_StrUtil);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Tabs2Spaces_obj, tos_Tabs2Spaces);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_ToggleZorNotZ_obj, tos_ToggleZorNotZ);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_MemCmp_obj, tos_MemCmp);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_MemCpy_obj, tos_MemCpy);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_MemSet_obj, tos_MemSet);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_MemSetI64_obj, tos_MemSetI64);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_MemSetU16_obj, tos_MemSetU16);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_MemSetU32_obj, tos_MemSetU32);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(tos_MemSetU8_obj, tos_MemSetU8);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_StrLen_obj, tos_StrLen);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_InU8_obj, tos_InU8);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_OutU8_obj, tos_OutU8);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Uf_obj, tos_Uf);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_U_obj, tos_U);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_StrNew_obj, tos_StrNew);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DelTree_obj, tos_DelTree);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_CopyTree_obj, tos_CopyTree);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Move_obj, tos_Move);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tos_Copy_obj, tos_Copy);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Del_obj, tos_Del);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Cd_obj, tos_Cd);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_Ed_obj, tos_Ed);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_Dir_obj, tos_Dir);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DirMk_obj, tos_DirMk);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(tos_DirCur_obj, tos_DirCur);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tos_DocWrite_obj, tos_DocWrite);

STATIC const mp_rom_map_elem_t tos_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_RandInt), MP_ROM_PTR(&tos_get_rnd_int_obj) },
    { MP_ROM_QSTR(MP_QSTR_RandWord), MP_ROM_PTR(&tos_god_word_obj) },
    { MP_ROM_QSTR(MP_QSTR_RandWordUPick), MP_ROM_PTR(&tos_god_word_pick_obj) },
    { MP_ROM_QSTR(MP_QSTR_GetInput), MP_ROM_PTR(&tos_get_input_obj) },
    { MP_ROM_QSTR(MP_QSTR_PopUpGetInput), MP_ROM_PTR(&tos_popup_get_input_obj) },
    { MP_ROM_QSTR(MP_QSTR_HolyCExec), MP_ROM_PTR(&tos_exec_buf_obj) },
    { MP_ROM_QSTR(MP_QSTR_P2S), MP_ROM_PTR(&tos_get_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_S2P), MP_ROM_PTR(&tos_make_str_obj) },
    { MP_ROM_QSTR(MP_QSTR_P2A), MP_ROM_PTR(&tos_ptr_to_addr_obj) },
    { MP_ROM_QSTR(MP_QSTR_PopUpYN), MP_ROM_PTR(&tos_popup_yn_obj) },
    { MP_ROM_QSTR(MP_QSTR_PeekPtr64), MP_ROM_PTR(&tos_get_uint64_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_PokePtr64), MP_ROM_PTR(&tos_put_uint64_obj) },
    { MP_ROM_QSTR(MP_QSTR_PeekPtr8), MP_ROM_PTR(&tos_get_uint8_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_PokePtr8), MP_ROM_PTR(&tos_put_uint8_obj) },
    { MP_ROM_QSTR(MP_QSTR_Peek), MP_ROM_PTR(&tos_peek_uint8_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_Poke), MP_ROM_PTR(&tos_poke_uint8_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_BibleView), MP_ROM_PTR(&tos_bible_view_obj) },
    { MP_ROM_QSTR(MP_QSTR_RandBible), MP_ROM_PTR(&tos_bible_passage_obj) },
    { MP_ROM_QSTR(MP_QSTR_RandBibleUPick), MP_ROM_PTR(&tos_bible_passage_pick_obj) },
{ MP_ROM_QSTR(MP_QSTR_ArcFinishCompression), MP_ROM_PTR(&tos_ArcFinishCompression_obj) },
{ MP_ROM_QSTR(MP_QSTR_AreYouSure), MP_ROM_PTR(&tos_AreYouSure_obj) },
{ MP_ROM_QSTR(MP_QSTR_Blink), MP_ROM_PTR(&tos_Blink_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkDevLock), MP_ROM_PTR(&tos_BlkDevLock_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkDevUnlock), MP_ROM_PTR(&tos_BlkDevUnlock_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkRead), MP_ROM_PTR(&tos_BlkRead_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkWrite), MP_ROM_PTR(&tos_BlkWrite_obj) },
{ MP_ROM_QSTR(MP_QSTR_BptR), MP_ROM_PTR(&tos_BptR_obj) },
{ MP_ROM_QSTR(MP_QSTR_BptS), MP_ROM_PTR(&tos_BptS_obj) },
{ MP_ROM_QSTR(MP_QSTR_BreakLock), MP_ROM_PTR(&tos_BreakLock_obj) },
{ MP_ROM_QSTR(MP_QSTR_BreakUnlock), MP_ROM_PTR(&tos_BreakUnlock_obj) },
{ MP_ROM_QSTR(MP_QSTR_ChkCodePtr), MP_ROM_PTR(&tos_ChkCodePtr_obj) },
{ MP_ROM_QSTR(MP_QSTR_ChkOnStk), MP_ROM_PTR(&tos_ChkOnStk_obj) },
{ MP_ROM_QSTR(MP_QSTR_ChkPtr), MP_ROM_PTR(&tos_ChkPtr_obj) },
{ MP_ROM_QSTR(MP_QSTR_DbgMode), MP_ROM_PTR(&tos_DbgMode_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvEnable), MP_ROM_PTR(&tos_DrvEnable_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvLock), MP_ROM_PTR(&tos_DrvLock_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvMap), MP_ROM_PTR(&tos_DrvMap_obj) },
{ MP_ROM_QSTR(MP_QSTR_Drv), MP_ROM_PTR(&tos_Drv_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvUnlock), MP_ROM_PTR(&tos_DrvUnlock_obj) },
{ MP_ROM_QSTR(MP_QSTR_Echo), MP_ROM_PTR(&tos_Echo_obj) },
{ MP_ROM_QSTR(MP_QSTR_EdLite), MP_ROM_PTR(&tos_EdLite_obj) },
{ MP_ROM_QSTR(MP_QSTR_E), MP_ROM_PTR(&tos_E_obj) },
{ MP_ROM_QSTR(MP_QSTR_FBlkRead), MP_ROM_PTR(&tos_FBlkRead_obj) },
{ MP_ROM_QSTR(MP_QSTR_FBlkWrite), MP_ROM_PTR(&tos_FBlkWrite_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoI64Ins), MP_ROM_PTR(&tos_FifoI64Ins_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoI64Peek), MP_ROM_PTR(&tos_FifoI64Peek_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoI64Rem), MP_ROM_PTR(&tos_FifoI64Rem_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoU8Ins), MP_ROM_PTR(&tos_FifoU8Ins_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoU8Peek), MP_ROM_PTR(&tos_FifoU8Peek_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoU8Rem), MP_ROM_PTR(&tos_FifoU8Rem_obj) },
{ MP_ROM_QSTR(MP_QSTR_FileFind), MP_ROM_PTR(&tos_FileFind_obj) },
{ MP_ROM_QSTR(MP_QSTR_FileNameChk), MP_ROM_PTR(&tos_FileNameChk_obj) },
{ MP_ROM_QSTR(MP_QSTR_Fix), MP_ROM_PTR(&tos_Fix_obj) },
{ MP_ROM_QSTR(MP_QSTR_GetOption), MP_ROM_PTR(&tos_GetOption_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsDbgMode), MP_ROM_PTR(&tos_IsDbgMode_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsDir), MP_ROM_PTR(&tos_IsDir_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsDotC), MP_ROM_PTR(&tos_IsDotC_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsDotZ), MP_ROM_PTR(&tos_IsDotZ_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsMute), MP_ROM_PTR(&tos_IsMute_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsRaw), MP_ROM_PTR(&tos_IsRaw_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsSilent), MP_ROM_PTR(&tos_IsSilent_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsSingleUser), MP_ROM_PTR(&tos_IsSingleUser_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsSuspended), MP_ROM_PTR(&tos_IsSuspended_obj) },
{ MP_ROM_QSTR(MP_QSTR_IsSysDbg), MP_ROM_PTR(&tos_IsSysDbg_obj) },
{ MP_ROM_QSTR(MP_QSTR_JobResScan), MP_ROM_PTR(&tos_JobResScan_obj) },
{ MP_ROM_QSTR(MP_QSTR_Kill), MP_ROM_PTR(&tos_Kill_obj) },
{ MP_ROM_QSTR(MP_QSTR_Man), MP_ROM_PTR(&tos_Man_obj) },
{ MP_ROM_QSTR(MP_QSTR_Mute), MP_ROM_PTR(&tos_Mute_obj) },
{ MP_ROM_QSTR(MP_QSTR_PutSrcLink), MP_ROM_PTR(&tos_PutSrcLink_obj) },
{ MP_ROM_QSTR(MP_QSTR_Raw), MP_ROM_PTR(&tos_Raw_obj) },
{ MP_ROM_QSTR(MP_QSTR_ScanKey), MP_ROM_PTR(&tos_ScanKey_obj) },
{ MP_ROM_QSTR(MP_QSTR_Silent), MP_ROM_PTR(&tos_Silent_obj) },
{ MP_ROM_QSTR(MP_QSTR_SingleUser), MP_ROM_PTR(&tos_SingleUser_obj) },
{ MP_ROM_QSTR(MP_QSTR_Suspend), MP_ROM_PTR(&tos_Suspend_obj) },
{ MP_ROM_QSTR(MP_QSTR_SysDbg), MP_ROM_PTR(&tos_SysDbg_obj) },
{ MP_ROM_QSTR(MP_QSTR_TaskValidate), MP_ROM_PTR(&tos_TaskValidate_obj) },
{ MP_ROM_QSTR(MP_QSTR_Trace), MP_ROM_PTR(&tos_Trace_obj) },
{ MP_ROM_QSTR(MP_QSTR_WildMatch), MP_ROM_PTR(&tos_WildMatch_obj) },
{ MP_ROM_QSTR(MP_QSTR_WinInside), MP_ROM_PTR(&tos_WinInside_obj) },
{ MP_ROM_QSTR(MP_QSTR_YorN), MP_ROM_PTR(&tos_YorN_obj) },
{ MP_ROM_QSTR(MP_QSTR_CompressBuf), MP_ROM_PTR(&tos_CompressBuf_obj) },
{ MP_ROM_QSTR(MP_QSTR_ArcCtrlNew), MP_ROM_PTR(&tos_ArcCtrlNew_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkDevChk), MP_ROM_PTR(&tos_BlkDevChk_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkDevNextFreeSlot), MP_ROM_PTR(&tos_BlkDevNextFreeSlot_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvIsWritable), MP_ROM_PTR(&tos_DrvIsWritable_obj) },
{ MP_ROM_QSTR(MP_QSTR_Let2BlkDev), MP_ROM_PTR(&tos_Let2BlkDev_obj) },
{ MP_ROM_QSTR(MP_QSTR_Str2ColorU16), MP_ROM_PTR(&tos_Str2ColorU16_obj) },
{ MP_ROM_QSTR(MP_QSTR_Str2ColorU32), MP_ROM_PTR(&tos_Str2ColorU32_obj) },
{ MP_ROM_QSTR(MP_QSTR_Now), MP_ROM_PTR(&tos_Now_obj) },
{ MP_ROM_QSTR(MP_QSTR_Str2Date), MP_ROM_PTR(&tos_Str2Date_obj) },
{ MP_ROM_QSTR(MP_QSTR_Struct2Date), MP_ROM_PTR(&tos_Struct2Date_obj) },
{ MP_ROM_QSTR(MP_QSTR_FilesFind), MP_ROM_PTR(&tos_FilesFind_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvChk), MP_ROM_PTR(&tos_DrvChk_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvMakeFreeSlot), MP_ROM_PTR(&tos_DrvMakeFreeSlot_obj) },
{ MP_ROM_QSTR(MP_QSTR_Let2Drv), MP_ROM_PTR(&tos_Let2Drv_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoI64New), MP_ROM_PTR(&tos_FifoI64New_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoU8New), MP_ROM_PTR(&tos_FifoU8New_obj) },
{ MP_ROM_QSTR(MP_QSTR_FOpen), MP_ROM_PTR(&tos_FOpen_obj) },
{ MP_ROM_QSTR(MP_QSTR_DefineLoad), MP_ROM_PTR(&tos_DefineLoad_obj) },
{ MP_ROM_QSTR(MP_QSTR_DefineLstLoad), MP_ROM_PTR(&tos_DefineLstLoad_obj) },
{ MP_ROM_QSTR(MP_QSTR_FunSegFind), MP_ROM_PTR(&tos_FunSegFind_obj) },
{ MP_ROM_QSTR(MP_QSTR_FramePtrAdd), MP_ROM_PTR(&tos_FramePtrAdd_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashPublic), MP_ROM_PTR(&tos_HashPublic_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashTableNew), MP_ROM_PTR(&tos_HashTableNew_obj) },
{ MP_ROM_QSTR(MP_QSTR_MHeapCtrl), MP_ROM_PTR(&tos_MHeapCtrl_obj) },
{ MP_ROM_QSTR(MP_QSTR_TaskExe), MP_ROM_PTR(&tos_TaskExe_obj) },
{ MP_ROM_QSTR(MP_QSTR_TaskText), MP_ROM_PTR(&tos_TaskText_obj) },
{ MP_ROM_QSTR(MP_QSTR_QueCopy), MP_ROM_PTR(&tos_QueCopy_obj) },
{ MP_ROM_QSTR(MP_QSTR_QueVectU8New), MP_ROM_PTR(&tos_QueVectU8New_obj) },
{ MP_ROM_QSTR(MP_QSTR_User), MP_ROM_PTR(&tos_User_obj) },
{ MP_ROM_QSTR(MP_QSTR_Clamp), MP_ROM_PTR(&tos_Clamp_obj) },
{ MP_ROM_QSTR(MP_QSTR_Max), MP_ROM_PTR(&tos_Max_obj) },
{ MP_ROM_QSTR(MP_QSTR_Min), MP_ROM_PTR(&tos_Min_obj) },
{ MP_ROM_QSTR(MP_QSTR_Ona2Freq), MP_ROM_PTR(&tos_Ona2Freq_obj) },
{ MP_ROM_QSTR(MP_QSTR_Pow10I64), MP_ROM_PTR(&tos_Pow10I64_obj) },
{ MP_ROM_QSTR(MP_QSTR_Str2F64), MP_ROM_PTR(&tos_Str2F64_obj) },
{ MP_ROM_QSTR(MP_QSTR_RandI16), MP_ROM_PTR(&tos_RandI16_obj) },
{ MP_ROM_QSTR(MP_QSTR_RandI32), MP_ROM_PTR(&tos_RandI32_obj) },
{ MP_ROM_QSTR(MP_QSTR_Adam), MP_ROM_PTR(&tos_Adam_obj) },
{ MP_ROM_QSTR(MP_QSTR_ATARep), MP_ROM_PTR(&tos_ATARep_obj) },
{ MP_ROM_QSTR(MP_QSTR_B2), MP_ROM_PTR(&tos_B2_obj) },
{ MP_ROM_QSTR(MP_QSTR_BCnt), MP_ROM_PTR(&tos_BCnt_obj) },
{ MP_ROM_QSTR(MP_QSTR_BirthWait), MP_ROM_PTR(&tos_BirthWait_obj) },
{ MP_ROM_QSTR(MP_QSTR_CeilI64), MP_ROM_PTR(&tos_CeilI64_obj) },
{ MP_ROM_QSTR(MP_QSTR_Char2ScanCode), MP_ROM_PTR(&tos_Char2ScanCode_obj) },
{ MP_ROM_QSTR(MP_QSTR_Clus2Blk), MP_ROM_PTR(&tos_Clus2Blk_obj) },
{ MP_ROM_QSTR(MP_QSTR_ClusBlkRead), MP_ROM_PTR(&tos_ClusBlkRead_obj) },
{ MP_ROM_QSTR(MP_QSTR_ClusBlkWrite), MP_ROM_PTR(&tos_ClusBlkWrite_obj) },
{ MP_ROM_QSTR(MP_QSTR_ClusNumNext), MP_ROM_PTR(&tos_ClusNumNext_obj) },
{ MP_ROM_QSTR(MP_QSTR_ClusRead), MP_ROM_PTR(&tos_ClusRead_obj) },
{ MP_ROM_QSTR(MP_QSTR_ClusWrite), MP_ROM_PTR(&tos_ClusWrite_obj) },
{ MP_ROM_QSTR(MP_QSTR_DayOfWeek), MP_ROM_PTR(&tos_DayOfWeek_obj) },
{ MP_ROM_QSTR(MP_QSTR_DefineCnt), MP_ROM_PTR(&tos_DefineCnt_obj) },
{ MP_ROM_QSTR(MP_QSTR_DefineMatch), MP_ROM_PTR(&tos_DefineMatch_obj) },
{ MP_ROM_QSTR(MP_QSTR_EndianI64), MP_ROM_PTR(&tos_EndianI64_obj) },
{ MP_ROM_QSTR(MP_QSTR_ExeFile), MP_ROM_PTR(&tos_ExeFile_obj) },
{ MP_ROM_QSTR(MP_QSTR_ExePrint), MP_ROM_PTR(&tos_ExePrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoI64Cnt), MP_ROM_PTR(&tos_FifoI64Cnt_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoU8Cnt), MP_ROM_PTR(&tos_FifoU8Cnt_obj) },
{ MP_ROM_QSTR(MP_QSTR_FirstDayOfMon), MP_ROM_PTR(&tos_FirstDayOfMon_obj) },
{ MP_ROM_QSTR(MP_QSTR_FirstDayOfYear), MP_ROM_PTR(&tos_FirstDayOfYear_obj) },
{ MP_ROM_QSTR(MP_QSTR_FloorI64), MP_ROM_PTR(&tos_FloorI64_obj) },
{ MP_ROM_QSTR(MP_QSTR_FlushMsgs), MP_ROM_PTR(&tos_FlushMsgs_obj) },
{ MP_ROM_QSTR(MP_QSTR_FramePtrDel), MP_ROM_PTR(&tos_FramePtrDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_FramePtrSet), MP_ROM_PTR(&tos_FramePtrSet_obj) },
{ MP_ROM_QSTR(MP_QSTR_FramePtr), MP_ROM_PTR(&tos_FramePtr_obj) },
{ MP_ROM_QSTR(MP_QSTR_FSize), MP_ROM_PTR(&tos_FSize_obj) },
{ MP_ROM_QSTR(MP_QSTR_GetS), MP_ROM_PTR(&tos_GetS_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashDefineLstAdd), MP_ROM_PTR(&tos_HashDefineLstAdd_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashEntrySize2), MP_ROM_PTR(&tos_HashEntrySize2_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashEntrySize), MP_ROM_PTR(&tos_HashEntrySize_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashLstAdd), MP_ROM_PTR(&tos_HashLstAdd_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashStr), MP_ROM_PTR(&tos_HashStr_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashTablePurge), MP_ROM_PTR(&tos_HashTablePurge_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashTableSize2), MP_ROM_PTR(&tos_HashTableSize2_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashTypeNum), MP_ROM_PTR(&tos_HashTypeNum_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashVal), MP_ROM_PTR(&tos_HashVal_obj) },
{ MP_ROM_QSTR(MP_QSTR_JobResGet), MP_ROM_PTR(&tos_JobResGet_obj) },
{ MP_ROM_QSTR(MP_QSTR_JobsHndlr), MP_ROM_PTR(&tos_JobsHndlr_obj) },
{ MP_ROM_QSTR(MP_QSTR_KbdMsEvtTime), MP_ROM_PTR(&tos_KbdMsEvtTime_obj) },
{ MP_ROM_QSTR(MP_QSTR_LastDayOfMon), MP_ROM_PTR(&tos_LastDayOfMon_obj) },
{ MP_ROM_QSTR(MP_QSTR_LastDayOfYear), MP_ROM_PTR(&tos_LastDayOfYear_obj) },
{ MP_ROM_QSTR(MP_QSTR_Let2BlkDevType), MP_ROM_PTR(&tos_Let2BlkDevType_obj) },
{ MP_ROM_QSTR(MP_QSTR_LinkedLstCnt), MP_ROM_PTR(&tos_LinkedLstCnt_obj) },
{ MP_ROM_QSTR(MP_QSTR_LinkedLstSize), MP_ROM_PTR(&tos_LinkedLstSize_obj) },
{ MP_ROM_QSTR(MP_QSTR_LstMatch), MP_ROM_PTR(&tos_LstMatch_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemPageTable), MP_ROM_PTR(&tos_MemPageTable_obj) },
{ MP_ROM_QSTR(MP_QSTR_MountIDEAuto), MP_ROM_PTR(&tos_MountIDEAuto_obj) },
{ MP_ROM_QSTR(MP_QSTR_PopUpPrint), MP_ROM_PTR(&tos_PopUpPrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_PopUp), MP_ROM_PTR(&tos_PopUp_obj) },
{ MP_ROM_QSTR(MP_QSTR_PressAKey), MP_ROM_PTR(&tos_PressAKey_obj) },
{ MP_ROM_QSTR(MP_QSTR_QueCnt), MP_ROM_PTR(&tos_QueCnt_obj) },
{ MP_ROM_QSTR(MP_QSTR_QueSize), MP_ROM_PTR(&tos_QueSize_obj) },
{ MP_ROM_QSTR(MP_QSTR_QueVectU8Get), MP_ROM_PTR(&tos_QueVectU8Get_obj) },
{ MP_ROM_QSTR(MP_QSTR_RandI64), MP_ROM_PTR(&tos_RandI64_obj) },
{ MP_ROM_QSTR(MP_QSTR_RoundI64), MP_ROM_PTR(&tos_RoundI64_obj) },
{ MP_ROM_QSTR(MP_QSTR_RunFile2), MP_ROM_PTR(&tos_RunFile2_obj) },
{ MP_ROM_QSTR(MP_QSTR_RunFile), MP_ROM_PTR(&tos_RunFile_obj) },
{ MP_ROM_QSTR(MP_QSTR_Scale2Mem), MP_ROM_PTR(&tos_Scale2Mem_obj) },
{ MP_ROM_QSTR(MP_QSTR_ScanChar), MP_ROM_PTR(&tos_ScanChar_obj) },
{ MP_ROM_QSTR(MP_QSTR_Seed), MP_ROM_PTR(&tos_Seed_obj) },
{ MP_ROM_QSTR(MP_QSTR_Spaces2Tabs), MP_ROM_PTR(&tos_Spaces2Tabs_obj) },
{ MP_ROM_QSTR(MP_QSTR_SrcLineNum), MP_ROM_PTR(&tos_SrcLineNum_obj) },
{ MP_ROM_QSTR(MP_QSTR_Str2I64), MP_ROM_PTR(&tos_Str2I64_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrCmp), MP_ROM_PTR(&tos_StrCmp_obj) },
{ MP_ROM_QSTR(MP_QSTR_StreamExePrint), MP_ROM_PTR(&tos_StreamExePrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrICmp), MP_ROM_PTR(&tos_StrICmp_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrNCmp), MP_ROM_PTR(&tos_StrNCmp_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrNICmp), MP_ROM_PTR(&tos_StrNICmp_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrOcc), MP_ROM_PTR(&tos_StrOcc_obj) },
{ MP_ROM_QSTR(MP_QSTR_Un), MP_ROM_PTR(&tos_Un_obj) },
{ MP_ROM_QSTR(MP_QSTR_UnusedStk), MP_ROM_PTR(&tos_UnusedStk_obj) },
{ MP_ROM_QSTR(MP_QSTR_YearStartDate), MP_ROM_PTR(&tos_YearStartDate_obj) },
{ MP_ROM_QSTR(MP_QSTR_Freq2Ona), MP_ROM_PTR(&tos_Freq2Ona_obj) },
{ MP_ROM_QSTR(MP_QSTR_AdamErr), MP_ROM_PTR(&tos_AdamErr_obj) },
{ MP_ROM_QSTR(MP_QSTR_AdamLog), MP_ROM_PTR(&tos_AdamLog_obj) },
{ MP_ROM_QSTR(MP_QSTR_ArcCompressBuf), MP_ROM_PTR(&tos_ArcCompressBuf_obj) },
{ MP_ROM_QSTR(MP_QSTR_ArcCtrlDel), MP_ROM_PTR(&tos_ArcCtrlDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_ArcExpandBuf), MP_ROM_PTR(&tos_ArcExpandBuf_obj) },
{ MP_ROM_QSTR(MP_QSTR_Beep), MP_ROM_PTR(&tos_Beep_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkDevDel), MP_ROM_PTR(&tos_BlkDevDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkDevsRelease), MP_ROM_PTR(&tos_BlkDevsRelease_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkPoolAdd), MP_ROM_PTR(&tos_BlkPoolAdd_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkPoolInit), MP_ROM_PTR(&tos_BlkPoolInit_obj) },
{ MP_ROM_QSTR(MP_QSTR_BlkWriteZero), MP_ROM_PTR(&tos_BlkWriteZero_obj) },
{ MP_ROM_QSTR(MP_QSTR_Break), MP_ROM_PTR(&tos_Break_obj) },
{ MP_ROM_QSTR(MP_QSTR_CallerRep), MP_ROM_PTR(&tos_CallerRep_obj) },
{ MP_ROM_QSTR(MP_QSTR_Date2Struct), MP_ROM_PTR(&tos_Date2Struct_obj) },
{ MP_ROM_QSTR(MP_QSTR_Da), MP_ROM_PTR(&tos_Da_obj) },
{ MP_ROM_QSTR(MP_QSTR_Dbg), MP_ROM_PTR(&tos_Dbg_obj) },
{ MP_ROM_QSTR(MP_QSTR_DeathWait), MP_ROM_PTR(&tos_DeathWait_obj) },
{ MP_ROM_QSTR(MP_QSTR_DefinePrint), MP_ROM_PTR(&tos_DefinePrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirContextDel), MP_ROM_PTR(&tos_DirContextDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirEntryDel2), MP_ROM_PTR(&tos_DirEntryDel2_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirEntryDel), MP_ROM_PTR(&tos_DirEntryDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirTreeDel2), MP_ROM_PTR(&tos_DirTreeDel2_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirTreeDel), MP_ROM_PTR(&tos_DirTreeDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_Dm), MP_ROM_PTR(&tos_Dm_obj) },
{ MP_ROM_QSTR(MP_QSTR_Dr), MP_ROM_PTR(&tos_Dr_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvDel), MP_ROM_PTR(&tos_DrvDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvRep), MP_ROM_PTR(&tos_DrvRep_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvsRelease), MP_ROM_PTR(&tos_DrvsRelease_obj) },
{ MP_ROM_QSTR(MP_QSTR_DskCacheInvalidate), MP_ROM_PTR(&tos_DskCacheInvalidate_obj) },
{ MP_ROM_QSTR(MP_QSTR_DskChg), MP_ROM_PTR(&tos_DskChg_obj) },
{ MP_ROM_QSTR(MP_QSTR_D), MP_ROM_PTR(&tos_D_obj) },
{ MP_ROM_QSTR(MP_QSTR_DVDImageRead), MP_ROM_PTR(&tos_DVDImageRead_obj) },
{ MP_ROM_QSTR(MP_QSTR_Exit), MP_ROM_PTR(&tos_Exit_obj) },
{ MP_ROM_QSTR(MP_QSTR_FClose), MP_ROM_PTR(&tos_FClose_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoI64Del), MP_ROM_PTR(&tos_FifoI64Del_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoI64Flush), MP_ROM_PTR(&tos_FifoI64Flush_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoU8Del), MP_ROM_PTR(&tos_FifoU8Del_obj) },
{ MP_ROM_QSTR(MP_QSTR_FifoU8Flush), MP_ROM_PTR(&tos_FifoU8Flush_obj) },
{ MP_ROM_QSTR(MP_QSTR_FixSet), MP_ROM_PTR(&tos_FixSet_obj) },
{ MP_ROM_QSTR(MP_QSTR_Free), MP_ROM_PTR(&tos_Free_obj) },
{ MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&tos_G2_obj) },
{ MP_ROM_QSTR(MP_QSTR_GetOutOfDollar), MP_ROM_PTR(&tos_GetOutOfDollar_obj) },
{ MP_ROM_QSTR(MP_QSTR_GridInit), MP_ROM_PTR(&tos_GridInit_obj) },
{ MP_ROM_QSTR(MP_QSTR_G), MP_ROM_PTR(&tos_G_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashAdd), MP_ROM_PTR(&tos_HashAdd_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashDel), MP_ROM_PTR(&tos_HashDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_HashTableDel), MP_ROM_PTR(&tos_HashTableDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_HeapCtrlDel), MP_ROM_PTR(&tos_HeapCtrlDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_Help), MP_ROM_PTR(&tos_Help_obj) },
{ MP_ROM_QSTR(MP_QSTR_HomeSet), MP_ROM_PTR(&tos_HomeSet_obj) },
{ MP_ROM_QSTR(MP_QSTR_InFile), MP_ROM_PTR(&tos_InFile_obj) },
{ MP_ROM_QSTR(MP_QSTR_InStr), MP_ROM_PTR(&tos_InStr_obj) },
{ MP_ROM_QSTR(MP_QSTR_In), MP_ROM_PTR(&tos_In_obj) },
{ MP_ROM_QSTR(MP_QSTR_JobDel), MP_ROM_PTR(&tos_JobDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_KbdTypeMatic), MP_ROM_PTR(&tos_KbdTypeMatic_obj) },
{ MP_ROM_QSTR(MP_QSTR_KeyDescSet), MP_ROM_PTR(&tos_KeyDescSet_obj) },
{ MP_ROM_QSTR(MP_QSTR_KeyDevRem), MP_ROM_PTR(&tos_KeyDevRem_obj) },
{ MP_ROM_QSTR(MP_QSTR_LinkedLstDel), MP_ROM_PTR(&tos_LinkedLstDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_Mem32DevFree), MP_ROM_PTR(&tos_Mem32DevFree_obj) },
{ MP_ROM_QSTR(MP_QSTR_Mem64DevFree), MP_ROM_PTR(&tos_Mem64DevFree_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemPagFree), MP_ROM_PTR(&tos_MemPagFree_obj) },
{ MP_ROM_QSTR(MP_QSTR_MPHalt), MP_ROM_PTR(&tos_MPHalt_obj) },
{ MP_ROM_QSTR(MP_QSTR_MPIntAll), MP_ROM_PTR(&tos_MPIntAll_obj) },
{ MP_ROM_QSTR(MP_QSTR_MPInt), MP_ROM_PTR(&tos_MPInt_obj) },
{ MP_ROM_QSTR(MP_QSTR_MPNMInt), MP_ROM_PTR(&tos_MPNMInt_obj) },
{ MP_ROM_QSTR(MP_QSTR_Msg), MP_ROM_PTR(&tos_Msg_obj) },
{ MP_ROM_QSTR(MP_QSTR_Panic), MP_ROM_PTR(&tos_Panic_obj) },
{ MP_ROM_QSTR(MP_QSTR_PrintErr), MP_ROM_PTR(&tos_PrintErr_obj) },
{ MP_ROM_QSTR(MP_QSTR_Print), MP_ROM_PTR(&tos_Print_obj) },
{ MP_ROM_QSTR(MP_QSTR_PrintWarn), MP_ROM_PTR(&tos_PrintWarn_obj) },
{ MP_ROM_QSTR(MP_QSTR_ProgressBarsRst), MP_ROM_PTR(&tos_ProgressBarsRst_obj) },
{ MP_ROM_QSTR(MP_QSTR_PutChars), MP_ROM_PTR(&tos_PutChars_obj) },
{ MP_ROM_QSTR(MP_QSTR_PutDirLink), MP_ROM_PTR(&tos_PutDirLink_obj) },
{ MP_ROM_QSTR(MP_QSTR_PutExcept), MP_ROM_PTR(&tos_PutExcept_obj) },
{ MP_ROM_QSTR(MP_QSTR_PutKey), MP_ROM_PTR(&tos_PutKey_obj) },
{ MP_ROM_QSTR(MP_QSTR_QueDel), MP_ROM_PTR(&tos_QueDel_obj) },
{ MP_ROM_QSTR(MP_QSTR_QueVectU8Del), MP_ROM_PTR(&tos_QueVectU8Del_obj) },
{ MP_ROM_QSTR(MP_QSTR_QueVectU8Put), MP_ROM_PTR(&tos_QueVectU8Put_obj) },
{ MP_ROM_QSTR(MP_QSTR_RawD), MP_ROM_PTR(&tos_RawD_obj) },
{ MP_ROM_QSTR(MP_QSTR_RawDm), MP_ROM_PTR(&tos_RawDm_obj) },
{ MP_ROM_QSTR(MP_QSTR_RawPrint), MP_ROM_PTR(&tos_RawPrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_RawPutChar), MP_ROM_PTR(&tos_RawPutChar_obj) },
{ MP_ROM_QSTR(MP_QSTR_Reboot), MP_ROM_PTR(&tos_Reboot_obj) },
{ MP_ROM_QSTR(MP_QSTR_ScanFlags), MP_ROM_PTR(&tos_ScanFlags_obj) },
{ MP_ROM_QSTR(MP_QSTR_Sleep), MP_ROM_PTR(&tos_Sleep_obj) },
{ MP_ROM_QSTR(MP_QSTR_SleepUntil), MP_ROM_PTR(&tos_SleepUntil_obj) },
{ MP_ROM_QSTR(MP_QSTR_Snd), MP_ROM_PTR(&tos_Snd_obj) },
{ MP_ROM_QSTR(MP_QSTR_SndRst), MP_ROM_PTR(&tos_SndRst_obj) },
{ MP_ROM_QSTR(MP_QSTR_SrvTaskCont), MP_ROM_PTR(&tos_SrvTaskCont_obj) },
{ MP_ROM_QSTR(MP_QSTR_StkRep), MP_ROM_PTR(&tos_StkRep_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrCpy), MP_ROM_PTR(&tos_StrCpy_obj) },
{ MP_ROM_QSTR(MP_QSTR_StreamPrint), MP_ROM_PTR(&tos_StreamPrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_S), MP_ROM_PTR(&tos_S_obj) },
{ MP_ROM_QSTR(MP_QSTR_TaskDerivedValsUpdate), MP_ROM_PTR(&tos_TaskDerivedValsUpdate_obj) },
{ MP_ROM_QSTR(MP_QSTR_TaskQueIns), MP_ROM_PTR(&tos_TaskQueIns_obj) },
{ MP_ROM_QSTR(MP_QSTR_TaskQueRem), MP_ROM_PTR(&tos_TaskQueRem_obj) },
{ MP_ROM_QSTR(MP_QSTR_TaskRstAwaitingMsg), MP_ROM_PTR(&tos_TaskRstAwaitingMsg_obj) },
{ MP_ROM_QSTR(MP_QSTR_TaskWait), MP_ROM_PTR(&tos_TaskWait_obj) },
{ MP_ROM_QSTR(MP_QSTR_throw), MP_ROM_PTR(&tos_throw_obj) },
{ MP_ROM_QSTR(MP_QSTR_ToFileLine), MP_ROM_PTR(&tos_ToFileLine_obj) },
{ MP_ROM_QSTR(MP_QSTR_TSSBusy), MP_ROM_PTR(&tos_TSSBusy_obj) },
{ MP_ROM_QSTR(MP_QSTR_UserCmdLine), MP_ROM_PTR(&tos_UserCmdLine_obj) },
{ MP_ROM_QSTR(MP_QSTR_UserTaskCont), MP_ROM_PTR(&tos_UserTaskCont_obj) },
{ MP_ROM_QSTR(MP_QSTR_VGAFlush), MP_ROM_PTR(&tos_VGAFlush_obj) },
{ MP_ROM_QSTR(MP_QSTR_WinDerivedValsUpdate), MP_ROM_PTR(&tos_WinDerivedValsUpdate_obj) },
{ MP_ROM_QSTR(MP_QSTR_XTalk), MP_ROM_PTR(&tos_XTalk_obj) },
{ MP_ROM_QSTR(MP_QSTR_XTalkStr), MP_ROM_PTR(&tos_XTalkStr_obj) },
{ MP_ROM_QSTR(MP_QSTR_XTalkStrWait), MP_ROM_PTR(&tos_XTalkStrWait_obj) },
{ MP_ROM_QSTR(MP_QSTR_XTalkWait), MP_ROM_PTR(&tos_XTalkWait_obj) },
{ MP_ROM_QSTR(MP_QSTR_Yield), MP_ROM_PTR(&tos_Yield_obj) },
{ MP_ROM_QSTR(MP_QSTR_EndianU16), MP_ROM_PTR(&tos_EndianU16_obj) },
{ MP_ROM_QSTR(MP_QSTR_PCIReadU16), MP_ROM_PTR(&tos_PCIReadU16_obj) },
{ MP_ROM_QSTR(MP_QSTR_RandU16), MP_ROM_PTR(&tos_RandU16_obj) },
{ MP_ROM_QSTR(MP_QSTR_EndianU32), MP_ROM_PTR(&tos_EndianU32_obj) },
{ MP_ROM_QSTR(MP_QSTR_PCIReadU32), MP_ROM_PTR(&tos_PCIReadU32_obj) },
{ MP_ROM_QSTR(MP_QSTR_RandU32), MP_ROM_PTR(&tos_RandU32_obj) },
{ MP_ROM_QSTR(MP_QSTR_CeilU64), MP_ROM_PTR(&tos_CeilU64_obj) },
{ MP_ROM_QSTR(MP_QSTR_FloorU64), MP_ROM_PTR(&tos_FloorU64_obj) },
{ MP_ROM_QSTR(MP_QSTR_RandU64), MP_ROM_PTR(&tos_RandU64_obj) },
{ MP_ROM_QSTR(MP_QSTR_ACAlloc), MP_ROM_PTR(&tos_ACAlloc_obj) },
{ MP_ROM_QSTR(MP_QSTR_AMAlloc), MP_ROM_PTR(&tos_AMAlloc_obj) },
{ MP_ROM_QSTR(MP_QSTR_AMAllocIdent), MP_ROM_PTR(&tos_AMAllocIdent_obj) },
{ MP_ROM_QSTR(MP_QSTR_AStrNew), MP_ROM_PTR(&tos_AStrNew_obj) },
{ MP_ROM_QSTR(MP_QSTR_Caller), MP_ROM_PTR(&tos_Caller_obj) },
{ MP_ROM_QSTR(MP_QSTR_CAlloc), MP_ROM_PTR(&tos_CAlloc_obj) },
{ MP_ROM_QSTR(MP_QSTR_CatPrint), MP_ROM_PTR(&tos_CatPrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_Char2KeyName), MP_ROM_PTR(&tos_Char2KeyName_obj) },
{ MP_ROM_QSTR(MP_QSTR_Color2Str), MP_ROM_PTR(&tos_Color2Str_obj) },
{ MP_ROM_QSTR(MP_QSTR_DefineSub), MP_ROM_PTR(&tos_DefineSub_obj) },
{ MP_ROM_QSTR(MP_QSTR_Define), MP_ROM_PTR(&tos_Define_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirFile), MP_ROM_PTR(&tos_DirFile_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirNameAbs), MP_ROM_PTR(&tos_DirNameAbs_obj) },
{ MP_ROM_QSTR(MP_QSTR_Drv2Let), MP_ROM_PTR(&tos_Drv2Let_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvModelNum), MP_ROM_PTR(&tos_DrvModelNum_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvNextFreeLet), MP_ROM_PTR(&tos_DrvNextFreeLet_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvSerialNum), MP_ROM_PTR(&tos_DrvSerialNum_obj) },
{ MP_ROM_QSTR(MP_QSTR_DrvTextAttrGet), MP_ROM_PTR(&tos_DrvTextAttrGet_obj) },
{ MP_ROM_QSTR(MP_QSTR_ExpandBuf), MP_ROM_PTR(&tos_ExpandBuf_obj) },
{ MP_ROM_QSTR(MP_QSTR_ExtChg), MP_ROM_PTR(&tos_ExtChg_obj) },
{ MP_ROM_QSTR(MP_QSTR_ExtDft), MP_ROM_PTR(&tos_ExtDft_obj) },
{ MP_ROM_QSTR(MP_QSTR_FileExtDot), MP_ROM_PTR(&tos_FileExtDot_obj) },
{ MP_ROM_QSTR(MP_QSTR_FileExtRem), MP_ROM_PTR(&tos_FileExtRem_obj) },
{ MP_ROM_QSTR(MP_QSTR_FileNameAbs), MP_ROM_PTR(&tos_FileNameAbs_obj) },
{ MP_ROM_QSTR(MP_QSTR_FileNameTmpTxt), MP_ROM_PTR(&tos_FileNameTmpTxt_obj) },
{ MP_ROM_QSTR(MP_QSTR_FileRead), MP_ROM_PTR(&tos_FileRead_obj) },
{ MP_ROM_QSTR(MP_QSTR_GetStr), MP_ROM_PTR(&tos_GetStr_obj) },
{ MP_ROM_QSTR(MP_QSTR_IntEntryGet), MP_ROM_PTR(&tos_IntEntryGet_obj) },
{ MP_ROM_QSTR(MP_QSTR_Let2Let), MP_ROM_PTR(&tos_Let2Let_obj) },
{ MP_ROM_QSTR(MP_QSTR_LinkedLstCopy), MP_ROM_PTR(&tos_LinkedLstCopy_obj) },
{ MP_ROM_QSTR(MP_QSTR_LstSub), MP_ROM_PTR(&tos_LstSub_obj) },
{ MP_ROM_QSTR(MP_QSTR_MAlloc), MP_ROM_PTR(&tos_MAlloc_obj) },
{ MP_ROM_QSTR(MP_QSTR_MAllocIdent), MP_ROM_PTR(&tos_MAllocIdent_obj) },
{ MP_ROM_QSTR(MP_QSTR_Mem32DevAlloc), MP_ROM_PTR(&tos_Mem32DevAlloc_obj) },
{ MP_ROM_QSTR(MP_QSTR_Mem64DevAlloc), MP_ROM_PTR(&tos_Mem64DevAlloc_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemPagAlloc), MP_ROM_PTR(&tos_MemPagAlloc_obj) },
{ MP_ROM_QSTR(MP_QSTR_MStrPrint), MP_ROM_PTR(&tos_MStrPrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_MStrUtil), MP_ROM_PTR(&tos_MStrUtil_obj) },
{ MP_ROM_QSTR(MP_QSTR_PCIReadU8), MP_ROM_PTR(&tos_PCIReadU8_obj) },
{ MP_ROM_QSTR(MP_QSTR_ScaleIndent), MP_ROM_PTR(&tos_ScaleIndent_obj) },
{ MP_ROM_QSTR(MP_QSTR_ScanCode2Char), MP_ROM_PTR(&tos_ScanCode2Char_obj) },
{ MP_ROM_QSTR(MP_QSTR_ScanCode2KeyName), MP_ROM_PTR(&tos_ScanCode2KeyName_obj) },
{ MP_ROM_QSTR(MP_QSTR_SrcEdLink), MP_ROM_PTR(&tos_SrcEdLink_obj) },
{ MP_ROM_QSTR(MP_QSTR_SrcFileName), MP_ROM_PTR(&tos_SrcFileName_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrFind), MP_ROM_PTR(&tos_StrFind_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrFirstOcc), MP_ROM_PTR(&tos_StrFirstOcc_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrFirstRem), MP_ROM_PTR(&tos_StrFirstRem_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrIMatch), MP_ROM_PTR(&tos_StrIMatch_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrLastOcc), MP_ROM_PTR(&tos_StrLastOcc_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrLastRem), MP_ROM_PTR(&tos_StrLastRem_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrMatch), MP_ROM_PTR(&tos_StrMatch_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrPrintFlags), MP_ROM_PTR(&tos_StrPrintFlags_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrPrintJoin), MP_ROM_PTR(&tos_StrPrintJoin_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrPrint), MP_ROM_PTR(&tos_StrPrint_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrScan), MP_ROM_PTR(&tos_StrScan_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrUtil), MP_ROM_PTR(&tos_StrUtil_obj) },
{ MP_ROM_QSTR(MP_QSTR_Tabs2Spaces), MP_ROM_PTR(&tos_Tabs2Spaces_obj) },
{ MP_ROM_QSTR(MP_QSTR_ToggleZorNotZ), MP_ROM_PTR(&tos_ToggleZorNotZ_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemCmp), MP_ROM_PTR(&tos_MemCmp_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemCpy), MP_ROM_PTR(&tos_MemCpy_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemSet), MP_ROM_PTR(&tos_MemSet_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemSetI64), MP_ROM_PTR(&tos_MemSetI64_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemSetU16), MP_ROM_PTR(&tos_MemSetU16_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemSetU32), MP_ROM_PTR(&tos_MemSetU32_obj) },
{ MP_ROM_QSTR(MP_QSTR_MemSetU8), MP_ROM_PTR(&tos_MemSetU8_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrLen), MP_ROM_PTR(&tos_StrLen_obj) },
{ MP_ROM_QSTR(MP_QSTR_InU8), MP_ROM_PTR(&tos_InU8_obj) },
{ MP_ROM_QSTR(MP_QSTR_OutU8), MP_ROM_PTR(&tos_OutU8_obj) },
{ MP_ROM_QSTR(MP_QSTR_Uf), MP_ROM_PTR(&tos_Uf_obj) },
{ MP_ROM_QSTR(MP_QSTR_U), MP_ROM_PTR(&tos_U_obj) },
{ MP_ROM_QSTR(MP_QSTR_StrNew), MP_ROM_PTR(&tos_StrNew_obj) },
{ MP_ROM_QSTR(MP_QSTR_DelTree), MP_ROM_PTR(&tos_DelTree_obj) },
{ MP_ROM_QSTR(MP_QSTR_CopyTree), MP_ROM_PTR(&tos_CopyTree_obj) },
{ MP_ROM_QSTR(MP_QSTR_Move), MP_ROM_PTR(&tos_Move_obj) },
{ MP_ROM_QSTR(MP_QSTR_Copy), MP_ROM_PTR(&tos_Copy_obj) },
{ MP_ROM_QSTR(MP_QSTR_Del), MP_ROM_PTR(&tos_Del_obj) },
{ MP_ROM_QSTR(MP_QSTR_Cd), MP_ROM_PTR(&tos_Cd_obj) },
{ MP_ROM_QSTR(MP_QSTR_Ed), MP_ROM_PTR(&tos_Ed_obj) },
{ MP_ROM_QSTR(MP_QSTR_Dir), MP_ROM_PTR(&tos_Dir_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirMk), MP_ROM_PTR(&tos_DirMk_obj) },
{ MP_ROM_QSTR(MP_QSTR_DirCur), MP_ROM_PTR(&tos_DirCur_obj) },
{ MP_ROM_QSTR(MP_QSTR_DocWrite), MP_ROM_PTR(&tos_DocWrite_obj) },
};

STATIC MP_DEFINE_CONST_DICT(tos_module_globals, tos_module_globals_table);

STATIC mp_obj_t tos_make_str(mp_obj_t buf) {
char * p0 = mp_obj_str_get_str(buf);
uint64_t *tmp=syscall1(__NR__tos_StrNew,p0);
return mp_obj_new_int(tmp);
}


const mp_obj_module_t tos_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&tos_module_globals,
};

