/**
* @brief A node of the specialized CGraph.
*
* Class to handle a graph's vertex. Each vertex is a 3D point into euclidean space. <BR>
* It stores an id, a 3D point, and a set of linked neighbours. <BR>
* Also, stores the information of its neighbours (link connectivity)
*/
#ifndef _GRAPHVERTEX_H_
#define _GRAPHVERTEX_H_

#include <set>
#include <iostream>
#include <vtkMath.h>

class CGraphVertex
{
private:
	int m_iIndex;	// indicates its position (i.e. an ID)
	std::set <std::pair<int, double>> m_vNeighbours;	// store a pair of values (id, distance) into a set
	double m_vPoint[3];	//point 3D

public:
  /// Default constructor
  CGraphVertex(); 
  
  /// Constructor with parameters
  /// @param pValue a pointer of 3 positions of double
  CGraphVertex(const double* pValue);
  
  /// Constructor with parameters
  /// @param iIndex id of this node (id inside the CGraph)
  /// @param pValue a pointer of 3 positions of double
  CGraphVertex(int iIndex, const double* pValue);

  /// Constructor with parameters
  /// @param iIndex id of this node (id inside the CGraph)
  /// @param pValue a pointer of 3 positions of double
  CGraphVertex(int iIndex, const double& dX, double dY, double dZ);

  /// Copy constructorparameter
  /// @param vertex object of a vertex
  CGraphVertex(const CGraphVertex &vertex);

  ~CGraphVertex() {}

  void setIndex(const int& index);

	//functions
	double & operator[](int index) { return m_vPoint[index]; }

	//add an edge
	void link(int index, double distance = 0)
	{
		m_vNeighbours.insert(std::make_pair(index, distance));
	}

	inline int getIndex() { return m_iIndex; }

	double* get_ptr() { return m_vPoint; }

	std::set<std::pair<int, double>>& getNeighbours()
	{
		return m_vNeighbours;
	}

	void printNeighbours()
	{
    auto iIt = m_vNeighbours.begin();
		while (iIt != m_vNeighbours.end())
		{
			std::cout << "(" << m_iIndex << ", " << iIt->first << ") = " << iIt->second << std::endl;
			iIt++;
		}
	}

  //testing
  bool isNear(const double p[3])
  {
    if (vtkMath::Distance2BetweenPoints(p, m_vPoint) < 0.1001f)
      return true;
    return false;
  }
};

#endif	//_GRAPHVERTEX_H_