#include <stdio.h>
#include <stdlib.h>
#include "orahandle.h"


const char* ORA_LIB = "libclntsh.so";        

oraHandle::oraHandle()
{
    pLibraryName = ORA_LIB;
    // 8.0.x calls
    OCIInitialize = NULL;
    OCIHandleAlloc = NULL;
    OCIHandleFree = NULL;
    OCIDescriptorAlloc = NULL;
    OCIDescriptorFree = NULL;
    OCIEnvInit = NULL;
    OCIServerAttach = NULL;
    OCIServerDetach = NULL;
    OCISessionBegin = NULL;
    OCISessionEnd = NULL;
    OCILogon = NULL;
    OCILogoff = NULL;
    OCIPasswordChange = NULL;
    OCIStmtPrepare = NULL;
    OCIBindByPos = NULL;
    OCIBindByName = NULL;
    OCIBindObject = NULL;
    OCIBindDynamic = NULL;
    OCIBindArrayOfStruct = NULL;
    OCIStmtGetPieceInfo = NULL;
    OCIStmtSetPieceInfo = NULL;
    OCIStmtExecute = NULL;
    OCIDefineByPos = NULL;
    OCIDefineObject = NULL;
    OCIDefineDynamic = NULL;
    OCIDefineArrayOfStruct = NULL;
    OCIStmtFetch = NULL;
    OCIStmtGetBindInfo = NULL;
    OCIDescribeAny = NULL;
    OCIParamGet = NULL;
    OCIParamSet = NULL;
    OCITransStart = NULL;
    OCITransDetach = NULL;
    OCITransCommit = NULL;
    OCITransRollback = NULL;
    OCITransPrepare = NULL;
    OCITransForget = NULL;
    OCIErrorGet = NULL;
    OCILobAppend = NULL;
    OCILobAssign = NULL;
    OCILobCharSetForm = NULL;
    OCILobCharSetId = NULL;
    OCILobCopy = NULL;
    OCILobDisableBuffering = NULL;
    OCILobEnableBuffering = NULL;
    OCILobErase = NULL;
    OCILobFileClose = NULL;
    OCILobFileCloseAll = NULL;
    OCILobFileExists = NULL;
    OCILobFileGetName = NULL;
    OCILobFileIsOpen = NULL;
    OCILobFileOpen = NULL;
    OCILobFileSetName = NULL;
    OCILobFlushBuffer = NULL;
    OCILobGetLength = NULL;
    OCILobIsEqual = NULL;
    OCILobLoadFromFile = NULL;
    OCILobLocatorIsInit = NULL;
    OCILobRead = NULL;
    OCILobTrim = NULL;
    OCILobWrite = NULL;
    OCIBreak = NULL;
    OCIReset = NULL;
    OCIServerVersion = NULL;
    OCIAttrGet = NULL;
    OCIAttrSet = NULL;
    OCISvcCtxToLda = NULL;
    OCILdaToSvcCtx = NULL;
    OCIResultSetToStmt = NULL;

    // 8.1.x (8i) calls
    OCIEnvCreate = NULL;
    OCIEnvNlsCreate = NULL;
    OCIDurationBegin = NULL;
    OCIDurationEnd = NULL;
    OCILobCreateTemporary = NULL;
    OCILobFreeTemporary = NULL;
    OCILobIsTemporary = NULL;

    OCIAQEnq = NULL;
    OCIAQDeq = NULL;
    OCIAQListen = NULL;
    OCISubscriptionRegister = NULL;
    OCISubscriptionPost = NULL;
    OCISubscriptionUnRegister = NULL;
    OCISubscriptionDisable = NULL;
    OCISubscriptionEnable = NULL;

    OCIDateTimeConstruct = NULL;
    OCIDateTimeGetDate = NULL;
    OCIDateTimeGetTime = NULL;

}

oraHandle::~oraHandle()
{

}

void oraHandle::loadDBFunc(void* dlHandle)
{
    /* Resolve the symbol (method) from the object */
    // 8.0.x calls
    loadFunc(dlHandle, (void*&)OCIInitialize, "OCIInitialize");
    loadFunc(dlHandle, (void*&)OCIHandleAlloc, "OCIHandleAlloc");
    loadFunc(dlHandle, (void*&)OCIHandleFree, "OCIHandleFree");
    loadFunc(dlHandle, (void*&)OCIDescriptorAlloc, "OCIDescriptorAlloc");
    loadFunc(dlHandle, (void*&)OCIDescriptorFree, "OCIDescriptorFree");
    loadFunc(dlHandle, (void*&)OCIEnvInit, "OCIEnvInit");
    loadFunc(dlHandle, (void*&)OCIServerAttach, "OCIServerAttach");
    loadFunc(dlHandle, (void*&)OCIServerDetach, "OCIServerDetach");
    loadFunc(dlHandle, (void*&)OCISessionBegin, "OCISessionBegin");
    loadFunc(dlHandle, (void*&)OCISessionEnd, "OCISessionEnd");
    loadFunc(dlHandle, (void*&)OCILogon, "OCILogon");
    loadFunc(dlHandle, (void*&)OCILogoff, "OCILogoff");
    loadFunc(dlHandle, (void*&)OCIPasswordChange, "OCIPasswordChange");
    loadFunc(dlHandle, (void*&)OCIStmtPrepare, "OCIStmtPrepare");
    loadFunc(dlHandle, (void*&)OCIBindByPos, "OCIBindByPos");
    loadFunc(dlHandle, (void*&)OCIBindByName, "OCIBindByName");
    loadFunc(dlHandle, (void*&)OCIBindObject, "OCIBindObject");
    loadFunc(dlHandle, (void*&)OCIBindDynamic, "OCIBindDynamic");
    loadFunc(dlHandle, (void*&)OCIBindArrayOfStruct, "OCIBindArrayOfStruct");
    loadFunc(dlHandle, (void*&)OCIStmtGetPieceInfo, "OCIStmtGetPieceInfo");
    loadFunc(dlHandle, (void*&)OCIStmtSetPieceInfo, "OCIStmtSetPieceInfo");
    loadFunc(dlHandle, (void*&)OCIStmtExecute, "OCIStmtExecute");
    loadFunc(dlHandle, (void*&)OCIDefineByPos, "OCIDefineByPos");
    loadFunc(dlHandle, (void*&)OCIDefineObject, "OCIDefineObject");
    loadFunc(dlHandle, (void*&)OCIDefineDynamic, "OCIDefineDynamic");
    loadFunc(dlHandle, (void*&)OCIDefineArrayOfStruct, "OCIDefineArrayOfStruct");
    loadFunc(dlHandle, (void*&)OCIStmtFetch, "OCIStmtFetch");
    loadFunc(dlHandle, (void*&)OCIStmtGetBindInfo, "OCIStmtGetBindInfo");
    loadFunc(dlHandle, (void*&)OCIDescribeAny, "OCIDescribeAny");
    loadFunc(dlHandle, (void*&)OCIParamGet, "OCIParamGet");
    loadFunc(dlHandle, (void*&)OCIParamSet, "OCIParamSet");
    loadFunc(dlHandle, (void*&)OCITransStart, "OCITransStart");
    loadFunc(dlHandle, (void*&)OCITransDetach, "OCITransDetach");
    loadFunc(dlHandle, (void*&)OCITransCommit, "OCITransCommit");
    loadFunc(dlHandle, (void*&)OCITransRollback, "OCITransRollback");
    loadFunc(dlHandle, (void*&)OCITransPrepare, "OCITransPrepare");
    loadFunc(dlHandle, (void*&)OCITransForget, "OCITransForget");
    loadFunc(dlHandle, (void*&)OCIErrorGet, "OCIErrorGet");
    loadFunc(dlHandle, (void*&)OCILobAppend, "OCILobAppend");
    loadFunc(dlHandle, (void*&)OCILobAssign, "OCILobAssign");
    loadFunc(dlHandle, (void*&)OCILobCharSetForm, "OCILobCharSetForm");
    loadFunc(dlHandle, (void*&)OCILobCharSetId, "OCILobCharSetId");
    loadFunc(dlHandle, (void*&)OCILobCopy, "OCILobCopy");
    loadFunc(dlHandle, (void*&)OCILobDisableBuffering, "OCILobDisableBuffering");
    loadFunc(dlHandle, (void*&)OCILobEnableBuffering, "OCILobEnableBuffering");
    loadFunc(dlHandle, (void*&)OCILobErase, "OCILobErase");
    loadFunc(dlHandle, (void*&)OCILobFileClose, "OCILobFileClose");
    loadFunc(dlHandle, (void*&)OCILobFileCloseAll, "OCILobFileCloseAll");
    loadFunc(dlHandle, (void*&)OCILobFileExists, "OCILobFileExists");
    loadFunc(dlHandle, (void*&)OCILobFileGetName, "OCILobFileGetName");
    loadFunc(dlHandle, (void*&)OCILobFileIsOpen, "OCILobFileIsOpen");
    loadFunc(dlHandle, (void*&)OCILobFileOpen, "OCILobFileOpen");
    loadFunc(dlHandle, (void*&)OCILobFileSetName, "OCILobFileSetName");
    loadFunc(dlHandle, (void*&)OCILobFlushBuffer, "OCILobFlushBuffer");
    loadFunc(dlHandle, (void*&)OCILobGetLength, "OCILobGetLength");
    loadFunc(dlHandle, (void*&)OCILobIsEqual, "OCILobIsEqual");
    loadFunc(dlHandle, (void*&)OCILobLoadFromFile, "OCILobLoadFromFile");
    loadFunc(dlHandle, (void*&)OCILobLocatorIsInit, "OCILobLocatorIsInit");
    loadFunc(dlHandle, (void*&)OCILobRead, "OCILobRead");
    loadFunc(dlHandle, (void*&)OCILobTrim, "OCILobTrim");
    loadFunc(dlHandle, (void*&)OCILobWrite, "OCILobWrite");
    loadFunc(dlHandle, (void*&)OCIBreak, "OCIBreak");
    loadFunc(dlHandle, (void*&)OCIReset, "OCIReset");
    loadFunc(dlHandle, (void*&)OCIServerVersion, "OCIServerVersion");
    loadFunc(dlHandle, (void*&)OCIAttrGet, "OCIAttrGet");
    loadFunc(dlHandle, (void*&)OCIAttrSet, "OCIAttrSet");
    loadFunc(dlHandle, (void*&)OCISvcCtxToLda, "OCISvcCtxToLda");
    loadFunc(dlHandle, (void*&)OCILdaToSvcCtx, "OCILdaToSvcCtx");
    loadFunc(dlHandle, (void*&)OCIResultSetToStmt, "OCIResultSetToStmt");

    // 8.1.x (8i) calls
    loadFunc(dlHandle, (void*&)OCIEnvCreate, "OCIEnvCreate");
    loadFunc(dlHandle, (void*&)OCIEnvNlsCreate, "OCIEnvNlsCreate");
    loadFunc(dlHandle, (void*&)OCIDurationBegin, "OCIDurationBegin");
    loadFunc(dlHandle, (void*&)OCIDurationEnd, "OCIDurationEnd");
    loadFunc(dlHandle, (void*&)OCILobCreateTemporary, "OCILobCreateTemporary");
    loadFunc(dlHandle, (void*&)OCILobFreeTemporary, "OCILobFreeTemporary");
    loadFunc(dlHandle, (void*&)OCILobIsTemporary, "OCILobIsTemporary");
    loadFunc(dlHandle, (void*&)OCIAQEnq, "OCIAQEnq");
    loadFunc(dlHandle, (void*&)OCIAQDeq, "OCIAQDeq");
    loadFunc(dlHandle, (void*&)OCIAQListen, "OCIAQListen");
    loadFunc(dlHandle, (void*&)OCISubscriptionRegister, "OCISubscriptionRegister");
    loadFunc(dlHandle, (void*&)OCISubscriptionPost, "OCISubscriptionPost");
    loadFunc(dlHandle, (void*&)OCISubscriptionUnRegister, "OCISubscriptionUnRegister");
    loadFunc(dlHandle, (void*&)OCISubscriptionDisable, "OCISubscriptionDisable");
    loadFunc(dlHandle, (void*&)OCISubscriptionEnable, "OCISubscriptionEnable");
    loadFunc(dlHandle, (void*&)OCIDateTimeConstruct, "OCIDateTimeConstruct");
    loadFunc(dlHandle, (void*&)OCIDateTimeGetDate, "OCIDateTimeGetDate");
    loadFunc(dlHandle, (void*&)OCIDateTimeGetTime, "OCIDateTimeGetTime");
    loadFunc(dlHandle, (void*&)OCINumberToInt, "OCINumberToInt");

}

