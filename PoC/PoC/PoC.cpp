#include <Python.h>

#define _ALLOW_RTCc_IN_STL
#include <string>
#include <iostream>
#include <codecvt>


std::wstring DiagnosisText;

std::string ws2s(std::wstring const &wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

bool isException = false;
void SetPyException(std::wstring const str)
{
	PyErr_SetString(PyExc_RuntimeError, ws2s(str).c_str()); // Throw an exception.
	isException = true;
}

PyObject *IncPyNone()
{
	Py_INCREF(Py_None);
	return Py_None;
}


PyObject *ActivateExceptionImmediately()
{
	if (isException)
	{
		return nullptr;
	}
	else return IncPyNone();
}

static PyObject *MyModule_Diagnosis(PyObject *const self, PyObject *args)
{
	(void *) self;

	PyObject *PyText     = nullptr;

	if (PyArg_UnpackTuple(args, "", 1, 1, &PyText))
	{
		if (PyText)
		{
			wchar_t *const wcText = PyUnicode_AsWideCharString(PyObject_Str(PyText), nullptr);
			DiagnosisText = std::wstring(wcText);
			PyMem_Free(wcText);
		}
		else SetPyException(L"PYERROR_INVALID_PARAMETER1_TYPE MyModule.Diagnosis");
	}
	else SetPyException(L"PYERROR_INVALID_PARAMETER MyModule.Diagnosis");
	std::wcout << DiagnosisText;
	return ActivateExceptionImmediately();
}

static struct PyMethodDef Methods[] =
{
	{"Diagnosis"  ,              MyModule_Diagnosis  , METH_VARARGS                , "Diagnosis(Text: str|object) -> None"},
	{nullptr      ,              nullptr             , 0                           , nullptr                              }
};


static struct PyModuleDef ModuleDefinitions =
{
	PyModuleDef_HEAD_INIT, 
	"MyModule",

	"Support module: MyModule.\n",

	1,                                                                     // size
	Methods,                                                               // methods
	nullptr, 
	nullptr, 
	nullptr, 
	nullptr
};

static PyObject *PyInit_MyModule()
{
	PyObject *Module = PyModule_Create(&ModuleDefinitions);

	return Module;
}

int main()
{
	int const Error = PyImport_AppendInittab("MyModule", &PyInit_MyModule);

	Py_Initialize();


	PyObject *pName   = PyUnicode_DecodeFSDefault("image_processor");
	PyObject *pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	
	if (pModule != NULL) {
		PyObject *pFunc = PyObject_GetAttrString(pModule, "process_image");
		
		if (pFunc && PyCallable_Check(pFunc)) {
			PyObject *pValue = PyObject_CallFunction
			(
				pFunc,
				nullptr ,
				nullptr 
			);
		
			if (pValue != NULL) {
				#define SHOW_CRASH
				#ifdef SHOW_CRASH
					std::wcout << L"Before";
					Py_XDECREF(pValue);
					std::wcout << L"After"; // not reached!
				#else
					//*static inline void Py_XDECREF(PyObject *op)
					{
						PyObject *op = pValue;
						if (op != _Py_NULL) {
							//*static inline Py_ALWAYS_INLINE void Py_DECREF(PyObject *op)
							{
								// Non-limited C API and limited C API for Python 3.9 and older access
								// directly PyObject.ob_refcnt.

								if (
									//*_Py_IsImmortal(op)
									//* static inline Py_ALWAYS_INLINE int _Py_IsImmortal(PyObject *op)
									//*{
									#if defined(Py_GIL_DISABLED)
										return (_Py_atomic_load_uint32_relaxed(&op->ob_ref_local) ==
												_Py_IMMORTAL_REFCNT_LOCAL);
									#elif SIZEOF_VOID_P > 4
										//*return 
										(_Py_CAST(PY_INT32_T, op->ob_refcnt_split[0]) < 0)//*; // & 0xFFFFFFFF or use op->ob_refcnt_split[0]
									#else
										return (op->ob_refcnt == _Py_IMMORTAL_REFCNT)//*;
									#endif
									//*}
								) {
									return 0;
								}
								_Py_DECREF_STAT_INC();
								if (--op->ob_refcnt == 0) {
									_Py_Dealloc(op);
								}
							}
						}
					}
					/**/
				#endif/**/
			} else {
				PyErr_Print();
			}
		} else {
			PyErr_Print();
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
	} else {
		PyErr_Print();
	}

	std::wcout << L"END";
	return 0;
}
