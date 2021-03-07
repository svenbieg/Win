//==========
// ComPtr.h
//==========

#pragma once


//=============
// COM-Pointer
//=============

template <class _obj_t>
class ComPtr
{
public:
	// Con-/Destructors
	ComPtr(): pObject(nullptr) {}
	ComPtr(_obj_t* Object): pObject(Object)
		{
		if(pObject)
			pObject->AddRef();
		}
	ComPtr(ComPtr const& Pointer): pObject(Pointer.pObject)
		{
		if(pObject)
			pObject->AddRef();
		}
	ComPtr(ComPtr && Pointer): pObject(Pointer.pObject)
		{
		Pointer.pObject=nullptr;
		}
	~ComPtr()
		{
		if(pObject)
			pObject->Release();
		}

	// Access
	operator _obj_t*()const { return pObject; }
	_obj_t* operator->()const { return pObject; }
	_obj_t* Get() { return pObject; }
	_obj_t* const* GetAddressOf()const { return &pObject; }

	// Modification
	ComPtr& operator=(_obj_t* Object)
		{
		if(Object==pObject)
			return *this;
		if(pObject)
			pObject->Release();
		pObject=Object;
		if(pObject)
			pObject->AddRef();
		return *this;
		}
	ComPtr& operator=(ComPtr const& Pointer) { return operator=(Pointer.pObject); }

private:
	// Common
	_obj_t* pObject;
};
