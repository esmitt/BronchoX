/**
* @brief Singleton class to handle utility functions.
*
* The CUtility is a class which includes a set of different functions to
* be used in any class. Also, here some global variables/constants
*/

#ifndef _UTILITY_H_
#define _UTILITY_H_

class CUtility
{
private:
	static CUtility* m_pInstance; 
	// Constructor
	CUtility();

  void ind2sub(const size_t *siz, int N, int idx, int *sub);
  int sub2ind(int *siz, int N, int *sub);
public:
	const float RADIUS_DISTANCE = 20;
  // Static access method.
  static CUtility* getInstance();
};

#endif  //_UTILITY_H_