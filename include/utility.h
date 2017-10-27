/**
* @brief Singleton class to handle utility functions.
*
* The CUtility is a class which includes a set of different functions to
* be used in any class. Also, here some global variables/constants
*/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <vtkCommand.h>
#include <vtkSmartPointer.h>

class CErrorObserver : public vtkCommand 
{
private:
  bool m_bError;
  bool m_bWarning;
  std::string m_strErrorMsg;
  std::string m_strWarningMsg;
public:
  CErrorObserver() : m_bError{ false }, m_bWarning{ false }, m_strErrorMsg{ "" }, m_strWarningMsg{ "" }
  {}
  //static vtkSmartPointer<CErrorObserver> New();
  static CErrorObserver* New() 
  {
    return new CErrorObserver;
  }

  bool GetError() 
  {
    return m_bError;
  }

  bool GetWarning()
  {
    return m_bWarning;
  }

  virtual void Clear()
  {
    m_bError = false;
    m_bWarning = false;
    m_strErrorMsg = std::string("");
    m_strWarningMsg = std::string("");
  }

  virtual void Execute(vtkObject *vtkNotUsed(pCaller),unsigned long iEvent, void *pCalldata)
  {
    switch (iEvent)
    {
      case vtkCommand::ErrorEvent:
        m_strErrorMsg = static_cast<char*>(pCalldata);
        m_bError = true;
        break;
      case vtkCommand::WarningEvent:
        m_bWarning = true;
        m_strWarningMsg = static_cast<char*>(pCalldata);
        break;
    }
  }

  std::string GetErrorMessage() 
  {
    return m_strErrorMsg;
  }

  std::string GetWarningMessage()
  {
    return m_strWarningMsg;
  }
};

class CUtility
{
private:
	static CUtility* m_pInstance;
  vtkSmartPointer<CErrorObserver> m_error = nullptr;
	// Constructor
	CUtility();

  void ind2sub(const size_t *siz, int N, int idx, int *sub);
  int sub2ind(int *siz, int N, int *sub);
  
public:
  vtkSmartPointer<CErrorObserver> GetErrorInstance();
	const float RADIUS_DISTANCE = 20;
  // Static access method.
  static CUtility* getInstance();
};

#endif  //_UTILITY_H_