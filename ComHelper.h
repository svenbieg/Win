//=============
// ComHelper.h
//=============

#pragma once


//========
// Common
//========

inline VOID ThrowIfFailed(HRESULT Status) { if(FAILED(Status))throw Status; }
